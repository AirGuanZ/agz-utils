#define SWIZZLE2(A, B)       _SWIZZLE2(A, B)
#define SWIZZLE3(A, B, C)    _SWIZZLE3(A, B, C)
#define SWIZZLE4(A, B, C, D) _SWIZZLE4(A, B, C, D)

#define _SWIZZLE2(A, B)       tcolor2<T> A##B()       const { return tcolor2<T>(A, B); }
#define _SWIZZLE3(A, B, C)    tcolor3<T> A##B##C()    const { return tcolor3<T>(A, B, C); }
#define _SWIZZLE4(A, B, C, D) tcolor4<T> A##B##C##D() const { return tcolor4<T>(A, B, C, D); }

SWIZZLE2(r, r)
SWIZZLE2(r, g)
SWIZZLE2(r, b)

SWIZZLE2(g, r)
SWIZZLE2(g, g)
SWIZZLE2(g, b)

SWIZZLE2(b, r)
SWIZZLE2(b, g)
SWIZZLE2(b, b)

SWIZZLE3(r, r, r)
SWIZZLE3(r, r, g)
SWIZZLE3(r, r, b)
SWIZZLE3(r, g, r)
SWIZZLE3(r, g, g)
SWIZZLE3(r, g, b)
SWIZZLE3(r, b, r)
SWIZZLE3(r, b, g)
SWIZZLE3(r, b, b)

SWIZZLE3(g, r, r)
SWIZZLE3(g, r, g)
SWIZZLE3(g, r, b)
SWIZZLE3(g, g, r)
SWIZZLE3(g, g, g)
SWIZZLE3(g, g, b)
SWIZZLE3(g, b, r)
SWIZZLE3(g, b, g)
SWIZZLE3(g, b, b)

SWIZZLE3(b, r, r)
SWIZZLE3(b, r, g)
SWIZZLE3(b, r, b)
SWIZZLE3(b, g, r)
SWIZZLE3(b, g, g)
SWIZZLE3(b, g, b)
SWIZZLE3(b, b, r)
SWIZZLE3(b, b, g)
SWIZZLE3(b, b, b)

#undef SWIZZLE2
#undef SWIZZLE3
#undef SWIZZLE4
#undef _SWIZZLE2
#undef _SWIZZLE3
#undef _SWIZZLE4
