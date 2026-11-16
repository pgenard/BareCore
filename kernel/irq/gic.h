#ifndef GIC_H
#define GIC_H

void gic_init(void);

void gic_redistributor_wake(void);
void gic_distributor_enable(void);
void gic_cpu_enable(void);

void gic_set_group1(unsigned int irq);
void gic_set_priority(unsigned int irq);
void gic_enable_irq(unsigned int irq);
void gic_route_irq(unsigned int irq);

#endif
