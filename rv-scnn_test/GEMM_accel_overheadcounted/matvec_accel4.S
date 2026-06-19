	.file	"matvec_accel4.c"
	.option nopic
	.attribute arch, "rv32i2p1_m2p0_c2p0_zicsr2p0_zmmul1p0_zca1p0"
	.attribute unaligned_access, 0
	.attribute stack_align, 16
	.text
	.align	1
	.type	read_cycles, @function
read_cycles:
	addi	sp,sp,-32
	sw	ra,28(sp)
	sw	s0,24(sp)
	addi	s0,sp,32
 #APP
# 20 "./chipyard_compatibility.h" 1
	rdcycleh t1
# 0 "" 2
 #NO_APP
	sw	t1,-20(s0)
 #APP
# 21 "./chipyard_compatibility.h" 1
	rdcycle  t1
# 0 "" 2
 #NO_APP
	sw	t1,-24(s0)
 #APP
# 22 "./chipyard_compatibility.h" 1
	rdcycleh t1
# 0 "" 2
 #NO_APP
	sw	t1,-28(s0)
	j	.L2
.L3:
	lw	t1,-28(s0)
	sw	t1,-20(s0)
 #APP
# 26 "./chipyard_compatibility.h" 1
	rdcycle  t1
# 0 "" 2
 #NO_APP
	sw	t1,-24(s0)
 #APP
# 27 "./chipyard_compatibility.h" 1
	rdcycleh t1
# 0 "" 2
 #NO_APP
	sw	t1,-28(s0)
.L2:
	lw	t3,-20(s0)
	lw	t1,-28(s0)
	bne	t3,t1,.L3
	lw	t1,-20(s0)
	mv	a6,t1
	li	a7,0
	slli	a5,a6,0
	li	a4,0
	lw	a6,-24(s0)
	mv	a2,a6
	li	a3,0
	or	a0,a4,a2
	or	a1,a5,a3
	mv	a4,a0
	mv	a5,a1
	mv	a0,a4
	mv	a1,a5
	lw	ra,28(sp)
	lw	s0,24(sp)
	addi	sp,sp,32
	jr	ra
	.size	read_cycles, .-read_cycles
	.align	1
	.type	read_instret, @function
read_instret:
	addi	sp,sp,-32
	sw	ra,28(sp)
	sw	s0,24(sp)
	addi	s0,sp,32
 #APP
# 40 "./chipyard_compatibility.h" 1
	rdinstreth t1
# 0 "" 2
 #NO_APP
	sw	t1,-20(s0)
 #APP
# 41 "./chipyard_compatibility.h" 1
	rdinstret  t1
# 0 "" 2
 #NO_APP
	sw	t1,-24(s0)
 #APP
# 42 "./chipyard_compatibility.h" 1
	rdinstreth t1
# 0 "" 2
 #NO_APP
	sw	t1,-28(s0)
	j	.L6
.L7:
	lw	t1,-28(s0)
	sw	t1,-20(s0)
 #APP
# 46 "./chipyard_compatibility.h" 1
	rdinstret  t1
# 0 "" 2
 #NO_APP
	sw	t1,-24(s0)
 #APP
# 47 "./chipyard_compatibility.h" 1
	rdinstreth t1
# 0 "" 2
 #NO_APP
	sw	t1,-28(s0)
.L6:
	lw	t3,-20(s0)
	lw	t1,-28(s0)
	bne	t3,t1,.L7
	lw	t1,-20(s0)
	mv	a6,t1
	li	a7,0
	slli	a5,a6,0
	li	a4,0
	lw	a6,-24(s0)
	mv	a2,a6
	li	a3,0
	or	a0,a4,a2
	or	a1,a5,a3
	mv	a4,a0
	mv	a5,a1
	mv	a0,a4
	mv	a1,a5
	lw	ra,28(sp)
	lw	s0,24(sp)
	addi	sp,sp,32
	jr	ra
	.size	read_instret, .-read_instret
	.align	1
	.globl	L_MODE
	.type	L_MODE, @function
