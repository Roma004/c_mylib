# C generic-type containers framework

Project provides an amount of standard containers which are generic-typed.

# Look and feel :)

Assume, you already have `c_mylib/*/include` directories in your include path.

## Generic Vector

```c
#inlcude "vector/vector.h"
#include "string.h"
#include "stdlib.h"
#include "assert.h"

#define ASSERT_OK assert(vstt == VEC_ok);

struct context {
    int a;
    char *b;
};

struct supercontext {
    const char *somestr;
    Vector(struct context) ctxs;
}

void context_init(struct supercontext *super, struct context *self) {
    self->a = 123;
    self->b = strdup(super->somestr);
}

void context_destroy(struct supercontext *super, struct context *self) {
    free(self->b);
}

int main(void) {
    // here you can declare Vector of any type you want
    enum VEC_STATUS vstt;
    struct supercontext ctx = {
        .somestr = "asdasd",
    };
    Vector(unsigned) vec_int;
    unsigned a, b, c;

    // There is a macro for soft initialization of such Vector.
    // You may provide a default constructor for element-type or a destructor
    // Also there is a way to provide a super object pointer, so that, you could
    // access it during construction or destruction of element
    VECTOR_INIT(&ctx.ctxs, context_init, context_destroy, &ctx);

    // or you may say, there is no need in them all.
    VECTOR_INIT(&vec_int, NULL, NULL, NULL);

    // here you can add some space right and default initialize it.
    VECTOR_ALLOC_BACK(&ctx.ctxs, 1);
    assert(VECTOR_ACCESS_RIGHT(&ctx.ctxs, 0)->a == 123);
    assert(strcmp(VECTOR_ACCESS_RIGHT(&ctx.ctxs, 0)->b, "asdasd") == 0);

    VECTOR_PUSH_BACK(vstt, &vec_int, 123); ASSERT_OK;
    VECTOR_PUSH_BACK(vstt, &vec_int, 555); ASSERT_OK;
    VECTOR_PUSH_BACK(vstt, &vec_int, 18);  ASSERT_OK;
    
    assert(*VECTOR_ACCESS(&vec_int, 0) == 123);
    assert(*VECTOR_ACCESS(&vec_int, 1) == 555);
    assert(*VECTOR_ACCESS(&vec_int, 2) == 18);
    assert(*VECTOR_ACCESS_RIGHT(&vec_int, 0) == 18);
    assert(*VECTOR_ACCESS_RIGHT(&vec_int, 1) == 555);
    assert(*VECTOR_ACCESS_RIGHT(&vec_int, 2) == 123);

    *VECTOR_ACCESS(&vec_int, 1) = 9;
    assert(*VECTOR_ACCESS(&vec_int, 1) == 9);

    assert(VECTOR_SIZE(&vec_int) == 3);

    return 0;
}

```

## Other containers:

In fact all the containers providen work similar to each other and have similar
inverfaces. Just look into the corresponding header file to explore API.

May-be one day there would be doxygen...

## List of containers:

* `Vector(T)`
* `Stack(T)`
* `Array(T)`
* `HashTable(Key, Val)` [may have bugs yet]
* `PrefixTree(Key, Val)`
* `StrictDigraph(NodeCt, EdgeCt)`

# IOUTIL module

    <here would be explanations may-be>
