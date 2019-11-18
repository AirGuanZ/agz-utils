#define SWIZZLE2(A, B)       _SWIZZLE2(A, B)
#define SWIZZLE3(A, B, C)    _SWIZZLE3(A, B, C)
#define SWIZZLE4(A, B, C, D) _SWIZZLE4(A, B, C, D)

#define _SWIZZLE2(A, B)       tcolor2<T> A##B()       const { return tcolor2<T>(A, B); }
#define _SWIZZLE3(A, B, C)    tcolor3<T> A##B##C()    const { return tcolor3<T>(A, B, C); }
#define _SWIZZLE4(A, B, C, D) tcolor4<T> A##B##C##D() const { return tcolor4<T>(A, B, C, D); }

SWIZZLE2(c, c)
SWIZZLE2(c, a)
SWIZZLE2(a, a)
SWIZZLE2(a, c)

#undef SWIZZLE2
#undef SWIZZLE3
#undef SWIZZLE4
#undef _SWIZZLE2
#undef _SWIZZLE3
#undef _SWIZZLE4