L_MODE:
	addi	sp,sp,-48
	sw	ra,44(sp)
	sw	s0,40(sp)
	addi	s0,sp,48
	sw	a0,-36(s0)
	sw	a1,-40(s0)
	sw	a2,-44(s0)
	sw	a3,-48(s0)
	lw	a5,-40(s0)
	slli	a4,a5,1
	lw	a5,-36(s0)
	add	a4,a4,a5
	lw	a5,-48(s0)
	slli	a5,a5,4
	add	a5,a4,a5
	sw	a5,-20(s0)
	lw	a5,-44(s0)
	sw	a5,-24(s0)
	lw	a5,-20(s0)
	lw	a4,-24(s0)
 #APP
# 6 "./../include/custom.h" 1
	addi zero,zero,0
.insn r 0x77, 2, 4, a5, a5, a4
# 0 "" 2
 #NO_APP
	sw	a5,-28(s0)
	nop
	lw	ra,44(sp)
	lw	s0,40(sp)
	addi	sp,sp,48
	jr	ra
	.size	L_MODE, .-L_MODE
	.align	1
	.globl	L_SCNN
	.type	L_SCNN, @function
L_SCNN:
	addi	sp,sp,-48
	sw	ra,44(sp)
	sw	s0,40(sp)
	addi	s0,sp,48
	sw	a0,-36(s0)
	sw	a1,-40(s0)
	sw	a2,-44(s0)
	sw	a3,-48(s0)
	lw	a5,-40(s0)
	slli	a5,a5,16
	lw	a4,-44(s0)
	add	a5,a4,a5
	sw	a5,-20(s0)
	lw	a5,-48(s0)
	slli	a5,a5,16
	lw	a4,-36(s0)
	add	a5,a4,a5
	sw	a5,-24(s0)
	lw	a5,-24(s0)
	lw	a4,-20(s0)
 #APP
# 18 "./../include/custom.h" 1
	addi zero,zero,0
.insn r 0x77, 0, 0, a5, a5, a4
# 0 "" 2
 #NO_APP
	sw	a5,-28(s0)
	nop
	lw	ra,44(sp)
	lw	s0,40(sp)
	addi	sp,sp,48
	jr	ra
	.size	L_SCNN, .-L_SCNN
	.align	1
	.globl	SCNN4x4
	.type	SCNN4x4, @function
SCNN4x4:
	addi	sp,sp,-48
	sw	ra,44(sp)
	sw	s0,40(sp)
	addi	s0,sp,48
	sw	a0,-36(s0)
	sw	a1,-40(s0)
	lw	a5,-36(s0)
	lw	a4,-40(s0)
 #APP
# 28 "./../include/custom.h" 1
	addi zero,zero,0
.insn r 0x77, 0, 1, a5, a5, a4
# 0 "" 2
 #NO_APP
	sw	a5,-20(s0)
	nop
	lw	ra,44(sp)
	lw	s0,40(sp)
	addi	sp,sp,48
	jr	ra
	.size	SCNN4x4, .-SCNN4x4
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
# 39 "./../include/custom.h" 1
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
	addi	sp,sp,-48
	sw	ra,44(sp)
	sw	s0,40(sp)
	addi	s0,sp,48
	sw	a0,-36(s0)
	lw	a5,-36(s0)
	lw	a4,-20(s0)
 #APP
# 51 "./../include/custom.h" 1
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
	.size	SCNN_WB_INT, .-SCNN_WB_INT
	.align	1
	.globl	POOL_RI
	.type	POOL_RI, @function
POOL_RI:
	addi	sp,sp,-48
	sw	ra,44(sp)
	sw	s0,40(sp)
	addi	s0,sp,48
	sw	a0,-36(s0)
	lw	a5,-36(s0)
	lw	a4,-20(s0)
 #APP
# 63 "./../include/custom.h" 1
	addi zero,zero,0
.insn r 0x77, 2, 0, a5, a5, a4
# 0 "" 2
 #NO_APP
	sw	a5,-24(s0)
	nop
	lw	ra,44(sp)
	lw	s0,40(sp)
	addi	sp,sp,48
	jr	ra
	.size	POOL_RI, .-POOL_RI
	.align	1
	.globl	POOL
	.type	POOL, @function
POOL:
	addi	sp,sp,-48
	sw	ra,44(sp)
	sw	s0,40(sp)
	addi	s0,sp,48
	sw	a0,-36(s0)
	sw	zero,-20(s0)
	lw	a5,-36(s0)
	lw	a4,-20(s0)
 #APP
