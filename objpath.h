#ifndef H_OBJPATH_INC
#define H_OBJPATH_INC

#define OBJPATH_KEY             1
#define OBJPATH_INDEX           2
#define OBJPATH_KEYS            3
#define OBJPATH_NEXTKEY         4
#define OBJPATH_VALUES          5
#define OBJPATH_NEXTVALUE       6
#define OBJPATH_ELEMENTS        7
#define OBJPATH_NEXTELEMENT     8
#define OBJPATH_FINAL           9

typedef union objpath_value_u {
    long index;
    char *string;
} objpath_value_t;

void *objpath_compile(char *data);

void *objpath_start(void *pattern);
int objpath_next(void *context, int *opcode, objpath_value_t *val,
    void **object, void **iterator);
void objpath_free(void *context);


#endif //  H_OBJPATH_INC
