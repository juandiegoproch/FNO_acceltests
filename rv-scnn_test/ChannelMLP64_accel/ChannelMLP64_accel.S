	.file	"matvec_accel64.c"
	.option nopic
	.attribute arch, "rv32i2p1_m2p0_c2p0_zicsr2p0"
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
	.globl	GEMM_Accelerated
	.type	GEMM_Accelerated, @function
GEMM_Accelerated:
	srai	a7,a0,31
	andi	a7,a7,3
	add	a7,a7,a0
	addi	sp,sp,-144
	mv	t0,a5
	srai	a5,a7,2
	sw	ra,140(sp)
	li	t1,3
	sw	a4,60(sp)
	sw	a5,88(sp)
	ble	a0,t1,.L12
	srai	a5,a1,31
	and	a5,a5,t1
	add	a5,a5,a1
	srai	a5,a5,2
	sw	s7,112(sp)
	sw	s11,96(sp)
	sw	a5,32(sp)
	mv	a0,a2
	slli	a4,a2,2
	ble	a1,t1,.L60
	sw	s1,136(sp)
	li	a5,262144
	lui	s1,%hi(.LANCHOR0)
	addi	a5,a5,4
	lui	t5,%hi(.LANCHOR0+128)
	addi	s1,s1,%lo(.LANCHOR0)
	mv	t6,a3
	sw	s5,120(sp)
	sw	s6,116(sp)
	sw	s8,108(sp)
	add	s6,s1,a4
	slli	s8,a2,1
	addi	s5,t5,%lo(.LANCHOR0+128)
	lui	a3,%hi(.LANCHOR0+256)
	sw	a5,40(sp)
	addi	a5,a0,1
	add	a2,s5,a4
	slli	a6,a1,1
	addi	a3,a3,%lo(.LANCHOR0+256)
	add	t2,t0,a1
	mv	t1,s6
	sw	s4,124(sp)
	sw	s9,104(sp)
	li	s4,0
	add	s9,t6,a0
	sw	s10,100(sp)
	sw	a5,48(sp)
	add	s10,s8,a0
	mv	a5,a4
	mv	a4,a1
	sw	s2,132(sp)
	sw	s3,128(sp)
	sw	a2,36(sp)
	sw	a3,44(sp)
	mv	s6,s8
	sw	a6,92(sp)
	mv	s8,s9
	li	a7,0
	mv	s9,s4
	li	a3,0
	mv	s4,s10
	li	s3,-128
	li	s2,127
	mv	s7,a0
	sw	a0,52(sp)
	sw	t6,56(sp)
	mv	a1,t0
	mv	a2,t2
	mv	s11,a4
	mv	s10,t1
.L15:
	add	a0,s11,a7
	add	a4,s11,a6
	li	t2,0
	add	t0,a1,a7
	add	t6,a1,a6
	mv	t5,s11
	sw	a4,64(sp)
	mv	s11,s8
	add	a7,a2,a7
	mv	s8,s7
	add	a6,a2,a6
	mv	s7,s6
	sw	a5,68(sp)
	mv	s6,s4
	sw	a3,72(sp)
	sw	a0,76(sp)
	mv	s4,t2
	sw	a1,80(sp)
	sw	a2,84(sp)
.L53:
	lw	a4,56(sp)
	lw	a3,52(sp)
	slli	a5,s4,2
	add	a2,a4,s9
	mv	a4,s5
	ble	a3,zero,.L52
.L16:
	lb	a0,0(a2)
	sub	a3,a2,s9
	add	a1,a3,s8
	sb	a0,0(a4)
	lb	a0,0(a1)
	add	a1,a3,s7
	add	a3,a3,s6
	sb	a0,1(a4)
	lb	a1,0(a1)
	addi	a2,a2,1
	addi	a4,a4,4
	sb	a1,-2(a4)
	lb	a3,0(a3)
	sb	a3,-1(a4)
	bne	s11,a2,.L16
	lw	a4,60(sp)
	add	a5,a4,a5
	mv	a4,s1
