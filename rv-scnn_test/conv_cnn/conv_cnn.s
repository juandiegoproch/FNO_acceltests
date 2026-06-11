	.file	"conv_cnn.c"
	.option nopic
	.attribute arch, "rv32i2p1_m2p0_c2p0_zicsr2p0_zmmul1p0_zca1p0"
	.attribute unaligned_access, 0
	.attribute stack_align, 16
	.text
	.align	1
	.globl	SCNN_WB
	.type	SCNN_WB, @function
SCNN_WB:
	addi	sp,sp,-48
	sw	ra,44(sp)
	sw	s0,40(sp)
	addi	s0,sp,48
	sw	a0,-36(s0)
	lw	a5,-36(s0)
	lw	a4,-20(s0)
 #APP
# 39 "../include/custom.h" 1
	addi zero,zero,0
.insn r 0x77, 0, 2, a5, a5, a4
# 0 "" 2
 #NO_APP
	sw	a5,-24(s0)
	nop
	lw	ra,44(sp)
	lw	s0,40(sp)
	addi	sp,sp,48
	jr	ra
	.size	SCNN_WB, .-SCNN_WB
	.align	1
	.globl	SCNN_WB_INT
	.type	SCNN_WB_INT, @function
SCNN_WB_INT:
	addi	sp,sp,-16
	sw	ra,12(sp)
	sw	s0,8(sp)
	addi	s0,sp,16
	mv	a5,a0
	mv	a0,a5
	lw	ra,12(sp)
	lw	s0,8(sp)
	addi	sp,sp,16
	tail	SCNN_WB
	.size	SCNN_WB_INT, .-SCNN_WB_INT
	.align	1
	.globl	POOL_WB
	.type	POOL_WB, @function
POOL_WB:
	addi	sp,sp,-48
	sw	ra,44(sp)
	sw	s0,40(sp)
	addi	s0,sp,48
	sw	a0,-36(s0)
	sw	zero,-20(s0)
	lw	a5,-36(s0)
	lw	a4,-20(s0)
 #APP
# 87 "../include/custom.h" 1
	addi zero,zero,0
.insn r 0x77, 2, 2, a5, a5, a4
# 0 "" 2
 #NO_APP
	sw	a5,-24(s0)
	nop
	lw	ra,44(sp)
	lw	s0,40(sp)
	addi	sp,sp,48
	jr	ra
	.size	POOL_WB, .-POOL_WB
	.align	1
	.globl	POOL_WB_INT
	.type	POOL_WB_INT, @function
POOL_WB_INT:
	addi	sp,sp,-48
	sw	ra,44(sp)
	sw	s0,40(sp)
	addi	s0,sp,48
	sw	a0,-36(s0)
	sw	a1,-40(s0)
	lw	a5,-36(s0)
	lw	a4,-40(s0)
 #APP
# 98 "../include/custom.h" 1
	addi zero,zero,0
.insn r 0x77, 2, 2, a5, a5, a4
# 0 "" 2
 #NO_APP
	sw	a5,-20(s0)
	nop
	lw	ra,44(sp)
	lw	s0,40(sp)
	addi	sp,sp,48
	jr	ra
	.size	POOL_WB_INT, .-POOL_WB_INT
	.align	1
	.globl	L_MODE
	.type	L_MODE, @function
L_MODE:
	slli	a1,a1,1
	add	a1,a1,a0
	slli	a3,a3,4
	add	a1,a1,a3
 #APP
# 6 "../include/custom.h" 1
	addi zero,zero,0
.insn r 0x77, 2, 4, a1, a1, a2
# 0 "" 2
 #NO_APP
	ret
	.size	L_MODE, .-L_MODE
	.align	1
	.globl	L_SCNN
	.type	L_SCNN, @function
L_SCNN:
	slli	a1,a1,16
	slli	a3,a3,16
	add	a3,a3,a0
	add	a1,a1,a2
 #APP
# 18 "../include/custom.h" 1
	addi zero,zero,0
.insn r 0x77, 0, 0, a3, a3, a1
# 0 "" 2
 #NO_APP
	ret
	.size	L_SCNN, .-L_SCNN
	.align	1
	.globl	SCNN4x4
	.type	SCNN4x4, @function
SCNN4x4:
 #APP
# 28 "../include/custom.h" 1
	addi zero,zero,0
