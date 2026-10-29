#include <stdio.h>
#include <string.h>
#include "engine_ecu.h"
#include "can_frame.h"

static int g_pass = 0;
static int g_fail = 0;

#define CHECK(cond, msg) do {                                   \
    if (cond) { g_pass++; }                                     \
    else { g_fail++; printf("  [FAIL] %s\n", msg); }            \
} while (0)

/* Tolérance de quantification :
 *  - RPM : step = 0.25 tr/min  → tolérance 1 tr/min (sécurité ×4)
 *  - Temp: step = 1 °C         → tolérance 1 °C
 *  - Spd : step = 0.01 km/h    → tolérance 1 km/h
 *  (on tolère l'arrondi bas de la division entière)
 */
static int approx_eq_u16(uint16_t a, uint16_t b, uint16_t tol) {
    return (a > b ? a - b : b - a) <= tol;
}
static int approx_eq_i16(int16_t a, int16_t b, int16_t tol) {
    return (a > b ? a - b : b - a) <= tol;
}

static void test_roundtrip_case(const char *label,
                                uint16_t rpm, int16_t temp,
                                uint8_t thr, uint16_t spd,
                                bool running, uint8_t dtc_count) {
    engine_ecu_state_t ecu;
    engine_ecu_init(&ecu);
    ecu.rpm            = rpm;
    ecu.coolant_temp_c = temp;
    ecu.throttle_pct   = thr;
    ecu.engine_running = running;
    ecu.dtc_count      = dtc_count;

    can_frame_t f = engine_encode_can(&ecu, spd);

    engine_status_decoded_t d;
    int ok = can_decode_engine_status(&f, &d);

    char msg[128];
    snprintf(msg, sizeof(msg), "%s : decode rejected valid frame", label);
    CHECK(ok == 1, msg);
    if (!ok) return;

    snprintf(msg, sizeof(msg), "%s : rpm",   label);
    CHECK(approx_eq_u16(d.rpm, rpm, 1), msg);
    snprintf(msg, sizeof(msg), "%s : temp",  label);
    CHECK(approx_eq_i16(d.coolant_temp_c, temp, 1), msg);
    snprintf(msg, sizeof(msg), "%s : thr",   label);
    CHECK(d.throttle_pct == thr, msg);
    snprintf(msg, sizeof(msg), "%s : spd",   label);
    CHECK(approx_eq_u16(d.vehicle_speed_kph, spd, 1), msg);
    snprintf(msg, sizeof(msg), "%s : run",   label);
    CHECK(d.engine_running == running, msg);
    snprintf(msg, sizeof(msg), "%s : dtc",   label);
    CHECK(d.dtc_count == dtc_count, msg);
}

static void test_reject_bad_id(void) {
    can_frame_t f = { .id = 0x999, .dlc = CAN_DLC, .data = {0} };
    engine_status_decoded_t d;
    int ok = can_decode_engine_status(&f, &d);
    CHECK(ok == 0, "reject bad id");
}

static void test_reject_bad_dlc(void) {
    can_frame_t f = { .id = CAN_ID_ENGINE_STATUS, .dlc = 4, .data = {0} };
    engine_status_decoded_t d;
    int ok = can_decode_engine_status(&f, &d);
    CHECK(ok == 0, "reject bad dlc");
}

static void test_reject_null(void) {
    engine_status_decoded_t d;
    CHECK(can_decode_engine_status(NULL, &d) == 0, "reject null frame");
    can_frame_t f = { .id = CAN_ID_ENGINE_STATUS, .dlc = CAN_DLC, .data = {0} };
    CHECK(can_decode_engine_status(&f, NULL) == 0, "reject null out");
}

int main(void) {
    printf("=== test_roundtrip ===\n");

    /* Cas nominaux */
    test_roundtrip_case("nominal", 0,    20,  0,   0,   false, 0);
    test_roundtrip_case("nominal", 3000, 90,  50,  120, true,  0);
    test_roundtrip_case("nominal", 7000, 105, 100, 200, true,  2);

    /* Bornes */
    test_roundtrip_case("bounds-min", 0,    -40, 0,   0,   false, 0);
    test_roundtrip_case("bounds-max", 8000, 150, 100, 240, true,  8);

    /* Quantification : valeurs non multiples du pas */
    test_roundtrip_case("quantized", 1234, 87,  33,  123, true,  1);
    test_roundtrip_case("quantized", 1235, 88,  34,  124, true,  1);

    /* Rejets */
    test_reject_bad_id();
    test_reject_bad_dlc();
    test_reject_null();

    printf("\n=== Results: %d passed, %d failed ===\n",
           g_pass, g_fail);
    return (g_fail == 0) ? 0 : 1;
}