.L17:
	lb	a3,0(a5)
	addi	a4,a4,4
	sb	a3,-4(a4)
	lb	a3,1(a5)
	sb	a3,-3(a4)
	lb	a3,2(a5)
	sb	a3,-2(a4)
	lb	a3,3(a5)
	add	a5,a5,t5
	sb	a3,-1(a4)
	bne	a4,s10,.L17
.L52:
	lw	a5,36(sp)
	lw	a4,40(sp)
	sw	t5,28(sp)
	sw	zero,0(a5)
	sw	zero,0(s10)
	lw	a5,48(sp)
	sw	t0,24(sp)
	sw	a7,20(sp)
	sw	t6,16(sp)
	sw	a6,12(sp)
	sw	zero,256(s1)
	sw	zero,260(s1)
	sw	zero,264(s1)
	sw	zero,268(s1)
	sw	zero,272(s1)
	sw	zero,276(s1)
	sw	zero,280(s1)
	sw	zero,284(s1)
	sw	zero,288(s1)
	sw	zero,292(s1)
	sw	zero,296(s1)
	sw	zero,300(s1)
	sw	zero,304(s1)
	sw	zero,308(s1)
	sw	zero,312(s1)
	sw	zero,316(s1)
 #APP
# 18 "../include/custom.h" 1
	addi zero,zero,0
.insn r 0x77, 0, 0, a5, a5, a4
# 0 "" 2
 #NO_APP
	li	a5,0
	li	a4,2
 #APP
# 6 "../include/custom.h" 1
	addi zero,zero,0
.insn r 0x77, 2, 4, a5, a4, a5
# 0 "" 2
# 28 "../include/custom.h" 1
	addi zero,zero,0
.insn r 0x77, 0, 1, a5, s5, s1
# 0 "" 2
 #NO_APP
	lw	a0,44(sp)
	call	SCNN_WB
	lw	a5,256(s1)
	lw	a6,12(sp)
	lw	t6,16(sp)
	lw	a7,20(sp)
	lw	t0,24(sp)
	lw	t5,28(sp)
	bge	a5,s3,.L18
	li	a5,-128
.L18:
	ble	a5,s2,.L19
	li	a5,127
.L19:
	sb	a5,0(t0)
	lw	a5,260(s1)
	bge	a5,s3,.L20
	li	a5,-128
.L20:
	ble	a5,s2,.L21
	li	a5,127
.L21:
	sb	a5,1(t0)
	lw	a5,264(s1)
	bge	a5,s3,.L22
	li	a5,-128
.L22:
	ble	a5,s2,.L23
	li	a5,127
.L23:
	sb	a5,2(t0)
	lw	a5,268(s1)
	bge	a5,s3,.L24
	li	a5,-128
.L24:
	ble	a5,s2,.L25
	li	a5,127
.L25:
	sb	a5,3(t0)
	lw	a5,272(s1)
	bge	a5,s3,.L26
	li	a5,-128
.L26:
	ble	a5,s2,.L27
	li	a5,127
.L27:
	sb	a5,0(a7)
	lw	a5,276(s1)
	bge	a5,s3,.L28
	li	a5,-128
.L28:
	ble	a5,s2,.L29
	li	a5,127
.L29:
	sb	a5,1(a7)
	lw	a5,280(s1)
	bge	a5,s3,.L30
	li	a5,-128
.L30:
	ble	a5,s2,.L31
	li	a5,127
.L31:
	sb	a5,2(a7)
	lw	a5,284(s1)
	bge	a5,s3,.L32
	li	a5,-128
.L32:
	ble	a5,s2,.L33
	li	a5,127
.L33:
	sb	a5,3(a7)
	lw	a5,288(s1)
	bge	a5,s3,.L34
	li	a5,-128
.L34:
	ble	a5,s2,.L35
	li	a5,127
.L35:
	sb	a5,0(t6)
	lw	a5,292(s1)
	bge	a5,s3,.L36
	li	a5,-128
.L36:
	ble	a5,s2,.L37
	li	a5,127
.L37:
	sb	a5,1(t6)
	lw	a5,296(s1)
	bge	a5,s3,.L38
	li	a5,-128
.L38:
	ble	a5,s2,.L39
	li	a5,127
.L39:
	sb	a5,2(t6)
	lw	a5,300(s1)
	bge	a5,s3,.L40
	li	a5,-128
