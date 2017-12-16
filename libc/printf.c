#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>

#define MAXBUFLEN 2048

void parsefmt(char print[], const char *fmt, va_list val);

void convert(uint64_t a, char* ch, int base);

int printf(const char *format, ...) {
	va_list val;
	char print[MAXBUFLEN];

	va_start(val, format);
	parsefmt(print, format, val);
	int size = strlen(print);
	write(1, print, size);
	va_end(val);
	return size;
}

void parsefmt(char print[], const char *fmt, va_list val) {

     int printcnt = 0;
     char intarr[25];
     char *strarg;
     while(*fmt != '\0') {
         while(*fmt != '%' && *fmt != '\0') {
            print[printcnt++] = *fmt;
            fmt++;
         }
         if(*fmt == '\0' || *(fmt+1) == '\0') {
            break;
         }
         fmt++;
         char fmtch = *fmt;
         fmt++;
         switch(fmtch) {
            case 'c':
                print[printcnt++] = va_arg(val, int);
                break;
            case 'd':
                convert(va_arg(val, int), intarr, 10);
                strcpy(print + printcnt, intarr);
                printcnt += strlen(intarr);
                break;
            case 'x':
                convert(va_arg(val, uint64_t), intarr, 16);
                strcpy(print+printcnt, intarr);
                printcnt += strlen(intarr);
                break;
            case 's':
                strarg = va_arg(val, char *);
                strcpy(print + printcnt, strarg);
                printcnt += strlen(strarg);
                break;
            case 'p':
                convert(va_arg(val, uint64_t), intarr, 16);
                strcpy(print+printcnt, "0x");
                printcnt += 2;
                strcpy(print+printcnt, intarr);
                printcnt += strlen(intarr);
                break;
            default:
                break;
	}
     }
      print[printcnt] = '\0';

}

void convert(uint64_t a, char* ch, int base) {
        int i = 0;
        if(a == 0)
           ch[i++] = '0';
        else
        {
                while(a > 0)
                {
                        int rem = a%base;
                        if(rem > 9)
                                ch[i++] = (rem-10) + 'a';
                        else
                                ch[i++] = rem + '0';
                        a /= base;
                }
        }
        for(int j = 0; j <= (i-1)/2; j++)
        {
                if(j != (i-j-1))
                {
                  char c = ch[i-j-1];
                  ch[i-j-1] = ch[j];
                  ch[j] = c;
                }
        }
        ch[i] = '\0';
}
