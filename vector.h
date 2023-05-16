#pragma once

#define printv2(V) ({v2 _V = V; printf(#V": (%d , %d)\n", _V.x, _V.y); _V;})
#define vsplat(x) ((v2){x, x})

typedef struct {
    int x, y;
} v2;

v2 vadd(v2 a, v2 b);
v2 vsub(v2 a, v2 b);
v2 vmul(v2 a, double b);
v2 vdiv(v2 a, double b);