# 75 "./../include/custom.h" 1
	addi zero,zero,0
.insn r 0x77, 2, 1, a5, a5, a4
# 0 "" 2
 #NO_APP
	sw	a5,-24(s0)
	nop
	lw	ra,44(sp)
	lw	s0,40(sp)
	addi	sp,sp,48
	jr	ra
	.size	POOL, .-POOL
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
# 87 "./../include/custom.h" 1
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
# 98 "./../include/custom.h" 1
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
	.globl	record
	.type	record, @function
record:
	addi	sp,sp,-32
	sw	ra,28(sp)
	sw	s0,24(sp)
	addi	s0,sp,32
 #APP
# 109 "./../include/custom.h" 1
	rdcycle a5
# 0 "" 2
 #NO_APP
	sw	a5,-20(s0)
	lw	a5,-20(s0)
	mv	a0,a5
	lw	ra,28(sp)
	lw	s0,24(sp)
	addi	sp,sp,32
	jr	ra
	.size	record, .-record
	.local	A_tile
	.comm	A_tile,128,128
	.local	B_tile
	.comm	B_tile,128,128
	.local	C_tile
	.comm	C_tile,64,128
	.local	A
	.comm	A,1024,128
	.local	B
	.comm	B,1024,128
	.local	C_hw
	.comm	C_hw,4096,128
	.local	accel_cycles_accum
	.comm	accel_cycles_accum,8,8
	.local	accel_instret_accum
	.comm	accel_instret_accum,8,8
	.align	1
	.globl	gemm
	.type	gemm, @function
gemm:
	addi	sp,sp,-128
	sw	ra,124(sp)
	sw	s0,120(sp)
	addi	s0,sp,128
	sw	a0,-100(s0)
	sw	a1,-104(s0)
	sw	a2,-108(s0)
	sw	a3,-112(s0)
	sw	a4,-116(s0)
	sw	a5,-120(s0)
	sw	zero,-20(s0)
	j	.L21
.L38:
	sw	zero,-24(s0)
	j	.L22
.L37:
	sw	zero,-28(s0)
	j	.L23
.L26:
	sw	zero,-32(s0)
	j	.L24
.L25:
	lw	a4,-28(s0)
	lw	a5,-100(s0)
	mul	a4,a4,a5
	lw	a5,-20(s0)
	slli	a5,a5,2
	add	a4,a4,a5
	lw	a5,-32(s0)
	add	a5,a4,a5
	mv	a4,a5
	lw	a5,-112(s0)
	add	a5,a5,a4
	lw	a4,-28(s0)
	slli	a3,a4,2
	lw	a4,-32(s0)
	add	a3,a3,a4
	lb	a4,0(a5)
	lui	a5,%hi(A_tile)
	addi	a5,a5,%lo(A_tile)
	add	a5,a3,a5
	sb	a4,0(a5)
	lw	a5,-32(s0)
	addi	a5,a5,1
	sw	a5,-32(s0)
.L24:
	lw	a4,-32(s0)
	li	a5,3
	ble	a4,a5,.L25
	lw	a5,-28(s0)
	addi	a5,a5,1
	sw	a5,-28(s0)
.L23:
	lw	a4,-28(s0)
	lw	a5,-108(s0)
	blt	a4,a5,.L26
	sw	zero,-36(s0)
	j	.L27
.L30:
	sw	zero,-40(s0)
	j	.L28
.L29:
	lw	a4,-36(s0)
	lw	a5,-104(s0)
	mul	a4,a4,a5
	lw	a5,-24(s0)
	slli	a5,a5,2
	add	a4,a4,a5
	lw	a5,-40(s0)
	add	a5,a4,a5
	mv	a4,a5
	lw	a5,-116(s0)
	add	a5,a5,a4
	lw	a4,-36(s0)
	slli	a3,a4,2
	lw	a4,-40(s0)
	add	a3,a3,a4
	lb	a4,0(a5)
	lui	a5,%hi(B_tile)
	addi	a5,a5,%lo(B_tile)
	add	a5,a3,a5
	sb	a4,0(a5)
	lw	a5,-40(s0)
	addi	a5,a5,1
	sw	a5,-40(s0)
