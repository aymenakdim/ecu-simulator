#include "dashboard_ecu.h"
#include <stdio.h>
#include <string.h>

void dashboard_ecu_init(dashboard_ecu_state_t *dash) {
    if (!dash) return;
    memset(dash, 0, sizeof(*dash));
    dash->has_data = false;
}

int dashboard_ecu_receive(dashboard_ecu_state_t *dash,
                          const can_frame_t *frame) {
    if (!dash || !frame) return 0;

    engine_status_decoded_t decoded;
    if (!can_decode_engine_status(frame, &decoded)) {
        dash->err_count++;
        return 0;
    }

    dash->last     = decoded;
    dash->has_data = true;
    dash->rx_count++;
    return 1;
}

void dashboard_ecu_print(const dashboard_ecu_state_t *dash) {
    if (!dash) return;

    if (!dash->has_data) {
        printf("[DASH] no data\n");
        return;
    }

    printf("[DASH] RPM=%4u  T=%3d C  Thr=%3u%%  Spd=%3u km/h  "
           "Run=%u  DTC=%u  (rx=%u err=%u)\n",
           dash->last.rpm,
           dash->last.coolant_temp_c,
           dash->last.throttle_pct,
           dash->last.vehicle_speed_kph,
           dash->last.engine_running ? 1U : 0U,
           dash->last.dtc_count,
           dash->rx_count,
           dash->err_count);
}