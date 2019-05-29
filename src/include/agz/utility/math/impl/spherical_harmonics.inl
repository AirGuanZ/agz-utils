#pragma once

namespace agz::math {

namespace sh_impl
{

    // see https://en.wikipedia.org/wiki/Table_of_spherical_harmonics
    template<typename T, int L, int M> struct sh_expr;

#define DEFINE_SH(L, M, COEF, EXPR) \
    template<typename T> \
    struct sh_expr<T, L, M> \
    { \
        static auto eval([[maybe_unused]] const tvec3<T> &v) noexcept \
        { \
            [[maybe_unused]] constexpr T pi = agz::math::PI<T>; \
            [[maybe_unused]] constexpr T x = v.x; \
            [[maybe_unused]] constexpr T y = v.y; \
            [[maybe_unused]] constexpr T z = v.z; \
            static const T C = (COEF); \
            return C * (EXPR); \
        } \
    }

    DEFINE_SH(0, 0, T(0.5) * std::sqrt(1 / pi), 1);

    DEFINE_SH(1, -1, std::sqrt(3 / (4 * pi)), y);
    DEFINE_SH(1, 0, std::sqrt(3 / (4 * pi)),  z);
    DEFINE_SH(1, 1, std::sqrt(3 / (4 * pi)),  x);

    DEFINE_SH(2, -2, T(0.5)  * std::sqrt(15 / pi), x * y);
    DEFINE_SH(2, -1, T(0.5)  * std::sqrt(15 / pi), y * z);
    DEFINE_SH(2, 0,  T(0.25) * std::sqrt(5 / pi),  -x * x - y * y + 2 * z * z);
    DEFINE_SH(2, 1,  T(0.5)  * std::sqrt(15 / pi), z * x);
    DEFINE_SH(2, 2,  T(0.25) * std::sqrt(15 / pi), x * x - y * y);

    DEFINE_SH(3, -3, T(0.25) * std::sqrt(35 / (2 * pi)), (3 * x * x - y * y) * y);
    DEFINE_SH(3, -2, T(0.5)  * std::sqrt(105 / pi),      x * y * z);
    DEFINE_SH(3, -1, T(0.25) * std::sqrt(21 / (2 * pi)), y * (4 * z * z - x * x - y * y));
    DEFINE_SH(3, 0,  T(0.25) * std::sqrt(7 / pi),        z * (2 * z * z - 3 * x * x - 3 * y * y));
    DEFINE_SH(3, 1,  T(0.25) * std::sqrt(21 / (2 * pi)), x * (4 * z * z - x * x - y * y));
    DEFINE_SH(3, 2,  T(0.25) * std::sqrt(105 / pi),      (x * x - y * y) * z);
    DEFINE_SH(3, 3,  T(0.25) * std::sqrt(35 / (2 * pi)), (x * x - 3 * y * y) * x);

    DEFINE_SH(4, -4, T(0.75)   * std::sqrt(35 / pi),       x * y * (x * x - y * y));
    DEFINE_SH(4, -3, T(0.75)   * std::sqrt(35 / (2 * pi)), (3 * x * x - y * y) * y * z);
    DEFINE_SH(4, -2, T(0.75)   * std::sqrt(5 / pi),        x * y * (7 * z * z - 1));
    DEFINE_SH(4, -1, T(0.75)   * std::sqrt(5 / (2 * pi)),  y * z * (7 * z * z - 3));
    DEFINE_SH(4, 0,  T(3) / 16 * std::sqrt(1 / pi),        (35 * (z * z) * (z * z) - 30 * z * z + 3));
    DEFINE_SH(4, 1,  T(0.75)   * std::sqrt(5 / (2 * pi)),  x * z * (7 * z * z - 3));
    DEFINE_SH(4, 2,  T(3) / 8  * std::sqrt(5 / pi),        (x * x - y * y) * (7 * z * z - 1));
    DEFINE_SH(4, 3,  T(0.75)   * std::sqrt(35 / (2 * pi)), (x * x - 3 * y * y) * x * z);
    DEFINE_SH(4, 4,  T(3) / 16 * std::sqrt(35 / pi),       x * x * (x * x - 3 * y * y) - y * y * (3 * x * x - y * y));

#undef DEFINE_SH

    template<typename T, int L> struct rotate_sh_coefs_impl;

    template<typename T>
    struct rotate_sh_coefs_impl<T, 0>
    {
        static void call(const tmat3_c<T>&, T*) noexcept
        {
            
        }
    };