.L28:
	lw	a4,-40(s0)
	li	a5,3
	ble	a4,a5,.L29
	lw	a5,-36(s0)
	addi	a5,a5,1
	sw	a5,-36(s0)
.L27:
	lw	a4,-36(s0)
	lw	a5,-108(s0)
	blt	a4,a5,.L30
	sw	zero,-44(s0)
	j	.L31
.L32:
	lui	a5,%hi(C_tile)
	lw	a4,-44(s0)
	slli	a4,a4,2
	addi	a5,a5,%lo(C_tile)
	add	a5,a4,a5
	sw	zero,0(a5)
	lw	a5,-44(s0)
	addi	a5,a5,1
	sw	a5,-44(s0)
.L31:
	lw	a4,-44(s0)
	li	a5,15
	ble	a4,a5,.L32
	call	read_cycles
	sw	a0,-64(s0)
	sw	a1,-60(s0)
	call	read_instret
	sw	a0,-72(s0)
	sw	a1,-68(s0)
	li	a3,0
	li	a2,4
	li	a1,4
	lw	a0,-108(s0)
	call	L_SCNN
	lui	a5,%hi(B_tile)
	addi	a1,a5,%lo(B_tile)
	lui	a5,%hi(A_tile)
	addi	a0,a5,%lo(A_tile)
	call	SCNN4x4
	lui	a5,%hi(C_tile)
	addi	a0,a5,%lo(C_tile)
	call	SCNN_WB_INT
	call	read_cycles
	sw	a0,-80(s0)
	sw	a1,-76(s0)
	call	read_instret
	sw	a0,-88(s0)
	sw	a1,-84(s0)
	lw	a4,-80(s0)
	lw	a5,-76(s0)
	lw	a0,-64(s0)
	lw	a1,-60(s0)
	sub	a2,a4,a0
	mv	a6,a2
	sgtu	a6,a6,a4
	sub	a3,a5,a1
	sub	a5,a3,a6
	mv	a3,a5
	lui	a5,%hi(accel_cycles_accum)
	lw	a0,%lo(accel_cycles_accum)(a5)
	lw	a1,%lo(accel_cycles_accum+4)(a5)
	add	a4,a2,a0
	mv	a6,a4
	sltu	a6,a6,a2
	add	a5,a3,a1
	add	a3,a6,a5
	mv	a5,a3
	lui	a3,%hi(accel_cycles_accum)
	sw	a4,%lo(accel_cycles_accum)(a3)
	sw	a5,%lo(accel_cycles_accum+4)(a3)
	lw	a4,-88(s0)
	lw	a5,-84(s0)
	lw	a0,-72(s0)
	lw	a1,-68(s0)
	sub	a2,a4,a0
	mv	a6,a2
	sgtu	a6,a6,a4
	sub	a3,a5,a1
	sub	a5,a3,a6
	mv	a3,a5
	lui	a5,%hi(accel_instret_accum)
	lw	a0,%lo(accel_instret_accum)(a5)
	lw	a1,%lo(accel_instret_accum+4)(a5)
	add	a4,a2,a0
	mv	a6,a4
	sltu	a6,a6,a2
	add	a5,a3,a1
	add	a3,a6,a5
	mv	a5,a3
	lui	a3,%hi(accel_instret_accum)
	sw	a4,%lo(accel_instret_accum)(a3)
	sw	a5,%lo(accel_instret_accum+4)(a3)
	sw	zero,-48(s0)
	j	.L33
.L36:
	sw	zero,-52(s0)
	j	.L34
.L35:
	lw	a5,-48(s0)
	slli	a4,a5,2
	lw	a5,-52(s0)
	add	a3,a4,a5
	lw	a5,-20(s0)
	slli	a4,a5,2
	lw	a5,-48(s0)
	add	a4,a4,a5
	lw	a5,-104(s0)
	mul	a4,a4,a5
	lw	a5,-24(s0)
	slli	a2,a5,2
	lw	a5,-52(s0)
	add	a5,a2,a5
	add	a5,a4,a5
	slli	a5,a5,2
	lw	a4,-120(s0)
	add	a5,a4,a5
	lui	a4,%hi(C_tile)
	slli	a3,a3,2
	addi	a4,a4,%lo(C_tile)
	add	a4,a3,a4
	lw	a4,0(a4)
	sw	a4,0(a5)
	lw	a5,-52(s0)
	addi	a5,a5,1
	sw	a5,-52(s0)
