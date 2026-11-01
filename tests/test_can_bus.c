#include <stdio.h>
#include <string.h>
#include "can_bus.h"

static int g_pass = 0, g_fail = 0;

#define CHECK(cond, msg) do {                                   \
    if (cond) { g_pass++; }                                     \
    else { g_fail++; printf("  [FAIL] %s\n", msg); }            \
} while (0)

static can_frame_t make_frame(uint32_t id, uint8_t tag) {
    can_frame_t f;
    memset(&f, 0, sizeof(f));
    f.id  = id;
    f.dlc = CAN_DLC;
    for (uint8_t i = 0; i < CAN_DLC; i++) f.data[i] = tag;
    return f;
}

static void test_fifo_order(void) {
    can_bus_t bus;
    can_bus_init(&bus);

    for (uint8_t i = 0; i < 5; i++) {
        can_frame_t f = make_frame(CAN_ID_ENGINE_STATUS, i);
        CHECK(can_bus_send(&bus, &f) == 1, "send 5 frames");
    }
    CHECK(can_bus_count(&bus) == 5, "count == 5");

    for (uint8_t i = 0; i < 5; i++) {
        can_frame_t f;
        CHECK(can_bus_receive(&bus, &f) == 1, "receive 5 frames");
        CHECK(f.id == CAN_ID_ENGINE_STATUS, "id preserved");
        CHECK(f.data[0] == i, "FIFO order preserved");
    }
    CHECK(can_bus_count(&bus) == 0, "count == 0 after drain");
}

static void test_overflow(void) {
    can_bus_t bus;
    can_bus_init(&bus);
    can_frame_t f = make_frame(CAN_ID_ENGINE_STATUS, 0xAA);

    for (uint32_t i = 0; i < CAN_BUS_QUEUE_SIZE; i++) {
        CHECK(can_bus_send(&bus, &f) == 1, "fill queue");
    }
    CHECK(can_bus_send(&bus, &f) == 0, "reject on full");
    CHECK(bus.drop_full == 1, "drop_full counted");
    CHECK(can_bus_count(&bus) == CAN_BUS_QUEUE_SIZE, "count at max");
}

static void test_underflow(void) {
    can_bus_t bus;
    can_bus_init(&bus);
    can_frame_t f;
    CHECK(can_bus_receive(&bus, &f) == 0, "receive on empty fails");
    CHECK(bus.rx_total == 0, "no rx counted");
}

static void test_wraparound(void) {
    can_bus_t bus;
    can_bus_init(&bus);

    /* Envoi/retrait en alternance → force head/tail à "faire le tour". */
    for (uint32_t cycle = 0; cycle < 100; cycle++) {
        uint8_t tag = (uint8_t)(cycle & 0xFF);
        can_frame_t f = make_frame(CAN_ID_ENGINE_STATUS, tag);
        CHECK(can_bus_send(&bus, &f) == 1, "wrap send");

        can_frame_t r;
        CHECK(can_bus_receive(&bus, &r) == 1, "wrap recv");
        CHECK(r.data[0] == tag, "wrap data intact");
    }
    CHECK(bus.tx_total == 100, "tx_total == 100");
    CHECK(bus.rx_total == 100, "rx_total == 100");
    CHECK(can_bus_count(&bus) == 0, "queue empty after wraps");
}

static void test_null(void) {
    can_frame_t f = make_frame(CAN_ID_ENGINE_STATUS, 0);
    CHECK(can_bus_send(NULL, &f) == 0, "send null bus");

    can_bus_t bus;
    can_bus_init(&bus);
    CHECK(can_bus_send(&bus, NULL) == 0, "send null frame");
    CHECK(can_bus_receive(NULL, &f) == 0, "recv null bus");
    CHECK(can_bus_receive(&bus, NULL) == 0, "recv null out");
    CHECK(can_bus_count(NULL) == 0, "count null");
}

static void test_clear(void) {
    can_bus_t bus;
    can_bus_init(&bus);
    can_frame_t f = make_frame(CAN_ID_ENGINE_STATUS, 0x55);
    can_bus_send(&bus, &f);
    can_bus_send(&bus, &f);
    CHECK(can_bus_count(&bus) == 2, "pre-clear count");
    can_bus_clear(&bus);
    CHECK(can_bus_count(&bus) == 0, "post-clear count");
    CHECK(bus.tx_total == 2, "stats preserved on clear");
}

int main(void) {
    printf("=== test_can_bus ===\n");
    test_fifo_order();
    test_overflow();
    test_underflow();
    test_wraparound();
    test_null();
    test_clear();
    printf("\n=== Results: %d passed, %d failed ===\n", g_pass, g_fail);
    return (g_fail == 0) ? 0 : 1;
}