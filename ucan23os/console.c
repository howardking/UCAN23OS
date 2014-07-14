/* filename:    console.c
 * description: the console window-->can type in command
 * author:      Howard
 * date:        2014-7-8
 * version:     v1.0
 */

#include "bootpack.h"
#include <stdio.h>
#include <string.h>

void console_task(struct SHEET *sheet, unsigned int memtotal)
{
	//struct FIFO32 fifo;
	//struct TIMER *timer;
	struct TASK *task = task_now();

	int i; //, fifobuf[128], cursor_x = 16, cursor_y = 28, cursor_c = -1;
	struct CONSOLE cons;
	char s[30]; 
	char cmdline[30];// *p;
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	//struct FILEINFO *finfo = (struct FILEINFO *) (ADR_DISKIMG + 0x002600);
	int *fat = (int *) memman_alloc_4k(memman, 4 * 2880);
	//struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) ADR_GDT;
	//int x, y;
	cons.sht = sheet;
	cons.cur_x =  8;
	cons.cur_y = 28;
	cons.cur_c = -1;
	//*((int *) 0x0fec) = (int) &cons;
	task->cons = &cons;
	
	//fifo32_init(&task->fifo, 128, fifobuf, task);
	if (cons.sht != 0) {
		cons.timer = timer_alloc();
		timer_init(cons.timer, &task->fifo, 1);
		timer_settime(cons.timer, 50);
	}
	file_readfat(fat, (unsigned char *) (ADR_DISKIMG + 0x000200));

	/* ������ʾ��# */
	//putfonts8_asc_sht(sheet, 8, 28, COL8_FFFFFF, COL8_000000, "#", 1);
	/* ��ʾ��ʾ��# */
	cons_putchar(&cons, '#', 1);
	
	for (;;) {
		io_cli();
		if (fifo32_status(&task->fifo) == 0) {
			task_sleep(task);
			io_sti();
		} else {
			i = fifo32_get(&task->fifo);
			io_sti();
			if (i <= 1 && cons.sht != 0) { /* ����ö�ʱ�� */
				if (i != 0) {
					timer_init(cons.timer, &task->fifo, 0); /* �´���0 */
					if (cons.cur_c >= 0){
						cons.cur_c = COL8_FFFFFF;
					}
				} else {
					timer_init(cons.timer, &task->fifo, 1); /* �´���1 */
					if (cons.cur_c >= 0){
						cons.cur_c = COL8_000000;
					}
				}
				timer_settime(cons.timer, 50);
			}
			if (i == 2) {	/* ���ON */
				cons.cur_c = COL8_FFFFFF;
			}
			if (i == 3) {	/* ���OFF */
				if (cons.sht != 0) {
					boxfill8(cons.sht->buf, cons.sht->bxsize, COL8_000000, cons.cur_x, cons.cur_y, cons.cur_x + 7, cons.cur_y + 15);
				}
				cons.cur_c = -1;
			}
			if (i == 4) {	/* ��������д��ڵġ�X����ť */
				cmd_exit(&cons, fat);
			}
			if (256 <= i && i <= 511) { /* �������ݣ�ͨ������A�� */
				if (i == 8 + 256) {
					/* �˸�� */
					if (cons.cur_x > 16) {
						/* �ÿհײ������󽫹����ǰ��һλ */
						//putfonts8_asc_sht(sheet, cursor_x, cursor_y, COL8_FFFFFF, COL8_000000, " ", 1);
						cons_putchar(&cons, ' ', 0);
						cons.cur_x -= 8;
					}
				 }else if (10 + 256 == i){
					/* Enter */
					/* �ÿհײ������󽫹����ǰ��һλ */
					//putfonts8_asc_sht(sheet, cursor_x, cursor_y, COL8_FFFFFF, COL8_000000, " ", 1);
					//cmdline[cursor_x / 8 - 2] = 0;
					//cursor_y = cons_newline(cursor_y, sheet);
					cons_putchar(&cons, ' ', 0);
					cmdline[cons.cur_x / 8 - 2] = 0;
					cons_newline(&cons);
					cons_runcmd(cmdline, &cons, fat, memtotal);	/* �������� */
					if (cons.sht == 0) {
						cmd_exit(&cons, fat);
					}
					/* ��ʾ��ʾ�� */
					cons_putchar(&cons, '#', 1);
				}else{
					/* һ���ַ� */
					if (cons.cur_x < 240) {
						/* ��ʾһ���ַ�֮�󽫹�����һλ */
						cmdline[cons.cur_x / 8 - 2] = i - 256;
						cons_putchar(&cons, i - 256, 1);
					}
				}
				//	if (strcmp(cmdline, "mem") == 0) {
						/* mem���� */
				//		sprintf(s, "total   %dMB", memtotal / (1024 * 1024));
				//		putfonts8_asc_sht(sheet, 8, cursor_y, COL8_FFFFFF, COL8_000000, s, 30);
				//		cursor_y = cons_newline(cursor_y, sheet);
				//		sprintf(s, "free %dKB", memman_total(memman) / 1024);
				//		putfonts8_asc_sht(sheet, 8, cursor_y, COL8_FFFFFF, COL8_000000, s, 30);
				//		cursor_y = cons_newline(cursor_y, sheet);
				//		cursor_y = cons_newline(cursor_y, sheet);
				//	} else if (strcmp(cmdline, "cls") == 0||strcmp(cmdline, "clean") == 0 || strcmp(cmdline, "reset") == 0){
						/* cls����clean����reset���� */
						
				//		for (y = 28; y < 28 + 128; y++) {
				//			for (x = 8; x < 8 + 240; x++) {
				//				sheet->buf[x + y * sheet->bxsize] = COL8_000000;
				//			}
				//		}
				//		sheet_refresh(sheet, 8, 28, 8 + 240, 28 + 128);
				//		cursor_y = 28;
				//		
				//	}else if (strcmp(cmdline, "yezi") == 0) {
						/* ��������Ҳ���ǿ��� */
				//		putfonts8_asc_sht(sheet, 8, cursor_y, COL8_FFFFFF, COL8_000000, "Welcome to Ucan23-OS.", 22);
				//		cursor_y = cons_newline(cursor_y, sheet);
				//		cursor_y = cons_newline(cursor_y, sheet);
				//	}else if (strcmp(cmdline, "520") == 0) {
				//		/* ��������Ҳ���ǿ��� */
				//		putfonts8_asc_sht(sheet, 8, cursor_y, COL8_FFFFFF, COL8_000000, "Wang Shuaihua Love You!", 24);
				//		cursor_y = cons_newline(cursor_y, sheet);
				//		cursor_y = cons_newline(cursor_y, sheet);
				//	}else if (strcmp(cmdline, "dir") == 0 || strcmp(cmdline, "ls") == 0) {
				//		/* dir���� */
				//		for (x = 0; x < 224; x++) {
				//			if (finfo[x].name[0] == 0x00) {
				//				break;
				//			}
				//			if (finfo[x].name[0] != 0xe5) {
				//				if ((finfo[x].type & 0x18) == 0) {
				//					sprintf(s, "filename.ext   %7d", finfo[x].size);
				//					for (y = 0; y < 8; y++) {
				//						s[y] = finfo[x].name[y];
				//					}
				//					s[ 9] = finfo[x].ext[0];
				//					s[10] = finfo[x].ext[1];
				//					s[11] = finfo[x].ext[2];
				//					putfonts8_asc_sht(sheet, 8, cursor_y, COL8_FFFFFF, COL8_000000, s, 30);
				//					cursor_y = cons_newline(cursor_y, sheet);
				//				}
				//			}
				//		}
				//		cursor_y = cons_newline(cursor_y, sheet);
				//	}else if (strncmp(cmdline, "type ", 5) == 0) {
				//		/* type���� */
				//		/* ׼���ļ��� */
				//		for (y = 0; y < 11; y++) {
				//			s[y] = ' ';
				//		}
				//		y = 0;
				//		for (x = 5; y < 11 && cmdline[x] != 0; x++) {
				//			if (cmdline[x] == '.' && y <= 8) {
				//				y = 8;
				//			} else {
				//				s[y] = cmdline[x];
				//				if ('a' <= s[y] && s[y] <= 'z') {
				//					/* ��Сд��ĸת���ɴ�д */
				//					s[y] -= 0x20;
				//				} 
				//				y++;
				//			}
				//		}
				//		/* Ѱ���ļ� */
				//		for (x = 0; x < 224; ) {
				//			if (finfo[x].name[0] == 0x00) {
				//				break;
				//			}
				//			if ((finfo[x].type & 0x18) == 0) {
				//				for (y = 0; y < 11; y++) {
				//					if (finfo[x].name[y] != s[y]) {
				//						goto type_next_file;
				//					}
				//				}
				//				break; /* �ҵ��ļ� */
				//			}
	//	type_next_file:
				//			x++;
				//		}
				//		if (x < 224 && finfo[x].name[0] != 0x00) {
							/* �ҵ��ļ������ */
							//y = finfo[x].size;
							//p = (char *) (finfo[x].clustno * 512 + 0x003e00 + ADR_DISKIMG);
				//			p = (char *) memman_alloc_4k(memman, finfo[x].size);
				//			file_loadfile(finfo[x].clustno, finfo[x].size, p, fat, (char *) (ADR_DISKIMG + 0x003e00));
				//			cursor_x = 8;
							
				//			for (y = 0; y < finfo[x].size; y++) {
								/* ������� */
				//				s[0] = p[y];
				//				s[1] = 0;
				//				if (s[0] == 0x09) {	/* �Ʊ�� */
				//					for (;;) {
				//						putfonts8_asc_sht(sheet, cursor_x, cursor_y, COL8_FFFFFF, COL8_000000, " ", 1);
				//						cursor_x += 8;
				//						if (cursor_x == 8 + 240) {
				//							cursor_x = 8;
				//							cursor_y = cons_newline(cursor_y, sheet);
				//						}
				//						if (((cursor_x - 8) & 0x1f) == 0) {
				//							break;	/* ��32������break */
				//						}
				//					}
				//				} else if (s[0] == 0x0a) {	/* ���� */
				//					cursor_x = 8;
				//					cursor_y = cons_newline(cursor_y, sheet);
				//				} else if (s[0] == 0x0d) {	/* �س� */
									/* ��ʱ���Բ����д��� */
				//				} else {	/* һ���ַ� */
				//					putfonts8_asc_sht(sheet, cursor_x, cursor_y, COL8_FFFFFF, COL8_000000, s, 1);
				//					cursor_x += 8;
				//					if (cursor_x == 8 + 240) {
				//						cursor_x = 8;
				//						cursor_y = cons_newline(cursor_y, sheet);
				//					}
				//				}
				//			}
				//			memman_free_4k(memman, (int) p, finfo[x].size);
				//		} else {
							/* û���ҵ��ļ������ */
				//			putfonts8_asc_sht(sheet, 8, cursor_y, COL8_FFFFFF, COL8_000000, "File not found.", 15);
				//			cursor_y = cons_newline(cursor_y, sheet);
				//		}
				//		cursor_y = cons_newline(cursor_y, sheet);
				//	}else if (strcmp(cmdline, "hlt") == 0) {
						/* ����Ӧ�ó���hlt.hrb */
				//		for (y = 0; y < 11; y++) {
				//			s[y] = ' ';
				//		}
				//		s[0] = 'H';
				//		s[1] = 'L';
				//		s[2] = 'T';
				//		s[8] = 'H';
				//		s[9] = 'R';
				//		s[10] = 'B';
				//		for (x = 0; x < 224; ) {
				//			if (finfo[x].name[0] == 0x00) {
				//				break;
				//			}
				//			if ((finfo[x].type & 0x18) == 0) {
				//				for (y = 0; y < 11; y++) {
				//					if (finfo[x].name[y] != s[y]) {
				//						goto hlt_next_file;
				//					}
				//				}
				//				break; /* �ҵ��ļ� */
				//			}
	//	hlt_next_file:
				//			x++;
				//		}
				//		if (x < 224 && finfo[x].name[0] != 0x00) {
				//			/* �ҵ��ļ������ */
				//			p = (char *) memman_alloc_4k(memman, finfo[x].size);
				//			file_loadfile(finfo[x].clustno, finfo[x].size, p, fat, (char *) (ADR_DISKIMG + 0x003e00));
				//			set_segmdesc(gdt + 1003, finfo[x].size - 1, (int) p, AR_CODE32_ER);
				//			farjmp(0, 1003 * 8);
				//			memman_free_4k(memman, (int) p, finfo[x].size);
				//		} else {
				//			/* �t�@û���ҵ��ļ������ */
				//			putfonts8_asc_sht(sheet, 8, cursor_y, COL8_FFFFFF, COL8_000000, "File not found.", 15);
				//			cursor_y = cons_newline(cursor_y, sheet);
				//		}
				//		cursor_y = cons_newline(cursor_y, sheet);
				//	}else if (cmdline[0] != 0) {
						/* ��������Ҳ���ǿ��� */
				//		putfonts8_asc_sht(sheet, 8, cursor_y, COL8_FFFFFF, COL8_000000, "Bad command.", 12);
				//		cursor_y = cons_newline(cursor_y, sheet);
				//		cursor_y = cons_newline(cursor_y, sheet);
				//	}
					
					//if (cursor_y < 28 + 112) {
					//	cursor_y += 16; /* ���� */
					//} else {
					//	/* ���� */
					//	for (y = 28; y < 28 + 112; y++) {
					//		for (x = 8; x < 8 + 240; x++) {
					//			sheet->buf[x + y * sheet->bxsize] = sheet->buf[x + (y + 16) * sheet->bxsize];
					//		}
					//	}
					//	for (y = 28 + 112; y < 28 + 128; y++) {
					//		for (x = 8; x < 8 + 240; x++) {
					//			sheet->buf[x + y * sheet->bxsize] = COL8_000000;
					//		}
					//	}
						//sheet_refresh(sheet, 8, 28, 8 + 240, 28 + 128);
					//}
					/* ��ʾ��ʾ�� */
				//	putfonts8_asc_sht(sheet, 8, cursor_y, COL8_FFFFFF, COL8_000000, "#", 1);
				//	cursor_x = 16;
				// }else{
					/* һ���ַ� */
				//	if (cursor_x < 240) {
						/* ��ʾһ���ַ�����������һλ */
				//		s[0] = i - 256;
				//		s[1] = 0;
				//		cmdline[cursor_x / 8 - 2] = i - 256;
				//		putfonts8_asc_sht(sheet, cursor_x, cursor_y, COL8_FFFFFF, COL8_000000, s, 1);
				//		cursor_x += 8;
				//	}
				//}
			}
			
			/* ������ʾ��� */
			if (cons.sht != 0) {
				if (cons.cur_c >= 0){
					boxfill8(cons.sht->buf, cons.sht->bxsize, cons.cur_c, cons.cur_x, cons.cur_y, cons.cur_x + 7, cons.cur_y + 15);
					//boxfill8(sheet->buf, sheet->bxsize, cursor_c, cursor_x, cursor_y, cursor_x + 7, cursor_y + 15);
				}	
				sheet_refresh(sheet, cons.cur_x, cons.cur_y, cons.cur_x + 8, cons.cur_y + 16);
				//sheet_refresh(sheet, cursor_x, cursor_y, cursor_x + 8, cursor_y + 16);
			}
		}
		
	}
}

