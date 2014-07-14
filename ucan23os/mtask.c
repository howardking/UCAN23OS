/* filename:    mtask.c
 * description: 多任务处理函数
 * author:      Howard
 * date:        2013-12-04
 * version:     v1.0
 */
 
 #include "bootpack.h"
 
 struct TASKCTL *taskctl;
 struct TIMER *task_timer;
 struct TIMER *mt_timer;
 int mt_tr;
 
 struct TASK *task_init(struct MEMMAN *memman)
 {
	int i;
	struct TASK *task, *idle;
	struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *)ADR_GDT;

	taskctl = (struct TASKCTL *)memman_alloc_4k(memman, sizeof (struct TASKCTL));
	for (i=0; i<MAX_TASKS; i++){
		taskctl->tasks0[i].flags = 0;
		taskctl->tasks0[i].sel = (TASK_GDT0+i) * 8;
		taskctl->tasks0[i].tss.ldtr = (TASK_GDT0 + MAX_TASKS + i) * 8;
		set_segmdesc(gdt+TASK_GDT0+i, 103, (int)&taskctl->tasks0[i].tss, AR_TSS32);		
		set_segmdesc(gdt + TASK_GDT0 + MAX_TASKS + i, 15, (int) taskctl->tasks0[i].ldt, AR_LDT);
	}
	
	for (i = 0; i < MAX_TASKLEVELS; i++) {
		taskctl->level[i].running = 0;
		taskctl->level[i].now = 0;
	}
	
	task = task_alloc();
	task->flags = 2;		/*活动中标志*/
	task->priority = 2;
//	taskctl->running = 1;
//	taskctl->now = 0;
	task->level = 0;  /* 最高LEVEL */
	task_add(task);
	task_switchsub();  /* LEVEL设置 */
