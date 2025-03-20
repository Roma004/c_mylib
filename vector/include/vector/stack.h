#pragma once
#include "vector.h"

#define Stack(type) Vector(type)

#define STACK_SIZE(st)    VECTOR_SIZE(st)
#define STACK_EL_SIZE(st) VECTOR_EL_SIZE(st)

#define STACK_INIT(st, _default_init, _destroy, _super) \
    VECTOR_INIT(st, _default_init, _destroy, _super)

#define STACK_ALLOC_SPACE(st, el_num) vector_resize(VEC(st), el_num)

#define STACK_TOP(st)              VECTOR_ACCESS_RIHGT(st, 0)
#define STACK_PUSH(status, st, el) VECTOR_PUSH_BACK(status, st, el)
#define STACK_POP(status, st) \
    { status = vector_remove(VEC(st), STACK_SIZE(st) - 1); }

#define STACK_FREE(st) vector_free(VEC(st))