.L40:
	ble	a5,s2,.L41
	li	a5,127
.L41:
	sb	a5,3(t6)
	lw	a5,304(s1)
	bge	a5,s3,.L42
	li	a5,-128
.L42:
	ble	a5,s2,.L43
	li	a5,127
.L43:
	sb	a5,0(a6)
	lw	a5,308(s1)
	bge	a5,s3,.L44
	li	a5,-128
.L44:
	ble	a5,s2,.L45
	li	a5,127
.L45:
	sb	a5,1(a6)
	lw	a5,312(s1)
	bge	a5,s3,.L46
	li	a5,-128
.L46:
	ble	a5,s2,.L47
	li	a5,127
.L47:
	sb	a5,2(a6)
	lw	a5,316(s1)
	bge	a5,s3,.L48
	li	a5,-128
.L48:
	ble	a5,s2,.L49
	li	a5,127
.L49:
	sb	a5,3(a6)
	lw	a5,32(sp)
	addi	s4,s4,1
	addi	t0,t0,4
	addi	a7,a7,4
	addi	t6,t6,4
	addi	a6,a6,4
	blt	s4,a5,.L53
	lw	a4,92(sp)
	lw	a0,76(sp)
	lw	a3,72(sp)
	add	a6,a4,t5
	lw	a4,64(sp)
	lw	a5,68(sp)
	add	a7,a0,a6
	add	a6,a4,a6
	lw	a4,88(sp)
	mv	s4,s6
	addi	a3,a3,1
	mv	s6,s7
	mv	s7,s8
	mv	s8,s11
	lw	a1,80(sp)
	lw	a2,84(sp)
	mv	s11,t5
	add	s9,s9,a5
	add	s7,s7,a5
	add	s6,s6,a5
	add	s4,s4,a5
	add	s8,s8,a5
	blt	a3,a4,.L15
	lw	s1,136(sp)
	lw	s2,132(sp)
	lw	s3,128(sp)
	lw	s4,124(sp)
	lw	s5,120(sp)
	lw	s6,116(sp)
	lw	s7,112(sp)
	lw	s8,108(sp)
	lw	s9,104(sp)
	lw	s10,100(sp)
	lw	s11,96(sp)
.L12:
	lw	ra,140(sp)
	addi	sp,sp,144
	jr	ra
.L60:
	lw	s7,112(sp)
	lw	s11,96(sp)
	j	.L12
	.size	GEMM_Accelerated, .-GEMM_Accelerated
	.section	.text.startup,"ax",@progbits
	.align	1
	.globl	main
	.type	main, @function
main:
	addi	sp,sp,-64
	sw	s1,56(sp)
	lui	s1,%hi(.LANCHOR1)
	addi	s1,s1,%lo(.LANCHOR1)
	addi	a0,s1,16
	sw	ra,60(sp)
	sw	s2,52(sp)
	sw	s3,48(sp)
	sw	s4,44(sp)
	sw	s5,40(sp)
	sw	s6,36(sp)
	sw	s7,32(sp)
	sw	s8,28(sp)
	sw	s9,24(sp)
	call	puts
	li	a5,24
	mv	a3,a5
	li	a1,64
	addi	a0,s1,76
	li	a4,12
	li	a2,4096
	call	printf
	lui	a3,%hi(heap.0)
	addi	s2,a3,%lo(heap.0)
	addi	s3,a3,%lo(heap.0)
	addi	a1,a3,%lo(heap.0)
	li	t1,0
	li	a7,0
	li	a6,24
	li	t3,98304
.L64:
	andi	a0,a7,7
	li	a5,0
.L63:
	add	a4,a1,a5
	sgt	a2,a0,a5
	sb	a2,0(a4)
	addi	a5,a5,1
	bne	a5,a6,.L63
	addi	t1,t1,24
	addi	a7,a7,1
	addi	a1,a1,24
	bne	t1,t3,.L64
	lui	a4,%hi(heap.0+98304)
	addi	a0,a4,%lo(heap.0+98304)
	li	a2,288
	li	a1,1
	call	memset
	lui	s8,%hi(heap.0+147744)
	addi	a0,s8,%lo(heap.0+147744)
	li	a2,288
	li	a1,1
	call	memset
 #APP