    template<typename T>
    struct rotate_sh_coefs_impl<T, 1>
    {
        static void call(const tmat3_c<T> &rot, T *coefs) noexcept
        {
            static const T INV_C = Sqrt(4 * PI<T> / 3);

            static const mat3f_c<T> INV_A(0,     0,     INV_C,
                                          INV_C, 0,     0,
                                          0,     INV_C, 0);

            // 由于N_i被选取为各轴上的方向向量，故M * Ni就是M的第i列

            auto PMN0 = spherical_harmonics::project_to_sh<1>(rot.get_col(0));
            auto PMN1 = spherical_harmonics::project_to_sh<1>(rot.get_col(1));
            auto PMN2 = spherical_harmonics::project_to_sh<1>(rot.get_col(2));
            auto S = tmat3_c<T>::from_cols(PMN0, PMN1, PMN2);

            tvec3<T> x(coefs[0], coefs[1], coefs[2]);
            x = S * (INV_A * x);

            coefs[0] = x[0];
            coefs[1] = x[1];
            coefs[2] = x[2];
        }
    };

    template<typename T>
    struct rotate_sh_coefs_impl<T, 2>
    {
        static void call(const tmat3_c<T> &rot, T *coefs) noexcept
        {
            constexpr T K = T(0.7071067811865475);
            static const tvec3<T> N2(K, K, T(0));
            static const tvec3<T> N3(K, T(0), K);
            static const tvec3<T> N4(T(0), K, K);

            constexpr T K0 = T(0.91529123286551084);
            constexpr T K1 = T(1.83058246573102168);
            constexpr T K2 = T(1.5853309190550713);

            // 计算A^{-1}x

            tvec<T, 5> invAx(K0 * (coefs[1] - coefs[3]) + K1 * coefs[4],
                             K0 * (coefs[0] - coefs[3] + coefs[4]) + K2 * coefs[2],
                             K1 * coefs[0],
                             K1 * coefs[3],
                             K1 * coefs[1]);

            // 构造S
            // IMPROVE：N是确定的，故P(M * N)可以优化

            auto PMN0 = spherical_harmonics::project_to_sh<2>(rot.get_col(0));
            auto PMN1 = spherical_harmonics::project_to_sh<2>(rot.get_col(2));
            auto PMN2 = spherical_harmonics::project_to_sh<2>(rot * N2);
            auto PMN3 = spherical_harmonics::project_to_sh<2>(rot * N3);
            auto PMN4 = spherical_harmonics::project_to_sh<2>(rot * N4);

            // 计算S(A^{-1}x)

            coefs[0] = PMN0[0] * invAx[0] + PMN1[0] * invAx[1] + PMN2[0] * invAx[2] + PMN3[0] * invAx[3] + PMN4[0] * invAx[4];
            coefs[1] = PMN0[1] * invAx[0] + PMN1[1] * invAx[1] + PMN2[1] * invAx[2] + PMN3[1] * invAx[3] + PMN4[1] * invAx[4];
            coefs[2] = PMN0[2] * invAx[0] + PMN1[2] * invAx[1] + PMN2[2] * invAx[2] + PMN3[2] * invAx[3] + PMN4[2] * invAx[4];
            coefs[3] = PMN0[3] * invAx[0] + PMN1[3] * invAx[1] + PMN2[3] * invAx[2] + PMN3[3] * invAx[3] + PMN4[3] * invAx[4];
            coefs[4] = PMN0[4] * invAx[0] + PMN1[4] * invAx[1] + PMN2[4] * invAx[2] + PMN3[4] * invAx[3] + PMN4[4] * invAx[4];
        }
    };

