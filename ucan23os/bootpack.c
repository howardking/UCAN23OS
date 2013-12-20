/* filename:    bootpack.c
 * description: the UcanMain()file
 * author:      Howard
 * date:        2013-11-28
 * version:     v1.0
 */


#include <stdio.h>
#include "bootpack.h"

void putfonts8_asc_sht(struct SHEET *sht, int x, int y, int c, int b, char *s, int l);
void set490(struct FIFO32 *fifo, int mode);
void make_textbox8(struct SHEET *sht, int x0, int y0, int sx, int sy, int c);
void task_b_main(struct SHEET *sht_back);

void UcanMain(void)
{
	
	char *vram;
	char s[50];
	int fifobuf[128];//mcursor[256], keybuf[32], mousebuf[128], timerbuf[8], timerbuf2[8], timerbuf3[8];
	int mx, my, cursor_x, cursor_c; //task_b_esp;
	//int xsize, ysize;
	int i ;// count = 0;;
	unsigned int memtotal;
	struct MOUSE_DEC mdec; /*鼠标解码缩放的数据*/
	struct SHTCTL *shtctl;
	struct FIFO32 fifo;
	//struct FIFO32 timerfifo, timerfifo2, timerfifo3;
	struct TIMER *timer;
	struct SHEET *sht_back, *sht_mouse, *sht_win, *sht_win_b[3];
	unsigned char *buf_back, buf_mouse[256], *buf_win, *buf_win_b;
	static char keytable[0x54] = {
		0,   0,   '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '^', 0,   0,
		'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '@', '[', 0,   0,   'A', 'S',
		'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', ':', 0,   0,   ']', 'Z', 'X', 'C', 'V',
		'B', 'N', 'M', ',', '.', '/', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1',
		'2', '3', '0', '.'
	};
	
	struct TASK *task_a,*task_b[3];
	struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *)ADR_GDT;
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	
	struct BOOTINFO *binfo = (struct BOOTINFO *) 0x0ff0;
			 
	init_gdtidt();
	init_pic();
	io_sti();
		
	fifo32_init(&fifo, 128, fifobuf, 0);
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
	
	/*sht_win_b*/
	for (i=0; i<3; i++){
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
		*((int *) (task_b[i]->tss.esp + 4)) = (int) sht_win_b[i];
		task_run(task_b[i], i+1);
	}
	
	/*sht_win*/
	sht_win		= sheet_alloc(shtctl);
	buf_win		= (unsigned char *)memman_alloc_4k(memman, 160*52);
	sheet_setbuf(sht_win, buf_win, 144, 52, -1);
	make_window8(buf_win, 144, 52, "task_a", 1);
	make_textbox8(sht_win, 8, 28, 128, 16, COL8_FFFFFF);
	cursor_x = 8;
	cursor_c = COL8_FFFFFF;
	timer = timer_alloc();
	timer_init(timer, &fifo, 1);
	timer_settime(timer, 50);
	
	/*sht_mouse*/
	sht_mouse = sheet_alloc(shtctl);
	sheet_setbuf(sht_mouse, buf_mouse, 16, 16, 99);
	init_mouse_cursor8(buf_mouse, 99);
	mx = (binfo->scrnx-16) / 2;
	my = (binfo->scrny-28-16) /2;
	
	//putfonts8_asc(buf_win, 160, 24, 28, COL8_000000, "Welcome to");
	//putfonts8_asc(buf_win, 160, 24, 44, COL8_000000, "      Ucan-OS!");
	sheet_slide(sht_back, 0, 0);
	//init_mouse_cursor8(mcursor, 99);
	//xsize = (*binfo).scrnx;
	//ysize = (*binfo).scrny;
	vram = (*binfo).vram;
	sheet_slide(sht_win_b[0], 168,  56);
	sheet_slide(sht_win_b[1],   8, 116);
	sheet_slide(sht_win_b[2], 168, 116);
	sheet_slide(sht_win,        8,  56);
	sheet_slide(sht_mouse, mx, my);
	sheet_updown(sht_back,     0);
	sheet_updown(sht_win_b[0], 1);
	sheet_updown(sht_win_b[1], 2);
	sheet_updown(sht_win_b[2], 3);
	sheet_updown(sht_win,      4);
	sheet_updown(sht_mouse,    5);
	
	
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
	
	sprintf(s, "(%3d, %3d)", mx, my);
	putfonts8_asc_sht(sht_back, 0, 0, COL8_FFFFFF, COL8_008484, s, 10);
	
	//io_out8(PIC0_IMR, 0xf9);
	//io_out8(PIC1_IMR, 0xef);
	
	
	sprintf(s, "Memory %dMB  free: %dKB", memtotal/(1024*1024), memman_total(memman)/1024);
	putfonts8_asc_sht(buf_back, 0, 32, COL8_FFFFFF, COL8_008484, s, 40);
	
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
	
	for (;;){
		/*计数器程序*/
		//count ++;
		//sprintf(s, "%010d", timerctl.count);
		//boxfill8(buf_win, 160, COL8_C6C6C6, 40, 28, 119, 43);
		//putfonts8_asc(buf_win, 160,  40, 28, COL8_000000, s);
		//sheet_refresh(sht_win, 40, 28, 120, 44);
		
		io_cli();    /*执行nashfunc.nas里的_io_hlt*/
		if (0==fifo32_status(&fifo)){
			task_sleep(task_a);
			io_sti();
		} else {
			i = fifo32_get(&fifo);
			io_sti();
			/*
			if (2==i){
				farjmp(0, 4*8);
				timer_settime(timer_ts, 2);
			}*/
			if (i>=256 && i<=511){ /*键盘数据*/
				sprintf(s, "%02X", i-256);
				//boxfill8(buf_back, binfo->scrnx, COL8_000000, 0, 120, 15, 135);
				//putfonts8_asc(buf_back, binfo->scrnx, 0, 120, COL8_FFFFFF, s);
				//sheet_refresh(sht_back, 0, 120, 15*8, 136);
				putfonts8_asc_sht(sht_back, 0, 16, COL8_FFFFFF, COL8_008484, s, 2);
				if (i<256+0x54)
				{ 
					if (keytable[i-256]!=0 && cursor_x<128){
						s[0] = keytable[i-256];
						s[1] = 0;
						putfonts8_asc_sht(sht_win, cursor_x, 28, COL8_000000, COL8_FFFFFF, s, 1);
						cursor_x += 8;
					}
					
				}
				if (256+0x0e==i && cursor_x>8){
					putfonts8_asc_sht(sht_win, cursor_x, 28, COL8_000000, COL8_FFFFFF, " ", 1);
					cursor_x -= 8;
				}
				boxfill8(sht_win->buf, sht_win->bxsize, cursor_c, cursor_x, 28, cursor_x + 7, 43);
				sheet_refresh(sht_win, cursor_x, 28, cursor_x + 8, 44);
			} else if (i>=512 && i<=767){ /*鼠标数据*/
				if (0 != mouse_decode(&mdec, i-512)){					
					sprintf(s, "[lcr %4d %4d]", mdec.x, mdec.y);
					if ((mdec.btn & 0x01)!=0){
						s[1] = 'L';
					}
					if ((mdec.btn & 0x02)!=0){
						s[3] = 'R';
					}
					if ((mdec.btn & 0x04)!=0){
						s[2] = 'C';
					}
					putfonts8_asc_sht(sht_back, 32, 16, COL8_FFFFFF, COL8_008484, s, 15);
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
					sprintf(s, "(%3d, %3d)", mx, my);
					putfonts8_asc_sht(sht_back, 0, 0, COL8_FFFFFF, COL8_008484, s, 10);
					sheet_slide(sht_mouse, mx, my);
					//boxfill8(buf_back, binfo->scrnx, COL8_008484,binfo->scrnx/2,100,binfo->scrnx-80 , 136);
					//putfonts8_asc(buf_back, binfo->scrnx, binfo->scrnx/2, 100, COL8_FFFFFF,s);
					//sheet_refresh(sht_back, binfo->scrnx/2, 100, binfo->scrnx-80, 136);
					//putblock8_8(binfo->vram,binfo->scrnx, 16, 16, mx, my, mcursor, 16);
					//sheet_slide(sht_mouse, mx, my);
					if (0!=(mdec.btn&0x01)){
						/*按下左键、移动sht_win*/
						sheet_slide(sht_win, mx-80, my-8);
					}
				}
			} /*else if (10==i){
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
			}*/ else if (i<=1){
				if (i != 0) {
					timer_init(timer, &fifo, 0); /* 師偼0傪 */
					cursor_c = COL8_000000;
				} else {
					timer_init(timer, &fifo, 1); /* 師偼1傪 */
					cursor_c = COL8_FFFFFF;
				}
				timer_settime(timer, 50);
				boxfill8(sht_win->buf, sht_win->bxsize, cursor_c, cursor_x, 28, cursor_x + 7, 43);
				sheet_refresh(sht_win, cursor_x, 28, cursor_x + 8, 44);
		
			}
		}
	}
}