# 20 "chipyard_compatibility.h" 1
	rdcycleh s4
# 0 "" 2
# 21 "chipyard_compatibility.h" 1
	rdcycle  s6
# 0 "" 2
# 22 "chipyard_compatibility.h" 1
	rdcycleh a5
# 0 "" 2
 #NO_APP
	lui	a4,%hi(heap.0+98304)
	lui	a3,%hi(heap.0)
	beq	s4,a5,.L65
.L66:
 #APP
# 26 "chipyard_compatibility.h" 1
	rdcycle  s6
# 0 "" 2
 #NO_APP
	mv	s4,a5
 #APP
# 27 "chipyard_compatibility.h" 1
	rdcycleh a5
# 0 "" 2
 #NO_APP
	bne	a5,s4,.L66
.L65:
 #APP
# 40 "chipyard_compatibility.h" 1
	rdinstreth s5
# 0 "" 2
# 41 "chipyard_compatibility.h" 1
	rdinstret  s7
# 0 "" 2
# 42 "chipyard_compatibility.h" 1
	rdinstreth a5
# 0 "" 2
 #NO_APP
	beq	s5,a5,.L67
.L68:
 #APP
# 46 "chipyard_compatibility.h" 1
	rdinstret  s7
# 0 "" 2
 #NO_APP
	mv	s5,a5
 #APP
# 47 "chipyard_compatibility.h" 1
	rdinstreth a5
# 0 "" 2
 #NO_APP
	bne	a5,s5,.L68
.L67:
	lui	s9,%hi(heap.0+98592)
	addi	a5,s9,%lo(heap.0+98592)
	addi	a4,a4,%lo(heap.0+98304)
	addi	a3,a3,%lo(heap.0)
	li	a2,24
	li	a1,12
	li	a0,4096
	call	GEMM_Accelerated
	lui	a5,%hi(heap.0+148032)
	addi	a4,s8,%lo(heap.0+147744)
	addi	a3,s9,%lo(heap.0+98592)
	addi	a5,a5,%lo(heap.0+148032)
	li	a2,12
	li	a1,24
	li	a0,4096
	call	GEMM_Accelerated
 #APP
# 40 "chipyard_compatibility.h" 1
	rdinstreth a0
# 0 "" 2
# 41 "chipyard_compatibility.h" 1
	rdinstret  a6
# 0 "" 2
# 42 "chipyard_compatibility.h" 1
	rdinstreth a5
# 0 "" 2
 #NO_APP
	beq	a0,a5,.L69
.L70:
 #APP
# 46 "chipyard_compatibility.h" 1
	rdinstret  a6
# 0 "" 2
 #NO_APP
	mv	a0,a5
 #APP
# 47 "chipyard_compatibility.h" 1
	rdinstreth a5
# 0 "" 2
 #NO_APP
	bne	a5,a0,.L70
.L69:
 #APP
# 20 "chipyard_compatibility.h" 1
	rdcycleh a1
# 0 "" 2
# 21 "chipyard_compatibility.h" 1
	rdcycle  a3
# 0 "" 2
# 22 "chipyard_compatibility.h" 1
	rdcycleh a5
# 0 "" 2
 #NO_APP
	beq	a1,a5,.L71
.L72:
 #APP
# 26 "chipyard_compatibility.h" 1
	rdcycle  a3
# 0 "" 2
 #NO_APP
	mv	a1,a5
 #APP
# 27 "chipyard_compatibility.h" 1
	rdcycleh a5
# 0 "" 2
 #NO_APP
	bne	a5,a1,.L72
