#include <libc.h>

uint64_t syscall0(uint64_t sysNum) {
        uint64_t ret;
        __asm__ volatile
        (
                "movq %[sysNum], %%rcx;"
                "int $0x80;"
                : "=a" (ret)
                : [sysNum]"a"(sysNum)
                : "cc","memory"
        );

        return ret;
}

uint64_t syscall1(uint64_t sysNum, void* arg1) {
	uint64_t ret;
        __asm__ volatile
        (
                "movq %[sysNum], %%rax;"
                "int $0x80;"
                :"=a"(ret)
                : [sysNum]"a"(sysNum), "D"(arg1)
                : "cc","rcx","memory"
        );
	return ret;
}

uint64_t syscall1_i(uint64_t sysNum, int arg1) {
	uint64_t ret;
        __asm__ volatile
        (
                "movq %[sysNum], %%rax;"
                "int $0x80;"
                :"=a"(ret)
                : [sysNum]"a"(sysNum), "D"(arg1)
                : "cc","rcx","memory"
        );
	return ret;
}

uint64_t syscall1_ip(uint64_t sysNum, int* arg1) {
	uint64_t ret;
        __asm__ volatile
        (
                "movq %[sysNum], %%rax;"
                "int $0x80;"
                :"=a"(ret)
                : [sysNum]"a"(sysNum), "D"(arg1)
                : "cc","rcx","memory"
        );
	return ret;
}

uint64_t syscall2(uint64_t sysNum, void* arg1, int arg2) {
	uint64_t ret;
        __asm__ volatile
        (
                "movq %[sysNum], %%rax;"
                "int $0x80;"
                :"=a"(ret)
                : [sysNum]"a"(sysNum), "D"(arg1), "S"(arg2)
                : "cc","rcx","r11","memory"
        );
	return ret;
}

uint64_t syscall2_ii(uint64_t sysNum, int arg1, int arg2) {
	uint64_t ret;
        __asm__ volatile
        (
                "movq %[sysNum], %%rax;"
                "int $0x80;"
                :"=a"(ret)
                : [sysNum]"a"(sysNum), "D"(arg1), "S"(arg2)
                : "cc","rcx","r11","memory"
        );
	return ret;
}

uint64_t syscall_ici(uint64_t sysNum, int arg1, char* arg2, int arg3) {
	uint64_t ret;
        __asm__ volatile
        (
                "movq %[sysNum], %%rax;"
                "int $0x80;"
                :"=a"(ret)
                : [sysNum]"a"(sysNum), "D"(arg1), "S"(arg2), "d"(arg3)
                : "cc","rcx","r11","memory"
        );
	return ret;
}


uint64_t syscall3(uint64_t sysNum, uint64_t arg1, uint64_t arg2, uint64_t arg3) {
	uint64_t ret;

        __asm__ volatile
        (
                "movq %[sysNum], %%rax;"
                "int $0x80;"

                :"=a"(ret)
		:[sysNum]"a"(sysNum), "D"(arg1), "S"(arg2), "d"(arg3)
                : "cc","rcx","r11","memory"
        );
	return ret;
}
