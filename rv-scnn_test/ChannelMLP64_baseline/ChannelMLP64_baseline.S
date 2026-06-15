	.file	"matvec_baseline64.c"
	.option nopic
	.attribute arch, "rv32i2p1_m2p0_c2p0_zicsr2p0"
	.attribute unaligned_access, 0
	.attribute stack_align, 16
	.text
	.align	1
	.type	gemm_plain.constprop.0, @function
gemm_plain.constprop.0:
	mv	t1,a0
	mv	t2,a4
	mv	t4,a1
	mv	t3,a2
	mv	a0,a3
	add	a6,t1,a4
	li	t0,4096
	li	t6,-128
	li	t5,127
.L2:
	li	a7,0
.L8:
	add	a3,t4,a7
	mv	a5,t1
	li	a2,0
.L3:
	lb	a4,0(a5)
	lb	a1,0(a3)
	addi	a5,a5,1
	add	a3,a3,a0
	mul	a4,a4,a1
	add	a2,a2,a4
	bne	a6,a5,.L3
	add	a5,t3,a7
	bge	a2,t6,.L4
	li	a2,-128
.L4:
	ble	a2,t5,.L5
	li	a2,127
.L5:
	sb	a2,0(a5)
	addi	a7,a7,1
	bne	a0,a7,.L8
	addi	t0,t0,-1
	add	t3,t3,a0
	add	t1,t1,t2
	add	a6,a6,t2
	bne	t0,zero,.L2
	ret
	.size	gemm_plain.constprop.0, .-gemm_plain.constprop.0
	.section	.rodata.str1.4,"aMS",@progbits,1
	.align	2
.LC0:
	.string	"PASS"
	.align	2
.LC1:
	.string	"FAIL"
	.align	2
.LC2:
	.string	"=== Baseline bare-metal: matvec ChannelMLP (sin acelerar) ==="
	.align	2
.LC3:
	.string	"SZCH=%d  L=%d  CIN0=%d  CMID=%d  COUT=%d\n"
	.align	2
.LC4:
	.string	"Formato: int8 (elem_t), acumulador int32 (acc_t)"
	.align	2
.LC5:
	.string	"Granularidad: GEMM apilado, I=L=%d\n\n"
	.align	2
.LC6:
	.string	"Baseline matvec: %llu cycles, %llu instructions\n"
	.align	2
.LC7:
	.string	"IPC = %d.%03d\n"
	.align	2
.LC8:
	.string	"Verificacion CPU: %s\n"
	.align	2
.LC9:
	.string	"C2[fila 0] col0..3 = %d %d %d %d  (esperado 0)\n"
	.align	2
.LC10:
	.string	"C2[fila 5] col0..3 = %d %d %d %d  (esperado 60)\n"
	.align	2
.LC11:
	.string	"C2[fila %d] col0..3 = %d %d %d %d  (esperado %d)\n"
	.align	2
.LC12:
	.string	"\n=== Fin baseline ==="
	.section	.text.startup,"ax",@progbits
	.align	1
	.globl	main
	.type	main, @function
main:
	lui	a0,%hi(.LC2)
	addi	sp,sp,-48
	addi	a0,a0,%lo(.LC2)
	sw	ra,44(sp)
	sw	s0,40(sp)
	sw	s1,36(sp)
	sw	s6,16(sp)
	sw	s2,32(sp)
	sw	s3,28(sp)
	sw	s4,24(sp)
	sw	s5,20(sp)
	sw	s7,12(sp)
	sw	s8,8(sp)
	sw	s9,4(sp)
	call	puts
	li	a5,24
	lui	a0,%hi(.LC3)
	li	a2,4096
	mv	a3,a5
	li	a4,12
	li	a1,64
	addi	a0,a0,%lo(.LC3)
	call	printf
	lui	a0,%hi(.LC4)
	addi	a0,a0,%lo(.LC4)
	call	puts
	lui	a0,%hi(.LC5)
	addi	a0,a0,%lo(.LC5)
	li	a1,4096
	call	printf
	lui	s6,%hi(heap.0)
	addi	s1,s6,%lo(heap.0)
	addi	s0,s6,%lo(heap.0)
	addi	a2,s6,%lo(heap.0)
	li	a7,0
	li	a6,0
	li	a0,24
	li	t1,98304
