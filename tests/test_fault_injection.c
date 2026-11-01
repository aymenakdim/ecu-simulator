#include <stdio.h>
#include <string.h>
#include "dashboard_ecu.h"
#include "fault_injector.h"
#include "engine_ecu.h"

static int g_pass = 0, g_fail = 0;

#define CHECK(cond, msg) do {                                   \
    if (cond) { g_pass++; }                                     \
    else { g_fail++; printf("  [FAIL] %s\n", msg); }            \
} while (0)

static can_frame_t make_valid_frame(void) {
    engine_ecu_state_t ecu;
    engine_ecu_init(&ecu);
    ecu.rpm = 3000;
    ecu.coolant_temp_c = 90;
    return engine_encode_can(&ecu, 120);
}

int main(void) {
    printf("=== test_fault_injection ===\n");
    dashboard_ecu_state_t dash;
    dashboard_ecu_init(&dash);

    /* 1. Trame saine (Contrôle) */
    can_frame_t f = make_valid_frame();
    CHECK(dashboard_ecu_receive(&dash, &f) == 1, "valid frame accepted");
    CHECK(dash.rx_count == 1 && dash.err_count == 0, "counters valid");

    /* 2. Trame avec ID inconnu */
    f = make_valid_frame();
    fault_make_unknown_id(&f, 0x999);
    CHECK(dashboard_ecu_receive(&dash, &f) == 0, "unknown id rejected");
    CHECK(dash.err_count == 1, "err_count incremented (bad id)");

    /* 3. Trame avec DLC invalide */
    f = make_valid_frame();
    fault_make_bad_dlc(&f, 4);
    CHECK(dashboard_ecu_receive(&dash, &f) == 0, "bad dlc rejected");
    CHECK(dash.err_count == 2, "err_count incremented (bad dlc)");

    /* 4. Trame corrompue au niveau structurel */
    f = make_valid_frame();
    fault_corrupt_frame(&f, 0, 0xFF);
    CHECK(dashboard_ecu_receive(&dash, &f) == 1, "data corruption accepted (no checksum)");

    printf("\n=== Results: %d passed, %d failed ===\n", g_pass, g_fail);
    return (g_fail == 0) ? 0 : 1;
}