.L34:
	lw	a4,-52(s0)
	li	a5,3
	ble	a4,a5,.L35
	lw	a5,-48(s0)
	addi	a5,a5,1
	sw	a5,-48(s0)
.L33:
	lw	a4,-48(s0)
	li	a5,3
	ble	a4,a5,.L36
	lw	a5,-24(s0)
	addi	a5,a5,1
	sw	a5,-24(s0)
.L22:
	lw	a5,-104(s0)
	srai	a4,a5,31
	andi	a4,a4,3
	add	a5,a4,a5
	srai	a5,a5,2
	mv	a4,a5
	lw	a5,-24(s0)
	blt	a5,a4,.L37
	lw	a5,-20(s0)
	addi	a5,a5,1
	sw	a5,-20(s0)
.L21:
	lw	a5,-100(s0)
	srai	a4,a5,31
	andi	a4,a4,3
	add	a5,a4,a5
	srai	a5,a5,2
	mv	a4,a5
	lw	a5,-20(s0)
	blt	a5,a4,.L38
	nop
	nop
	lw	ra,124(sp)
	lw	s0,120(sp)
	addi	sp,sp,128
	jr	ra
	.size	gemm, .-gemm
	.align	1
	.globl	gemm_unaccel
	.type	gemm_unaccel, @function
gemm_unaccel:
	addi	sp,sp,-64
	sw	ra,60(sp)
	sw	s0,56(sp)
	addi	s0,sp,64
	sw	a0,-36(s0)
	sw	a1,-40(s0)
	sw	a2,-44(s0)
	sw	a3,-48(s0)
	sw	a4,-52(s0)
	sw	a5,-56(s0)
	sw	zero,-20(s0)
	j	.L40
.L41:
	lw	a5,-20(s0)
	slli	a5,a5,2
	lw	a4,-56(s0)
	add	a5,a4,a5
	sw	zero,0(a5)
	lw	a5,-20(s0)
	addi	a5,a5,1
	sw	a5,-20(s0)
.L40:
	lw	a4,-36(s0)
	lw	a5,-40(s0)
	mul	a5,a4,a5
	lw	a4,-20(s0)
	blt	a4,a5,.L41
	sw	zero,-24(s0)
	j	.L42
.L47:
	sw	zero,-28(s0)
	j	.L43
.L46:
	sw	zero,-32(s0)
	j	.L44
.L45:
	lw	a4,-24(s0)
	lw	a5,-40(s0)
	mul	a4,a4,a5
	lw	a5,-28(s0)
	add	a5,a4,a5
	slli	a5,a5,2
	lw	a4,-56(s0)
	add	a5,a4,a5
	lw	a3,0(a5)
	lw	a4,-32(s0)
	lw	a5,-36(s0)
	mul	a4,a4,a5
	lw	a5,-24(s0)
	add	a5,a4,a5
	mv	a4,a5
	lw	a5,-48(s0)
	add	a5,a5,a4
	lb	a5,0(a5)
	mv	a2,a5
	lw	a4,-32(s0)
	lw	a5,-40(s0)
	mul	a4,a4,a5
	lw	a5,-28(s0)
	add	a5,a4,a5
	mv	a4,a5
	lw	a5,-52(s0)
	add	a5,a5,a4
	lb	a5,0(a5)
	mul	a4,a2,a5
	lw	a2,-24(s0)
	lw	a5,-40(s0)
	mul	a2,a2,a5
	lw	a5,-28(s0)
	add	a5,a2,a5
	slli	a5,a5,2
	lw	a2,-56(s0)
	add	a5,a2,a5
	add	a4,a3,a4
	sw	a4,0(a5)
	lw	a5,-32(s0)
	addi	a5,a5,1
	sw	a5,-32(s0)
.L44:
	lw	a4,-32(s0)
	lw	a5,-44(s0)
	blt	a4,a5,.L45
	lw	a5,-28(s0)
	addi	a5,a5,1
	sw	a5,-28(s0)