void make_window8(unsigned char *buf, int xsize, int ysize, char *title, char act)
{
	static char closebtn[14][16] = {
		"OOOOOOOOOOOOOOO@",
		"OQQQQQQQQQQQQQ$@",
		"OQQQQQQQQQQQQQ$@",
		"OQQQ@@QQQQ@@QQ$@",
		"OQQQQ@@QQ@@QQQ$@",
		"OQQQQQ@@@@QQQQ$@",
		"OQQQQQQ@@QQQQQ$@",
		"OQQQQQ@@@@QQQQ$@",
		"OQQQQ@@QQ@@QQQ$@",
		"OQQQ@@QQQQ@@QQ$@",
		"OQQQQQQQQQQQQQ$@",
		"OQQQQQQQQQQQQQ$@",
		"O$$$$$$$$$$$$$$@",
		"@@@@@@@@@@@@@@@@"
	};
	int x, y;
	char c, tc, tbc;
	if (0!=act){
		tc = COL8_FFFFFF;
		tbc = COL8_000084;
	}
	boxfill8(buf, xsize, COL8_C6C6C6, 0,         0,         xsize - 1, 0        );
	boxfill8(buf, xsize, COL8_FFFFFF, 1,         1,         xsize - 2, 1        );
	boxfill8(buf, xsize, COL8_C6C6C6, 0,         0,         0,         ysize - 1);
	boxfill8(buf, xsize, COL8_FFFFFF, 1,         1,         1,         ysize - 2);
	boxfill8(buf, xsize, COL8_848484, xsize - 2, 1,         xsize - 2, ysize - 2);
	boxfill8(buf, xsize, COL8_000000, xsize - 1, 0,         xsize - 1, ysize - 1);
	boxfill8(buf, xsize, COL8_C6C6C6, 2,         2,         xsize - 3, ysize - 3);
	boxfill8(buf, xsize, COL8_000084, 3,         3,         xsize - 4, 20       );
	boxfill8(buf, xsize, COL8_848484, 1,         ysize - 2, xsize - 2, ysize - 2);
	boxfill8(buf, xsize, COL8_000000, 0,         ysize - 1, xsize - 1, ysize - 1);
	putfonts8_asc(buf, xsize, 24, 4, COL8_FFFFFF, title);
	for (y = 0; y < 14; y++) {
		for (x = 0; x < 16; x++) {
			c = closebtn[y][x];
			if (c == '@') {
				c = COL8_000000;
			} else if (c == '$') {
				c = COL8_848484;
			} else if (c == 'Q') {
				c = COL8_C6C6C6;
			} else {
				c = COL8_FFFFFF;
			}
			buf[(5 + y) * xsize + (xsize - 21 + x)] = c;
		}
	}
	return;
}