void cons_putchar(struct CONSOLE *cons, int chr, char move)
{
	char s[2];
	s[0] = chr;
	s[1] = 0;
	if (s[0] == 0x09) {	/* �Ʊ�� */
		for (;;) {
			if (cons->sht != 0) {
				putfonts8_asc_sht(cons->sht, cons->cur_x, cons->cur_y, COL8_FFFFFF, COL8_000000, " ", 1);
			}
			cons->cur_x += 8;
			if (cons->cur_x == 8 + 240) {
				cons_newline(cons);
			}
			if (((cons->cur_x - 8) & 0x1f) == 0) {
				break;	/* ��32������break */
			}
		}
	} else if (s[0] == 0x0a) {	/* ���� */
		cons_newline(cons);
	} else if (s[0] == 0x0d) {	/* �س� */
		/* ��ʱ���Բ����д��� */
	} else {	/* һ���ַ� */
		if (cons->sht != 0) {
			putfonts8_asc_sht(cons->sht, cons->cur_x, cons->cur_y, COL8_FFFFFF, COL8_000000, s, 1);
		}
		if (move != 0) {
			/* moveΪ0ʱ��겻����� */
			cons->cur_x += 8;
			if (cons->cur_x == 8 + 240) {
				cons_newline(cons);
			}
		}
	}
	return;
}

