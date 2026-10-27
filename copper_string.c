// Known issue: begin pointer might dangle
// Possible solution: NULL?
#ifndef COPPER_STRING_H
#define COPPER_STRING_H
#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <limits.h>
#ifndef NO_COPPER_DA
#include "copper_da.h"
#endif
typedef struct String_Slice String_Slice;
size_t cstring_len(char *cstring);
String_Slice string_slice_init(char *ptr, size_t len);
String_Slice string_slice_from_cstring(char *cstring);

int cstring_compare_exact(char *cstr1, char *cstr2);
int string_slice_compare_exact(String_Slice *s1, String_Slice *s2);
int string_slice_compare_cstring_exact(String_Slice *str, char *cstr);
int cstring_compare_strict(char *cstr1, char *cstr2);
int string_slice_compare_strict(String_Slice *s1, String_Slice *s2);
int string_slice_compare_cstring_strict(String_Slice *str, char *cstr);
int cstring_compare_fuzzy(char *cstr1, char *cstr2);
int string_slice_compare_fuzzy(String_Slice *s1, String_Slice *s2);
int string_slice_compare_cstring_fuzzy(String_Slice *str, char *cstr);


#ifdef COPPER_STRING_SLICER
typedef struct String_Slices String_Slices;
String_Slices string_slice_split_strict(String_Slice *source, String_Slice *delimiters);
String_Slices string_slice_split_strict_cstring(String_Slice *source, char *delimiters);
String_Slices string_slice_split(String_Slice *source, String_Slice *delimiters);
String_Slices string_slice_split_cstring(String_Slice *source, char *delimiters);
#endif
#ifdef COPPER_STRING_FACTORY
typedef struct String_Factory String_Factory;
void string_factory_append_cstring(String_Factory *builder, char *cstr);
void string_factory_read_from_file(String_Factory *builder, const char *path);
void string_factory_write_to_file(String_Factory *builder, const char *path);
#endif
#if defined(COPPER_STRING_FACTORY) && defined(COPPER_STRING_SLICER)
String_Slices string_factory_split_strict(String_Factory *builder, String_Slice *delimiters);
String_Slices string_factory_split_strict_cstring(String_Factory *builder, char *delimiters);
String_Slices string_factory_split(String_Factory *builder, String_Slice *delimiters);
String_Slices string_factory_split_cstring(String_Factory *builder, char *delimiters);
#endif

#endif

#ifdef COPPER_STRING_IMPLEMENTATION
struct String_Slice {
    char *begin;
    size_t len;
};
#define string_slice_print(str) do {\
    if((str)->len <= INT_MAX) printf("%.*s", (int)(str)->len, (str)->begin);\
    if((str)->len > INT_MAX) fprintf(stderr, "String slice is too big!\n");\
} while(0)

size_t cstring_len(char *cstring){
    size_t result = 0;
    while(cstring[result] != '\0') result++;
    return result;
}

String_Slice string_slice_init(char *ptr, size_t len){
    return (String_Slice){
        .begin = ptr,
        .len = len
    };
}

String_Slice string_slice_from_cstring(char *cstring){
    return (String_Slice){
        .begin = cstring,
        .len = cstring_len(cstring)
    };
}

int cstring_compare_exact(char *cstr1, char *cstr2){
    size_t len = 0, index = 0, result = 0;
    if(cstring_len(cstr1) < cstring_len(cstr2)) len = cstring_len(cstr2);
    if(cstring_len(cstr1) >= cstring_len(cstr2)) len = cstring_len(cstr1);
    while(index < len){
        if(cstr1[index] != cstr2[index]) result++;
        index++;
    }
    return (int)result;
}

int string_slice_compare_exact(String_Slice *s1, String_Slice *s2){
    size_t len = 0, index = 0, result = 0;
    if(s1->len < s2->len) len = s2->len;
    if(s1->len >= s2->len) len = s1->len;
    while(index < len){
        if(s1->begin[index] != s2->begin[index]) result++;
        index++;
    }
    return (int)result;
}