.L43:
	lw	a4,-28(s0)
	lw	a5,-40(s0)
	blt	a4,a5,.L46
	lw	a5,-24(s0)
	addi	a5,a5,1
	sw	a5,-24(s0)
.L42:
	lw	a4,-24(s0)
	lw	a5,-36(s0)
	blt	a4,a5,.L47
	nop
	nop
	lw	ra,60(sp)
	lw	s0,56(sp)
	addi	sp,sp,64
	jr	ra
	.size	gemm_unaccel, .-gemm_unaccel
	.section	.rodata
	.align	2
.LC0:
	.string	"%s:\n"
	.align	2
.LC1:
	.string	"  total          : cycles=%llu  instret=%llu\n"
	.align	2
.LC2:
	.string	"  accel only     : cycles=%llu  instret=%llu\n"
	.align	2
.LC3:
	.string	"  gather/scatter : cycles=%llu  instret=%llu\n"
	.align	2
.LC4:
	.string	"  tiles          : %d  (accel cycles/tile=%llu)\n"
	.text
	.align	1
	.globl	instrument
	.type	instrument, @function
instrument:
	addi	sp,sp,-128
	sw	ra,124(sp)
	sw	s0,120(sp)
	sw	s2,116(sp)
	sw	s3,112(sp)
	addi	s0,sp,128
	sw	a0,-116(s0)
	sw	a1,-120(s0)
	sw	a2,-124(s0)
	sw	a3,-128(s0)
	sw	zero,-20(s0)
	j	.L49
.L50:
	lw	a3,-20(s0)
	li	a5,780902400
	addi	a5,a5,745
	mulh	a5,a3,a5
	srai	a4,a5,1
	srai	a5,a3,31
	sub	a4,a4,a5
	mv	a5,a4
	slli	a5,a5,1
	add	a5,a5,a4
	slli	a5,a5,2
	sub	a5,a5,a4
	sub	a4,a3,a5
	andi	a5,a4,0xff
	addi	a5,a5,-5
	andi	a5,a5,0xff
	slli	a4,a5,24
	srai	a4,a4,24
	lui	a5,%hi(A)
	addi	a3,a5,%lo(A)
	lw	a5,-20(s0)
	add	a5,a3,a5
	sb	a4,0(a5)
	lw	a5,-20(s0)
	addi	a5,a5,1
	sw	a5,-20(s0)
.L49:
	lw	a4,-120(s0)
	lw	a5,-128(s0)
	mul	a5,a4,a5
	lw	a4,-20(s0)
	blt	a4,a5,.L50
	sw	zero,-24(s0)
	j	.L51
.L52:
	lw	a4,-24(s0)
	li	a5,954437632
	addi	a5,a5,-455
	mulh	a5,a4,a5
	srai	a3,a5,1
	srai	a5,a4,31
	sub	a3,a3,a5
	mv	a5,a3
	slli	a5,a5,3
	add	a5,a5,a3
	sub	a3,a4,a5
	andi	a5,a3,0xff
	addi	a5,a5,-4
	andi	a5,a5,0xff
	slli	a4,a5,24
	srai	a4,a4,24
	lui	a5,%hi(B)
	addi	a3,a5,%lo(B)
	lw	a5,-24(s0)
	add	a5,a3,a5
	sb	a4,0(a5)
	lw	a5,-24(s0)
	addi	a5,a5,1
	sw	a5,-24(s0)
.L51:
	lw	a4,-128(s0)
	lw	a5,-124(s0)
	mul	a5,a4,a5
	lw	a4,-24(s0)
	blt	a4,a5,.L52
	sw	zero,-28(s0)
	j	.L53
.L54:
	lui	a5,%hi(C_hw)
	addi	a4,a5,%lo(C_hw)
	lw	a5,-28(s0)
	slli	a5,a5,2
	add	a5,a4,a5
	sw	zero,0(a5)
	lw	a5,-28(s0)
	addi	a5,a5,1
	sw	a5,-28(s0)