.insn r 0x77, 0, 1, a0, a0, a1
# 0 "" 2
 #NO_APP
	ret
	.size	SCNN4x4, .-SCNN4x4
	.align	1
	.globl	POOL_RI
	.type	POOL_RI, @function
POOL_RI:
	li	a5,0
 #APP
# 63 "../include/custom.h" 1
	addi zero,zero,0
.insn r 0x77, 2, 0, a0, a0, a5
# 0 "" 2
 #NO_APP
	ret
	.size	POOL_RI, .-POOL_RI
	.align	1
	.globl	POOL
	.type	POOL, @function
POOL:
	li	a5,0
 #APP
# 75 "../include/custom.h" 1
	addi zero,zero,0
.insn r 0x77, 2, 1, a0, a0, a5
# 0 "" 2
 #NO_APP
	ret
	.size	POOL, .-POOL
	.align	1
	.globl	record
	.type	record, @function
record:
 #APP
# 109 "../include/custom.h" 1
	rdcycle a0
# 0 "" 2
 #NO_APP
	ret
	.size	record, .-record
	.align	1
	.globl	Base_conv_mp
	.type	Base_conv_mp, @function
Base_conv_mp:
	addi	sp,sp,-176
	sw	s6,148(sp)
	mv	s6,a0
	addi	a0,a0,-2
	srli	a5,a0,31
	add	a5,a5,a0
	sw	s8,140(sp)
	srai	s8,a5,1
	sw	s1,168(sp)
	mul	s1,s8,s8
	sw	a1,48(sp)
	li	a1,4
	sw	s2,164(sp)
	sw	s3,160(sp)
	sw	s4,156(sp)
	sw	ra,172(sp)
	mv	s4,a2
	sw	a2,16(sp)
	mv	s2,a3
	mul	a0,s1,a2
	mv	s3,a4
	call	calloc
	mv	a6,a0
	ble	s4,zero,.L12
	li	a5,3
	ble	s6,a5,.L12
	lw	a4,16(sp)
	lw	a5,16(sp)
	sw	s9,136(sp)
	slli	a4,a4,2
	sw	a4,20(sp)
	lw	a4,16(sp)
	slli	a5,a5,1
	add	a3,s3,a5
	slli	a4,a4,3
	sw	a4,52(sp)
	lw	a5,20(sp)
	lw	a4,16(sp)
	mul	s9,s6,s6
	slli	a7,s8,2
	add	a5,a5,a4
	sw	a5,60(sp)
	lw	a5,52(sp)
	li	a1,0
	sw	a7,108(sp)
	sub	a5,a5,a4
	sw	a5,56(sp)
	lw	a5,52(sp)
	sw	s5,152(sp)
	sw	s7,144(sp)
	add	a5,a5,a4
	sw	s10,132(sp)
	sw	s11,128(sp)
	sw	a5,24(sp)
	mv	a7,a1
	mv	a2,s3
	mv	t3,a0
.L15:
	slli	a5,a1,2
	li	a0,0
	sw	s1,112(sp)
	sw	s9,8(sp)
	sw	a7,116(sp)
	li	t1,0
	sw	a2,84(sp)
	add	a6,t3,a5
	sw	a3,88(sp)
	sw	a1,120(sp)
	sw	t3,124(sp)
.L24:
	slli	a5,a0,1
	add	a5,s2,a5
	mv	a2,a6
	li	a1,0
	sw	s8,80(sp)
	sw	t1,92(sp)
	sw	a6,96(sp)
	sw	a0,100(sp)
	sw	s2,104(sp)
.L22:
	lw	a4,48(sp)
	ble	a4,zero,.L25
	lw	s8,88(sp)
	add	s7,a5,s6
	lw	t5,84(sp)
	add	s10,s6,s7
	add	s2,s6,s10
	mv	t6,s8
	mv	s5,s10
	mv	s8,s7
	li	t0,0
	li	s11,0
	sw	zero,12(sp)
	sw	a5,64(sp)
	sw	a2,68(sp)
	sw	a1,72(sp)
	li	s4,0
	li	s7,0
	mv	s10,a5
	sw	s6,76(sp)
