GLOBL	_tos(SB), $4

TEXT	_main(SB), 1, $0
	ADD	$-8, R29
	MOVW	$setR30(SB), R30
	MOVW	R1, _tos(SB)

	MOVW	$_apemain(SB), R1
	MOVW	$0, R31
	JMP	_callmain(SB)
