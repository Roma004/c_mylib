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

struct context {
    int a;
    char *b;
};

void context_init(void *el) {
    struct context *ctx = el;
    ctx->a = 123;
    ctx->b = strdup("asdasd");
}

void context_destroy(void *el) {
    struct context *ctx = el;
    free(ctx->b);
}

int main(void) {
    // here you can declare Vector of any type you want
    Vector(struct context) vec_ctx;
    Vector(unsigned) vec_int;
    unsigned a, b, c;

    // There is a macro for soft initialization of such Vector.
    // You may provide a default constructor for element-type or a destructor
    VECTOR_INIT(&vec_ctx, context_init, context_destroy);

    // or you may say, there is no need in them.
    VECTOR_INIT(&vec_int, NULL, NULL);

    VECTOR_PUSH_BACK(&vec_int, 123);
    VECTOR_PUSH_BACK(&vec_int, 555);
    VECTOR_PUSH_BACK(&vec_int, 18);
    
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
* `HashTable(Key, Val)`

# IOUTIL module

    <here would be explanations may-be>
