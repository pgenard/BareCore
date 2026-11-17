#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "systime.h"

typedef void (*task_entry_t)(void);

typedef struct {
  task_entry_t entry;
  systime_t period;
  systime_t last_run;
} task_desc_t;

void scheduler_init(void);
int scheduler_add_task(task_entry_t entry, systime_t period);
void scheduler_run(void);
void scheduler_idle(void);

unsigned long scheduler_get_idle_count(void);
unsigned long scheduler_get_idle_wake_count(void);

#endif
