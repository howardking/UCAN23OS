/* filename:    bootpack.c
 * description: the UcanMain()file
 * author:      Howard
 * date:        2013-11-28
 * version:     v1.0
 */


#include <stdio.h>
#include <string.h>
#include "bootpack.h"

//struct FILEINFO {
//	unsigned char name[8], ext[3], type;
//	char reserve[10];
//	unsigned short time, date, clustno;
//	unsigned int size;
//};

//void putfonts8_asc_sht(struct SHEET *sht, int x, int y, int c, int b, char *s, int l);
void set490(struct FIFO32 *fifo, int mode);
//void make_textbox8(struct SHEET *sht, int x0, int y0, int sx, int sy, int c);
void task_b_main(struct SHEET *sht_back);

void keywin_off(struct SHEET *key_win);
void keywin_on(struct SHEET *key_win);
void close_console(struct SHEET *sht);
void close_constask(struct TASK *task);

#define KEYCMD_LED		0xed

void UcanMain(void)
{
	
	char *vram;
	char s[50];
	int fifobuf[128], keycmd_buf[32], *cons_fifo[2];//mcursor[256], keybuf[32], mousebuf[128], timerbuf[8], timerbuf2[8], timerbuf3[8];
	int mx, my, i, new_mx = -1, new_my = 0, new_wx = 0x7fffffff, new_wy = 0;// cursor_x, cursor_c; //task_b_esp;
	//int xsize, ysize;
	//int i ;// count = 0;;
	unsigned int memtotal;
	struct MOUSE_DEC mdec; /*鼠标解码缩放的数据*/
	struct SHTCTL *shtctl;
	struct FIFO32 fifo, keycmd;
	//struct FIFO32 timerfifo, timerfifo2, timerfifo3;
	//struct TIMER *timer;
	struct SHEET *sht_back, *sht_mouse; //, *sht_cons[2];, *sht_win//*sht_win_b[3];
	unsigned char *buf_back, buf_mouse[256], *buf_cons[2];//*buf_win,;//*buf_win_b;
	static char keytable0[0x80] = {
		0,   0,   '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '^', 0x08, 0,
		'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '@', '[', 0x0a, 0, 'A', 'S',
		'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', ':', 0,   0,   ']', 'Z', 'X', 'C', 'V',
		'B', 'N', 'M', ',', '.', '/', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1',
		'2', '3', '0', '.', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   0x5c, 0,  0,   0,   0,   0,   0,   0,   0,   0,   0x5c, 0,  0
	};
	static char keytable1[0x80] = {
		0,   0,   '!', 0x22, '#', '$', '%', '&', 0x27, '(', ')', '~', '=', '~', 0x08, 0,
		'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '`', '{', 0x0a, 0, 'A', 'S',
		'D', 'F', 'G', 'H', 'J', 'K', 'L', '+', '*', 0,   0,   '}', 'Z', 'X', 'C', 'V',
		'B', 'N', 'M', '<', '>', '?', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1',
		'2', '3', '0', '.', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   '_', 0,   0,   0,   0,   0,   0,   0,   0,   0,   '|', 0,   0
	};
	
	struct TASK *task_a, *task, *task_cons[2];
	struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *)ADR_GDT;
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	
	struct BOOTINFO *binfo = (struct BOOTINFO *) 0x0ff0;
	int key_to = 0, key_shift = 0, key_leds = (binfo->leds >> 4) & 7, keycmd_wait = -1;
	struct CONSOLE *cons;
	int j, x, y, mmx = -1, mmy = -1, mmx2 = 0;
	struct SHEET *sht = 0, *key_win, *sht2;
			 
	init_gdtidt();
	init_pic();
	io_sti();
		
	fifo32_init(&fifo, 128, fifobuf, 0);
	*((int *) 0x0fec) = (int) &fifo;
	fifo32_init(&keycmd, 32, keycmd_buf, 0);
	//fifo8_init(&mousefifo, 128, mousebuf);
	//fifo8_init(&timerfifo, 8, timerbuf);
	//fifo8_init(&timerfifo2, 8, timerbuf2);
	//fifo8_init(&timerfifo3, 8, timerbuf3);
	
	init_pit();
	
	init_keyboard(&fifo, 256);
	enable_mouse(&fifo, 512, &mdec);
	io_out8(PIC0_IMR, 0xf8);
	io_out8(PIC1_IMR, 0xef);
	
	/*
	timer = timer_alloc();
	timer2 = timer_alloc();
	timer3 = timer_alloc();
	timer_ts = timer_alloc();
	
	timer_init(timer, &fifo, 10);
	timer_init(timer2, &fifo, 3);
	timer_init(timer3, &fifo, 1);
	timer_init(timer_ts, &fifo, 2);
	
	timer_settime(timer, 1000);
	timer_settime(timer2, 300);
	timer_settime(timer3, 50);
	timer_settime(timer_ts, 2);
	*/
	//settimer(1000, &timerfifo, 1);
	
	
	memtotal = memtest(0x00400000, 0xbfffffff);
	memman_init(memman);
	memman_free(memman, 0x00001000, 0x0009e000);
	memman_free(memman, 0x00400000, memtotal - 0x00400000);
	
	init_palette();
	
	shtctl    = shtctl_init(memman, binfo->vram, binfo->scrnx, binfo->scrny);
	task_a = task_init(memman);
	fifo.task = task_a;
	task_run(task_a, 1, 2);
	*((int *) 0x0fe4) = (int) shtctl;
	
	sht_back  = sheet_alloc(shtctl);
	//sht_mouse = sheet_alloc(shtctl);
	//sht_win   = sheet_alloc(shtctl);
	buf_back  = (unsigned char *)memman_alloc_4k(memman, binfo->scrnx*binfo->scrny);
	//buf_win   = (unsigned char *)memman_alloc_4k(memman, 160*52);
	sheet_setbuf(sht_back, buf_back, binfo->scrnx, binfo->scrny, -1);
	//sheet_setbuf(sht_mouse, buf_mouse, 16, 16, 99);
	//sheet_setbuf(sht_win, buf_win, 160, 52, -1);
	
	//init_screen(binfo->vram, binfo->scrnx, binfo->scrny);
	init_screen(buf_back, binfo->scrnx, binfo->scrny);
	
	/* sht_cons */
	key_win = open_console(shtctl, memtotal);
	//sht_cons[0] = open_console(shtctl, memtotal);
	//sht_cons[1] = 0; /* 未打开状态 */
	
	/*for (i = 0; i < 2; i++) {
		sht_cons[i] = sheet_alloc(shtctl);
		buf_cons[i] = (unsigned char *) memman_alloc_4k(memman, 256 * 165);
		sheet_setbuf(sht_cons[i], buf_cons[i], 256, 165, -1); *//* 无透明色 */
	/*	make_window8(buf_cons[i], 256, 165, "console", 0);
		make_textbox8(sht_cons[i], 8, 28, 240, 128, COL8_000000);
		task_cons[i] = task_alloc();
		task_cons[i]->tss.esp = memman_alloc_4k(memman, 64 * 1024) + 64 * 1024 - 8;
		task_cons[i]->tss.eip = (int) &console_task;
		task_cons[i]->tss.es = 1 * 8;
		task_cons[i]->tss.cs = 2 * 8;
		task_cons[i]->tss.ss = 1 * 8;
		task_cons[i]->tss.ds = 1 * 8;
		task_cons[i]->tss.fs = 1 * 8;
		task_cons[i]->tss.gs = 1 * 8;
		*((int *) (task_cons[i]->tss.esp + 4)) = (int) sht_cons[i];
		*((int *) (task_cons[i]->tss.esp + 8)) = memtotal;
		task_run(task_cons[i], 2, 2);*/ /* level=2, priority=2 */
	/*	sht_cons[i]->task = task_cons[i];
		sht_cons[i]->flags |= 0x20;	*//* 有光标 */
	/*	cons_fifo[i] = (int *) memman_alloc_4k(memman, 128 * 4);
		fifo32_init(&task_cons[i]->fifo, 128, cons_fifo[i], task_cons[i]);
	}*/
	/*sht_win_b*/
	/*for (i=0; i<3; i++){
		sht_win_b[i] = sheet_alloc(shtctl);
		buf_win_b = (unsigned char *)memman_alloc_4k(memman, 144*52);
		sheet_setbuf(sht_win_b[i], buf_win_b, 144,52, -1);
		sprintf(s, "task_b%d", i);
		make_window8(buf_win_b, 144, 52, s, 0);
		task_b[i] = task_alloc();
		task_b[i]->tss.esp = memman_alloc_4k(memman, 64*1024) + 64 * 1024 - 8;
		task_b[i]->tss.eip = (int) &task_b_main;
		task_b[i]->tss.es = 1 * 8;
		task_b[i]->tss.cs = 2 * 8;
		task_b[i]->tss.ss = 1 * 8;
		task_b[i]->tss.ds = 1 * 8;
		task_b[i]->tss.fs = 1 * 8;
		task_b[i]->tss.gs = 1 * 8;
		*((int *) (task_b[i]->tss.esp + 4)) = (int) sht_win_b[i];*/
		/*task_run(task_b[i], 2, i+1);*/
	/*}*/
	
	/*sht_win*/
	/*sht_win		= sheet_alloc(shtctl);
	buf_win		= (unsigned char *)memman_alloc_4k(memman, 160*52);
	sheet_setbuf(sht_win, buf_win, 144, 52, -1);
	make_window8(buf_win, 144, 52, "task_a", 1);
	make_textbox8(sht_win, 8, 28, 128, 16, COL8_FFFFFF);
	cursor_x = 8;
	cursor_c = COL8_FFFFFF;
	timer = timer_alloc();
	timer_init(timer, &fifo, 1);
	timer_settime(timer, 50);*/
	
	/*sht_mouse*/
	sht_mouse = sheet_alloc(shtctl);
	sheet_setbuf(sht_mouse, buf_mouse, 16, 16, 99);
	init_mouse_cursor8(buf_mouse, 99);
	mx = (binfo->scrnx-16) / 2;
	my = (binfo->scrny-28-16) /2;
	
	//putfonts8_asc(buf_win, 160, 24, 28, COL8_000000, "Welcome to");
	//putfonts8_asc(buf_win, 160, 24, 44, COL8_000000, "      Ucan-OS!");
	sheet_slide(sht_back, 0, 0);
	//sheet_slide(sht_cons[1], 56,  6);
	//sheet_slide(sht_cons[0],  8,  2);
	//sheet_slide(sht_cons[0], 32,  4);
	sheet_slide(key_win,   32, 4);
	//init_mouse_cursor8(mcursor, 99);
	//xsize = (*binfo).scrnx;
	//ysize = (*binfo).scrny;
	//vram = (*binfo).vram;
	/*sheet_slide(sht_win_b[0], 168,  56);
	sheet_slide(sht_win_b[1],   8, 116);
	sheet_slide(sht_win_b[2], 168, 116);*/
	//sheet_slide(sht_win,        64,  56);
	sheet_slide(sht_mouse, mx, my);
	sheet_updown(sht_back,     0);
	/*sheet_updown(sht_win_b[0], 1);
	sheet_updown(sht_win_b[1], 2);
	sheet_updown(sht_win_b[2], 3);*/
	sheet_updown(key_win,  1);
	//sheet_updown(sht_cons[1],  2);
	//sheet_updown(sht_win,   3);
	sheet_updown(sht_mouse, 2);
	
	
	//sheet_slide(sht_mouse, mx, my);
	//sheet_slide(sht_win, 80, 72);
	//sheet_updown(sht_back,  0);
	//sheet_updown(sht_win,   1);
	//sheet_updown(sht_mouse, 2);
	//putblock8_8(buf_back, binfo->scrnx, 16, 16, mx, my, buf_mouse, 16);
	//putfonts8_asc(buf_back, binfo->scrnx, 8, 8, COL8_FFFFFF, "Hello, world!");
	//putfonts8_asc(buf_back, binfo->scrnx, 31, 31, COL8_000000, "Ucan23-OS");
	//putfonts8_asc(buf_back, binfo->scrnx, 30, 30, COL8_FFFFFF, "Ucan23-OS");
	//putfonts8_asc_sht(sht_back, binfo->scrnx, 8, 0, COL8_FFFFFF, "Hello, world", 1);
	//putfonts8_asc_sht(sht_back, binfo->scrnx, 31, 0, COL8_000000, "Ucan23-OS", 4);
	//putfonts8_asc_sht(sht_back, binfo->scrnx, 30, 0, COL8_FFFFFF, "Ucan23-OS", 4);
	
	//sprintf(s, "scrnx = %d", binfo->scrnx);
	//putfonts8_asc(buf_back, binfo->scrnx, 16, 64, COL8_FFFFFF, s);
	
	//sprintf(s, "(%3d, %3d)", mx, my);
	//putfonts8_asc_sht(sht_back, 0, 0, COL8_FFFFFF, COL8_008484, s, 10);
	
	//io_out8(PIC0_IMR, 0xf9);
	//io_out8(PIC1_IMR, 0xef);
	
	
	//sprintf(s, "Memory %dMB  free: %dKB", memtotal/(1024*1024), memman_total(memman)/1024);
	//putfonts8_asc_sht(buf_back, 0, 32, COL8_FFFFFF, COL8_008484, s, 40);
	
	//sheet_refresh(sht_back, 0, 0, binfo->scrnx, binfo->scrny);
	/*
	task_b = task_alloc();
	task_b->tss.esp = memman_alloc_4k(memman, 64*1024) + 64*1024 - 8;
	task_b->tss.eip = (int)& task_b_main;
	task_b->tss.es = 1 * 8;
	task_b->tss.cs = 2 * 8;
	task_b->tss.ss = 1 * 8;
	task_b->tss.ds = 1 * 8;
	task_b->tss.fs = 1 * 8;
	task_b->tss.gs = 1 * 8;
	*((int *) (task_b->tss.esp+4)) = (int) sht_back;
	task_run(task_b);
	*/
	
	//key_win = sht_cons[0];
	keywin_on(key_win);
	//sht_cons->task = task_cons;
	//sht_cons->flags |= 0x20;	/* 有光标 */
	
	fifo32_put(&keycmd, KEYCMD_LED);
	fifo32_put(&keycmd, key_leds);
	
	for (;;){
		if (fifo32_status(&keycmd) > 0 && keycmd_wait < 0) {
			/* 如果存在向键盘控制器发送的数据，则发送它 */
			keycmd_wait = fifo32_get(&keycmd);
			wait_KBC_sendready();
			io_out8(PORT_KEYDAT, keycmd_wait);
		}
		/*计数器程序*/
		//count ++;
		//sprintf(s, "%010d", timerctl.count);
		//boxfill8(buf_win, 160, COL8_C6C6C6, 40, 28, 119, 43);
		//putfonts8_asc(buf_win, 160,  40, 28, COL8_000000, s);
		//sheet_refresh(sht_win, 40, 28, 120, 44);
		
		io_cli();    /*执行nashfunc.nas里的_io_hlt*/
		if (0==fifo32_status(&fifo)){
			/* FIFO为空，当存在搁置的绘图操作时立即执行 */
			if (new_mx >= 0) {
				io_sti();
				sheet_slide(sht_mouse, new_mx, new_my);
				new_mx = -1;
			} else if (new_wx != 0x7fffffff) {
				io_sti();
				sheet_slide(sht, new_wx, new_wy);
				new_wx = 0x7fffffff;
			} else {
				task_sleep(task_a);
				io_sti();
			}
			
			//task_sleep(task_a);
			//io_sti();
		} else {
			i = fifo32_get(&fifo);
			io_sti();
			/*
			if (2==i){
				farjmp(0, 4*8);
				timer_settime(timer_ts, 2);
			}*/
			
			if (key_win != 0 && key_win->flags == 0) {	/* 输入窗口被关闭 */
				if (shtctl->top == 1) {	/* 当画面上只剩鼠标和背景 */
					key_win = 0;
				} else {
					key_win = shtctl->sheets[shtctl->top - 1];
					keywin_on(key_win);
				}
			}
			
			if (i>=256 && i<=511){ /*键盘数据*/
				//sprintf(s, "%02X", i-256);
				//boxfill8(buf_back, binfo->scrnx, COL8_000000, 0, 120, 15, 135);
				//putfonts8_asc(buf_back, binfo->scrnx, 0, 120, COL8_FFFFFF, s);
				//sheet_refresh(sht_back, 0, 120, 15*8, 136);
				//putfonts8_asc_sht(sht_back, 0, 16, COL8_FFFFFF, COL8_008484, s, 2);
				if (i<256+0x80 )
				{ /* 将键盘编码转换成字符编码 */
					if (key_shift == 0){
						s[0] = keytable0[i - 256];
					}else {
						s[0] = keytable1[i - 256];
					}
				} else {
					s[0] = 0;
				}
				
				if ('A' <= s[0] && s[0] <= 'Z') {	/* 当输入英文字母时 */
					if (((key_leds & 4) == 0 && key_shift == 0) ||
							((key_leds & 4) != 0 && key_shift != 0)) {
						s[0] += 0x20;	/* 将大写转换成小写 */
					}
				}
				
				if (s[0] != 0 && key_win != 0){  /* 一般字符 */
					fifo32_put(&key_win->task->fifo, s[0] + 256);
				}
				//	if (key_win == sht_win) { /* 发送至任务A */
				//		if ( cursor_x < 128){
				//			//s[0] = keytable[i-256];
				//			s[1] = 0;
				///			putfonts8_asc_sht(sht_win, cursor_x, 28, COL8_000000, COL8_FFFFFF, s, 1);
				//			cursor_x += 8;
				//		}
				//	}else{ /* 发送至命令行窗口 */
				//		fifo32_put(&key_win->task->fifo, s[0] + 256);
				//	}
				//}
				
				
				//if (256+0x0e==i){
				//	if (key_win == sht_win){
				//		if (cursor_x > 8){
				//			putfonts8_asc_sht(sht_win, cursor_x, 28, COL8_000000, COL8_FFFFFF, " ", 1);
				//			cursor_x -= 8;
				//		}
				//	}else {
				//		fifo32_put(&key_win->task->fifo, 8 + 256);
				//	}
					
				//}
				
				//if (i == 256 + 0x1c) {	/* Enter */
				//	if (key_win != sht_win) {	/* 发送至命令行窗口 */
				//		fifo32_put(&key_win->task->fifo, 10 + 256);
				//	}
				//}
				
				/* 开始处理Tab键切换 */
				if (i == 256 + 0x0f && key_win != 0) { /* Tab */
				//	if (key_to == 0) {
				//		key_to = 1;
				//		make_wtitle8(buf_win,  sht_win->bxsize,  "task_a",  0);
				//		make_wtitle8(buf_cons, sht_cons->bxsize, "console", 1);
				//		cursor_c = -1; /* 不显示鼠标 */
				//		boxfill8(sht_win->buf, sht_win->bxsize, COL8_FFFFFF, cursor_x, 28, cursor_x + 7, 43);
				//		fifo32_put(&task_cons->fifo, 2); /* 命令行窗口光标ON */
				//	} else {
				//		key_to = 0;
				//		make_wtitle8(buf_win,  sht_win->bxsize,  "task_a",  1);
				//		make_wtitle8(buf_cons, sht_cons->bxsize, "console", 0);
				//		cursor_c = COL8_000000; /* 显示鼠标 */
				//		fifo32_put(&task_cons->fifo, 3); /* 命令行窗口光标OFF */
				//	}
				//	sheet_refresh(sht_win,  0, 0, sht_win->bxsize,  21);
				//	sheet_refresh(sht_cons, 0, 0, sht_cons->bxsize, 21);
				
				//	cursor_c = keywin_off(key_win, sht_win, cursor_c, cursor_x);
					keywin_off(key_win);
					j = key_win->height - 1;
					if (j == 0) {
						j = shtctl->top - 1;
					}
					key_win = shtctl->sheets[j];
					//cursor_c = keywin_on(key_win, sht_win, cursor_c);
					keywin_on(key_win);
				
				}
				/* 到此结束Tab键切换的处理 */
				
				if (i == 256 + 0x2a) {	/* 左Shift ON */
					key_shift |= 1;
				}
				if (i == 256 + 0x36) {	/* 右Shift ON */
					key_shift |= 2;
				}
				if (i == 256 + 0xaa) {	/* 左Shift OFF */
					key_shift &= ~1;
				}
				if (i == 256 + 0xb6) {	/* 右Shift OFF */
					key_shift &= ~2;
				}
				
				if (i == 256 + 0x3a) {	/* CapsLock */
					key_leds ^= 4;
					fifo32_put(&keycmd, KEYCMD_LED);
					fifo32_put(&keycmd, key_leds);
				}
				if (i == 256 + 0x45) {	/* NumLock */
					key_leds ^= 2;
					fifo32_put(&keycmd, KEYCMD_LED);
					fifo32_put(&keycmd, key_leds);
				}
				if (i == 256 + 0x46) {	/* ScrollLock */
					key_leds ^= 1;
					fifo32_put(&keycmd, KEYCMD_LED);
					fifo32_put(&keycmd, key_leds);
				}
				
				if (i == 256 + 0x3b && key_shift != 0 && key_win != 0) {	/* Shift+F1 */
					task = key_win->task;
					if (task != 0 && task->tss.ss0 != 0) {
						//cons = (struct CONSOLE *) *((int *) 0x0fec);
						cons_putstr0(task->cons, "\nBreak(key) :\n");
						io_cli();	/* 不能再改变寄存器值时切换到其他任务 */
						task->tss.eax = (int) &(task->tss.esp0);
						task->tss.eip = (int) asm_end_app;
						io_sti();
						task_run(task, -1, 0);	/* 为了确实执行结束处理，如果处于休眠则唤醒 */
					}
				}
				
				if (i == 256 + 0x3c && key_shift != 0) {	/* Shift+F2 */
					//sht_cons[1] = open_console(shtctl, memtotal);
					if (key_win != 0) {
						keywin_off(key_win);
					}
					//sheet_updown(sht_cons[1], shtctl->top);
					/* 自动将焦点切换到刚打开的窗口 */
					//keywin_off(key_win);
					key_win = open_console(shtctl, memtotal);
					sheet_slide(key_win, 32, 4);
					//key_win = open_console(shtctl, memtotal);
					sheet_updown(key_win, shtctl->top);
					keywin_on(key_win);
				}
				
				if (i == 256 + 0x57 && shtctl->top > 2) {	/* F11 */
					sheet_updown(shtctl->sheets[1], shtctl->top - 1);
				}
				
				if (i == 256 + 0xfa) {	/* 键盘成功接收到数据 */
					keycmd_wait = -1;
				}
				if (i == 256 + 0xfe) {	/* 键盘没有成功接收到数据 */
					wait_KBC_sendready();
					io_out8(PORT_KEYDAT, keycmd_wait);
				}
				/* 重新显示光标 */
				//if (cursor_c >= 0) {
				//	boxfill8(sht_win->buf, sht_win->bxsize, cursor_c, cursor_x, 28, cursor_x + 7, 43);
				//}
				//boxfill8(sht_win->buf, sht_win->bxsize, cursor_c, cursor_x, 28, cursor_x + 7, 43);
				//sheet_refresh(sht_win, cursor_x, 28, cursor_x + 8, 44);
			} else if (i>=512 && i<=767){ /*鼠标数据*/
				if (0 != mouse_decode(&mdec, i-512)){					
					//sprintf(s, "[lcr %4d %4d]", mdec.x, mdec.y);
					//if ((mdec.btn & 0x01)!=0){
					//	s[1] = 'L';
					//}
					//if ((mdec.btn & 0x02)!=0){
					//	s[3] = 'R';
					//}
					//if ((mdec.btn & 0x04)!=0){
					//	s[2] = 'C';
					//}
					//putfonts8_asc_sht(sht_back, 32, 16, COL8_FFFFFF, COL8_008484, s, 15);
					//boxfill8(buf_back, binfo->scrnx, COL8_008484, 32, 120, 32+15*8-1,135);
					//putfonts8_asc(buf_back, binfo->scrnx, 32, 120, COL8_FFFFFF, s);
					//sheet_refresh(sht_back, 32, 120, 32+15*8, 136);
					/*鼠标指针的移动*/
					//boxfill8(buf_back, binfo->scrnx, COL8_008484, mx, my, mx+15, my+15);/*隐藏鼠标*/
					mx += mdec.x;
					my += mdec.y;
					if (mx<0){
						mx = 0;
					}
					if (my<0){
						my = 0;
					}
					if (mx>binfo->scrnx-1){
						mx = binfo->scrnx-1;
					}
					if (my>binfo->scrny-1){
						my = binfo->scrny-1;
					}
					new_mx = mx;
					new_my = my;
					//sprintf(s, "(%3d, %3d)", mx, my);
					//putfonts8_asc_sht(sht_back, 0, 0, COL8_FFFFFF, COL8_008484, s, 10);
					//sheet_slide(sht_mouse, mx, my);
					//boxfill8(buf_back, binfo->scrnx, COL8_008484,binfo->scrnx/2,100,binfo->scrnx-80 , 136);
					//putfonts8_asc(buf_back, binfo->scrnx, binfo->scrnx/2, 100, COL8_FFFFFF,s);
					//sheet_refresh(sht_back, binfo->scrnx/2, 100, binfo->scrnx-80, 136);
					//putblock8_8(binfo->vram,binfo->scrnx, 16, 16, mx, my, mcursor, 16);
					//sheet_slide(sht_mouse, mx, my);
					if (0!=(mdec.btn&0x01)){
						/*按下左键、移动sht_win*/
						if (mmx < 0) {
							/*sheet_slide(sht_win, mx-80, my-8);*/
							for (j = shtctl->top - 1; j > 0; j--) {
								/* 按照从上到下的顺序寻早鼠标所指向的图层 */
								sht = shtctl->sheets[j];
								x = mx - sht->vx0;
								y = my - sht->vy0;
								if (0 <= x && x < sht->bxsize && 0 <= y && y < sht->bysize) {
									if (sht->buf[y * sht->bxsize + x] != sht->col_inv) {
										sheet_updown(sht, shtctl->top - 1);
										
										if (sht != key_win) {
											//cursor_c = keywin_off(key_win, sht_win, cursor_c, cursor_x);
											keywin_off(key_win);
											key_win = sht;
											//cursor_c = keywin_on(key_win, sht_win, cursor_c);
											keywin_on(key_win);
										}
										
										if (3 <= x && x < sht->bxsize - 3 && 3 <= y && y < 21) {
											mmx = mx;	/* 进入窗口移动模式 */
											mmy = my;
											mmx2 = sht->vx0;
											new_wy = sht->vy0;
										}
										if (sht->bxsize - 21 <= x && x < sht->bxsize - 5 && 5 <= y && y < 19) {
											/* 点击“X按钮” */
											if ((sht->flags & 0x10) != 0) {	/* 该窗口是否为应用程序窗口？ */
												//cons = (struct CONSOLE *) *((int *) 0x0fec);
												task = sht->task;
												cons_putstr0(task->cons, "\nBreak(mouse) :\n");
												io_cli();	/* 强行结束处理中禁止切换任务 */
												task->tss.eax = (int) &(task->tss.esp0);
												task->tss.eip = (int) asm_end_app;
												io_sti();
												task_run(task, -1, 0);
											} else {	/* 命令行窗口 */
												task = sht->task;
												sheet_updown(sht, -1); /* 暂时隐藏该图层 */
												keywin_off(key_win);
												key_win = shtctl->sheets[shtctl->top - 1];
												keywin_on(key_win);
												io_cli();
												fifo32_put(&task->fifo, 4);
												io_sti();
											}
										}
										break;
									}
								}
							}
						}else {
							/* 如果处于窗口移动模式 */
							x = mx - mmx;	/* 计算鼠标的移动距离 */
							y = my - mmy;
							new_wx = (mmx2 + x + 2) & ~3;
							new_wy = new_wy + y;
							//sheet_slide(sht, sht->vx0 + x, sht->vy0 + y);
							//sheet_slide(sht, (mmx2 + x + 2) & ~3, sht->vy0 + y);
							//mmx = mx;	/* 更新为移动后的坐标 */
							mmy = my;
						}
					}else {
						/* 没有按下鼠标左键 */
						mmx = -1;	/* 返回通常模式 */
						if (new_wx != 0x7fffffff) {
							sheet_slide(sht, new_wx, new_wy);	/* 固定图层位置 */
							new_wx = 0x7fffffff;
						}
					}
				}
			} else if (768 <= i && i <= 1023) {	/* 命令行窗口关闭处理 */
				close_console(shtctl->sheets0 + (i - 768));
			} else if (1024 <= i && i <= 2023) {
				close_constask(taskctl->tasks0 + (i - 1024));
			}else if (2024 <= i && i <= 2279) {	/* 只关闭命令行窗口 */
				sht2 = shtctl->sheets0 + (i - 2024);
				memman_free_4k(memman, (int) sht2->buf, 256 * 165);
				sheet_free(sht2);
			}/*else if (10==i){
					//boxfill8(buf_back, binfo->scrnx, COL8_008484,binfo->scrnx-100,0,binfo->scrnx , 16);
					putfonts8_asc(buf_back, binfo->scrnx, binfo->scrnx-100, 0, COL8_FFFFFF, "10[sec]" );
					sheet_refresh(sht_back, binfo->scrnx-100, 0, binfo->scrnx, 16);
					//sprintf(s, "%010d", count);
					//putfonts8_asc(sht_win, binfo->scrnx, 40, 28, COL8_000000, s);
					//sheet_refresh(sht_win, 0, 64, 56, 80);
					//putfonts8_asc_sht(sht_win, 40, 28, COL8_000000, COL8_C6C6C6, s, 10);
					//taskswitch4();
			} else if(3==i) {
				putfonts8_asc(buf_back, binfo->scrnx, binfo->scrnx-100, 16, COL8_FFFFFF, "3[sec]" );
				sheet_refresh(sht_back, binfo->scrnx-100, 16, binfo->scrnx, 32);
				//count = 0;
			}*/ //else if (i<=1){
				//if (i != 0) {
				//	timer_init(timer, &fifo, 0); /* 下次置0 */
				//	if (cursor_c >= 0){
				//		cursor_c = COL8_000000;
				//	}
				//} else {
				//	timer_init(timer, &fifo, 1); /* 下次置1 */
				//	if (cursor_c >= 0){
				//		cursor_c = COL8_FFFFFF;
				//	}
					
				//}
				//timer_settime(timer, 50);
				//if (cursor_c >= 0){
				//	boxfill8(sht_win->buf, sht_win->bxsize, cursor_c, cursor_x, 28, cursor_x + 7, 43);
				//	sheet_refresh(sht_win, cursor_x, 28, cursor_x + 8, 44);
				//}
				
			//}
			
		}
	}
}


void keywin_off(struct SHEET *key_win)
{
	change_wtitle8(key_win, 0);
	if ((key_win->flags & 0x20) != 0) {
		fifo32_put(&key_win->task->fifo, 3); /* 命令行窗口光标OFF */
	}
	return;
}

void keywin_on(struct SHEET *key_win)
{
	change_wtitle8(key_win, 1);
	if ((key_win->flags & 0x20) != 0) {
		fifo32_put(&key_win->task->fifo, 2); /* 命令行窗口光标ON */
	}
	return;
}



void set490(struct FIFO32 *fifo, int mode)
{
	int i;
	struct TIMER *timer;
	if (0!=mode){
		for (i=0; i<490; i++){
			timer = timer_alloc();
			timer_init(timer, fifo, 1024+i);
			timer_settime(timer, 100*60*60*24*50+i*100);
		}
	}
	return;
}


void task_b_main(struct SHEET *sht_win_b)
{
	struct FIFO32 fifo;
	struct TIMER *timer_ls;
	int i, fifobuf[128], count = 0, count0 = 0;
	char s[12];
	
	
	fifo32_init(&fifo, 128, fifobuf, 0);
	//timer_ts = timer_alloc();
	//timer_put = timer_alloc();
	timer_ls = timer_alloc();
	//timer_init(timer_ts, &fifo, 2);
	//timer_init(timer_put, &fifo, 1);
	timer_init(timer_ls, &fifo, 100);
	//timer_settime(timer_ts, 2);
	//timer_settime(timer_put, 1);
	timer_settime(timer_ls, 100);
	//sht_back = (struct SHEET *) *((int *)0x0fec);
	
	for (;;){
		count ++;
		//sprintf(s, "%10d", count);
		//putfonts8_asc(sht_back, 320, 40, 28, COL8_000000, s);
		//sheet_refresh(sht_back, 0, 64, 56, 80);
		//putfonts8_asc_sht(sht_back, 220, 144, COL8_FFFFFF, COL8_008484, s, 11);
		io_cli();
		if (0==(fifo32_status(&fifo))){
			io_sti();
			//io_hlt();
		} else {
			i = fifo32_get(&fifo);
			io_sti();
			/*if (1==i){
				sprintf(s, "%11d", count);
				putfonts8_asc_sht(sht_back, 220, 144, COL8_FFFFFF, COL8_008484, s, 11);
				timer_settime(timer_put, 1);
			}else if (2==i){
				farjmp(0, 3*8);
				timer_settime(timer_ts, 2);
				//taskswitch3();
			}else*/
			if (100==i){
				sprintf(s, "%11d", count-count0);
				putfonts8_asc_sht(sht_win_b, 24, 28, COL8_000000, COL8_C6C6C6, s, 11);
				count0 = count;
				timer_settime(timer_ls, 100);
			}
		}
		
	}
}

struct TASK *open_constask(struct SHEET *sht, unsigned int memtotal)
{
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct TASK *task = task_alloc();
	int *cons_fifo = (int *) memman_alloc_4k(memman, 128 * 4);
	task->cons_stack = memman_alloc_4k(memman, 64 * 1024);
	task->tss.esp = task->cons_stack + 64 * 1024 - 12;
	task->tss.eip = (int) &console_task;
	task->tss.es = 1 * 8;
	task->tss.cs = 2 * 8;
	task->tss.ss = 1 * 8;
	task->tss.ds = 1 * 8;
	task->tss.fs = 1 * 8;
	task->tss.gs = 1 * 8;
	*((int *) (task->tss.esp + 4)) = (int) sht;
	*((int *) (task->tss.esp + 8)) = memtotal;
	task_run(task, 2, 2); /* level=2, priority=2 */
	fifo32_init(&task->fifo, 128, cons_fifo, task);
	return task;
}

struct SHEET *open_console(struct SHTCTL *shtctl, unsigned int memtotal)
{
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct SHEET *sht = sheet_alloc(shtctl);
	unsigned char *buf = (unsigned char *) memman_alloc_4k(memman, 256 * 165);
	//struct TASK *task = task_alloc();
	//int *cons_fifo = (int *) memman_alloc_4k(memman, 128 * 4);
	sheet_setbuf(sht, buf, 256, 165, -1); /* 无透明色 */
	make_window8(buf, 256, 165, "console", 0);
	make_textbox8(sht, 8, 28, 240, 128, COL8_000000);
	sht->task = open_constask(sht, memtotal);
	sht->flags |= 0x20;	/* 有光标 */
	//task->cons_stack = memman_alloc_4k(memman, 64 * 1024);
	//task->tss.esp = task->cons_stack + 64 * 1024 - 12;
	//task->tss.eip = (int) &console_task;
	//task->tss.es = 1 * 8;
	//task->tss.cs = 2 * 8;
	//task->tss.ss = 1 * 8;
	//task->tss.ds = 1 * 8;
	//task->tss.fs = 1 * 8;
	//task->tss.gs = 1 * 8;
	//*((int *) (task->tss.esp + 4)) = (int) sht;
	//*((int *) (task->tss.esp + 8)) = memtotal;
	//task_run(task, 2, 2); /* level=2, priority=2 */
	//sht->task = task;
	//sht->flags |= 0x20;	/* 有光标 */
	//fifo32_init(&task->fifo, 128, cons_fifo, task);
	return sht;
}

void close_constask(struct TASK *task)
{
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	task_sleep(task);
	memman_free_4k(memman, task->cons_stack, 64 * 1024);
	memman_free_4k(memman, (int) task->fifo.buf, 128 * 4);
	task->flags = 0; /* 用来代替task_free(task) */
	return;
}

void close_console(struct SHEET *sht)
{
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct TASK *task = sht->task;
	memman_free_4k(memman, (int) sht->buf, 256 * 165);
	sheet_free(sht);
	close_constask(task);
	return;
}