void cons_newline(struct CONSOLE *cons)
{
	int x, y;
	struct SHEET *sheet = cons->sht;
	if (cons->cur_y < 28 + 112) {
		cons->cur_y += 16; /* ���� */
	} else {
		/* ���� */
		if (sheet != 0) {
			for (y = 28; y < 28 + 112; y++) {
				for (x = 8; x < 8 + 240; x++) {
					sheet->buf[x + y * sheet->bxsize] = sheet->buf[x + (y + 16) * sheet->bxsize];
				}
			}
			for (y = 28 + 112; y < 28 + 128; y++) {
				for (x = 8; x < 8 + 240; x++) {
					sheet->buf[x + y * sheet->bxsize] = COL8_000000;
				}
			}
			sheet_refresh(sheet, 8, 28, 8 + 240, 28 + 128);
		}
	}
	cons->cur_x = 8;
	return;
}

void cons_putstr0(struct CONSOLE *cons, char *s)
{
	for (; *s != 0; s++) {
		cons_putchar(cons, *s, 1);
	}
	return;
}

void cons_putstr1(struct CONSOLE *cons, char *s, int l)
{
	int i;
	for (i = 0; i < l; i++) {
		cons_putchar(cons, s[i], 1);
	}
	return;
}

void cons_runcmd(char *cmdline, struct CONSOLE *cons, int *fat, unsigned int memtotal)
{
	if (strcmp(cmdline, "mem") == 0) {
		cmd_mem(cons, memtotal);
	} else if ((strcmp(cmdline, "cls") == 0|| strcmp(cmdline, "clean") == 0 || strcmp(cmdline, "reset") == 0)){
		/* cls����clean����reset���� */
		cmd_cls(cons);
	} else if (strcmp(cmdline, "dir") == 0 || strcmp(cmdline, "ls") == 0) {
		cmd_dir(cons);
	} else if (strncmp(cmdline, "type ", 5) == 0) {
		cmd_type(cons, fat, cmdline);
	} /*else if (strcmp(cmdline, "hlt") == 0) {
		cmd_hlt(cons, fat);
	}*/ else if (strcmp(cmdline, "yezi") == 0){
		cmd_yezi(cons);
	}else if (strcmp(cmdline, "520") == 0){
		cmd_wsh520(cons);
	} else if (strcmp(cmdline, "exit") == 0) {
		cmd_exit(cons, fat);
	}else if (strncmp(cmdline, "start ", 6) == 0) {
		cmd_start(cons, cmdline, memtotal);
	}else if (strncmp(cmdline, "ncst ", 5) == 0) {
		cmd_ncst(cons, cmdline, memtotal);
	}else if (cmdline[0] != 0) {
		if (cmd_app(cons, fat, cmdline) == 0) {
			/* ��������Ҳ���ǿ��� */
			/*putfonts8_asc_sht(cons->sht, 8, cons->cur_y, COL8_FFFFFF, COL8_000000, "Bad command.", 12);
			cons_newline(cons);
			cons_newline(cons);*/
			cons_putstr0(cons, "Bad command.\n\n");
		}
	}
	return;
}

