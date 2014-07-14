OBJS_BOOTPACK = bootpack.obj naskfunc.obj yezfont.obj graphic.obj dsctbl.obj \
				int.obj fifo.obj keyboard.obj mouse.obj memory.obj sheet.obj \
				timer.obj mtask.obj window.obj console.obj file.obj

OBJS_API =	api001.obj api002.obj api003.obj api004.obj api005.obj api006.obj \
			api007.obj api008.obj api009.obj api010.obj api011.obj api012.obj \
			api013.obj api014.obj api015.obj api016.obj api017.obj api018.obj \
			api019.obj api020.obj

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
GOLIB    = $(TOOLPATH)golib00.exe 
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

apilib.lib : Makefile $(OBJS_API)
	$(GOLIB) $(OBJS_API) out:apilib.lib
	
hello.hrb : hello.nas Makefile
	$(NASK) hello.nas hello.hrb hello.lst
	
hello2.hrb : hello2.nas Makefile
	$(NASK) hello2.nas hello2.hrb hello2.lst
	
a.bim : a.obj apilib.lib Makefile
	$(OBJ2BIM) @$(RULEFILE) out:a.bim map:a.map a.obj apilib.lib

a.hrb : a.bim Makefile
	$(BIM2HRB) a.bim a.hrb 0
	
hello3.bim : hello3.obj apilib.lib Makefile
	$(OBJ2BIM) @$(RULEFILE) out:hello3.bim map:hello3.map hello3.obj apilib.lib

hello3.hrb : hello3.bim Makefile
	$(BIM2HRB) hello3.bim hello3.hrb 0
	
hello4.bim : hello4.obj apilib.lib Makefile
	$(OBJ2BIM) @$(RULEFILE) out:hello4.bim stack:1k map:hello4.map \
		hello4.obj apilib.lib

hello4.hrb : hello4.bim Makefile
	$(BIM2HRB) hello4.bim hello4.hrb 0
	
hello5.bim : hello5.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:hello5.bim stack:1k map:hello5.map hello5.obj

hello5.hrb : hello5.bim Makefile
	$(BIM2HRB) hello5.bim hello5.hrb 0
	
winhello.bim : winhello.obj apilib.lib Makefile
	$(OBJ2BIM) @$(RULEFILE) out:winhello.bim stack:1k map:winhello.map \
		winhello.obj apilib.lib

winhello.hrb : winhello.bim Makefile
	$(BIM2HRB) winhello.bim winhello.hrb 0
	
#winhelo2.bim : winhelo2.obj apilib.lib Makefile
#	$(OBJ2BIM) @$(RULEFILE) out:winhelo2.bim stack:1k map:winhelo2.map \
#		winhelo2.obj apilib.lib

#winhelo2.hrb : winhelo2.bim Makefile
#	$(BIM2HRB) winhelo2.bim winhelo2.hrb 0
	
#winhelo3.bim : winhelo3.obj apilib.lib Makefile
#	$(OBJ2BIM) @$(RULEFILE) out:winhelo3.bim stack:1k map:winhelo3.map \
#		winhelo3.obj apilib.lib

#winhelo3.hrb : winhelo3.bim Makefile
#	$(BIM2HRB) winhelo3.bim winhelo3.hrb 40k
	
star1.bim : star1.obj apilib.lib Makefile
	$(OBJ2BIM) @$(RULEFILE) out:star1.bim stack:1k map:star1.map \
		star1.obj apilib.lib

star1.hrb : star1.bim Makefile
	$(BIM2HRB) star1.bim star1.hrb 47k
	
stars.bim : stars.obj apilib.lib Makefile
	$(OBJ2BIM) @$(RULEFILE) out:stars.bim stack:1k map:stars.map \
		stars.obj apilib.lib

stars.hrb : stars.bim Makefile
	$(BIM2HRB) stars.bim stars.hrb 47k
	
stars2.bim : stars2.obj apilib.lib Makefile
	$(OBJ2BIM) @$(RULEFILE) out:stars2.bim stack:1k map:stars2.map \
		stars2.obj apilib.lib

stars2.hrb : stars2.bim Makefile
	$(BIM2HRB) stars2.bim stars2.hrb 47k
	
	
lines.bim : lines.obj apilib.lib Makefile
	$(OBJ2BIM) @$(RULEFILE) out:lines.bim stack:1k map:lines.map \
		lines.obj apilib.lib

lines.hrb : lines.bim Makefile
	$(BIM2HRB) lines.bim lines.hrb 48k
	
walk.bim : walk.obj apilib.lib Makefile
	$(OBJ2BIM) @$(RULEFILE) out:walk.bim stack:1k map:walk.map \
		walk.obj apilib.lib

walk.hrb : walk.bim Makefile
	$(BIM2HRB) walk.bim walk.hrb 48k
	
