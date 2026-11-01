#include "can_bus.h"
#include <string.h>

void can_bus_init(can_bus_t *bus) {
    if (!bus) return;
    memset(bus, 0, sizeof(*bus));
}

int can_bus_send(can_bus_t *bus, const can_frame_t *frame) {
    if (!bus || !frame) return 0;

    if (bus->count >= CAN_BUS_QUEUE_SIZE) {
        bus->drop_full++;
        return 0;
    }

    bus->frames[bus->tail] = *frame;
    bus->tail = (bus->tail + 1U) % CAN_BUS_QUEUE_SIZE;
    bus->count++;
    bus->tx_total++;
    return 1;
}

int can_bus_receive(can_bus_t *bus, can_frame_t *out) {
    if (!bus || !out) return 0;

    if (bus->count == 0U) return 0;

    *out = bus->frames[bus->head];
    bus->head = (bus->head + 1U) % CAN_BUS_QUEUE_SIZE;
    bus->count--;
    bus->rx_total++;
    return 1;
}

uint32_t can_bus_count(const can_bus_t *bus) {
    if (!bus) return 0;
    return bus->count;
}

void can_bus_clear(can_bus_t *bus) {
    if (!bus) return;
    bus->head  = 0U;
    bus->tail  = 0U;
    bus->count = 0U;
    /* Les compteurs tx_total / rx_total / drop_full sont conservés. */
}