.L17:
	lw	a5,16(sp)
	lb	a4,0(t5)
	lb	s6,0(s10)
	add	a3,t5,a5
	lb	t3,0(a3)
	lb	a0,1(s10)
	mul	s6,s6,a4
	lb	a3,0(t6)
	lb	t4,2(s10)
	lw	a2,16(sp)
	lb	a1,0(s8)
	lb	a5,1(s8)
	add	a2,t6,a2
	lb	s1,0(a2)
	lw	a2,20(sp)
	lb	s3,2(s8)
	mul	t2,t3,a0
	add	s6,s6,s11
	add	a2,t5,a2
	lb	t1,0(a2)
	lw	a2,60(sp)
	lb	s9,0(s5)
	lb	a6,1(s5)
	add	a2,t5,a2
	lb	a7,0(a2)
	lw	a2,12(sp)
	add	s6,s6,t2
	mul	t2,a4,a0
	addi	a2,a2,1
	sw	a2,12(sp)
	lb	a2,3(s10)
	lw	s11,8(sp)
	lb	a0,2(s5)
	add	s10,s10,s11
	add	t0,t2,t0
	mul	t2,a3,t4
	add	s6,s6,t2
	mul	t2,a4,a1
	mul	a1,a1,s1
	add	t2,t2,s7
	sw	t2,32(sp)
	lw	s7,24(sp)
	add	a1,s6,a1
	mul	s6,a5,t1
	mul	t2,a4,a5
	add	a1,a1,s6
	lw	a4,20(sp)
	add	a4,t6,a4
	lb	a4,0(a4)
	add	t6,t6,s7
	mul	t4,t3,t4
	add	t2,t2,s4
	sw	t2,40(sp)
	lw	t2,56(sp)
	add	t2,t5,t2
	lb	t2,0(t2)
	mul	s6,s3,a7
	add	t4,t0,t4
	lw	t0,52(sp)
	add	t0,t5,t0
	lb	s4,0(t0)
	add	t5,t5,s7
	mul	a2,a2,a3
	add	s6,a1,s6
	add	a2,t4,a2
	mul	a1,s9,a4
	sw	a2,28(sp)
	lb	t0,3(s8)
	lb	a2,3(s5)
	add	s8,s8,s11
	add	s5,s5,s11
	lb	s7,0(s2)
	lb	t4,3(s2)
	mul	s11,a6,t2
	add	s6,s6,a1
	sw	t4,44(sp)
	lw	t4,8(sp)
	add	s6,s6,s11
	mul	s11,a0,s4
	mul	a1,s7,a4
	add	s11,s6,s11
	lb	s7,2(s2)
	mul	s6,s1,a5
	sw	a1,36(sp)
	lb	a1,1(s2)
	add	s2,s2,t4
	lw	t4,28(sp)
	mul	a5,t3,a5
	add	s6,t4,s6
	lw	t4,32(sp)
	mul	t3,t3,s3
	add	a5,t4,a5
	lw	t4,40(sp)
	add	t3,t4,t3
	mul	t4,t1,s3
	mul	s3,a3,s3
	add	t4,s6,t4
	mul	a3,a3,t0
	add	a5,a5,s3
	mul	s9,s1,s9
	add	a3,t3,a3
	mul	t3,t1,a6
	add	a5,a5,s9
	mul	s1,s1,a6
	add	a5,a5,t3
	mul	t1,t1,a0
	add	a3,a3,s1
	mul	t3,a7,a0
	add	t1,a3,t1
	lw	a3,36(sp)
	mul	a6,a4,a6
	add	a5,a5,t3
	add	a5,a5,a3
	mul	a4,a4,a1
	mul	a1,t2,a1
	mul	t0,a7,t0
	add	a1,a5,a1
	lw	a5,44(sp)
	mul	a7,a7,a2
	add	t0,t4,t0
	add	a6,t0,a6
	mul	a0,t2,a0
	add	a7,t1,a7
	add	a4,a7,a4
	mul	t2,t2,s7
	add	a0,a6,a0
	mul	a2,s4,a2
	add	a4,a4,t2
	mul	s7,s4,s7
	add	t0,a0,a2
	mul	s4,a5,s4
	lw	a5,48(sp)
	add	s7,a1,s7
	add	s4,a4,s4
	lw	a4,12(sp)
	bne	a5,a4,.L17
	lw	a5,64(sp)
	lw	a2,68(sp)
	lw	a1,72(sp)
	lw	s6,76(sp)
	mv	s5,s7
	mv	t6,s4
	mv	t5,s11
	bge	s11,t0,.L18
	mv	t5,t0
.L18:
	bge	t5,s5,.L19
	mv	t5,s5
.L19:
	bge	t5,t6,.L16
	mv	t5,t6
