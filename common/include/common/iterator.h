#pragma once

typedef struct iterator_base Iterator;

typedef const void *(*ref_mtd)(const Iterator *);
typedef void (*get_mtd)(const Iterator *, void *);
typedef void (*set_mtd)(Iterator *, const void *);
typedef void (*inc_mtd)(Iterator *);
typedef void (*dec_mtd)(Iterator *);
typedef int (*cmp_mtd)(const Iterator *, const Iterator *);
typedef void (*free_mtd)(Iterator *);
typedef void (*copy_mtd)(const Iterator *, Iterator *);

struct iterator_vtable {
    ref_mtd ref;
    get_mtd get;
    set_mtd set;
    inc_mtd inc;
    dec_mtd dec;
    cmp_mtd cmp;
    free_mtd free;
    copy_mtd copy;
};

#define ITERATOR_BASE struct iterator_vtable *__vtable;

struct iterator_base {
    ITERATOR_BASE;
};

#define ITER_VTABLE_INIT(it, methods) it->__vtable = methods;

#define ITER_INC(it) (((Iterator *)(it))->__vtable->inc((Iterator *)(it)))
#define ITER_DEC(it) (((Iterator *)(it))->__vtable->dec((Iterator *)(it)))

#define ITER_SET(it, src) \
    (((Iterator *)(it))->__vtable->set((Iterator *)(it), src))

#define ITER_GET(it, dst) \
    (((Iterator *)(it))->__vtable->get((Iterator *)(it), dst))

#define ITER_REF(it) (((Iterator *)(it))->__vtable->ref((Iterator *)(it)))

#define ITER_CMP(a, b) \
    (((Iterator *)(a))->__vtable->cmp((Iterator *)(a), (Iterator *)(b)))

#define ITER_COPY(it, dst) \
    (((Iterator *)(it))->__vtable->copy((Iterator *)(it), (Iterator *)(dst)))

#define ITER_FREE(it) (((Iterator *)(it))->__vtable->free((Iterator *)(it)))