void putfonts8_asc_sht(struct SHEET *sht, int x, int y, int c, int b, char *s, int l)
{
	boxfill8(sht->buf, sht->bxsize, b, x, y, x+l*8-1, y+15);
	putfonts8_asc(sht->buf, sht->bxsize, x, y, c, s);
	sheet_refresh(sht, x, y, x+l*8, y+16);
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

void make_textbox8(struct SHEET *sht, int x0, int y0, int sx, int sy, int c)
{
	int x1 = x0 + sx, y1 = y0 + sy;
	boxfill8(sht->buf, sht->bxsize, COL8_848484, x0 - 2, y0 - 3, x1 + 1, y0 - 3);
	boxfill8(sht->buf, sht->bxsize, COL8_848484, x0 - 3, y0 - 3, x0 - 3, y1 + 1);
	boxfill8(sht->buf, sht->bxsize, COL8_FFFFFF, x0 - 3, y1 + 2, x1 + 1, y1 + 2);
	boxfill8(sht->buf, sht->bxsize, COL8_FFFFFF, x1 + 2, y0 - 3, x1 + 2, y1 + 2);
	boxfill8(sht->buf, sht->bxsize, COL8_000000, x0 - 1, y0 - 2, x1 + 0, y0 - 2);
	boxfill8(sht->buf, sht->bxsize, COL8_000000, x0 - 2, y0 - 2, x0 - 2, y1 + 0);
	boxfill8(sht->buf, sht->bxsize, COL8_C6C6C6, x0 - 2, y1 + 1, x1 + 0, y1 + 1);
	boxfill8(sht->buf, sht->bxsize, COL8_C6C6C6, x1 + 1, y0 - 2, x1 + 1, y1 + 1);
	boxfill8(sht->buf, sht->bxsize, c,           x0 - 1, y0 - 1, x1 + 0, y1 + 0);
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