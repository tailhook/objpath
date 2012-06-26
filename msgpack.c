#include <assert.h>
#include <unistd.h>

#include "msgpack.h"
#include "objpath.h"


int read_more(msgpack_unpacker *pac) {
    msgpack_unpacker_reserve_buffer(pac, 16384);
    int res = read(0, msgpack_unpacker_buffer(pac), 16384);
    if(res > 0) {
        msgpack_unpacker_buffer_consumed(pac, res);
        return 1;
    }
    return 0;
}

void match(msgpack_object *obj, void *pattern) {
    void *ctx = objpath_start(pattern);
    msgpack_object *cur = obj;
    struct {
        msgpack_object_kv *kv;
        msgpack_object *el;
    } iter;
    objpath_value_t val;
    int opcode;
    while(objpath_next(ctx, &opcode, &val, (void **)&cur, (void **)&iter)) {
        switch(opcode) {
        case OBJPATH_KEY:
            if(cur->type != MSGPACK_OBJECT_MAP)
                goto fail;
            int klen = strlen(val.string);
			msgpack_object_kv* p = cur->via.map.ptr;
            msgpack_object_kv* const pend = cur->via.map.ptr + cur->via.map.size;
			for(; p < pend; ++p) {
                if(p->key.type == MSGPACK_OBJECT_RAW
                   && p->key.via.raw.size == klen
                   && !strncmp(p->key.via.raw.ptr, val.string, klen)) {
                    cur = &p->val;
                    goto success;
                }
            }
            goto fail;
        case OBJPATH_INDEX:
            if(cur->type != MSGPACK_OBJECT_ARRAY)
                goto fail;
            if(val.index < 0 || cur->via.array.size < val.index)
                goto fail;
            cur = &cur->via.array.ptr[val.index];
            goto success;

        case OBJPATH_KEYS:
            if(cur->type != MSGPACK_OBJECT_MAP)
                goto fail;
            iter.kv = cur->via.map.ptr;
        case OBJPATH_NEXTKEY:
            if(iter.kv >= cur->via.map.ptr + cur->via.map.size)
                goto fail;
            cur = &iter.kv->key;
            goto success;

        case OBJPATH_VALUES:
            if(cur->type != MSGPACK_OBJECT_MAP)
                goto fail;
            iter.kv = cur->via.map.ptr;
        case OBJPATH_NEXTVALUE:
            if(iter.kv >= cur->via.map.ptr + cur->via.map.size)
                goto fail;
            cur = &iter.kv->val;
            goto success;

        case OBJPATH_ELEMENTS:
            if(cur->type != MSGPACK_OBJECT_ARRAY)
                goto fail;
            iter.el = cur->via.array.ptr;
        case OBJPATH_NEXTELEMENT:
            if(iter.el >= cur->via.array.ptr + cur->via.array.size)
                goto fail;
            cur = iter.el;
            goto success;

        case OBJPATH_FINAL:
            msgpack_object_print(stdout, *cur);
            puts("");
            goto success;
        }
        success:
            continue;
        fail:
            cur = NULL;
            continue;
    }
    objpath_free(ctx);
}

int main(int argc, char **argv) {
    void *pattern = objpath_compile(argv[1]);
    msgpack_unpacker pac;
    msgpack_unpacker_init(&pac, MSGPACK_UNPACKER_INIT_BUFFER_SIZE);

    msgpack_unpacked result;
    msgpack_unpacked_init(&result);
    while(1) {
        int rc = read_more(&pac);
        if(!rc) break;
        while(msgpack_unpacker_next(&pac, &result)) {
            match(&result.data, pattern);
        }
    }
    return 0;
}