    template<typename T>
    struct rotate_sh_coefs_impl<T, 3>
    {
        static void call(const tmat3_c<T> &rot, T *coefs) noexcept
        {
            static const tvec3<T> N[7] = {
                tvec3<T>(T(1),   T(0), T(0))   .normalize(),
                tvec3<T>(T(0),   T(1), T(0))   .normalize(),
                tvec3<T>(T(0.3), T(0), T(1))   .normalize(),
                tvec3<T>(T(0),   T(1), T(1))   .normalize(),
                tvec3<T>(T(1),   T(0), T(1))   .normalize(),
                tvec3<T>(T(1),   T(1), T(0.78)).normalize(),
                tvec3<T>(T(1),   T(1), T(1))   .normalize()
            };
            
            static const T invA[7][7] = {
                { T(  0.707711955885399), T( 0.643852929494021), T( -0.913652206352009), T(-0.093033334712756), T(  0.328680372803511), T( -1.131667680791894), T(  1.949384763080401) },
                { T( -1.114187338255984), T( 0.643852929494021), T( -0.749554866243252), T(-0.093033334712757), T(  0.164583032694754), T( -0.232204002745663), T(  0.127485468939019) },
                { T(  2.296023687102124), T(                 0), T( -2.964153834214758), T(                 0), T(  2.964153834214758), T( -3.749390980495911), T(  2.296023687102124) },
                { T(  2.392306681179504), T(-1.099424142052695), T( -3.088454645076318), T(-2.129025696294232), T(  3.766408103751610), T( -5.313883353254694), T(  2.917447172170129) },
                { T(  1.878707739441422), T(-1.099424142052695), T( -2.425401262415870), T(-2.129025696294233), T(  3.103354721091161), T( -2.518204820606409), T(  2.403848230432046) },
                { T( 13.656934981397061), T(-4.181565269348606), T(-17.631027247729438), T(-8.097566324633245), T( 14.325209638780166), T(-20.210898801851609), T( 11.096259672385109) },
                { T(-13.139185354460187), T( 5.820633765367933), T( 16.962615353518899), T( 7.790578559853934), T(-13.782124974734103), T( 19.444681101542464), T(-10.675588100498899) },
            };
            
            tvec<T, 7> invAx(T(0));
            for(int i = 0; i != 7; ++i)
            {
                for(int j = 0; j != 7; ++j)
                    invAx[i] += invA[i][j] * coefs[j];
            }
            
            tvec<T, 7> PMN[7];
            for(int i = 0; i < 7; ++i)
                PMN[i] = spherical_harmonics::project_to_sh<3>(rot * N[i]);
            
            for(int i = 0; i < 7; ++i)
            {
                coefs[i] = T(0);
                for(int j = 0; j < 7; ++j)
                    coefs[i] += PMN[j][i] * invAx[j];
            }
        }
    };

    template<typename T>
    struct rotate_sh_coefs_impl<T, 4>
    {
        static void call(const tmat3_c<T> &rot, T *coefs) noexcept
        {
            static const tvec3<T> N[9] = {
                tvec3<T>(T(1),    T(0),    T(0))   .normalize(),
                tvec3<T>(T(0),    T(1),    T(0))   .normalize(),
                tvec3<T>(T(0.3),  T(0),    T(1))   .normalize(),
                tvec3<T>(T(0),    T(1),    T(1))   .normalize(),
                tvec3<T>(T(1),    T(0),    T(1))   .normalize(),
                tvec3<T>(T(1),    T(0.54), T(0.78)).normalize(),
                tvec3<T>(T(1),    T(1),    T(0.78)).normalize(),
                tvec3<T>(T(0.31), T(1),    T(0.78)).normalize(),
                tvec3<T>(T(1),    T(1),    T(1))   .normalize()
            };

            static const T invA[9][9] = {
                { T(-1.948206991589258), T(1.912687049138671),  T(-0.763091021186035), T(-0.286837642392582), T(-0.341264679278342), T(0.594477634079894),  T(-1.056887279361603), T(0.578857155270682),  T(0.971984464556520)  },
                { T(2.171192074917378),  T(-0.142084581369102), T(-1.577618721617938), T(0.828536347413562),  T(-0.705532540822805), T(0.382031320127708),  T(1.056887279361603),  T(-2.513802449733083), T(1.156701984383617)  },
                { T(2.053952330860290),  T(-0.094158653118148), T(-0.750956907863241), T(-1.098731135021785), T(-0.335838138831051), T(1.931188736063331),  T(0),                  T(-1.051043414216722), T(0.170301019159901)  },
                { T(3.993132334888566),  T(1.179414191911931),  T(-4.808985771815311), T(1.266884703225481),  T(-3.095952538204609), T(2.811562290853012),  T(0),                  T(-4.022967497037739), T(1.569934476060706)  },
                { T(-1.543780567538975), T(1.894449743774703),  T(-2.499709102566265), T(-0.207318037527907), T(-2.063212615945576), T(1.725864595116423),  T(0),                  T(-0.365404044003703), T(1.046239752465574)  },
                { T(3.435134010827782),  T(-2.932684025967419), T(4.231264528651311),  T(-2.972023260715974), T(1.892279023369589),  T(-1.718456688280952), T(0),                  T(2.458880397035034),  T(-0.959560600640598) },
                { T(3.689266412234284),  T(1.985158283498190),  T(-7.403078714786565), T(-3.123392326177335), T(-3.310757449808909), T(3.006635497533013),  T(0),                  T(-4.302091019418769), T(1.678860447048080)  },
                { T(-0.367659806642012), T(-3.222124483746851), T(4.648868038376401),  T(-3.265346293642776), T(2.079036990447149),  T(-1.888059306949047), T(0),                  T(2.701558933638689),  T(-1.054264174928627) },
                { T(-4.515212732000947), T(3.220651333447782),  T(0.208527587656698),  T(6.066568738154828),  T(-0.970215938306426), T(0.881093140952614),  T(0),                  T(-1.260725782049042), T(0.491989276959057)  },
            };

            tvec<T, 9> invAx(T(0));
            for(int i = 0; i != 9; ++i)
            {
                for(int j = 0; j != 9; ++j)
                    invAx[i] += invA[i][j] * coefs[j];
            }

            tvec<T, 9> PMN[9];
            for(int i = 0; i < 9; ++i)
                PMN[i] = spherical_harmonics::project_to_sh<4>(rot * N[i]);

            for(int i = 0; i < 9; ++i)
            {
                coefs[i] = T(0);
                for(int j = 0; j < 9; ++j)
                    coefs[i] += PMN[j][i] * invAx[j];
            }
        }
    };

} // namespace sh_impl

namespace spherical_harmonics
{

