[INSTRSET "i486p"]
[BITS 32]
	MOV		ECX, msg
	MOV		EDX, 1
putloop:
	MOV		AL, [CS:ECX]
	CMP		AL, 0
	JE		fin
	INT		0X40
	ADD		ECX, 1
	JMP		putloop
fin:
	MOV		EDX, 4
	INT		0x40

msg:
	DB	"Hello World", 0