void cmd_mem(struct CONSOLE *cons, unsigned int memtotal)
{
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	char s[60];
	sprintf(s, "total   %dMB\nfree %dKB\n\n", memtotal / (1024 * 1024), memman_total(memman) / 1024);
	cons_putstr0(cons, s);
	/*putfonts8_asc_sht(cons->sht, 8, cons->cur_y, COL8_FFFFFF, COL8_000000, s, 30);
	cons_newline(cons);
	sprintf(s, "free %dKB", memman_total(memman) / 1024);
	putfonts8_asc_sht(cons->sht, 8, cons->cur_y, COL8_FFFFFF, COL8_000000, s, 30);
	cons_newline(cons);
	cons_newline(cons);*/
	return;
}

void cmd_cls(struct CONSOLE *cons)
{
	int x, y;
	struct SHEET *sheet = cons->sht;
	for (y = 28; y < 28 + 128; y++) {
		for (x = 8; x < 8 + 240; x++) {
			sheet->buf[x + y * sheet->bxsize] = COL8_000000;
		}
	}
	sheet_refresh(sheet, 8, 28, 8 + 240, 28 + 128);
	cons->cur_y = 28;
	return;
}

void cmd_dir(struct CONSOLE *cons)
{
	struct FILEINFO *finfo = (struct FILEINFO *) (ADR_DISKIMG + 0x002600);
	int i, j;
	char s[30];
	for (i = 0; i < 224; i++) {
		if (finfo[i].name[0] == 0x00) {
			break;
		}
		if (finfo[i].name[0] != 0xe5) {
			if ((finfo[i].type & 0x18) == 0) {
				sprintf(s, "filename.ext   %7d\n", finfo[i].size);
				for (j = 0; j < 8; j++) {
					s[j] = finfo[i].name[j];
				}
				s[ 9] = finfo[i].ext[0];
				s[10] = finfo[i].ext[1];
				s[11] = finfo[i].ext[2];
				/*putfonts8_asc_sht(cons->sht, 8, cons->cur_y, COL8_FFFFFF, COL8_000000, s, 30);
				cons_newline(cons);*/
				cons_putstr0(cons, s);
			}
		}
	}
	cons_newline(cons);
	return;
}

