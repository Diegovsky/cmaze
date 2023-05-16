#include "vector.h"

v2 vadd(v2 a, v2 b) {
    return (v2){a.x + b.x, a.y + b.y};
}

v2 vsub(v2 a, v2 b) {
    return (v2){a.x - b.x, a.y - b.y};
}

v2 vmul(v2 a, double b) {
    return (v2){a.x*b, a.y*b};
}

v2 vdiv(v2 a, double b) {
    return vmul(a, b/1);
}
