/* filename:    fifo.c
 * description: �������йػ������Ĳ���
 * author:      Howard
 * date:        2013-12-03
 * version:     v1.0
 */


#include "bootpack.h"

#define FLAGS_OVERRUN		0x0001

void fifo32_init(struct FIFO32 *fifo, int size, int *buf, struct TASK *task)
{
	fifo->size = size;
	fifo->buf = buf;
	fifo->free = size;
	fifo->flags = 0;
	fifo->p = 0;
	fifo->q = 0;
	fifo->task = task;
	return;
}

int fifo32_put(struct FIFO32 *fifo, int data)
{
	/*��FIFO�������ݲ�����*/
	if (0==fifo->free){
		/*���*/
		fifo->flags |= FLAGS_OVERRUN;
		return -1;
	}
	fifo->buf[fifo->p] = data;
	fifo->p ++;
	if (fifo->p == fifo->size){
		fifo->p = 0;
	}
	fifo->free --;
	if (fifo->task!=0){
		if (fifo->task->flags != 2){
			task_run(fifo->task, 0);
		}
	}
	return 0;
}

int fifo32_get(struct FIFO32 *fifo)
{
	/*��FIFO��ȡһ������*/
	int data;
	if (fifo->free == fifo->size){
		/*������Ϊ�գ�û������*/
		return -1;
	}
	data = fifo->buf[fifo->q];
	fifo->q ++;
	if (fifo->q == fifo->size){
		fifo->q = 0;
	}
	fifo->free ++;
	
	return data;
}
int fifo32_status(struct FIFO32 *fifo)
{
	/*���ػ������л��ж�������*/
	return (fifo->size - fifo->free);
}