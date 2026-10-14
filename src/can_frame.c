#include "can_frame.h"
#include <string.h>

can_frame_t engine_encode_can(const engine_ecu_state_t *ecu,
                              uint16_t vehicle_speed_kph) {
    can_frame_t frame;
    memset(&frame, 0, sizeof(frame));

    if (!ecu) return frame;

    frame.id  = CAN_ID_ENGINE_STATUS;
    frame.dlc = CAN_DLC;

    /* RPM : 0.25 tr/min par bit → ×4 */
    uint16_t rpm_encoded = (uint16_t)(ecu->rpm * 4U);
    frame.data[0] = (uint8_t)(rpm_encoded >> 8);
    frame.data[1] = (uint8_t)(rpm_encoded & 0xFF);

    /* Coolant temp : offset +40 */
    frame.data[2] = (uint8_t)(ecu->coolant_temp_c + 40);

    /* Throttle direct */
    frame.data[3] = ecu->throttle_pct;

    /* Speed : 0.01 km/h par bit → ×100 */
    uint16_t speed_encoded = (uint16_t)(vehicle_speed_kph * 100U);
    frame.data[4] = (uint8_t)(speed_encoded >> 8);
    frame.data[5] = (uint8_t)(speed_encoded & 0xFF);

    frame.data[6] = ecu->engine_running ? 1U : 0U;
    frame.data[7] = ecu->dtc_count;

    return frame;
}