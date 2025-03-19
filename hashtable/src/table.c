#include "common/iterator.h"
#include "hashtable/hashtable-inners.h"
#include "hashtable/hashtable.h"
#include "vector/vector.h"

void destroy_pair(const void *super, void *self) {
    const HashTable *tb = super;
    TB_NODE_KEY_DESTROY(tb, self);
    TB_NODE_VAL_DESTROY(tb, self);
}

enum TB_STATUS hashtable_init(
    HashTable *tb, size_t key_size, size_t val_size, struct pair_manage *mtds
) {
    enum TB_STATUS stt = TB_ok;
    struct el_manage vec_mtds = {
        .destroy = &destroy_pair,
        .super = tb,
    };

    *tb = (struct hashtable){
        .key_size = key_size,
        .val_size = val_size,
        .mtds = *mtds,
        .nnodes = 0,
    };

    vector_init(VEC(&tb->data), NODE_SIZE(tb), &vec_mtds);
    if (vector_alloc_back(VEC(&tb->data), 11)) return TB_memory_error;
    reset_data(tb);

    return TB_ok;
}

void hashtable_free(HashTable *tb) { vector_free(VEC(&tb->data)); }

void hashtable_begin(const HashTable *tb, HashTableIterator *it) {
    if (!it) return;
    if (*TB_NODE_BUSY_PTR(tb, 0) != BUSY)
        hashtable_iter_init(tb, it, next_idx(tb, 0));
    else hashtable_iter_init(tb, it, 0);
}

void hashtable_end(const HashTable *tb, HashTableIterator *it) {
    if (!it) return;
    hashtable_iter_init(tb, it, next_idx(tb, TB_CAPACITY(tb)));
}

enum TB_STATUS
hashtable_find(const HashTable *tb, const void *key, HashTableIterator *pos) {
    if (tb->nnodes == 0) {
        hashtable_end(tb, pos);
        return TB_key_error;
    }

    for (uint32_t i = hash(tb, key), j = 0; j < TB_CAPACITY(tb);
         i = (i + STEP) % TB_CAPACITY(tb), j++) {
        if (*TB_NODE_BUSY_PTR(tb, i) != BUSY) {
            hashtable_end(tb, pos);
            return TB_key_error;
        }
        if (TB_KEY_CMP(tb, key, TB_NODE_KEY_PTR(tb, i))) {
            hashtable_iter_init(tb, pos, i);
            return TB_ok;
        }
    }

    hashtable_end(tb, pos);
    return TB_key_error;
}

enum TB_STATUS hashtable_insert(
    HashTable *tb, const void *key, const void *val, HashTableIterator *ins_pos
) {
    if (hashtable_find(tb, key, NULL) == TB_ok) {
        hashtable_end(tb, ins_pos);
        return TB_key_error;
    }

    if ((double)tb->nnodes / TB_CAPACITY(tb) > MAX_FILL_FACTOR)
        table_expand(tb);

    uint32_t i = hash(tb, key);
    for (uint32_t j = 0; j < TB_CAPACITY(tb);
         i = (i + STEP) % TB_CAPACITY(tb), j++) {
        if (*TB_NODE_BUSY_PTR(tb, i) != BUSY) break;
    }
    emplace_node(tb, i, key, val);

    hashtable_iter_init(tb, ins_pos, i);
    return TB_ok;
}

enum TB_STATUS
hashtable_erase(HashTable *tb, const void *key, HashTableIterator *next) {
    HashTableIterator it;
    if (hashtable_find(tb, key, &it) == TB_key_error) return TB_key_error;

    if ((double)(tb->nnodes - 1) / TB_CAPACITY(tb) <= MIN_FILL_FACTOR) {
        table_shrink(tb);
        hashtable_find(tb, key, &it);
    }

    if (next) {
        *next = it;
        ITER_INC(next);
    }

    *TB_NODE_BUSY_PTR(tb, it.cur_pos) = DEL;
    tb->nnodes--;

    return TB_ok;
}

void hashtable_erase_by_iter(HashTableIterator *it) {
    *TB_NODE_BUSY_PTR(it->tb, it->cur_pos) = DEL;
    it->tb->nnodes--;
}
