; ucan23-os boot asm
; TAB=4
; 此程序前一个版本存在的错误：将skip写成了ship
; 将[INSTRSET "i486p"]写成了[INSTREST "i486p"]
; 将waitkbdout写成了waitkdbout

[INSTRSET "i486p"]

VBEMODE	EQU		0x105

BOTPAK	EQU		0x00280000
DSKCAC	EQU		0x00100000
DSKCAC0	EQU		0x00008000

; 有关BOOT_INFO
CYLS	EQU		0x0ff0
LEDS	EQU		0x0ff1
VMODE	EQU		0x0ff2
SCRNX	EQU		0x0ff4
SCRNY	EQU		0x0ff6
VRAM	EQU 	0x0ff8
		
		ORG		0xc200

;确认VBE是否存在

		MOV		AX, 0x9000
		MOV		ES, AX
		MOV		DI, 0
		MOV		AX, 0x4f00
		INT		0x10
		CMP		AX, 0x004f
;		JNE		scrn320 ;为了使用320*200模式这里做出了修改
		JMP		scrn320
		
;检查VBE的版本

		MOV		AX, [ES:DI+4]
		CMP		AX, 0x0200
		JB		scrn320
		
;取得画面模式信息

		MOV		CX, VBEMODE
		MOV		AX, 0x4f01
		INT		0x10
		CMP		AX, 0x004f
		JNE		scrn320

;画面模式信息的确认
		
		CMP		BYTE [ES:DI+0x19],8
		JNE		scrn320
		CMP		BYTE [ES:DI+0x1b],4
		JNE		scrn320
		MOV		AX,[ES:DI+0x00]
		AND		AX,0x0080
		JZ		scrn320	

;画面模式的切换
		
		MOV		BX,VBEMODE+0x4000
		MOV		AX,0x4f02
		INT		0x10
		MOV		BYTE [VMODE],8	
		MOV		AX,[ES:DI+0x12]
		MOV		[SCRNX],AX
		MOV		AX,[ES:DI+0x14]
		MOV		[SCRNY],AX
		MOV		EAX,[ES:DI+0x28]
		MOV		[VRAM],EAX
		JMP		keystatus
		
;		MOV		BX, 0x4105
;		MOV		AX, 0x4f02
;		INT		0x10
;		MOV		BYTE [VMODE], 8
;		MOV		WORD [SCRNX], 1024
;		MOV		WORD [SCRNY], 768
;		MOV		DWORD [VRAM], 0xe0000000

scrn320:		
		MOV		AL, 0x13
		MOV		AH, 0x00
		INT		0x10
		MOV 	BYTE [VMODE], 8
		MOV		WORD [SCRNX], 320
		MOV		WORD [SCRNY], 200
		MOV		DWORD [VRAM], 0x000a0000

; 用BIOS获取LED指示灯的状态

keystatus:
		MOV		AH, 0x02
		INT		0x16          ; keyboard BIOS
		MOV		[LEDS], AL
		
		MOV		AL, 0xff
		OUT		0x21, AL
		NOP
		OUT		0xa1, AL
		
		CLI
		
		CALL	waitkbdout
		MOV		AL, 0xd1
		OUT		0x64, AL
		CALL	waitkbdout
		MOV		AL, 0xdf
		OUT		0x60, AL
		CALL	waitkbdout
		
[INSTRSET "i486p"]

		LGDT	[GDTR0]
		MOV		EAX,CR0
		AND		EAX, 0x7fffffff
		OR		EAX, 0x00000001
		MOV		CR0, EAX
		JMP		pipelineflush
pipelineflush:
		MOV		AX, 1*8
		MOV		DS, AX
		MOV 	ES, AX
		MOV		FS, AX
		MOV		GS, AX
		MOV 	SS, AX
		
		
		MOV		ESI, bootpack
		MOV		EDI, BOTPAK
		MOV		ECX, 512*1024/4
		CALL	memcpy
		
		
		MOV		ESI, 0x7c00
		MOV		EDI, DSKCAC
		MOV		ECX, 512/4
		CALL	memcpy
		
		
		MOV		ESI, DSKCAC0+512
		MOV		EDI, DSKCAC+512
		MOV		ECX, 0
		MOV		CL, BYTE [CYLS]
		IMUL	ECX, 512*18*2/4
		SUB		ECX, 512/4
		CALL	memcpy
		
		MOV		EBX, BOTPAK
		MOV		ECX, [EBX+16]
		ADD		ECX, 3
		SHR		ECX, 2
		JZ		skip
		MOV		ESI, [EBX+20]
		ADD		ESI, EBX
		MOV		EDI, [EBX+12]
		CALL	memcpy
		
skip:
		MOV		ESP, [EBX+12]
		JMP		DWORD 2*8:0x0000001b
		
waitkbdout:
		IN		AL, 0x64
		AND		AL, 0x02
		JNZ		waitkbdout
		RET
		
memcpy:
		MOV		EAX, [ESI]
		ADD		ESI, 4
		MOV		[EDI], EAX
		ADD		EDI, 4
		SUB		ECX, 1
		JNZ		memcpy
		RET
		
		ALIGNB	16
GDT0:
		RESB 	8
		DW		0xffff,0x0000,0x9200,0x00cf
		DW		0xffff,0x0000,0x9a28,0x0047
		
		DW		0

GDTR0:
		DW		8*3-1
		DD		GDT0
		
		ALIGNB	16
		
bootpack:
