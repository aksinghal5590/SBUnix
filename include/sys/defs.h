#ifndef _DEFS_H
#define _DEFS_H

#define NULL ((void*)0)

#define VIRTUAL_BASE 0xffffffff80000000
#define VIDEO_MEM_VIRTUAL  (VIRTUAL_BASE+0xb8000)
#define VIDEO_MEM 0xb8000
#define VIDEO_COL_VIRTUAL (VIRTUAL_BASE+0xb8001)
#define PAGEINDEX 512
#define PAGESIZE 4096
#define PAGE_COUNT 24000

#define ALL_ZERO 0x0
#define GET_40_BITS 0xFFFFFFF000

#define S_TOP 0xF00000000
#define S_SIZE 0x100000
#define H_BASE 0xF0000000
#define H_END  0x100000

typedef unsigned long  uint64_t;
typedef          long   int64_t;
typedef unsigned int   uint32_t;
typedef          int    int32_t;
typedef unsigned short uint16_t;
typedef          short  int16_t;
typedef unsigned char   uint8_t;
typedef          char    int8_t;

typedef uint64_t size_t;
typedef int64_t ssize_t;

typedef uint64_t off_t;

typedef uint32_t pid_t;

#endif