.L13:
	andi	a1,a6,7
	li	a5,0
.L12:
	add	a4,a2,a5
	sgt	a3,a1,a5
	sb	a3,0(a4)
	addi	a5,a5,1
	bne	a5,a0,.L12
	addi	a7,a7,24
	addi	a6,a6,1
	addi	a2,a2,24
	bne	a7,t1,.L13
	lui	s9,%hi(heap.0+98304)
	addi	a5,s9,%lo(heap.0+98304)
	li	a4,16842752
	addi	a3,a5,288
	addi	a4,a4,257
.L14:
	sw	a4,0(a5)
	sw	a4,4(a5)
	sw	a4,8(a5)
	addi	a5,a5,12
	bne	a3,a5,.L14
	lui	s8,%hi(heap.0+147744)
	addi	a0,s8,%lo(heap.0+147744)
	li	a2,288
	li	a1,1
	call	memset
 #APP
# 20 "chipyard_compatibility.h" 1
	rdcycleh s3
# 0 "" 2
# 21 "chipyard_compatibility.h" 1
	rdcycle  s5
# 0 "" 2
# 22 "chipyard_compatibility.h" 1
	rdcycleh a5
# 0 "" 2
 #NO_APP
	beq	s3,a5,.L15
.L16:
 #APP
# 26 "chipyard_compatibility.h" 1
	rdcycle  s5
# 0 "" 2
 #NO_APP
	mv	s3,a5
 #APP
# 27 "chipyard_compatibility.h" 1
	rdcycleh a5
# 0 "" 2
 #NO_APP
	bne	a5,s3,.L16
.L15:
 #APP
# 40 "chipyard_compatibility.h" 1
	rdinstreth s2
# 0 "" 2
# 41 "chipyard_compatibility.h" 1
	rdinstret  s4
# 0 "" 2
# 42 "chipyard_compatibility.h" 1
	rdinstreth a5
# 0 "" 2
 #NO_APP
	beq	s2,a5,.L17
.L18:
 #APP
# 46 "chipyard_compatibility.h" 1
	rdinstret  s4
# 0 "" 2
 #NO_APP
	mv	s2,a5
 #APP
# 47 "chipyard_compatibility.h" 1
	rdinstreth a5
# 0 "" 2
 #NO_APP
	bne	a5,s2,.L18
.L17:
	lui	s7,%hi(heap.0+98592)
	addi	a1,s9,%lo(heap.0+98304)
	addi	a0,s6,%lo(heap.0)
	addi	a2,s7,%lo(heap.0+98592)
	li	a4,24
	li	a3,12
	call	gemm_plain.constprop.0
	lui	a2,%hi(heap.0+148032)
	addi	a1,s8,%lo(heap.0+147744)
	addi	a0,s7,%lo(heap.0+98592)
	addi	a2,a2,%lo(heap.0+148032)
	li	a4,12
	li	a3,24
	call	gemm_plain.constprop.0
 #APP
# 40 "chipyard_compatibility.h" 1
	rdinstreth a5
# 0 "" 2
# 41 "chipyard_compatibility.h" 1
	rdinstret  a2
# 0 "" 2
# 42 "chipyard_compatibility.h" 1
	rdinstreth a4
# 0 "" 2
 #NO_APP
	beq	a5,a4,.L19
.L20:
 #APP
# 46 "chipyard_compatibility.h" 1
	rdinstret  a2
# 0 "" 2
 #NO_APP
	mv	a5,a4
 #APP
# 47 "chipyard_compatibility.h" 1
	rdinstreth a4
# 0 "" 2
 #NO_APP
	bne	a4,a5,.L20
.L19:
 #APP
# 20 "chipyard_compatibility.h" 1
	rdcycleh a3
# 0 "" 2
# 21 "chipyard_compatibility.h" 1
	rdcycle  a1
# 0 "" 2
# 22 "chipyard_compatibility.h" 1
	rdcycleh a4
# 0 "" 2
 #NO_APP
	beq	a3,a4,.L21
.L22:
 #APP
# 26 "chipyard_compatibility.h" 1
	rdcycle  a1
# 0 "" 2
 #NO_APP
	mv	a3,a4
 #APP