//	taskctl->tasks[0] = task;
	load_tr(task->sel);
	task_timer = timer_alloc();
	timer_settime(task_timer, task->priority);
	
	/* 最低优先级的进程，其可在没有程序运行的时候自己运行 */
	idle = task_alloc();
	idle->tss.esp = memman_alloc_4k(memman, 64 * 1024) + 64 * 1024;
	idle->tss.eip = (int) &task_idle;
	idle->tss.es = 1 * 8;
	idle->tss.cs = 2 * 8;
	idle->tss.ss = 1 * 8;
	idle->tss.ds = 1 * 8;
	idle->tss.fs = 1 * 8;
	idle->tss.gs = 1 * 8;
	task_run(idle, MAX_TASKLEVELS - 1, 1);
	
	return task;
 }
 
 struct TASK *task_alloc(void)
 {
	int i;
	struct TASK *task;
	for (i=0; i<MAX_TASKS; i++){
		if (0==taskctl->tasks0[i].flags){
			task = &taskctl->tasks0[i];
			task->flags = 1;
			task->tss.eflags = 0x00000202;
			task->tss.eax = 0;
			task->tss.ecx = 0;
			task->tss.edx = 0;
			task->tss.ebx = 0;
			//tss_b.esp = task_b_esp;
			task->tss.ebp = 0;
			task->tss.esi = 0;
			task->tss.edi = 0;
			task->tss.es = 0;
			//tss_b.cs = 2 * 8;
			//tss_b.ss = 1 * 8;
			task->tss.ds = 0;
			task->tss.fs = 0;
			task->tss.gs = 0;
			//task->tss.ldtr = 0;
			task->tss.iomap = 0x40000000;
			task->tss.ss0 = 0;
			return task;
		}
	}
	return 0;
 }
 
 void task_run(struct TASK *task, int level, int priority)
 {
	if (level < 0) {
		level = task->level; /* 不改变level */
	}
 
	if (priority>0){
		task->priority = priority;
	} 
	
	if (task->flags == 2 && task->level != level) { /* 改变活动中的level */
		task_remove(task); /* flag的值会变为1，下面的if语句也会被执行 */
	}
	
	if ( 2!=task->flags){
		/* 从休眠的情况唤醒 */
		task->level = level;
		task_add(task);
	}
	
	taskctl->lv_change = 1; /* 下次任务切换时检查level */
	
	return;
 }
 
 void task_switch(void)
 {
	struct TASKLEVEL *tl = &taskctl->level[taskctl->now_lv];
	struct TASK *new_task, *now_task = tl->tasks[tl->now];
	tl->now++;
	if (tl->now == tl->running) {
		tl->now = 0;
	}
	if (taskctl->lv_change != 0) {
		task_switchsub();
		tl = &taskctl->level[taskctl->now_lv];
	}
	new_task = tl->tasks[tl->now];
	timer_settime(task_timer, new_task->priority);
	if (new_task != now_task) {
		farjmp(0, new_task->sel);
	}
	return;
 }
 
 void task_sleep(struct TASK *task)
 {
	//int i;
	//char ts = 0;
	struct TASK *now_task;
	//if (task->flags == 2){ /*任务处于唤醒状态*/
	//	if (task==taskctl->tasks[taskctl->now]){
	//		ts = 1; /*让自己休眠的话，之后需要进行任务切换*/
	//	}
	//	for (i=0; i<taskctl->running; i++){
	//		if (taskctl->tasks[i] == task){
	//			break;
	//		}
	//	}
	//	taskctl->running --;
	//	if (i<taskctl->now){
	//		taskctl->now --; /*需要移动成员、要相应地处理*/
	//	}
	//	for (; i<taskctl->running; i++){
	//		taskctl->tasks[i] = taskctl->tasks[i+1];
	//	}
	//	task->flags = 1;   /*不工作状态*/
	//	if (0!=ts){
	//		if (taskctl->now>=taskctl->running){
	//			taskctl->now = 0;
	//		}
	//		farjmp(0, taskctl->tasks[taskctl->now]->sel);
	//	}
		
	//}
	
	if (task->flags == 2) {
		/* 任务处于唤醒状态 */
		now_task = task_now();
		task_remove(task); /* flag的值将变为1 */
		if (task == now_task) {
			/* 如果是让自己休眠，则需要任务切换 */
			task_switchsub();
			now_task = task_now(); /* 获取当前要切换的任务 */
			farjmp(0, now_task->sel);
		}
	}
	
	return;
 }
 
 struct TASK *task_now(void)
 {
	struct TASKLEVEL *t1 = &taskctl->level[taskctl->now_lv];
	return t1->tasks[t1->now];
 }
 
 void task_add(struct TASK *task)
{
	struct TASKLEVEL *tl = &taskctl->level[task->level];
	tl->tasks[tl->running] = task;
	tl->running++;
	task->flags = 2; /* 活动中 */
	return;
}

void task_remove(struct TASK *task)
{
	int i;
	struct TASKLEVEL *tl = &taskctl->level[task->level];

	/* 寻找task所在位置 */
	for (i = 0; i < tl->running; i++) {
		if (tl->tasks[i] == task) {
			/* 找到了要删除的任务 */
			break;
		}
	}

	tl->running--;
	if (i < tl->now) {
		tl->now--; /* 需要移动成员，要相应的处理 */
	}
	if (tl->now >= tl->running) {
		/* 如果now的值出现异常，则进行修正 */
		tl->now = 0;
	}
	task->flags = 1; /* 休眠中 */

	/* 移动 */
	for (; i < tl->running; i++) {
		tl->tasks[i] = tl->tasks[i + 1];
	}

	return;
} 


void task_switchsub(void)
{
	int i;
	/* 寻找最上层的LEVEL */
	for (i = 0; i < MAX_TASKLEVELS; i++) {
		if (taskctl->level[i].running > 0) {
			break; /* 找到了 */
		}
	}
	taskctl->now_lv = i;
	taskctl->lv_change = 0;
	return;
}

void task_idle(void)
{
	for (;;) {
		io_hlt();
	}
}

void mt_init(void)
{
	mt_timer = timer_alloc();
	timer_settime(mt_timer, 2);
	mt_tr = 3*8;	
	return;
}
 
void mt_taskswitch(void)
 {
	if (mt_tr == 3*8){
		mt_tr = 4*8;
	}else{
		mt_tr = 3*8;
	}
	timer_settime(mt_timer, 2);
	farjmp(0, mt_tr);
}