int string_slice_compare_cstring_exact(String_Slice *str, char *cstr){
    size_t len = 0, index = 0, result = 0;
    if(str->len < cstring_len(cstr)) len = cstring_len(cstr);
    if(str->len >= cstring_len(cstr)) len = str->len;
    while(index < len){
        if(str->begin[index] != cstr[index]) result++;
        index++;
    }
    return (int)result;
}

int cstring_compare_strict(char *cstr1, char *cstr2){
    if(cstring_len(cstr1) < cstring_len(cstr2)) return -1;
    size_t sub_index = 0, dupes = 0;
    for(size_t i = 0; i < cstring_len(cstr1); i++){
        if(sub_index == cstring_len(cstr2)){
            dupes++;
            sub_index = 0;
        }
        if(cstr1[i] != cstr2[i]){
            sub_index = 0;
        }
        sub_index++;
    }
    return dupes;
}

int string_slice_compare_strict(String_Slice *s1, String_Slice *s2){
    if(s1->len < s2->len) return -1;
    size_t sub_index = 0, dupes = 0;
    for(size_t i = 0; i < s1->len; i++){
        if(sub_index == s2->len){
            dupes++;
            sub_index = 0;
        }
        if(s1->begin[i] != s2->begin[i]){
            sub_index = 0;
        }
        sub_index++;
    }
    return dupes;
}

int string_slice_compare_cstring_strict(String_Slice *str, char *cstr){
    if(str->len < cstring_len(cstr)) return -1;
    size_t sub_index = 0, dupes = 0;
    for(size_t i = 0; i < str->len; i++){
        if(str->begin[i] != cstr[sub_index]){
            sub_index = 0;
        }
        sub_index++;
        if(sub_index == cstring_len(cstr)){
            dupes++;
            sub_index = 0;
        }
    }
    return dupes;
}

int cstring_compare_fuzzy(char *cstr1, char *cstr2){
    if(cstring_len(cstr1) < cstring_len(cstr2)) return -1;
    size_t sub_index = 0;
    for(size_t i = 0; i < cstring_len(cstr1); i++){
        for(size_t j = 0; j < cstring_len(cstr2); j++){
            if(cstr1[i] == cstr2[j]){
                sub_index++;
                break;
            }
        }
    }
    return sub_index;
}

int string_slice_compare_fuzzy(String_Slice *s1, String_Slice *s2){
    if(s1->len < s2->len) return -1;
    size_t sub_index = 0;
    for(size_t i = 0; i < s1->len; i++){
        for(size_t j = 0; j < s2->len; j++){
            if(s1->begin[i] == s2->begin[j]){
                sub_index++;
                break;
            }
        }
    }
    return sub_index;
}

int string_slice_compare_cstring_fuzzy(String_Slice *str, char *cstr){
    if(str->len < cstring_len(cstr)) return -1;
    size_t sub_index = 0;
    for(size_t i = 0; i < str->len; i++){
        for(size_t j = 0; j < cstring_len(cstr); j++){
            if(str->begin[i] == cstr[j]){
                sub_index++;
                break;
            }
        }
    }
    return sub_index;
}

#ifdef COPPER_STRING_FACTORY
struct String_Factory {
    char *data;
    size_t count;
    size_t capacity;
};
#ifndef NO_COPPER_DA
#define string_factory_append(str, ch) da_append((str), (ch))
#define string_factory_free(str) da_free((str))
#define string_factory_pop(str, ch) da_pop((str), (ch))
#define string_factory_pop_all(str, ch) da_pop_all((str), (ch))
#endif
void string_factory_append_cstring(String_Factory *builder, char *cstr){
    for(size_t i = 0; i < cstring_len(cstr); i++){
        string_factory_append(*builder, cstr[i]);
    }
    string_factory_append(*builder, '\0');
}

void string_factory_read_from_file(String_Factory *builder, const char *path){
    FILE *fp = fopen(path, "rb");
    if(fp == NULL){
        fprintf(stderr, "Cannot read file! Path: %s\n", path);
        exit(-1);
    }

    int character = fgetc(fp);
    while(character != EOF){
        string_factory_append(*builder, (char)character);
        character = fgetc(fp);
    }
    string_factory_append(*builder, '\0');
    fclose(fp);
}

