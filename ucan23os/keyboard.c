/* filename:    keyboard.c
 * description: 包含了有关键盘的操作
 * author:      Howard
 * date:        2013-12-01
 * version:     v1.0
 */


#include "bootpack.h"

struct FIFO32 *keyfifo;
int keydata0;

void inthandler21(int *esp)
{
	/*ps/2键盘中断*/
	//struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;
	int data;
	io_out8(PIC0_OCW2, 0x61);
	data = io_in8(PORT_KEYDAT);
	
	fifo32_put(keyfifo, data+keydata0);
	/*if (keybuf.len<32){
		keybuf.data[keybuf.next_w] = data;
		keybuf.len ++;
		keybuf.next_w ++;
		if (keybuf.next_w == 32) {
			keybuf.next_w = 0;
		}
	}
	*/
	
	return;
}

void wait_KBC_sendready(void)
{
	for (;;){
		if (0==(io_in8(PORT_KEYSTA) & KEYSTA_SEND_NOTREADY)){
			break;
		}
	}
	return;
}

void init_keyboard(struct FIFO32 *fifo, int data0)
{
	keyfifo = fifo;
	keydata0 = data0;
	wait_KBC_sendready();
	io_out8(PORT_KEYCMD, KEYCMD_WRITE_MODE);
	wait_KBC_sendready();
	io_out8(PORT_KEYDAT, KBC_MODE);
	return;
}