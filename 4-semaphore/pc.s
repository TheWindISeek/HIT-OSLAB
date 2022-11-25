	.file	"pc.c"
	.globl	fileName
	.section	.rodata
.LC0:
	.string	"./pc.log"
	.data
	.align 8
	.type	fileName, @object
	.size	fileName, 8
fileName:
	.quad	.LC0
	.comm	full,32,32
	.comm	empty,32,32
	.comm	access_mutex,32,32
	.comm	print_mutex,32,32
	.globl	kill
	.bss
	.align 4
	.type	kill, @object
	.size	kill, 4
kill:
	.zero	4
	.text
	.globl	strlen
	.type	strlen, @function
strlen:
.LFB2:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movq	%rdi, -24(%rbp)
	movq	$0, -8(%rbp)
	jmp	.L2
.L3:
	addq	$1, -8(%rbp)
.L2:
	movq	-24(%rbp), %rax
	leaq	1(%rax), %rdx
	movq	%rdx, -24(%rbp)
	movzbl	(%rax), %eax
	testb	%al, %al
	jne	.L3
	movq	-8(%rbp), %rax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE2:
	.size	strlen, .-strlen
	.section	.rodata
.LC1:
	.string	"000"
.LC2:
	.string	"producer"
.LC3:
	.string	"%d\n"
	.text
	.globl	producer
	.type	producer, @function
producer:
.LFB3:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movl	%edi, -20(%rbp)
	movl	$500, -12(%rbp)
	movq	$.LC1, -8(%rbp)
	movl	$.LC2, %edi
	call	puts
	movl	$0, -16(%rbp)
	jmp	.L6
.L8:
	movl	$empty, %edi
	call	sem_wait
	movl	$access_mutex, %edi
	call	sem_wait
	movl	-16(%rbp), %ecx
	movl	$1717986919, %edx
	movl	%ecx, %eax
	imull	%edx
	sarl	$2, %edx
	movl	%ecx, %eax
	sarl	$31, %eax
	subl	%eax, %edx
	movl	%edx, %eax
	sall	$2, %eax
	addl	%edx, %eax
	addl	%eax, %eax
	subl	%eax, %ecx
	movl	%ecx, %edx
	testl	%edx, %edx
	jne	.L7
	movl	-20(%rbp), %eax
	movl	$0, %edx
	movl	$0, %esi
	movl	%eax, %edi
	call	lseek
.L7:
	movl	-16(%rbp), %edx
	movq	-8(%rbp), %rax
	movl	$.LC3, %esi
	movq	%rax, %rdi
	movl	$0, %eax
	call	sprintf
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	call	strlen
	movq	%rax, %rdx
	movq	-8(%rbp), %rcx
	movl	-20(%rbp), %eax
	movq	%rcx, %rsi
	movl	%eax, %edi
	call	write
	movl	$access_mutex, %edi
	call	sem_post
	movl	$full, %edi
	call	sem_post
	addl	$1, -16(%rbp)
.L6:
	movl	-16(%rbp), %eax
	cmpl	-12(%rbp), %eax
	jl	.L8
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE3:
	.size	producer, .-producer
	.section	.rodata
.LC4:
	.string	"00000"
.LC5:
	.string	"consumer pid->%d\n"
.LC6:
	.string	"%d: %s"
	.text
	.globl	consumer
	.type	consumer, @function
consumer:
.LFB4:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movl	%edi, -20(%rbp)
	call	getpid
	movl	%eax, -12(%rbp)
	movq	$.LC4, -8(%rbp)
	movl	-12(%rbp), %eax
	movl	%eax, %esi
	movl	$.LC5, %edi
	movl	$0, %eax
	call	printf
	jmp	.L10
.L11:
	movl	$full, %edi
	call	sem_wait
	movl	$access_mutex, %edi
	call	sem_wait
	movq	-8(%rbp), %rcx
	movl	-20(%rbp), %eax
	movl	$4, %edx
	movq	%rcx, %rsi
	movl	%eax, %edi
	call	read
	movl	$access_mutex, %edi
	call	sem_post
	movl	$print_mutex, %edi
	call	sem_wait
	movq	-8(%rbp), %rdx
	movl	-12(%rbp), %eax
	movl	%eax, %esi
	movl	$.LC6, %edi
	movl	$0, %eax
	call	printf
	movl	$print_mutex, %edi
	call	sem_post
	movl	$empty, %edi
	call	sem_post
.L10:
	movl	kill(%rip), %eax
	testl	%eax, %eax
	je	.L11
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE4:
	.size	consumer, .-consumer
	.section	.rodata
.LC7:
	.string	"create producer process"
.LC8:
	.string	"create consumer process"
.LC9:
	.string	"wait all subprocess exit"
	.text
	.globl	main
	.type	main, @function
main:
.LFB5:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movl	%edi, -20(%rbp)
	movq	%rsi, -32(%rbp)
	movq	fileName(%rip), %rax
	movl	$2, %esi
	movq	%rax, %rdi
	movl	$0, %eax
	call	open
	movl	%eax, -4(%rbp)
	movl	$0, %edx
	movl	$1, %esi
	movl	$full, %edi
	call	sem_init
	movl	$10, %edx
	movl	$1, %esi
	movl	$empty, %edi
	call	sem_init
	movl	$1, %edx
	movl	$1, %esi
	movl	$access_mutex, %edi
	call	sem_init
	movl	$1, %edx
	movl	$1, %esi
	movl	$print_mutex, %edi
	call	sem_init
	movl	$.LC7, %edi
	call	puts
	call	fork
	testl	%eax, %eax
	jne	.L13
	movl	-4(%rbp), %eax
	movl	%eax, %edi
	call	producer
	movl	$0, %eax
	jmp	.L17
.L13:
	movl	$.LC8, %edi
	call	puts
	movl	$0, -8(%rbp)
	jmp	.L15
.L16:
	movl	-8(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -8(%rbp)
.L15:
	movl	-8(%rbp), %eax
	cmpl	$9, %eax
	jle	.L16
	movl	$.LC9, %edi
	call	puts
	leaq	-8(%rbp), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	wait
	movl	-4(%rbp), %eax
	movl	%eax, %edi
	call	close
	movl	$full, %edi
	call	sem_destroy
	movl	$empty, %edi
	call	sem_destroy
	movl	$access_mutex, %edi
	call	sem_destroy
	movl	$print_mutex, %edi
	call	sem_destroy
	movl	$0, %eax
.L17:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE5:
	.size	main, .-main
	.ident	"GCC: (Ubuntu 4.8.4-2ubuntu1~14.04.4) 4.8.4"
	.section	.note.GNU-stack,"",@progbits
