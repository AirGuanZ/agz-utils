#define SWIZZLE2(A, B)       _SWIZZLE2(A, B)
#define SWIZZLE3(A, B, C)    _SWIZZLE3(A, B, C)
#define SWIZZLE4(A, B, C, D) _SWIZZLE4(A, B, C, D)

#define _SWIZZLE2(A, B)       tvec2<T> A##B()       const { return tvec2<T>(A, B); }
#define _SWIZZLE3(A, B, C)    tvec3<T> A##B##C()    const { return tvec3<T>(A, B, C); }
#define _SWIZZLE4(A, B, C, D) tvec4<T> A##B##C##D() const { return tvec4<T>(A, B, C, D); }

SWIZZLE2(x, x)
SWIZZLE2(x, y)
SWIZZLE2(y, y)
SWIZZLE2(y, x)

#undef SWIZZLE2
#undef SWIZZLE3
#undef SWIZZLE4
#undef _SWIZZLE2
#undef _SWIZZLE3
#undef _SWIZZLE4