void cmd_type(struct CONSOLE *cons, int *fat, char *cmdline)
{
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct FILEINFO *finfo = file_search(cmdline + 5, (struct FILEINFO *) (ADR_DISKIMG + 0x002600), 224);
	char *p;
	int i;
	if (finfo != 0) {
		/* �ҵ��ļ������ */
		p = (char *) memman_alloc_4k(memman, finfo->size);
		file_loadfile(finfo->clustno, finfo->size, p, fat, (char *) (ADR_DISKIMG + 0x003e00));
		/*for (i = 0; i < finfo->size; i++) {
			cons_putchar(cons, p[i], 1);
		}*/
		cons_putstr1(cons, p, finfo->size);
		memman_free_4k(memman, (int) p, finfo->size);
	} else {
		/* û���ҵ��ļ������ */
		/*putfonts8_asc_sht(cons->sht, 8, cons->cur_y, COL8_FFFFFF, COL8_000000, "File not found.", 15);
		cons_newline(cons);*/
		cons_putstr0(cons, "File not found.\n");
	}
	cons_newline(cons);
	return;
}

void cmd_exit(struct CONSOLE *cons, int *fat)
{
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct TASK *task = task_now();
	struct SHTCTL *shtctl = (struct SHTCTL *) *((int *) 0x0fe4);
	struct FIFO32 *fifo = (struct FIFO32 *) *((int *) 0x0fec);
	if (cons->sht != 0) {
		timer_cancel(cons->timer);
	}
	memman_free_4k(memman, (int) fat, 4 * 2880);
	io_cli();
	if (cons->sht != 0) {
		fifo32_put(fifo, cons->sht - shtctl->sheets0 + 768);	/* 768~1023 */
	} else {
		fifo32_put(fifo, task - taskctl->tasks0 + 1024);	/* 1024~2023 */
	}
	io_sti();
	for (;;) {
		task_sleep(task);
	}
}

void cmd_start(struct CONSOLE *cons, char *cmdline, int memtotal)
{
	struct SHTCTL *shtctl = (struct SHTCTL *) *((int *) 0x0fe4);
	struct SHEET *sht = open_console(shtctl, memtotal);
	struct FIFO32 *fifo = &sht->task->fifo;
	int i;
	sheet_slide(sht, 32, 4);
	sheet_updown(sht, shtctl->top);
	/* ��������������ַ������ָ��Ƶ��µ������д����� */
	for (i = 6; cmdline[i] != 0; i++) {
		fifo32_put(fifo, cmdline[i] + 256);
	}
	fifo32_put(fifo, 10 + 256);	/* Enter */
	cons_newline(cons);
	return;
}

void cmd_ncst(struct CONSOLE *cons, char *cmdline, int memtotal)
{
	struct TASK *task = open_constask(0, memtotal);
	struct FIFO32 *fifo = &task->fifo;
	int i;
	/* ��������������ַ������ָ��Ƶ��µ������д����� */
	for (i = 5; cmdline[i] != 0; i++) {
		fifo32_put(fifo, cmdline[i] + 256);
	}
	fifo32_put(fifo, 10 + 256);	/* Enter */
	cons_newline(cons);
	return;
}

