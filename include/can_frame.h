#ifndef CAN_FRAME_H
#define CAN_FRAME_H

#include <stdint.h>
#include "engine_ecu.h"

/* IDs CAN (format custom, à documenter dans un DBC en vrai projet) */
#define CAN_ID_ENGINE_STATUS  0x100U

#define CAN_DLC 8U

typedef struct {
    uint32_t id;
    uint8_t  dlc;
    uint8_t  data[8];
} can_frame_t;

/*
 * Format documenté de la trame ENGINE_STATUS (ID 0x100) :
 *
 *  Octet 0-1 : RPM            (big-endian, 0.25 tr/min par bit)
 *  Octet 2   : Coolant temp   (°C + 40, offset pour rester positif)
 *  Octet 3   : Throttle       (%)
 *  Octet 4-5 : Vehicle speed  (big-endian, 0.01 km/h par bit)
 *  Octet 6   : engine_running (0 ou 1)
 *  Octet 7   : dtc_count
 */
can_frame_t engine_encode_can(const engine_ecu_state_t *ecu,
                              uint16_t vehicle_speed_kph);

#endif /* CAN_FRAME_H */