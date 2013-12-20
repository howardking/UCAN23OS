/* filename:    timer.c
 * description: 定时器函数
 * author:      Howard
 * date:        2013-12-02
 * version:     v1.0
 */
 
 #include "bootpack.h"
 
 struct TIMERCTL timerctl;
 
 void init_pit(void)
{
	int i;
	struct TIMER *t;
	io_out8(PIT_CTRL, 0x34);
	io_out8(PIT_CNT0, 0x9c);
	io_out8(PIT_CNT0, 0x2e);
	timerctl.count = 0;
	//timerctl.next = 0xffffffff;
	for (i=0; i<MAX_TIMER; i++){
		timerctl.timers0[i].flags = 0;   /*未使用*/
	}
	t = timer_alloc();
	t->timeout = 0xffffffff;
	t->flags = TIMER_FLAGS_USING;
	t->next = 0;
	timerctl.t0 = t;
	timerctl.next = 0xffffffff;
	return;
}

struct TIMER *timer_alloc(void)
{
	int i;
	for (i=0; i<MAX_TIMER; i++){
		if (timerctl.timers0[i].flags == 0){
			timerctl.timers0[i].flags = TIMER_FLAGS_ALLOC;
			return &timerctl.timers0[i];
		}
	}
	return 0;
}

void timer_free(struct TIMER *timer)
{
	timer->flags = 0;
	return;
}

void timer_init(struct TIMER *timer, struct FIFO32 *fifo, int data)
{
	timer->fifo = fifo;
	timer->data = data;
	return;
}

void timer_settime(struct TIMER *timer, unsigned int timeout)
{
	int e; 
	struct TIMER *t, *s;
	timer->timeout = timeout + timerctl.count;
	timer->flags = TIMER_FLAGS_USING;
	e = io_load_eflags();
	io_cli();
	/*t = timerctl.t0;
	if (1==timerctl.using){
		timerctl.t0 = timer;
		timer->next = 0;
		timerctl.next = timer->timeout;
		io_store_eflags(e);
		return;
	}*/
	t = timerctl.t0;
	if (timer->timeout<=t->timeout){
		timerctl.t0 = timer;
		timer->next = t;
		timerctl.next = timer->timeout;
		io_store_eflags(e);
		return;
	}
	for (;;){
		s = t;
		t = t->next;
		if (timer->timeout<=t->timeout){
			s->next = timer;
			timer->next = t;
			io_store_eflags(e);
			return;
		}
	}
}
void inthandler20(int *esp)
{
	struct TIMER *timer;
	char ts = 0;
	io_out8(PIC0_OCW2, 0x60);
	timerctl.count ++;
	if (timerctl.next>timerctl.count){
		return;
	}
	timer = timerctl.t0;
	for (;;){
		if (timer->timeout>timerctl.count){
			break;
		}
		timer->flags = TIMER_FLAGS_ALLOC;
		if (timer!=task_timer){
			fifo32_put(timer->fifo, timer->data);
		}else {
			ts = 1;
		}
		
		timer = timer->next;
	} 
	//timerctl.using -=i;
	/*for (j=0; j<timerctl.using; j++){
		timerctl.timers[j] = timerctl.timers[i+j];
	}*/
	timerctl.t0 = timer;
	//if (timerctl.using>0){
	timerctl.next = timer->timeout;
	//}else{
	//	timerctl.next = 0xffffffff;
	//}
	if (0!=ts){
		task_switch();
	}
	return;
}