    template<int L, int M, typename T>
    auto eval(const tvec3<T> &v) noexcept
    {
        return sh_impl::sh_expr<T, L, M>::eval(v.normalize());
    }

    constexpr int to_linear_index(int L, int M) noexcept
    {
        return L * (L + 1) + M;
    }

    inline std::pair<int, int> from_linear_index(int index) noexcept
    {
        // IMPROVE: see https://en.wikipedia.org/wiki/Methods_of_computing_square_roots
        int L = static_cast<int>(std::sqrt(static_cast<float>(index)));
        int M = index - L * (L + 1);
        return { L, M };
    }

    template<typename T>
    sh_func_t<T> *linear_table() noexcept
    {
        static sh_func_t<T> table[] =
        {
            &agz::math::spherical_harmonics::eval<0, 0, T>,

            &agz::math::spherical_harmonics::eval<1, -1, T>,
            &agz::math::spherical_harmonics::eval<1,  0, T>,
            &agz::math::spherical_harmonics::eval<1,  1, T>,

            &agz::math::spherical_harmonics::eval<2, -2, T>,
            &agz::math::spherical_harmonics::eval<2, -1, T>,
            &agz::math::spherical_harmonics::eval<2,  0, T>,
            &agz::math::spherical_harmonics::eval<2,  1, T>,
            &agz::math::spherical_harmonics::eval<2,  2, T>,

            &agz::math::spherical_harmonics::eval<3, -3, T>,
            &agz::math::spherical_harmonics::eval<3, -2, T>,
            &agz::math::spherical_harmonics::eval<3, -1, T>,
            &agz::math::spherical_harmonics::eval<3,  0, T>,
            &agz::math::spherical_harmonics::eval<3,  1, T>,
            &agz::math::spherical_harmonics::eval<3,  2, T>,
            &agz::math::spherical_harmonics::eval<3,  3, T>,

            &agz::math::spherical_harmonics::eval<4, -4, T>,
            &agz::math::spherical_harmonics::eval<4, -3, T>,
            &agz::math::spherical_harmonics::eval<4, -2, T>,
            &agz::math::spherical_harmonics::eval<4, -1, T>,
            &agz::math::spherical_harmonics::eval<4,  0, T>,
            &agz::math::spherical_harmonics::eval<4,  1, T>,
            &agz::math::spherical_harmonics::eval<4,  2, T>,
            &agz::math::spherical_harmonics::eval<4,  3, T>,
            &agz::math::spherical_harmonics::eval<4,  4, T>,
        };
        return table;
    }

    template<int L, typename T>
    tvec<T, 2 * L + 1> project_to_sh(const tvec3<T> &v) noexcept
    {
        auto nv = v.normalize();
        auto sh_tab = linear_table<T>();

        tvec<T, 2 * L + 1> ret(UNINIT);
        for(int i = 0, j = L * L; i != 2 * L + 1; ++i, ++j)
            ret[i] = sh_tab[j](nv);

        return ret;
    }

    template<int L, typename T>
    void rotate_sh_coefs(const tmat3_c<T> &rot, T *coefs) noexcept
    {
        sh_impl::rotate_sh_coefs_impl<T, L>::call(rot, coefs);
    }

} // namespace spherical_harmonics

} // namespace agz::math
