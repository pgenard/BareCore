#include "scheduler.h"

#define MAX_TASKS 10

static task_desc_t tasks[MAX_TASKS];
static unsigned int num_tasks = 0;

static volatile unsigned long idle_count = 0;
static volatile unsigned long idle_wake_count = 0;

/*
 * Save processor interrupt state and disable IRQ exceptions.
 */
static inline unsigned long scheduler_irq_save(void) {
    unsigned long daif;

    asm volatile(
        "mrs %0, daif\n"
        "msr daifset, #2\n"
        "isb"
        : "=r"(daif)
        :
        : "memory"
    );

    return daif;
}


/*
 * Restore previous processor interrupt state.
 */
static inline void scheduler_irq_restore(unsigned long daif) {
    asm volatile(
        "msr daif, %0\n"
        "isb"
        :
        : "r"(daif)
        : "memory"
    );
}


/*
 * Check whether at least one task is ready.
 */
static int scheduler_has_ready_task(void) {
    systime_t now = systime_get();

    for (unsigned int i = 0; i < num_tasks; i++) {
        task_desc_t *task = &tasks[i];

        if ((now - task->last_run) >= task->period) {
            return 1;
        }
    }

    return 0;
}


/*
 * Race-free CPU idle.
 */
void scheduler_idle(void) {
    unsigned long daif;

    /*
     * Prevent IRQ handlers from running during
     * the readiness check and idle transition.
     */
    daif = scheduler_irq_save();

    /*
     * Recheck task readiness with IRQs masked.
     */
    if (scheduler_has_ready_task()) {
        scheduler_irq_restore(daif);
        return;
    }

    /*
     * Enter low-power state.
     *
     * A pending IRQ can wake WFI even while
     * IRQ exceptions are masked.
     */
    idle_count++;

    asm volatile(
        "dsb sy\n"
        "wfi"
        ::: "memory"
    );

    idle_wake_count++;

    /*
     * Restore original interrupt state.
     */
    scheduler_irq_restore(daif);
}


/*
 * Register a periodic task.
 */
int scheduler_add_task(task_entry_t entry, systime_t period) {
    if (num_tasks >= MAX_TASKS ||
        entry == 0 ||
        period == 0) {
        return -1;
    }

    tasks[num_tasks].entry = entry;
    tasks[num_tasks].period = period;
    tasks[num_tasks].last_run = systime_get();

    num_tasks++;

    return 0;
}


/*
 * Main scheduler loop.
 */
void scheduler_run(void) {
    while (1) {
        systime_t now = systime_get();

        for (unsigned int i = 0; i < num_tasks; i++) {
            task_desc_t *task = &tasks[i];

            if ((now - task->last_run) >= task->period) {
                task->last_run = now;
                task->entry();
            }
        }

        /*
         * No immediately runnable task:
         * attempt race-free idle.
         */
        scheduler_idle();
    }
}


/*
 * Initialize scheduler.
 */
void scheduler_init(void) {
    num_tasks = 0;

    idle_count = 0;
    idle_wake_count = 0;
}


/*
 * Idle statistics.
 */
unsigned long scheduler_get_idle_count(void) {
  return idle_count;
}

unsigned long scheduler_get_idle_wake_count(void) {
  return idle_wake_count;
}