int cmd_app(struct CONSOLE *cons, int *fat, char *cmdline)
{
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct FILEINFO *finfo;
	struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) ADR_GDT;
	char name[18], *p, *q;
	struct TASK *task = task_now();
	int i, segsiz, datsiz, esp, dathrb;
	struct SHTCTL *shtctl;
	struct SHEET *sht;
	
	
	/* ���������������ļ� */
	for (i = 0; i < 13; i++) {
		if (cmdline[i] <= ' ') {
			break;
		}
		name[i] = cmdline[i];
	}
	name[i] = 0; /* ���ļ�����0��β */

	/* Ѱ���ļ� */
	finfo = file_search(name, (struct FILEINFO *) (ADR_DISKIMG + 0x002600), 224);
	if (finfo == 0 && name[i - 1] != '.') {
		/* �����Ҳ����ļ��������ļ����������.hrb�������� */
		name[i    ] = '.';
		name[i + 1] = 'H';
		name[i + 2] = 'R';
		name[i + 3] = 'B';
		name[i + 4] = 0;
		finfo = file_search(name, (struct FILEINFO *) (ADR_DISKIMG + 0x002600), 224);
	}

	if (finfo != 0) {
		/* �ҵ��ļ������ */
		p = (char *) memman_alloc_4k(memman, finfo->size);
		// q = (char *) memman_alloc_4k(memman, 64 * 1024);
		//*((int *) 0xfe8) = (int) p;
		file_loadfile(finfo->clustno, finfo->size, p, fat, (char *) (ADR_DISKIMG + 0x003e00));
		//set_segmdesc(gdt + 1003, finfo->size - 1, (int) p, AR_CODE32_ER + 0x60);
		//set_segmdesc(gdt + 1004, 64 * 1024 - 1,   (int) q, AR_DATA32_RW + 0x60);
		
		if (finfo->size >= 8 && strncmp(p + 4, "Hari", 4) == 0 && *p == 0x00) { /* �޸��ļ���Ȩ�� */
			segsiz = *((int *) (p + 0x0000));
			esp    = *((int *) (p + 0x000c));
			datsiz = *((int *) (p + 0x0010));
			dathrb = *((int *) (p + 0x0014));
			q = (char *) memman_alloc_4k(memman, segsiz);
			//*((int *) 0xfe8) = (int) q;
			task->ds_base = (int) q;
			//set_segmdesc(gdt + 1003, finfo->size - 1, (int) p, AR_CODE32_ER + 0x60);
			//set_segmdesc(gdt + 1004, segsiz - 1,      (int) q, AR_DATA32_RW + 0x60);
			/* ���¸�д����Ĵ���κ����ݶ� */
			//set_segmdesc(gdt + task->sel / 8 + 1000, finfo->size - 1, (int) p, AR_CODE32_ER + 0x60);
			//set_segmdesc(gdt + task->sel / 8 + 2000, segsiz - 1,      (int) q, AR_DATA32_RW + 0x60);
			
			set_segmdesc(task->ldt + 0, finfo->size - 1, (int) p, AR_CODE32_ER + 0x60);
			set_segmdesc(task->ldt + 1, segsiz - 1,      (int) q, AR_DATA32_RW + 0x60);
			
			for (i = 0; i < datsiz; i++) {
				q[esp + i] = p[dathrb + i];
			}
			//start_app(0x1b, 1003 * 8, esp, 1004 * 8, &(task->tss.esp0));
			//start_app(0x1b, task->sel + 1000 * 8, esp, task->sel + 2000 * 8, &(task->tss.esp0));
			start_app(0x1b, 0 * 8 + 4, esp, 1 * 8 + 4, &(task->tss.esp0));
			shtctl = (struct SHTCTL *) *((int *) 0x0fe4);
			for (i = 0; i < MAX_SHEETS; i++) {
				sht = &(shtctl->sheets0[i]);
				if ((sht->flags & 0x11) == 0x11 && sht->task == task) {
					/* �ҵ���Ӧ�ó��������Ĵ��� */
					sheet_free(sht);	/* �ر� */
				}
			}
			timer_cancelall(&task->fifo);
			memman_free_4k(memman, (int) q, segsiz);
		}else{
			//start_app(0, 1003 * 8, 64 * 1024, 1004 * 8, &(task->tss.esp0));
			cons_putstr0(cons, ".hrb file format error.\n");
		}
		
		
		memman_free_4k(memman, (int) p, finfo->size);
		//memman_free_4k(memman, (int) q, 64 * 1024);
		
		cons_newline(cons);
		return 1;
	}
	/* û�ҵ��ļ������ */
	return 0;
	
	//if (finfo != 0) {
		/* �ҵ��ļ������ */
	//	p = (char *) memman_alloc_4k(memman, finfo->size);
	//	file_loadfile(finfo->clustno, finfo->size, p, fat, (char *) (ADR_DISKIMG + 0x003e00));
	//	set_segmdesc(gdt + 1003, finfo->size - 1, (int) p, AR_CODE32_ER);
	//	farcall(0, 1003 * 8);
	//	memman_free_4k(memman, (int) p, finfo->size);
	//} else {
		/* û�ҵ��ļ������ */
	//	putfonts8_asc_sht(cons->sht, 8, cons->cur_y, COL8_FFFFFF, COL8_000000, "File not found.", 15);
	//	cons_newline(cons);
	//}
	//cons_newline(cons);
	//return;
}

