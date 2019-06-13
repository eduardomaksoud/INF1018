	push %rbp
	movq %rsp, %rbp
	subq $32, %rsp
	movl $10, -4(%rbp)
	movl $5, -20(%rbp)
	movl -4(%rbp),%r10d
	movl %r10d, -20(%rbp)
	leave
	ret 


 55                   	
 48 89 e5             	
 48 83 ec 20          	
 c7 45 fc 0a 00 00 00 	
 c7 45 ec 05 00 00 00 	
 44 8b 55 fc          	
 44 89 55 ec          	
 c9                   	 
 c3                   	  
v5 < v1