.L16:
	lw	a4,80(sp)
	sw	t5,0(a2)
	addi	a1,a1,1
	addi	a2,a2,4
	addi	a5,a5,2
	bgt	a4,a1,.L22
	lw	t1,92(sp)
	lw	a6,96(sp)
	lw	a0,100(sp)
	lw	a5,108(sp)
	addi	t1,t1,1
	lw	s2,104(sp)
	mv	s8,a4
	add	a6,a6,a5
	add	a0,a0,s6
	bgt	a4,t1,.L24
	lw	a7,116(sp)
	lw	s1,112(sp)
	lw	a2,84(sp)
	lw	a3,88(sp)
	lw	a1,120(sp)
	lw	a5,16(sp)
	addi	a7,a7,1
	lw	s9,8(sp)
	lw	t3,124(sp)
	add	a1,a1,s1
	addi	a2,a2,1
	addi	a3,a3,1
	bne	a5,a7,.L15
	lw	s5,152(sp)
	lw	s7,144(sp)
	lw	s9,136(sp)
	lw	s10,132(sp)
	lw	s11,128(sp)
	mv	a6,t3
.L12:
	lw	ra,172(sp)
	lw	s1,168(sp)
	lw	s2,164(sp)
	lw	s3,160(sp)
	lw	s4,156(sp)
	lw	s6,148(sp)
	lw	s8,140(sp)
	mv	a0,a6
	addi	sp,sp,176
	jr	ra
.L25:
	li	t5,0
	j	.L16
	.size	Base_conv_mp, .-Base_conv_mp
	.align	1
	.globl	error_check
	.type	error_check, @function
error_check:
	ble	a2,zero,.L36
	slli	a2,a2,2
	mv	a4,a0
	add	a2,a0,a2
	li	a0,0
.L35:
	lw	a3,0(a1)
	lw	a5,0(a4)
	addi	a4,a4,4
	addi	a1,a1,4
	sub	a5,a5,a3
	srai	a3,a5,31
	xor	a5,a3,a5
	sub	a5,a5,a3
	add	a0,a0,a5
	bne	a2,a4,.L35
	ret
.L36:
	li	a0,0
	ret
	.size	error_check, .-error_check
	.section	.text.startup,"ax",@progbits
	.align	1
	.globl	main
	.type	main, @function
main:
	addi	sp,sp,-80
	li	a0,16384
	sw	ra,76(sp)
	sw	s1,72(sp)
	sw	s2,68(sp)
	sw	s3,64(sp)
	sw	s4,60(sp)
	sw	s5,56(sp)
	sw	s6,52(sp)
	sw	s7,48(sp)
	sw	s8,44(sp)
	call	malloc
	mv	s1,a0
	li	a0,36864
	call	malloc
	li	a3,954437632
	mv	a4,a0
	addi	a3,a3,-455
	li	a2,0
	li	a6,16384
.L39:
	mulhu	a1,a2,a3
	add	a0,s1,a2
	srli	a1,a1,3
	slli	a5,a1,3
	add	a5,a5,a1
	slli	a5,a5,2
	sub	a5,a2,a5
	addi	a5,a5,1
	sb	a5,0(a0)
	addi	a2,a2,1
	bne	a2,a6,.L39
	li	a3,-1840701440
	addi	a3,a3,1171
	li	a2,0
	li	a6,36864
.L40:
	mulh	a5,a2,a3
	srai	a1,a2,31
	add	a0,a4,a2
	add	a5,a5,a2
	srai	a5,a5,2
	sub	a5,a5,a1
	slli	a1,a5,3
	sub	a5,a1,a5
	sub	a5,a2,a5
	addi	a5,a5,-5
	sb	a5,0(a0)
	addi	a2,a2,1
	bne	a2,a6,.L40
	sw	zero,28(sp)
 #APP
# 109 "../include/custom.h" 1
	rdcycle s7
# 0 "" 2
 #NO_APP
	li	a5,262144
	addi	a5,a5,1
	li	s4,1024
 #APP
# 18 "../include/custom.h" 1
	addi zero,zero,0
.insn r 0x77, 0, 0, a5, s4, a5
# 0 "" 2
 #NO_APP
	li	a3,18
	li	a5,0
 #APP
# 6 "../include/custom.h" 1
	addi zero,zero,0
.insn r 0x77, 2, 4, a3, a3, a5
# 0 "" 2
 #NO_APP
	li	a3,20
 #APP
# 6 "../include/custom.h" 1
	addi zero,zero,0