.L71:
	sub	a4,a6,s7
	sub	a2,a3,s6
	sgtu	a5,a4,a6
	sub	a1,a1,s4
	sgtu	a3,a2,a3
	sub	a0,a0,s5
	sub	a5,a0,a5
	sub	a3,a1,a3
	addi	a0,s1,116
	li	s5,98304
	call	printf
	add	s4,s3,s5
	lb	a4,303(s4)
	lb	a3,302(s4)
	lb	a2,301(s4)
	lb	a1,300(s4)
	addi	a0,s1,168
	call	printf
	lb	a4,375(s4)
	lb	a3,374(s4)
	lb	a2,373(s4)
	lb	a1,372(s4)
	addi	a0,s1,216
	call	printf
	li	a5,147456
	add	s3,s3,a5
	lb	a0,607(s3)
	lb	a7,606(s3)
	lb	a6,605(s3)
	lb	a5,604(s3)
	lb	a4,603(s3)
	lb	a3,602(s3)
	lb	a2,601(s3)
	lb	a1,600(s3)
	sw	a0,0(sp)
	addi	a0,s1,264
	call	printf
	lb	a0,631(s3)
	lb	a7,630(s3)
	lb	a6,629(s3)
	lb	a1,624(s3)
	lb	a5,628(s3)
	lb	a4,627(s3)
	lb	a3,626(s3)
	lb	a2,625(s3)
	sw	a0,0(sp)
	addi	a0,s1,332
	call	printf
	li	a6,147456
	addi	a6,a6,576
	li	t1,0
	li	a1,0
	li	a7,24
.L76:
	andi	a0,a1,7
	slli	a5,a0,1
	add	a0,a0,a5
	slli	a3,a0,2
	mv	a0,a3
	li	a2,0
	j	.L75
.L73:
	addi	a2,a2,1
	beq	a2,a7,.L90
.L75:
	add	a5,a2,a6
	add	a5,s2,a5
	lb	a4,0(a5)
	beq	a4,a0,.L73
	addi	a0,s1,400
	call	printf
	lui	a5,%hi(.LC1)
	addi	a1,a5,%lo(.LC1)
	li	s2,1
.L74:
	addi	a0,s1,452
	call	printf
	lw	ra,60(sp)
	lw	s1,56(sp)
	lw	s3,48(sp)
	lw	s4,44(sp)
	lw	s5,40(sp)
	lw	s6,36(sp)
	lw	s7,32(sp)
	lw	s8,28(sp)
	lw	s9,24(sp)
	mv	a0,s2
	lw	s2,52(sp)
	addi	sp,sp,64
	jr	ra
.L90:
	addi	t1,t1,24
	addi	a1,a1,1
	addi	s2,s2,24
	bne	t1,s5,.L76
	lui	a5,%hi(.LC0)
	addi	a1,a5,%lo(.LC0)
	li	s2,0
	j	.L74
	.size	main, .-main
	.section	.rodata
	.align	2
	.set	.LANCHOR1,. + 0
.LC0:
	.string	"PASS"
	.zero	3
.LC1:
	.string	"FAIL"
	.zero	3
.LC2:
	.string	"=== Accelerated: matvec ChannelMLP (RV-SCNN, K en HW) ==="
	.zero	2
.LC3:
	.string	"SZCH=%d L=%d CIN0=%d CMID=%d COUT=%d\n\n"
	.zero	1
.LC4:
	.string	"Accelerated matvec: %llu cycles, %llu instructions\n"
.LC5:
	.string	"C1[fila1] col0..3 = %d %d %d %d  (esperado 1)\n"
	.zero	1
.LC6:
	.string	"C1[fila7] col0..3 = %d %d %d %d  (esperado 7)\n"
	.zero	1
.LC7:
	.string	"C2[fila1] j0..7  = %d %d %d %d %d %d %d %d  (esperado todos 12)\n"
	.zero	3
.LC8:
	.string	"C2[fila2] j0..7  = %d %d %d %d %d %d %d %d  (esperado todos 24)\n"
	.zero	3
.LC9:
	.string	"Primer fallo: i=%d j=%d esperado=%d obtenido=%d\n"
	.zero	3
.LC10:
	.string	"Verificacion HW: %s\n"
	.bss
	.align	7
	.set	.LANCHOR0,. + 0
	.type	b_tile, @object
	.size	b_tile, 128
b_tile:
	.zero	128
	.type	a_tile, @object
	.size	a_tile, 128
a_tile:
	.zero	128
	.type	res_tile, @object
	.size	res_tile, 64
res_tile:
	.zero	64
	.type	heap.0, @object
	.size	heap.0, 250432
heap.0:
	.zero	250432
	.ident	"GCC: ('corev-openhw-gcc-ubuntu2204-20240530') 14.1.0"
	.section	.note.GNU-stack,"",@progbits
