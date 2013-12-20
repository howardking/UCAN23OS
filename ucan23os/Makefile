OBJS_BOOTPACK = bootpack.obj naskfunc.obj yezfont.obj graphic.obj dsctbl.obj \
				int.obj fifo.obj keyboard.obj mouse.obj memory.obj sheet.obj \
				timer.obj mtask.obj

TOOLPATH = ../tolset/z_tools/
INCPATH	 = ../tolset/z_tools/haribote/

MAKE	 = $(TOOLPATH)make.exe -r
NASK	 = $(TOOLPATH)nask.exe
CC1		 = $(TOOLPATH)cc1.exe -I$(INCPATH) -Os -Wall -quiet
GAS2NASK = $(TOOLPATH)gas2nask.exe -a
OBJ2BIM	 = $(TOOLPATH)obj2bim.exe
MAKEFONT = $(TOOLPATH)makefont.exe
BIN2OBJ  = $(TOOLPATH)bin2obj.exe
BIM2HRB	 = $(TOOLPATH)bim2hrb.exe
RULEFILE = $(TOOLPATH)haribote/haribote.rul
EDIMG	 = $(TOOLPATH)edimg.exe
IMGTOL   = $(TOOLPATH)imgtol.com
COPY	 = copy
DEL		 = del

default:
	$(MAKE) img
ucan23ipl.bin: ucan23ipl.nas Makefile
	$(NASK) ucan23ipl.nas ucan23ipl.bin ucan23ipl.lst

ucan23.bin: ucan23.nas Makefile
	$(NASK) ucan23.nas ucan23.bin ucan23.lst
#bootpack.gas: bootpack.c Makefile
#	$(CC1) -o bootpack.gas bootpack.c
	
#bootpack.nas: bootpack.gas Makefile
#	$(GAS2NASK) bootpack.gas bootpack.nas
	
#bootpack.obj: bootpack.nas Makefile
#	$(NASK) bootpack.nas bootpack.obj bookpack.lst

#naskfunc.obj: naskfunc.nas Makefile
#	$(NASK) naskfunc.nas naskfunc.obj naskfunc.lst

yezfont.bin: yezfont.txt Makefile
	$(MAKEFONT) yezfont.txt yezfont.bin
	
yezfont.obj: yezfont.bin Makefile
	$(BIN2OBJ) yezfont.bin yezfont.obj _yezfont
	
#graphic.gas: graphic.c Makefile
#	$(CC1) -o graphic.gas graphic.c
	
#graphic.nas: graphic.gas Makefile
#	$(GAS2NASK) graphic.gas graphic.nas
	
#graphic.obj: graphic.nas Makefile
#	$(NASK) graphic.nas graphic.obj graphic.lst 
	
#dsctbl.gas: dsctbl.c Makefile
#	$(CC1) -o dsctbl.gas dsctbl.c
	
#dsctbl.nas: dsctbl.gas Makefile
#	$(GAS2NASK) dsctbl.gas dsctbl.nas
	
#dsctbl.obj: dsctbl.nas Makefile
#	$(NASK) dsctbl.nas dsctbl.obj dsctbl.lst
	
#bootpack.bim: bootpack.obj naskfunc.obj yezfont.obj graphic.obj dsctbl.obj Makefile
#	$(OBJ2BIM) @$(RULEFILE) out:bootpack.bim stack:3136k map:bootpack.map \
#	bootpack.obj naskfunc.obj yezfont.obj graphic.obj dsctbl.obj

bootpack.bim: $(OBJS_BOOTPACK) Makefile
	$(OBJ2BIM) @$(RULEFILE) out:bootpack.bim stack:3136k map:bootpack.map \
	$(OBJS_BOOTPACK)
	
bootpack.hrb: bootpack.bim Makefile
	$(BIM2HRB) bootpack.bim bootpack.hrb 0
	
ucan23.sys: ucan23.bin bootpack.hrb Makefile
	copy /B ucan23.bin+bootpack.hrb ucan23.sys
	
ucan23.img: ucan23ipl.bin ucan23.sys Makefile
	$(EDIMG) imgin:../tolset/z_tools/fdimg0at.tek \
	wbinimg src:ucan23ipl.bin len:512 from:0 to:0 \
	copy from:ucan23.sys to:@: \
	imgout:ucan23.img
	
%.gas: %.c Makefile
	$(CC1) -o $*.gas $*.c
	
%.nas: %.gas Makefile
	$(GAS2NASK) $*.gas $*.nas
	
%.obj: %.nas Makefile
	$(NASK) $*.nas $*.obj $*.lst
	

asm:
	$(MAKE) ucan23ipl.bin
	
img:
	$(MAKE) ucan23.img
	
run:
	$(MAKE) img
	$(COPY) ucan23.img ..\tolset\z_tools\qemu\fdimage0.bin
	$(MAKE) -C ../tolset/z_tools/qemu
	
install:
	$(MAKE) img
	$(IMGTOL) w a: ucan23.img
	
clean:
	-$(DEL) *.bin
	-$(DEL) *.lst
	-$(DEL) *.gas
	-$(DEL) *.obj
	-$(DEL) bootpack.nas
	-$(DEL) bootpack.map
	-$(DEL) bootpack.bim
	-$(DEL) bootpack.hrb
	-$(DEL) ucan23.sys
	
src_only:
	$(MAKE) clean
	-$(DEL) ucan23.img