.L53:
	lw	a4,-120(s0)
	lw	a5,-124(s0)
	mul	a5,a4,a5
	lw	a4,-28(s0)
	blt	a4,a5,.L54
	lui	a5,%hi(accel_cycles_accum)
	li	a3,0
	li	a4,0
	sw	a3,%lo(accel_cycles_accum)(a5)
	sw	a4,%lo(accel_cycles_accum+4)(a5)
	lui	a5,%hi(accel_instret_accum)
	sw	a3,%lo(accel_instret_accum)(a5)
	sw	a4,%lo(accel_instret_accum+4)(a5)
	call	read_cycles
	sw	a0,-56(s0)
	sw	a1,-52(s0)
	call	read_instret
	sw	a0,-64(s0)
	sw	a1,-60(s0)
	lui	a5,%hi(C_hw)
	addi	a5,a5,%lo(C_hw)
	lui	a4,%hi(B)
	addi	a4,a4,%lo(B)
	lui	a3,%hi(A)
	addi	a3,a3,%lo(A)
	lw	a2,-128(s0)
	lw	a1,-124(s0)
	lw	a0,-120(s0)
	call	gemm
	call	read_cycles
	sw	a0,-72(s0)
	sw	a1,-68(s0)
	call	read_instret
	sw	a0,-80(s0)
	sw	a1,-76(s0)
	lw	a2,-72(s0)
	lw	a3,-68(s0)
	lw	a0,-56(s0)
	lw	a1,-52(s0)
	sub	a4,a2,a0
	mv	a6,a4
	sgtu	a6,a6,a2
	sub	a5,a3,a1
	sub	a3,a5,a6
	mv	a5,a3
	sw	a4,-88(s0)
	sw	a5,-84(s0)
	lw	a2,-80(s0)
	lw	a3,-76(s0)
	lw	a0,-64(s0)
	lw	a1,-60(s0)
	sub	a4,a2,a0
	mv	a6,a4
	sgtu	a6,a6,a2
	sub	a5,a3,a1
	sub	a3,a5,a6
	mv	a5,a3
	sw	a4,-96(s0)
	sw	a5,-92(s0)
	lui	a5,%hi(accel_cycles_accum)
	lw	a4,%lo(accel_cycles_accum)(a5)
	lw	a5,%lo(accel_cycles_accum+4)(a5)
	lw	a3,-84(s0)
	mv	a2,a5
	bltu	a3,a2,.L55
	lw	a3,-84(s0)
	mv	a2,a5
	bne	a3,a2,.L63
	lw	a3,-88(s0)
	mv	a5,a4
	bltu	a3,a5,.L55
.L63:
	lui	a5,%hi(accel_cycles_accum)
	lw	a0,%lo(accel_cycles_accum)(a5)
	lw	a1,%lo(accel_cycles_accum+4)(a5)
	lw	a2,-88(s0)
	lw	a3,-84(s0)
	sub	a4,a2,a0
	mv	a6,a4
	sgtu	a6,a6,a2
	sub	a5,a3,a1
	sub	a3,a5,a6
	mv	a5,a3
	sw	a4,-40(s0)
	sw	a5,-36(s0)
	j	.L57
.L55:
	li	a5,0
	li	a6,0
	sw	a5,-40(s0)
	sw	a6,-36(s0)
.L57:
	lui	a5,%hi(accel_instret_accum)
	lw	a4,%lo(accel_instret_accum)(a5)
	lw	a5,%lo(accel_instret_accum+4)(a5)
	lw	a3,-92(s0)
	mv	a2,a5
	bltu	a3,a2,.L58
	lw	a3,-92(s0)
	mv	a2,a5
	bne	a3,a2,.L64
	lw	a3,-96(s0)
	mv	a5,a4
	bltu	a3,a5,.L58
.L64:
	lui	a5,%hi(accel_instret_accum)
	lw	a0,%lo(accel_instret_accum)(a5)
	lw	a1,%lo(accel_instret_accum+4)(a5)
	lw	a2,-96(s0)
	lw	a3,-92(s0)
	sub	a4,a2,a0
	mv	a6,a4
	sgtu	a6,a6,a2
	sub	a5,a3,a1
	sub	a3,a5,a6
	mv	a5,a3
	sw	a4,-48(s0)
	sw	a5,-44(s0)
	j	.L60
