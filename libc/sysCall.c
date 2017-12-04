#include <libc.h>

void syscall0(uint64_t sysNum) {

        __asm__ volatile
        (
                "movq %[sysNum], %%rax;"
                "int $0x80;"
                :
                : [sysNum]"a"(sysNum)
                : "cc","memory"
        );
}

void syscall1(uint64_t sysNum, void* arg1) {

        __asm__ volatile
        (
                "movq %[sysNum], %%rax;"
                "int $0x80;"
                :
                : [sysNum]"a"(sysNum), "D"(arg1)
                : "cc","rcx","memory"
        );
}

void syscall1_i(uint64_t sysNum, int arg1) {

        __asm__ volatile
        (
                "movq %[sysNum], %%rax;"
                "int $0x80;"
                :
                : [sysNum]"a"(sysNum), "D"(arg1)
                : "cc","rcx","memory"
        );
}

void syscall1_ip(uint64_t sysNum, int* arg1) {

        __asm__ volatile
        (
                "movq %[sysNum], %%rax;"
                "int $0x80;"
                :
                : [sysNum]"a"(sysNum), "D"(arg1)
                : "cc","rcx","memory"
        );
}

void syscall2(uint64_t sysNum, void* arg1, int arg2) {

        __asm__ volatile
        (
                "movq %[sysNum], %%rax;"
                "int $0x80;"
                :
                : [sysNum]"a"(sysNum), "D"(arg1), "S"(arg2)
                : "cc","rcx","r11","memory"
        );
}

void syscall2_ii(uint64_t sysNum, int arg1, int arg2) {

        __asm__ volatile
        (
                "movq %[sysNum], %%rax;"
                "int $0x80;"
                :
                : [sysNum]"a"(sysNum), "D"(arg1), "S"(arg2)
                : "cc","rcx","r11","memory"
        );
}

void syscall_ici(uint64_t sysNum, int arg1, char* arg2, int arg3) {

        __asm__ volatile
        (
                "movq %[sysNum], %%rax;"
                "int $0x80;"
                :
                : [sysNum]"a"(sysNum), "D"(arg1), "S"(arg2), "d"(arg3)
                : "cc","rcx","r11","memory"
        );
}

void syscall3(uint64_t sysNum, uint64_t arg1, uint64_t arg2, uint64_t arg3) {

        __asm__ volatile
        (
                "movq %[sysNum], %%rax;"
                "int $0x80;"
                :
                : [sysNum]"a"(sysNum), "D"(arg1), "S"(arg2), "d"(arg3)
                : "cc","rcx","r11","memory"
        );
}
