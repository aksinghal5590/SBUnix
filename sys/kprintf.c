#include <sys/kprintf.h>
#include<sys/stdarg.h>
#include<sys/string.h>
#include<sys/defs.h>

#define MAXBUFLEN 2048
#define MAXINTLEN 25

static char *memlo = (char *)0xb8000;
void kprintf(const char *fmt, ...)
{
     va_list val;
     va_start(val, fmt);
     char kstring[MAXBUFLEN];
     parsefmt(kstring, fmt, val);
     register char *temp1, *temp2;
     for(temp2 = (char*)0xb8001; temp2 < (char*)0xb8000+160*25; temp2 += 2) *temp2 = 7;
     for(temp1 = kstring;*temp1;temp1 += 1, memlo+=2) 
     { 
      if(memlo > (char*)0xb8000+160*24) {
         memshift();
      }
      if(*temp1 == '\n' || *temp1 == '\r') {
          memlo = (char *)0xb8000 + (((memlo - (char*)0xb8000)/160)+1)*160 - 2; 
          continue;
      }
      *memlo = *temp1;
     }     
     va_end(val);
}

void memshift() {
    register char *temp1, *temp2;
    for(temp1 = (char*)0xb8000; temp1 < (char*)0xb8000+160*23; temp1 += 1) {
         *(temp1) = *((char*)(temp1 + 160));
    }
    for(temp2 = (char*)temp1; temp2 < (char*)0xb8000+160*24; temp2+=1) {
         *(temp2) = 0;
    }
    memlo = (char*)(0xb8000+160*23);
}

void printtimer(uint64_t seconds) {
    char intarr[MAXINTLEN];
    register char *temp1,*temp2;
    convert(seconds, intarr, 10);
     //for(temp2 = (char*)0xb8001; temp2 < (char*)0xb8000+160*25; temp2 += 2) *temp2 = 7;
    for(temp1 = (char*)0xb8001+160*24; temp1 < (char*)0xb8000+160*25; temp1 += 2) *temp1 = 7;
    for(temp1 = (char*)0xb8000+160*24, temp2 = (char*)intarr; *temp2 && temp1 < (char*)0xb8000+160*25; temp1 += 2,temp2 += 1) {
         *(temp1) = *(temp2);
    }
}

void parsefmt(char print[], const char *fmt, va_list val)
{

     //char print[MAXBUFLEN];
     int printcnt = 0;
     char intarr[MAXINTLEN];
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
                print[printcnt] = va_arg(val, int);
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


void convert(uint64_t a, char* ch, int base)
{
	int i = 0;
	while(a > 0)
	{
		int rem = a%base;
		if(rem > 9)
			ch[i++] = (rem-10) + 'a';
		else
			ch[i++] = rem + '0';
		a /= base;
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