int *hrb_api(int edi, int esi, int ebp, int esp, int ebx, int edx, int ecx, int eax)
{
	struct TASK *task = task_now();
	int cs_base = task->ds_base;
	struct CONSOLE *cons = task->cons;
	//int cs_base = *((int *) 0xfe8);
	//struct CONSOLE *cons = (struct CONSOLE *) *((int *) 0x0fec);
	struct SHTCTL *shtctl = (struct SHTCTL *) *((int *) 0x0fe4);
	struct SHEET *sht;
	struct FIFO32 *sys_fifo = (struct FIFO32 *) *((int *) 0x0fec);
	int *reg = &eax + 1; /* eax����ĵ�ַ */
	/* ǿ���޸�PUSHAD�����ֵ */
		/* reg[0] : EDI,   reg[1] : ESI,   reg[2] : EBP,   reg[3] : ESP */
		/* reg[4] : EBX,   reg[5] : EDX,   reg[6] : ECX,   reg[7] : EAX */
	
	int i;
		
	if (edx == 1) {
		cons_putchar(cons, eax & 0xff, 1);
	} else if (edx == 2) {
		cons_putstr0(cons, (char *) ebx + cs_base);
	} else if (edx == 3) {
		cons_putstr1(cons, (char *) ebx + cs_base, ecx);
	} else if (edx == 4) {
		return &(task->tss.esp0);
	} else if (edx == 5) {
		sht = sheet_alloc(shtctl);
		sht->task = task;
		sht->flags |= 0x10;
		sheet_setbuf(sht, (char *) ebx + cs_base, esi, edi, eax);
		make_window8((char *) ebx + cs_base, esi, edi, (char *) ecx + cs_base, 0);
		//sheet_slide(sht, 100, 50);
		//sheet_updown(sht, 3);	/* ������߶�3λ��task_a֮�� */
		//sheet_slide(sht, (shtctl->xsize - esi) / 2, (shtctl->ysize - edi) / 2);
		sheet_slide(sht, ((shtctl->xsize - esi) / 2) & ~3, (shtctl->ysize - edi) / 2);
		sheet_updown(sht, shtctl->top); /* ������ͼ��߶�ָ��Ϊ�������ͼ��ĸ߶ȣ�����Ƶ��ϲ� */
		reg[7] = (int) sht;
	}  else if (edx == 6) {
		sht = (struct SHEET *) (ebx & 0xfffffffe);
		putfonts8_asc(sht->buf, sht->bxsize, esi, edi, eax, (char *) ebp + cs_base);
		if ((ebx & 1) == 0) {
			sheet_refresh(sht, esi, edi, esi + ecx * 8, edi + 16);
		}
	} else if (edx == 7) {
		sht = (struct SHEET *) (ebx & 0xfffffffe);
		boxfill8(sht->buf, sht->bxsize, ebp, eax, ecx, esi, edi);
		if ((ebx & 1) == 0) {
			sheet_refresh(sht, eax, ecx, esi + 1, edi + 1);
		}
	} else if (edx == 8) {
		memman_init((struct MEMMAN *) (ebx + cs_base));
		ecx &= 0xfffffff0;	/* ��16�ֽ�Ϊ��λ */
		memman_free((struct MEMMAN *) (ebx + cs_base), eax, ecx);
	} else if (edx == 9) {
		ecx = (ecx + 0x0f) & 0xfffffff0; /* ��16�ֽ�Ϊ��λ����ȡ�� */
		reg[7] = memman_alloc((struct MEMMAN *) (ebx + cs_base), ecx);
	} else if (edx == 10) {
		ecx = (ecx + 0x0f) & 0xfffffff0; /* ��16�ֽ�Ϊ��λ����ȡ�� */
		memman_free((struct MEMMAN *) (ebx + cs_base), eax, ecx);
	} else if (edx == 11) {
		sht = (struct SHEET *) (ebx & 0xfffffffe);
		sht->buf[sht->bxsize * edi + esi] = eax;
		if ((ebx & 1) == 0) {
			sheet_refresh(sht, esi, edi, esi + 1, edi + 1);
		}
	} else if (edx == 12) {
		sht = (struct SHEET *) ebx;
		sheet_refresh(sht, eax, ecx, esi, edi);
	} else if (edx == 13) {
		sht = (struct SHEET *) (ebx & 0xfffffffe);
		hrb_api_linewin(sht, eax, ecx, esi, edi, ebp);
		if ((ebx & 1) == 0) {
			sheet_refresh(sht, eax, ecx, esi + 1, edi + 1);
		}
	} else if (edx == 14) {
		sheet_free((struct SHEET *) ebx);
	} else if (edx == 15) {
		for (;;) {
			io_cli();
			if (fifo32_status(&task->fifo) == 0) {
				if (eax != 0) {
					task_sleep(task);	/* FIFOΪ�գ����߲��ȴ� */
				} else {
					io_sti();
					reg[7] = -1;
					return 0;
				}
			}
			i = fifo32_get(&task->fifo);
			io_sti();
			if (i <= 1) { /* ����ö�ʱ�� */
				/* Ӧ�ó�������ʱ����Ҫ��꣬������ǽ��´���ʾ�õ�ֵ��Ϊ1 */
				timer_init(cons->timer, &task->fifo, 1); /* �´���Ϊ1 */
				timer_settime(cons->timer, 50);
			}
			if (i == 2) {	/* ���ON */
				cons->cur_c = COL8_FFFFFF;
			}
			if (i == 3) {	/* ���OFF */
				cons->cur_c = -1;
			}
			
			if (i == 4) {	/* ֻ�ر������д��� */
				timer_cancel(cons->timer);
				io_cli();
				fifo32_put(sys_fifo, cons->sht - shtctl->sheets0 + 2024);	/* 2024�`2279 */
				cons->sht = 0;
				io_sti();
			}
			
			if (256 <= i) { /* �������ݣ�ͨ������A�� */
				reg[7] = i - 256;
				return 0;
			}
		}
	} else if (edx == 16) {
		reg[7] = (int) timer_alloc();
		((struct TIMER *) reg[7])->flags2 = 1; /* �����Զ�ȡ�� */
	} else if (edx == 17) {
		timer_init((struct TIMER *) ebx, &task->fifo, eax + 256);
	} else if (edx == 18) {
		timer_settime((struct TIMER *) ebx, eax);
	} else if (edx == 19) {
		timer_free((struct TIMER *) ebx);
	} else if (edx == 20) {
		if (eax == 0) {
			i = io_in8(0x61);
			io_out8(0x61, i & 0x0d);
		} else {
			i = 1193180000 / eax;
			io_out8(0x43, 0xb6);
			io_out8(0x42, i & 0xff);
			io_out8(0x42, i >> 8);
			i = io_in8(0x61);
			io_out8(0x61, (i | 0x03) & 0x0f);
		}
	}
	
	return 0;
}

