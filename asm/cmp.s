foo:
	pushq	%rbp
	movq	%rsp, %rbp
	movl	-4(%rbp), %eax
	movl	$0, %ebx
label:
	cmpl	%ebx,%eax
	jle		label
	leave
	ret
