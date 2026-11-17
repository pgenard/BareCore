#include "gic.h"

#define GICD_BASE 0x38800000UL
#define GICR_BASE 0x38880000UL
#define GICR_SGI_BASE (GICR_BASE + 0x10000)

#define UART1_GIC_ID 58
#define GPT1_GIC_ID 87

#define GICD_CTLR (*(volatile unsigned int*) (GICD_BASE + 0x0000))
#define GICD_RWP (*(volatile unsigned int*) (GICD_BASE + 0x0A0))

#define GICD_IGROUPR(n) (*(volatile unsigned int*) (GICD_BASE + 0x080 + (((n) / 32) * 4)))
#define GICD_ISENABLER(n) (*(volatile unsigned int*) (GICD_BASE + 0x100 + (((n) / 32) * 4)))
#define GICD_IPRIORITYR(n) (*(volatile unsigned char*) (GICD_BASE + 0x400 + (n)))
#define GICD_IROUTER(n) (*(volatile unsigned long long*) (GICD_BASE + 0x6000 + ((n) * 8)))

#define GICR_WAKER (*(volatile unsigned int*) (GICR_BASE + 0x0014))

static void gic_wait_rwp(void) {
  while (GICD_RWP & 1);
}

void gic_redistributor_wake(void) {
  unsigned int waker;

  waker = GICR_WAKER;

  /* Clear ProcessorSleep */
  waker &= ~(1u << 1);

  GICR_WAKER = waker;

  /* Wait until ChildrenAsleep is cleared */
  while (GICR_WAKER & (1u << 2));
}

void gic_disable_distributor(void) {
  // unsigned int ctlr = GICD_CTLR;

    GICD_CTLR = 0;

    asm volatile("dsb sy");
    asm volatile("isb");

    gic_wait_rwp();
}

void gic_distributor_enable(void) { unsigned int ctlr;
  ctlr = GICD_CTLR;

  /* Enable affinity routing */
  ctlr |= (1u << 4);

  /* Enable Group 1 Non-secure interrupts */
  ctlr |= (1u << 1);

  GICD_CTLR = ctlr;

  /* Ensure write completes */
  // asm volatile("dsb sy");
  asm volatile("dsb sy" ::: "memory");
  asm volatile("isb");
}

void gic_cpu_enable(void) {
  unsigned long value;

  /* Enable system register interface */
  asm volatile(
    "mrs %0, ICC_SRE_EL2\n"
    "orr %0, %0, #1\n"
    "msr ICC_SRE_EL2, %0\n"
    "isb"
    : "=&r"(value)
    :
    : "memory"
  );

  /* Accept all interrupt priorities */
  value = 0xff;

  asm volatile(
    "msr ICC_PMR_EL1, %0\n"
    "isb"
    :
    : "r"(value)
    : "memory"
  );

  /* Enable Group 1 interrupts */
  value = 1;

  asm volatile(
    "msr ICC_IGRPEN1_EL1, %0\n"
    "isb"
    :
    : "r"(value)
    : "memory"
  );
}

void gic_set_group1(unsigned int irq) {
  GICD_IGROUPR(irq) |= (1u << (irq % 32));
}

void gic_set_priority(unsigned int irq) {
  GICD_IPRIORITYR(irq) = 0x80;
}

void gic_enable_irq(unsigned int irq) {
  GICD_ISENABLER(irq) = (1u << (irq % 32));
}

void gic_route_irq(unsigned int irq) {
  GICD_IROUTER(irq) = 0;
}

void gic_init(void) {
  gic_redistributor_wake();
  gic_distributor_enable();

  gic_set_group1(UART1_GIC_ID);
  gic_route_irq(UART1_GIC_ID);
  gic_set_priority(UART1_GIC_ID);
  gic_enable_irq(UART1_GIC_ID);

  gic_set_group1(GPT1_GIC_ID);
  gic_route_irq(GPT1_GIC_ID);
  gic_set_priority(GPT1_GIC_ID);
  gic_enable_irq(GPT1_GIC_ID);

  gic_cpu_enable();

  asm volatile("dsb sy" ::: "memory");
  asm volatile("isb");
}
