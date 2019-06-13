push %rbp
movq %rsp, %rbp
subq $32, %rsp

movl $100, -4(%rbp)
movl $100, -8(%rbp)
movl $100, -12(%rbp)
movl $100, -16(%rbp)
movl $100, -20(%rbp)

movl -4(%rbp),%eax

leave
ret