.L58:
	li	a5,0
	li	a6,0
	sw	a5,-48(s0)
	sw	a6,-44(s0)
.L60:
	lw	a5,-120(s0)
	srai	a4,a5,31
	andi	a4,a4,3
	add	a5,a4,a5
	srai	a5,a5,2
	mv	a3,a5
	lw	a5,-124(s0)
	srai	a4,a5,31
	andi	a4,a4,3
	add	a5,a4,a5
	srai	a5,a5,2
	mul	a5,a3,a5
	sw	a5,-100(s0)
	lw	a1,-116(s0)
	lui	a5,%hi(.LC0)
	addi	a0,a5,%lo(.LC0)
	call	printf
	lw	a4,-96(s0)
	lw	a5,-92(s0)
	lw	a2,-88(s0)
	lw	a3,-84(s0)
	lui	a1,%hi(.LC1)
	addi	a0,a1,%lo(.LC1)
	call	printf
	lui	a5,%hi(accel_cycles_accum)
	lw	a2,%lo(accel_cycles_accum)(a5)
	lw	a3,%lo(accel_cycles_accum+4)(a5)
	lui	a5,%hi(accel_instret_accum)
	lw	a4,%lo(accel_instret_accum)(a5)
	lw	a5,%lo(accel_instret_accum+4)(a5)
	lui	a1,%hi(.LC2)
	addi	a0,a1,%lo(.LC2)
	call	printf
	lw	a4,-48(s0)
	lw	a5,-44(s0)
	lw	a2,-40(s0)
	lw	a3,-36(s0)
	lui	a1,%hi(.LC3)
	addi	a0,a1,%lo(.LC3)
	call	printf
	lw	a5,-100(s0)
	beq	a5,zero,.L61
	lui	a5,%hi(accel_cycles_accum)
	lw	a4,%lo(accel_cycles_accum)(a5)
	lw	a5,%lo(accel_cycles_accum+4)(a5)
	lw	a3,-100(s0)
	mv	s2,a3
	srai	a3,a3,31
	mv	s3,a3
	mv	a2,s2
	mv	a3,s3
	mv	a0,a4
	mv	a1,a5
	call	__udivdi3
	mv	a4,a0
	mv	a5,a1
	j	.L62
.L61:
	li	a4,0
	li	a5,0
.L62:
	mv	a2,a4
	mv	a3,a5
	lw	a1,-100(s0)
	lui	a5,%hi(.LC4)
	addi	a0,a5,%lo(.LC4)
	call	printf
	nop
	lw	ra,124(sp)
	lw	s0,120(sp)
	lw	s2,116(sp)
	lw	s3,112(sp)
	addi	sp,sp,128
	jr	ra
	.size	instrument, .-instrument
	.section	.rodata
	.align	2
.LC5:
	.string	"4x4 * 4x4"
	.align	2
.LC6:
	.string	"8x8 * 8x8"
	.align	2
.LC7:
	.string	"16x16 * 16x16"
	.align	2
.LC8:
	.string	"32x32 * 32x32"
	.text
	.align	1
	.globl	main
	.type	main, @function
main:
	addi	sp,sp,-16
	sw	ra,12(sp)
	sw	s0,8(sp)
	addi	s0,sp,16
	li	a3,4
	li	a2,4
	li	a1,4
	lui	a5,%hi(.LC5)
	addi	a0,a5,%lo(.LC5)
	call	instrument
	li	a3,8
	li	a2,8
	li	a1,8
	lui	a5,%hi(.LC6)
	addi	a0,a5,%lo(.LC6)
	call	instrument
	li	a3,16
	li	a2,16
	li	a1,16
	lui	a5,%hi(.LC7)
	addi	a0,a5,%lo(.LC7)
	call	instrument
	li	a3,32
	li	a2,32
	li	a1,32
	lui	a5,%hi(.LC8)
	addi	a0,a5,%lo(.LC8)
	call	instrument
	li	a5,0
	mv	a0,a5
	lw	ra,12(sp)
	lw	s0,8(sp)
	addi	sp,sp,16
	jr	ra
	.size	main, .-main
	.globl	__udivdi3
	.ident	"GCC: (g5115c7e44) 15.2.0"
	.section	.note.GNU-stack,"",@progbits
