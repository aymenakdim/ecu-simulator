#include <stdio.h>
#include <string.h>
#include "dashboard_ecu.h"
#include "can_frame.h"
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
    return engine_encode_can(&ecu, 50);
}

static void test_timeout_lifecycle(void) {
    dashboard_ecu_state_t dash;
    dashboard_ecu_init(&dash);
    can_frame_t f = make_valid_frame();

    /* 1. Init : pas de data, pas frais */
    CHECK(dash.has_data == false, "init no data");
    CHECK(dash.is_stale == true, "init is stale");

    /* 2. Réception à t=1000 : data fraîche */
    CHECK(dashboard_ecu_receive_at(&dash, &f, 1000) == 1, "receive at t=1000");
    CHECK(dash.is_stale == false, "fresh after receive");
    CHECK(dash.last_rx_ms == 1000, "last_rx_ms updated");

    /* 3. Tick à t=1400 (400ms écoulées) : toujours frais */
    dashboard_ecu_tick(&dash, 1400);
    CHECK(dash.is_stale == false, "still fresh at 400ms");

    /* 4. Tick à t=1500 (exactement 500ms) : devient périmé (>=) */
    dashboard_ecu_tick(&dash, 1500);
    CHECK(dash.is_stale == true, "stale at exactly 500ms");

    /* 5. Tick à t=2000 : toujours périmé */
    dashboard_ecu_tick(&dash, 2000);
    CHECK(dash.is_stale == true, "still stale at 1000ms");

    /* 6. Nouvelle réception à t=2100 : redevient frais */
    CHECK(dashboard_ecu_receive_at(&dash, &f, 2100) == 1, "receive at t=2100");
    CHECK(dash.is_stale == false, "fresh again");
}

static void test_timeout_rollover(void) {
    dashboard_ecu_state_t dash;
    dashboard_ecu_init(&dash);
    can_frame_t f = make_valid_frame();

    /* Test du rollover uint32_t (proche de la limite max) */
    uint32_t t_start = 0xFFFFFF00U; /* Proche du max uint32 */
    dashboard_ecu_receive_at(&dash, &f, t_start);
    
    /* Avance de 600ms (dépasse le max uint32 et revient à 0) */
    uint32_t t_end = t_start + 600U; 
    
    dashboard_ecu_tick(&dash, t_end);
    CHECK(dash.is_stale == true, "stale across uint32 rollover");
}

int main(void) {
    printf("=== test_dashboard_timeout ===\n");
    test_timeout_lifecycle();
    test_timeout_rollover();
    printf("\n=== Results: %d passed, %d failed ===\n", g_pass, g_fail);
    return (g_fail == 0) ? 0 : 1;
}