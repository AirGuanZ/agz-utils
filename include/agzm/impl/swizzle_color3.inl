#define SWIZZLE3(A, B, C)    _SWIZZLE3(A, B, C)
#define SWIZZLE4(A, B, C, D) _SWIZZLE4(A, B, C, D)

#define _SWIZZLE3(A, B, C)    tcolor3<T> A##B##C()    const { return tcolor3<T>(A, B, C); }
#define _SWIZZLE4(A, B, C, D) tcolor4<T> A##B##C##D() const { return tcolor4<T>(A, B, C, D); }

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

#undef SWIZZLE3
#undef SWIZZLE4
#undef _SWIZZLE3
#undef _SWIZZLE4