# 27 "chipyard_compatibility.h" 1
	rdcycleh a4
# 0 "" 2
 #NO_APP
	bne	a4,a3,.L22
.L21:
	sub	s5,a1,s5
	sub	s4,a2,s4
	sgtu	a1,s5,a1
	sgtu	a2,s4,a2
	sub	a3,a3,s3
	sub	a5,a5,s2
	sub	s3,a3,a1
	sub	s2,a5,a2
	lui	a0,%hi(.LC6)
	mv	a5,s2
	mv	a3,s3
	mv	a4,s4
	mv	a2,s5
	addi	a0,a0,%lo(.LC6)
	call	printf
	or	a5,s5,s3
	bne	a5,zero,.L49
.L23:
	li	a0,10
	call	putchar
	li	a5,147456
	addi	a5,a5,600
	add	a2,s1,a5
	li	a1,0
	li	a0,4096
.L28:
	andi	a5,a1,7
	slli	a4,a5,1
	add	a5,a5,a4
	slli	a3,a5,2
	addi	a5,a2,-24
	j	.L26
.L51:
	beq	a5,a2,.L50
.L26:
	lb	a4,0(a5)
	addi	a5,a5,1
	beq	a4,a3,.L51
	lui	a5,%hi(.LC1)
	addi	a1,a5,%lo(.LC1)
	li	s2,1
.L25:
	lui	a0,%hi(.LC8)
	addi	a0,a0,%lo(.LC8)
	li	s1,147456
	call	printf
	add	s1,s0,s1
	lb	a4,579(s1)
	lb	a3,578(s1)
	lb	a2,577(s1)
	lb	a1,576(s1)
	lui	a0,%hi(.LC9)
	addi	a0,a0,%lo(.LC9)
	call	printf
	lb	a4,699(s1)
	lb	a3,698(s1)
	lb	a2,697(s1)
	lb	a1,696(s1)
	lui	a0,%hi(.LC10)
	addi	a0,a0,%lo(.LC10)
	call	printf
	li	a2,245760
	add	a2,s0,a2
	lb	a5,555(a2)
	lb	a4,554(a2)
	lb	a3,553(a2)
	lb	a2,552(a2)
	li	a1,4096
	lui	a0,%hi(.LC11)
	addi	a1,a1,-1
	li	a6,84
	addi	a0,a0,%lo(.LC11)
	call	printf
	lui	a0,%hi(.LC12)
	addi	a0,a0,%lo(.LC12)
	call	puts
	lw	ra,44(sp)
	lw	s0,40(sp)
	lw	s1,36(sp)
	lw	s3,28(sp)
	lw	s4,24(sp)
	lw	s5,20(sp)
	lw	s6,16(sp)
	lw	s7,12(sp)
	lw	s8,8(sp)
	lw	s9,4(sp)
	mv	a0,s2
	lw	s2,32(sp)
	addi	sp,sp,48
	jr	ra
.L50:
	addi	a1,a1,1
	beq	a1,a0,.L52
	addi	a2,a2,24
	j	.L28
.L49:
	li	a0,1000
	mulhu	a5,s4,a0
	mv	a2,s5
	mv	a3,s3
	mul	a1,s2,a0
	mul	a0,s4,a0
	add	a1,a1,a5
	call	__udivdi3
	li	a2,1000
	li	a3,0
	call	__umoddi3
	mv	a5,a0
	mv	a2,s5
	mv	a3,s3
	mv	a1,s2
	mv	a0,s4
	mv	s2,a5
	call	__udivdi3
	mv	a1,a0
	lui	a0,%hi(.LC7)
	mv	a2,s2
	addi	a0,a0,%lo(.LC7)
	call	printf
	j	.L23
.L52:
	lui	a5,%hi(.LC0)
	addi	a1,a5,%lo(.LC0)
	li	s2,0
	j	.L25
	.size	main, .-main
	.bss
	.align	2
	.type	heap.0, @object
	.size	heap.0, 250432
heap.0:
	.zero	250432
	.globl	__umoddi3
	.globl	__udivdi3
	.ident	"GCC: ('corev-openhw-gcc-ubuntu2204-20240530') 14.1.0"
	.section	.note.GNU-stack,"",@progbits