noodle.bim : noodle.obj apilib.lib Makefile
	$(OBJ2BIM) @$(RULEFILE) out:noodle.bim stack:1k map:noodle.map \
		noodle.obj apilib.lib

noodle.hrb : noodle.bim Makefile
	$(BIM2HRB) noodle.bim noodle.hrb 40k

#crack1.bim : crack1.obj Makefile
#	$(OBJ2BIM) @$(RULEFILE) out:crack1.bim map:crack1.map crack1.obj apilib.lib

#crack1.hrb : crack1.bim Makefile
#	$(BIM2HRB) crack1.bim crack1.hrb 0
	
#crack2.hrb : crack2.nas Makefile
#	$(NASK) crack2.nas crack2.hrb crack2.lst
	
#crack3.hrb : crack3.nas Makefile
#	$(NASK) crack3.nas crack3.hrb crack3.lst
	
#crack4.hrb : crack4.nas Makefile
#	$(NASK) crack4.nas crack4.hrb crack4.lst
	
#crack5.hrb : crack5.nas Makefile
#	$(NASK) crack5.nas crack5.hrb crack5.lst
	
#crack6.hrb : crack6.nas Makefile
#	$(NASK) crack6.nas crack6.hrb crack6.lst
	
bug1.bim : bug1.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:bug1.bim map:bug1.map bug1.obj apilib.lib

bug1.hrb : bug1.bim Makefile
	$(BIM2HRB) bug1.bim bug1.hrb 0
	
bug2.bim : bug2.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:bug2.bim map:bug2.map bug2.obj

bug2.hrb : bug2.bim Makefile
	$(BIM2HRB) bug2.bim bug2.hrb 0
	
bug3.bim : bug3.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:bug3.bim map:bug3.map bug3.obj apilib.lib

bug3.hrb : bug3.bim Makefile
	$(BIM2HRB) bug3.bim bug3.hrb 0
	
beepdown.bim : beepdown.obj apilib.lib Makefile
	$(OBJ2BIM) @$(RULEFILE) out:beepdown.bim stack:1k map:beepdown.map \
		beepdown.obj apilib.lib

beepdown.hrb : beepdown.bim Makefile
	$(BIM2HRB) beepdown.bim beepdown.hrb 40k
	
color.bim : color.obj apilib.lib Makefile
	$(OBJ2BIM) @$(RULEFILE) out:color.bim stack:1k map:color.map \
		color.obj apilib.lib

color.hrb : color.bim Makefile
	$(BIM2HRB) color.bim color.hrb 56k
	
color2.bim : color2.obj apilib.lib Makefile
	$(OBJ2BIM) @$(RULEFILE) out:color2.bim stack:1k map:color2.map \
		color2.obj apilib.lib

color2.hrb : color2.bim Makefile
	$(BIM2HRB) color2.bim color2.hrb 56k
	
crack7.bim : crack7.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:crack7.bim stack:1k map:crack7.map crack7.obj

crack7.hrb : crack7.bim Makefile
	$(BIM2HRB) crack7.bim crack7.hrb 0k
	
ucan23.sys: ucan23.bin bootpack.hrb Makefile
	copy /B ucan23.bin+bootpack.hrb ucan23.sys
	
ucan23.img: ucan23ipl.bin ucan23.sys hello.hrb a.hrb hello3.hrb bug1.hrb \
            bug2.hrb bug3.hrb hello4.hrb hello5.hrb winhello.hrb   \
			star1.hrb stars.hrb stars2.hrb lines.hrb walk.hrb noodle.hrb \
			beepdown.hrb color.hrb color2.hrb crack7.hrb Makefile
	$(EDIMG) imgin:../tolset/z_tools/fdimg0at.tek \
	wbinimg src:ucan23ipl.bin len:512 from:0 to:0 \
	copy from:ucan23.sys to:@: \
	copy from:ucan23ipl.nas to:@: \
	copy from:make.bat to:@: \
	copy from:hello.hrb to:@: \
	copy from:hello2.hrb to:@: \
	copy from:a.hrb to:@: \
	copy from:hello3.hrb to:@: \
	copy from:bug1.hrb to:@: \
	copy from:bug2.hrb to:@: \
	copy from:bug3.hrb to:@: \
	copy from:hello4.hrb to:@: \
	copy from:hello5.hrb to:@: \
	copy from:winhello.hrb to:@: \
	copy from:star1.hrb to:@: \
	copy from:stars.hrb to:@: \
	copy from:stars2.hrb to:@: \
	copy from:lines.hrb to:@: \
	copy from:walk.hrb to:@: \
	copy from:noodle.hrb to:@: \
	copy from:beepdown.hrb to:@: \
	copy from:color.hrb to:@: \
	copy from:color2.hrb to:@: \
	copy from:crack7.hrb to:@: \
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