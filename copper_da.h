#ifndef COPPER_DA
#define COPPER_DA

#define INIT_CAP 128
#define da_append(arr, str) do {                                                \
    if((arr).count >= (arr).capacity){                                          \
        if((arr).capacity == 0) (arr).capacity = INIT_CAP;                      \
        else (arr).capacity *= 2;                                               \
        (arr).data = realloc((arr).data, (arr).capacity * sizeof(*(arr).data)); \
    }                                                                           \
    (arr).data[(arr).count++] = (str);                                          \
} while (0)
#define da_free(arr) do {           \
    free((arr).data);               \
    (arr).count = 0;                \
    (arr).capacity = 0;             \
} while (0)
#define da_pop(arr) do { (arr).count--; } while (0)
#define da_pop_all(arr) do { (arr).count = 0; } while (0)

#endif
