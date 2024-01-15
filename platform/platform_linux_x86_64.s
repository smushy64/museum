#if defined(LD_PLATFORM_LINUX) && defined(LD_ARCH_X86) && defined(LD_ARCH_64_BIT)

/* intel syntax & no % prefix for registers */
.intel_syntax noprefix

/* IMPORTANT(alicia): this asm code is for AMD64 ONLY!!! */

.text
    .globl _start

    _start:
        xor rbp, rbp
        pop rdi
        mov rsi, rsp
        and rsp, -16
        call main
        mov rdi, rax
        mov rax, 60 /* SYS_EXIT */
        syscall
        ret

#endif

