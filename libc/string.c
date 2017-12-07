#include <string.h>
#include "sys/defs.h"

char* strcat(char* d, const char* s) {
        if(s == NULL || d == NULL)
                return NULL;
        char *ch = d;
        while(*ch != '\0') ch++;
        for(int i = 0; i < strlen(s); i++, ch++)
                *ch = s[i];
        *ch = '\0';
        return d;
}

int strcmp(const char *s, const char* t) {
    size_t a = strlen(s);
    size_t b = strlen(t);

    if(a > b) return 1;
    if(a < b) return -1;

    while(*s != '\0' && *t !='\0') {
      if(*s < *t) {
         return -1;
      } else if (*s > *t) {
         return 1;
      }
      s++;
      t++;
    }

    return 0;
}

char* strcpy(char* d, const char* s) {
        if(s == NULL || d == NULL)
                return NULL;
        int i;
        int len = strlen(s);
        for(i = 0; i < len; i++)
                d[i] = s[i];
        d[i] = '\0';
        return d;
}

size_t strlen(const char* s) {
   size_t len = 0;
   while(*s++ != '\0') {
      len++;
   }
   return len;
}
