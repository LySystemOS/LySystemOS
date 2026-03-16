#include <LySys/types.h>

void reverse(char* str, size_t len) {
    size_t i = 0;
    size_t j = len - 1;
    while (i < j) {
        char temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++;
        j--;
    }
}

char* itoa(uint64_t value, char* str, int base) {
    size_t i = 0;
    
    do {
        uint64_t remainder = value % base;
        str[i++] = (remainder < 10) ? (remainder + '0') : (remainder - 10 + 'A');
        value /= base;
    } while (value > 0);

    str[i] = '\0';
    
    reverse(str, i);
    
    return str;
}

int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

void* memcpy(void* dest, const void* src, size_t n) {
    uint8_t* d = (uint8_t*)dest;
    const uint8_t* s = (const uint8_t*)src;

    while (n--) {
        *d++ = *s++;
    }

    return dest;
}

size_t strlen(const char *str) {
    size_t len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}

void* memset(void* dest, int val, size_t n) {
    uint8_t* d = (uint8_t*)dest;
    while (n--) {
        *d++ = (uint8_t)val;
    }
    return dest;
}

char* strcpy(char* dest, const char* src) {
    char* ptr = dest;
    while (*src) {
        *ptr++ = *src++;
    }
    *ptr = '\0';
    return dest;
}

char* strncpy(char* dest, const char* src, size_t n) {
    size_t i;
    for (i = 0; i < n && src[i]; i++)
        dest[i] = src[i];
    for (; i < n; i++)
        dest[i] = '\0';
    return dest;
}

char* strrchr(const char* s, int c) {
    char* last = 0;
    while (*s != '\0') {
        if (*s == (char)c) {
            last = (char*)s;
        }
        s++;
    }
    if ((char)c == '\0') {
        return (char*)s;
    }
    return last;
}

char* strchr(const char* s, int c) {
    while (*s != (char)c) {
        if (*s == '\0') {
            return 0;
        }
        s++;
    }
    return (char*)s;
}

char* strtok(char* str, const char* delimiters) {
    static char* last_pos = 0;
    
    if (str != 0) {
        last_pos = str;
    } else if (last_pos == 0) {
        return 0;
    }

    char* start = last_pos;
    while (*start) {
        const char* d = delimiters;
        int is_delimiter = 0;
        while (*d) {
            if (*start == *d) {
                is_delimiter = 1;
                break;
            }
            d++;
        }
        if (!is_delimiter) break;
        start++;
    }

    if (*start == '\0') {
        last_pos = 0;
        return 0;
    }

    char* end = start;
    while (*end) {
        const char* d = delimiters;
        int is_delimiter = 0;
        while (*d) {
            if (*end == *d) {
                is_delimiter = 1;
                break;
            }
            d++;
        }
        if (is_delimiter) break;
        end++;
    }

    if (*end != '\0') {
        *end = '\0';
        last_pos = end + 1;
    } else {
        last_pos = 0;
    }

    return start;
}