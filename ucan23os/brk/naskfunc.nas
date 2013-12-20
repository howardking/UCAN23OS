; naskfunc
; TAB=4

[FORMAT "WCOFF"]				; 僆僽僕僃僋僩僼傽僀儖傪嶌傞儌乕僪	
[INSTRSET "i486p"]				; 486偺柦椷傑偱巊偄偨偄偲偄偆婰弎
[BITS 32]						; 32價僢僩儌乕僪梡偺婡夿岅傪嶌傜偣傞
[FILE "naskfunc.nas"]			; 僜乕僗僼傽僀儖柤忣曬

		GLOBAL	_io_hlt,_write_mem8

[SECTION .text]

_io_hlt:	; void io_hlt(void);
		HLT
		RET

_write_mem8:	; void write_mem8(int addr, int data);
		MOV		ECX,[ESP+4]		; [ESP+4]偵addr偑擖偭偰偄傞偺偱偦傟傪ECX偵撉傒崬傓
		MOV		AL,[ESP+8]		; [ESP+8]偵data偑擖偭偰偄傞偺偱偦傟傪AL偵撉傒崬傓
		MOV		[ECX],AL
		RET