void string_factory_write_to_file(String_Factory *builder, const char *path){
    FILE *fp = fopen(path, "wb");
    if(fp == NULL){
        fprintf(stderr, "Cannot read file! Path: %s\n", path);
        exit(-1);
    }
    fwrite(builder->data, builder->count * sizeof(*builder->data), 1, fp);
    fclose(fp);
}
#endif

#ifdef COPPER_STRING_SLICER
struct String_Slices {
    String_Slice *data;
    size_t count;
    size_t capacity;
};
#ifndef NO_COPPER_DA
#define string_slices_append(strs, str) da_append((strs), (str))
#define string_slices_free(strs) da_free((strs))
#define string_slices_pop(strs, str) da_pop((strs), (str))
#define string_slices_pop_all(strs, str) da_pop_all((strs), (str))
#endif

String_Slices __buffer_split_strict(char *str1, size_t len1, char *str2, size_t len2){
    String_Slices result = {0};
    String_Slice temp = {
        .begin = str1,
        .len = 0
    };

    unsigned char found = 0;
    size_t index = 0;
    //size_t offset = 0;
    while(index < len1){
        found = 0;
        for(size_t j = 0; j < len2; j++){
            if(temp.begin[index] != str2[j]){
                break;
            }
            if(j == len2 - 1){
                found = 1;
            }
        }
        if(found == 1){
            temp.len = index - (len2 - 1);
            da_append(result, temp);
            temp.begin = temp.begin + index + 1;
            temp.len = 0;
            len1 -= index;
            index = 0;
        }
        index++;
    }
    temp.len = index;
    da_append(result, temp);

    return result;
}   //Lengths have excluded \0 at the end of buffers

String_Slices __buffer_split(char *str1, size_t len1, char *str2, size_t len2){
    String_Slices result = {0};
    String_Slice temp = {
        .begin = str1,
        .len = 0
    };

    size_t offset = 0;
    size_t is_valid = 0;
    for(size_t i = 0; i < len1; i++){
        is_valid = 1;
        for(size_t j = 0; j < len2; j++){
            if (str1[i] == str2[j]) is_valid = 0;
        }

        if(is_valid == 0){
            offset++;
        }

        if(is_valid == 1){
            if(offset > 0){
                da_append(result, temp);
                temp.begin = str1 + i;
                temp.len = 0;
                offset = 0;
            }
            temp.len++;
        }
    }
    da_append(result, temp);
    
    return result;
}

String_Slices string_slice_split_strict(String_Slice *source, String_Slice *delimiters){
    return __buffer_split_strict(source->begin, source->len, delimiters->begin, delimiters->len);
}

String_Slices string_slice_split_strict_cstring(String_Slice *source, char *delimiters){
    return __buffer_split_strict(source->begin, source->len, delimiters, cstring_len(delimiters));
}

String_Slices string_slice_split(String_Slice *source, String_Slice *delimiters){
    return __buffer_split(source->begin, source->len, delimiters->begin, delimiters->len);
}

String_Slices string_slice_split_cstring(String_Slice *source, char *delimiters){
    return __buffer_split(source->begin, source->len, delimiters, cstring_len(delimiters));
}

#endif

#if defined(COPPER_STRING_FACTORY) && defined(COPPER_STRING_SLICER)

String_Slices string_factory_split_strict(String_Factory *builder, String_Slice *delimiters){
    return __buffer_split_strict(builder->data, cstring_len(builder->data), delimiters->begin, delimiters->len);
}

String_Slices string_factory_split_strict_cstring(String_Factory *builder, char *delimiters){
    return __buffer_split_strict(builder->data, cstring_len(builder->data), delimiters, cstring_len(delimiters));
}

String_Slices string_factory_split(String_Factory *builder, String_Slice *delimiters){
    return __buffer_split(builder->data, cstring_len(builder->data), delimiters->begin, delimiters->len);
}

String_Slices string_factory_split_cstring(String_Factory *builder, char *delimiters){
    return __buffer_split(builder->data, cstring_len(builder->data), delimiters, cstring_len(delimiters));
}

#endif
#endif
