#ifndef MSTRING_H
#define MSTRING_H

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

#define MSTRING_IS_TRIM(c) (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\b' || c == '\f')

typedef struct __mstring {
    char* _data;
    size_t _length;
} _mstring, *mstring;

mstring mstring_alloc();
mstring mstring_new(const char* data);
mstring mstring_clone(mstring str);

void mstring_free(mstring str);

const size_t MSTRING_NPOS = (size_t)(-1);

const char* mstring_raw(mstring str);
size_t mstring_length(mstring str);

bool mstring_empty(mstring str);
bool mstring_contains(mstring str, const char* what);

mstring mstring_substr(mstring str, size_t start, size_t length);
char mstring_at(mstring str, size_t i);

void mstring_clear(mstring str);
void mstring_erase(mstring str, size_t start, size_t end);

size_t mstring_first_index(mstring str, char c);
size_t mstring_last_index(mstring str, char c);

bool mstring_ends_with(mstring str, const char* end);
bool mstring_starts_with(mstring str, const char* end);

mstring mstring_upper(mstring str);
mstring mstring_lower(mstring str);

mstring mstring_replace(mstring str, const char* what, const char* to);
mstring mstring_remove(mstring str, const char* what);

mstring mstring_join(mstring str1, mstring str2);

mstring mstring_trim_left(mstring str);
mstring mstring_trim_right(mstring str);
mstring mstring_trim(mstring str);

void* mstring_convert(mstring str, const char* pattern);

#ifdef MSTRING_IMPL
#undef MSTRING_IMPL

mstring mstring_alloc() {
    return (mstring)malloc(sizeof(_mstring));
}

mstring mstring_new(const char* data) {
    mstring str = mstring_alloc();

    str->_length = strlen(data);
    str->_data = (char*)malloc(str->_length + 1);

    memcpy(str->_data, data, str->_length + 1);

    return str;
}

mstring mstring_clone(mstring str) {
    return mstring_new((const char*)str->_data);
}

void mstring_free(mstring str) {
    free(str->_data);
    free(str);
    str = NULL;
}

const char* mstring_raw(mstring str) {
    return str->_data;
}

size_t mstring_length(mstring str) {
    return str->_length;
}

bool mstring_empty(mstring str) {
    return str->_length == 0;
}

bool mstring_contains(mstring str, const char* what) {
    return strstr(str->_data, what) != NULL;
}

mstring mstring_substr(mstring str, size_t start, size_t length) {
    mstring new_str = (mstring)malloc(sizeof(_mstring));

    new_str->_data = (char*)malloc(length + 1);
    new_str->_length = length;

    strncpy(new_str->_data, str->_data + start, length);
    new_str->_data[new_str->_length] = '\0';

    return new_str;
}

char mstring_at(mstring str, size_t i) {
    return str->_data[i];
}

void mstring_clear(mstring str) {
    free(str->_data);
    str->_length = 0;
}

void mstring_erase(mstring str, size_t start, size_t end) {
    size_t diff = end - start + 1;

    strncpy(str->_data + start, str->_data + end + 1, str->_length - end);

    str->_length -= diff;
    str->_data = realloc(str->_data, str->_length + 1);
    str->_data[str->_length] = '\0';
}

size_t mstring_first_index(mstring str, char c) {
    for (size_t i = 0; i < str->_length; i++) {
        if (str->_data[i] == c)
            return i;
    }

    return MSTRING_NPOS;
}

size_t mstring_last_index(mstring str, char c) {
    for (size_t i = str->_length - 1; i != 0; i--) {
        if (str->_data[i] == c)
            return i;
    }

    return MSTRING_NPOS;
}

bool mstring_ends_with(mstring str, const char* end) {
    size_t end_len = strlen(end);

    for (size_t i = 0; i < end_len; i++) {
        if (str->_data[str->_length - i - 1] != end[end_len - i - 1])
            return false;
    }

    return true;
}

bool mstring_starts_with(mstring str, const char* end) {
    size_t end_len = strlen(end);

    for (size_t i = 0; i < end_len; i++) {
        if (str->_data[i] != end[i])
            return false;
    }

    return true;
}

mstring mstring_upper(mstring str) {
    mstring new_str = mstring_clone(str);
    for (size_t i = 0; i < new_str->_length; i++)
        new_str->_data[i] = (char)toupper((char)new_str->_data[i]);

    return new_str;
}

mstring mstring_lower(mstring str) {
    mstring new_str = mstring_clone(str);
    for (size_t i = 0; i < new_str->_length; i++)
        new_str->_data[i] = (char)tolower((char)new_str->_data[i]);

    return new_str;
}

int mstring_compare(mstring str1, mstring str2) {
    return strcmp((const char*)str1->_data, (const char*)str2->_data);
}

bool mstring_equals(mstring str1, mstring str2) {
    return mstring_compare(str1, str2) == 0;
}

mstring mstring_replace(mstring str, const char* what, const char* to) {
    size_t len_what = strlen(what);
    size_t len_to = strlen(to);

    size_t count = 0;

    char* insert_pos = str->_data;
    char* temp = NULL;

    while ((temp = strstr(insert_pos, what)) != NULL) {
        insert_pos = temp + len_what;
        count++;
    }

    mstring new_str = mstring_alloc();
    temp = new_str->_data = malloc(str->_length + (len_to - len_what) * count + 1);

    while (count--) {
        insert_pos = strstr(str->_data, what);
        size_t len_front = insert_pos - str->_data;
        temp = strncpy(temp, str->_data, len_front) + len_front;
        temp = strcpy(temp, to) + len_to;
        str->_data += len_front + len_what;
    }

    strcpy(temp, str->_data);
    return new_str;
}

mstring mstring_remove(mstring str, const char* what) {
    mstring new_str = mstring_clone(str);

    char *p, *q, *r;

    if (*what && (q = r = strstr(new_str->_data, what)) != NULL) {
        size_t len = strlen(what);

        while ((r = strstr(p = r + len, what)) != NULL) {
            while (p < r)
                *q++ = *p++;
        }

        while ((*q++ = *p++) != '\0')
            continue;
    }

    return new_str;
}

mstring mstring_join(mstring str1, mstring str2) {
    size_t length = str1->_length + str2->_length;

    char* output = (char*)malloc(length + 1);
    memset(output, 0, length + 1);

    strcat(output, str1->_data);
    strcat(output, str2->_data);

    mstring new_str = mstring_alloc();
    new_str->_data = output;
    new_str->_length = length;

    return new_str;
}

mstring mstring_trim_left(mstring str) {
    size_t new_start = 0;

    while (new_start < str->_length && MSTRING_IS_TRIM(str->_data[new_start]))
        new_start++;

    return mstring_substr(str, new_start, str->_length - new_start);
}

mstring mstring_trim_right(mstring str) {
    size_t new_length = str->_length - 1;

    while (new_length > 0 && MSTRING_IS_TRIM(str->_data[new_length]))
        new_length--;

    return mstring_substr(str, 0, new_length + 1);
}

mstring mstring_trim(mstring str) {
    size_t new_start = 0;
    size_t new_end = str->_length - 1;

    while (new_start < str->_length && MSTRING_IS_TRIM(str->_data[new_start]))
        new_start++;

    while (new_end > 0 && MSTRING_IS_TRIM(str->_data[new_end]))
        new_end--;

    return mstring_substr(str, new_start, new_end - new_start + 1);
}

void* mstring_convert(mstring str, const char* pattern) {
    void* value;
    sscanf(str->_data, pattern, value);
    return value;
}

#endif

#endif