.insn r 0x77, 2, 4, a3, a3, a5
# 0 "" 2
 #NO_APP
	li	a3,22
 #APP
# 6 "../include/custom.h" 1
	addi zero,zero,0
.insn r 0x77, 2, 4, a3, a3, a5
# 0 "" 2
 #NO_APP
	li	a3,24
 #APP
# 6 "../include/custom.h" 1
	addi zero,zero,0
.insn r 0x77, 2, 4, a3, a3, a5
# 0 "" 2
# 28 "../include/custom.h" 1
	addi zero,zero,0
.insn r 0x77, 0, 1, a3, a4, s1
# 0 "" 2
 #NO_APP
	li	s2,2
	sw	a4,12(sp)
 #APP
# 75 "../include/custom.h" 1
	addi zero,zero,0
.insn r 0x77, 2, 1, a5, s2, a5
# 0 "" 2
 #NO_APP
	li	a1,1
	addi	a0,sp,28
	call	POOL_WB_INT
	mv	a1,s2
	addi	a0,sp,28
	lw	s8,28(sp)
	call	POOL_WB_INT
	addi	a0,sp,28
	li	a1,3
	lw	s5,28(sp)
	call	POOL_WB_INT
	addi	a0,sp,28
	li	a1,4
	lw	s3,28(sp)
	call	POOL_WB_INT
	lw	s2,28(sp)
 #APP
# 109 "../include/custom.h" 1
	rdcycle s6
# 0 "" 2
 #NO_APP
	lw	a4,12(sp)
	li	a2,4
	mv	a3,s1
	mv	a1,s4
	mv	a0,a2
	call	Base_conv_mp
	lw	a3,4(a0)
	lw	a2,0(a0)
	lw	a4,8(a0)
	lw	a5,12(a0)
	sub	a3,s5,a3
	sub	a2,s8,a2
	srai	a6,a3,31
	srai	a0,a2,31
	sub	a4,s3,a4
	xor	a1,a6,a3
	sub	a5,s2,a5
	xor	a3,a0,a2
	srai	a2,a4,31
	sub	a1,a1,a6
	sub	a3,a3,a0
	xor	a4,a2,a4
	srai	a0,a5,31
	lui	s2,%hi(.LANCHOR0)
	sub	a4,a4,a2
	addi	s2,s2,%lo(.LANCHOR0)
	add	a1,a1,a3
	xor	s1,a0,a5
	li	a3,4
	add	a5,a4,a1
	mv	a2,s4
	mv	a1,a3
	sub	s1,s1,a0
	mv	a0,s2
	add	s1,s1,a5
	call	printf
	mv	a1,s1
	addi	a0,s2,28
	call	printf
	sub	a1,s6,s7
	addi	a0,s2,40
	call	printf
	sub	a0,s6,s7
	call	__floatsidf
	mv	a2,a0
	mv	a3,a1
	lw	a0,64(s2)
	lw	a1,68(s2)
	call	__divdf3
	lw	a2,72(s2)
	lw	a3,76(s2)
	call	__muldf3
	call	__truncdfsf2
	call	__extendsfdf2
	mv	a2,a0
	mv	a3,a1
	addi	a0,s2,80
	call	printf
	lui	a0,%hi(.LC6)
	addi	a0,a0,%lo(.LC6)
	call	puts
	lw	ra,76(sp)
	lw	s1,72(sp)
	lw	s2,68(sp)
	lw	s3,64(sp)
	lw	s4,60(sp)
	lw	s5,56(sp)
	lw	s6,52(sp)
	lw	s7,48(sp)
	lw	s8,44(sp)
	li	a0,0
	addi	sp,sp,80
	jr	ra
	.size	main, .-main
	.section	.rodata
	.align	3
	.set	.LANCHOR0,. + 0
.LC0:
	.string	"inside:%d c_in:%d c_out:%d\n"
.LC1:
	.string	"error:%d\n"
	.zero	2
.LC2:
	.string	"cycle count::%d\n"
	.zero	7
.LC3:
	.word	0
	.word	1090650112
.LC4:
	.word	858993459
	.word	1070805811
.LC5:
	.string	"GOPS:%.4f\n"
	.zero	1
.LC6:
	.string	"Hola xd"
	.globl	__extendsfdf2
	.globl	__truncdfsf2
	.globl	__muldf3
	.globl	__divdf3
	.globl	__floatsidf
	.ident	"GCC: (g5115c7e44) 15.2.0"
	.section	.note.GNU-stack,"",@progbits
