
/* filename:    mouse.c
 * description: �������й����Ĳ���
 * author:      Howard
 * date:        2013-12-01
 * version:     v1.0
 */


#include "bootpack.h"

struct FIFO32 *mousefifo;
int mousedata0;

void inthandler2c(int *esp)
{
	/*ps/2����ж�*/
	int data;
	io_out8(PIC1_OCW2, 0x64); /*֪ͨPIC1 IRQ-12�������Ѿ����*/
	io_out8(PIC0_OCW2, 0x62); /*֪ͨPIC0 IRQ-02�������Ѿ����*/
	data = io_in8(PORT_KEYDAT);
	fifo32_put(mousefifo, data+mousedata0);
	return;
}

void enable_mouse(struct FIFO32	*fifo, int data0, struct MOUSE_DEC *mdec)
{
	mousefifo = fifo;
	mousedata0 = data0;
	wait_KBC_sendready();
	io_out8(PORT_KEYCMD, KEYCMD_SENDTO_MOUSE);
	wait_KBC_sendready();
	io_out8(PORT_KEYDAT, MOUSECMD_ENABLE);
	mdec->phase = 0;
	return;
}

int mouse_decode(struct MOUSE_DEC *mdec, unsigned char dat)
{
	if (0 == mdec->phase){
		/*�ȴ�����0xfa״̬*/
		if (0xfa == dat){
			mdec->phase = 1;
		}
		return 0;
	}
	if (1 == mdec->phase){
		/*�ȴ����ĵ�һ���ֽ�*/
		if ((dat & 0xc8) == 0x08){
			mdec->buf[0] = dat;
			mdec->phase = 2;
		
		}
		return 0;
	}
	if (2 == mdec->phase){
		/*�ȴ����ĵڶ����ֽ�*/
		mdec->buf[1] = dat;
		mdec->phase = 3;
		return 0;
	} 
	if (3 == mdec->phase){
		mdec->buf[2] = dat;
		mdec->phase = 1;
		mdec->btn = mdec->buf[0] & 0x07;
		mdec->x = mdec->buf[1];
		mdec->y = mdec->buf[2];
		if (0!=(mdec->buf[0] & 0x10)){
			mdec->x |= 0xffffff00;
		}
		if (0!=(mdec->buf[0] & 0x20)){
			mdec->y |= 0xffffff00;
		}
		mdec->y = -mdec->y;
		return 1;
	}
	return -1;
}
