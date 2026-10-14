#include <stdio.h>
#include <windows.h>  /* Sleep() sous Windows */
#include "engine_ecu.h"
#include "can_frame.h"

static void print_can_frame(const can_frame_t *f) {
    printf("CAN ID=0x%03X DLC=%u DATA=", f->id, f->dlc);
    for (uint8_t i = 0; i < f->dlc; i++) {
        printf("%02X ", f->data[i]);
    }
}

int main(void) {
    engine_ecu_state_t ecu;
    engine_ecu_init(&ecu);

    sensor_inputs_t in = {
        .rpm = 0,
        .coolant_temp_c = 20,
        .pedal_pct = 0,
        .vehicle_speed_kph = 0
    };

    printf("=== ECU Simulator starting ===\n");
    printf("Cycle | RPM  | T(C) | Pedal | DTC | CAN frame\n");
    printf("------+------+------+-------+-----+-----------------\n");

    for (int t = 0; t < 100; t++) {
        in.pedal_pct = (t < 50) ? (uint8_t)(t * 2) : 100U;
        in.rpm = (uint16_t)(in.pedal_pct * 60);
        in.coolant_temp_c = (int16_t)(20 + t);

        engine_ecu_update(&ecu, &in);
        can_frame_t f = engine_encode_can(&ecu, in.vehicle_speed_kph);

        printf("%5d | %4u | %4d | %4u%% | %3u | ",
               t, ecu.rpm, ecu.coolant_temp_c,
               ecu.throttle_pct, ecu.dtc_count);
        print_can_frame(&f);
        printf("\n");

        Sleep(100);
    }

    printf("\n=== Simulation terminée ===\n");
    printf("DTC actifs : %u\n", ecu.dtc_count);
    for (uint8_t i = 0; i < ecu.dtc_count; i++) {
        printf("  - DTC 0x%04X\n", ecu.dtc_list[i]);
    }
    return 0;
}