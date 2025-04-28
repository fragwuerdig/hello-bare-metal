#ifndef IRQ_H
#define IRQ_H

typedef void (*irq_handler_t)(void);

typedef struct {
    irq_handler_t handlers[256];
} irq_descriptor_t;

void init_irq_descriptor();
void install_irq_handler(uint64_t irq_number, irq_handler_t handler);

#endif // IRQ_H