#include<stdint.h>
#include "irq.h"
#include <mem/paging.h>

static volatile irq_descriptor_t * g_irq_descriptor;

void irq_dispatcher(uint64_t irq_number) {
    
    if (g_irq_descriptor->handlers[irq_number] != 0) {
        g_irq_descriptor->handlers[irq_number]();
    } else {
        // TODO: Handle unhandled IRQ
    }

}

void init_irq_descriptor() {

    g_irq_descriptor = (irq_descriptor_t *)mem_alloc_pages(NUM_PAGES_FOR_TYPE(irq_descriptor_t));

    if (g_irq_descriptor == 0) {
        // TODO: Handle allocation failure
    }
    
    for (int i = 0; i < 2; i++) {
        g_irq_descriptor->handlers[i] = 0;
    }


}

void install_irq_handler(uint64_t irq_number, irq_handler_t handler) {
    
    if (irq_number < 256) {
        g_irq_descriptor->handlers[irq_number] = handler;
    }
    // TODO: Some kind of warning

}