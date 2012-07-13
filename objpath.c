#include <stdint.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <stdlib.h>

#include "objpath.h"

enum {
    NODE_UNKNOWN,
    NODE_KEY,
    NODE_INDEX,
    NODE_KEYS,
    NODE_VALUES,
    NODE_SLICE,
    NODE_ELEMENTS,
    NODE_SUBMIT
};

typedef struct node {
    int type;
    union {
        char str[1];
        size_t index;
        struct {
            size_t start;
            size_t end;
        } slice;
    } data;
    struct node *next;
} node_t;

typedef struct pattern {
    node_t *head;
} pattern_t;

typedef struct context {
    pattern_t *pattern;
    node_t *cur;
} context_t;


void *objpath_compile(char *data) {
    pattern_t *pat = malloc(sizeof(pattern_t));
    assert(pat);
    node_t *prev = NULL;
    char *c = data;
    while(*c) {
        node_t *next = NULL;
        while(isspace(*c)) ++c;
        switch(*c) {
        case '/': {
            ++ c;
            char *key = c;
            while(isalnum(*c) || *c == '-' || *c == '_') ++c;
            if(*c == '*') {
                while(isalnum(*c) || *c == '*') ++c;
                next = malloc(sizeof(node_t) + (c - key + 1));
                assert(next);
                next->type = NODE_VALUES;
                memcpy(next->data.str, key, c - key);
                next->data.str[c - key] = 0;
            } else if(*c == '%') {
                while(isalnum(*c) || *c == '%') ++c;
                next = malloc(sizeof(node_t) + (c - key + 1));
                assert(next);
                next->type = NODE_KEYS;
                memcpy(next->data.str, key, c - key);
                next->data.str[c - key] = 0;
            } else {
                next = malloc(sizeof(node_t) + (c - key + 1));
                assert(next);
                next->type = NODE_KEY;
                memcpy(next->data.str, key, c - key);
                next->data.str[c - key] = 0;
            }
            } break;
        case ':': {
            ++ c;
            char *key = c;
            while(isdigit(*c)) ++c;
            if(*c == '-') {
                next = malloc(sizeof(node_t));
                assert(next);
                next->type = NODE_SLICE;
                next->data.slice.start = atoi(key);
                next->data.slice.end = atoi(c);
                while(isdigit(*c)) ++c;
            } else {
                next = malloc(sizeof(node_t));
                assert(next);
                next->type = NODE_INDEX;
                next->data.index = atoi(key);
            }
            } break;
        case '[':
            assert (0 /* not implemented */);
            break;
        case '?':
            assert (0 /* not implemented */);
            break;
        default:
            return NULL;
        }
        if(next) {
            if(prev) {
                prev->next = next;
            } else {
                pat->head = next;
            }
            prev = next;
        } else {
            assert (0);
        }
    }
    node_t *final = malloc(sizeof(node_t));
    assert(final);
    final->type = NODE_SUBMIT;
    final->next = NULL;
    if(prev) {
        prev->next = final;
    } else {
        pat->head = final;
    }
    return pat;
}

void *objpath_start(void *pattern) {
    context_t *ctx = malloc(sizeof(context_t));
    ctx->pattern = pattern;
    ctx->cur = ctx->pattern->head;
    return ctx;
}

int objpath_next(void *context, int *opcode, objpath_value_t *val,
    void **object, void **iterator) {
    context_t *ctx = context;

    if(!*object)  // No backtracking yet
        return 0;
    if(!ctx->cur)  // No backtracking yet
        return 0;

    switch(ctx->cur->type) {
    case NODE_KEY:
        *opcode = OBJPATH_KEY;
        val->string = ctx->cur->data.str;
        break;
    case NODE_INDEX:
        *opcode = OBJPATH_INDEX;
        val->index = ctx->cur->data.index;
        break;
    case NODE_SUBMIT:
        *opcode = OBJPATH_FINAL;
        break;
    default:
        fprintf(stderr, "Wrong node %d\n", ctx->cur->type);
        assert(0);
    }
    ctx->cur = ctx->cur->next;
    return 1;
}

void objpath_free(void *context) {
    free(context);
}
