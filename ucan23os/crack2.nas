[INSTRSET "i486p"]
[BITS 32]
		MOV		EAX,1*8			; ����ϵͳ�õĶκ�
		MOV		DS,AX			; �������DS
		MOV		BYTE [0x102600],0
		MOV		EDX, 4
		INT		0x40