void cmd_yezi(struct CONSOLE *cons)
{
	//putfonts8_asc_sht(sheet, 8, cursor_y, COL8_FFFFFF, COL8_000000, "Welcome to Ucan23-OS.", 22);
	//putfonts8_asc_sht(cons->sht, 8, cons->cur_y, COL8_FFFFFF, COL8_000000, "Welcome to Ucan23-OS.", 22);
	//cons_newline(cons);
	cons_putstr0(cons, "Welcome to Ucan23-OS.\n");
}

void cmd_wsh520(struct CONSOLE *cons)
{
	//putfonts8_asc_sht(sheet, 8, cursor_y, COL8_FFFFFF, COL8_000000, "Welcome to Ucan23-OS.", 22);
	//putfonts8_asc_sht(cons->sht, 8, cons->cur_y, COL8_FFFFFF, COL8_000000, "Wang Shuaihua Love You!", 24);
	//cons_newline(cons);
	cons_putstr0(cons, "Wang Shuaihua Love You!\n");
}

/*void hrb_api(int edi, int esi, int ebp, int esp, int ebx, int edx, int ecx, int eax)
{
	int cs_base = *((int *) 0xfe8);
	struct CONSOLE *cons = (struct CONSOLE *) *((int *) 0x0fec);
	if (edx == 1) {
		cons_putchar(cons, eax & 0xff, 1);
	} else if (edx == 2) {
		cons_putstr0(cons, (char *) ebx + cs_base);
	} else if (edx == 3) {
		cons_putstr1(cons, (char *) ebx + cs_base, ecx);
	}
	return;
}*/

int *inthandler0c(int *esp)
{
	//struct CONSOLE *cons = (struct CONSOLE *) *((int *) 0x0fec);
	struct TASK *task = task_now();
	struct CONSOLE *cons = task->cons;
	char s[30];
	cons_putstr0(cons, "\nINT 0C :\n Stack Exception.\n");
	sprintf(s, "EIP = %08X\n", esp[11]);
	cons_putstr0(cons, s);
	return &(task->tss.esp0);	/* ǿ�Ƴ������ */
}

int inthandler0d(int *esp)
{
	//struct CONSOLE *cons = (struct CONSOLE *) *((int *) 0x0fec);
	struct TASK *task = task_now();
	struct CONSOLE *cons = task->cons;
	char s[30];
	cons_putstr0(cons, "\nINT 0D :\n General Protected Exception.\n");
	sprintf(s, "EIP = %08X\n", esp[11]);
	cons_putstr0(cons, s);
	return &(task->tss.esp0); /* ǿ�ƽ������� */
}

void hrb_api_linewin(struct SHEET *sht, int x0, int y0, int x1, int y1, int col)
{
	int i, x, y, len, dx, dy;

	dx = x1 - x0;
	dy = y1 - y0;
	x = x0 << 10;
	y = y0 << 10;
	if (dx < 0) {
		dx = - dx;
	}
	if (dy < 0) {
		dy = - dy;
	}
	if (dx >= dy) {
		len = dx + 1;
		if (x0 > x1) {
			dx = -1024;
		} else {
			dx =  1024;
		}
		if (y0 <= y1) {
			dy = ((y1 - y0 + 1) << 10) / len;
		} else {
			dy = ((y1 - y0 - 1) << 10) / len;
		}
	} else {
		len = dy + 1;
		if (y0 > y1) {
			dy = -1024;
		} else {
			dy =  1024;
		}
		if (x0 <= x1) {
			dx = ((x1 - x0 + 1) << 10) / len;
		} else {
			dx = ((x1 - x0 - 1) << 10) / len;
		}
	}

	for (i = 0; i < len; i++) {
		sht->buf[(y >> 10) * sht->bxsize + (x >> 10)] = col;
		x += dx;
		y += dy;
	}

	return;
}


//int cons_newline(int cursor_y, struct SHEET *sheet)
//{
//	int x, y;
//	if (cursor_y < 28 + 112) {
//		cursor_y += 16; /* ���� */
//	} else {
//		/* ���� */
//		for (y = 28; y < 28 + 112; y++) {
//			for (x = 8; x < 8 + 240; x++) {
//				sheet->buf[x + y * sheet->bxsize] = sheet->buf[x + (y + 16) * sheet->bxsize];
//			}
//		}
//		for (y = 28 + 112; y < 28 + 128; y++) {
//			for (x = 8; x < 8 + 240; x++) {
//				sheet->buf[x + y * sheet->bxsize] = COL8_000000;
//			}
//		}
//		sheet_refresh(sheet, 8, 28, 8 + 240, 28 + 128);
//	}
//	return cursor_y;
//}