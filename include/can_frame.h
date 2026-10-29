#ifndef CAN_FRAME_H
#define CAN_FRAME_H

#include <stdint.h>
#include <stdbool.h>
#include "engine_ecu.h"

/* IDs CAN */
#define CAN_ID_ENGINE_STATUS  0x100U
#define CAN_DLC               8U

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

/* Image physique décodée d'une trame ENGINE_STATUS.
 * Distincte de engine_ecu_state_t : c'est ce qu'un récepteur
 * (Dashboard, outil diag) voit sur le bus, sans accès à l'état
 * interne de l'ECU émetteur. */
typedef struct {
    uint16_t rpm;
    int16_t  coolant_temp_c;
    uint8_t  throttle_pct;
    uint16_t vehicle_speed_kph;
    bool     engine_running;
    uint8_t  dtc_count;
} engine_status_decoded_t;

/* Encode l'état ECU en trame CAN (Session 1). */
can_frame_t engine_encode_can(const engine_ecu_state_t *ecu,
                              uint16_t vehicle_speed_kph);

/* Décode une trame CAN en image physique (Session 2).
 * Retourne 1 si la trame est valide (bon ID, bon DLC), 0 sinon.
 * En cas d'erreur, *out est laissé dans un état indéterminé :
 * l'appelant doit tester le retour AVANT d'utiliser *out. */
int can_decode_engine_status(const can_frame_t *frame,
                             engine_status_decoded_t *out);

#endif /* CAN_FRAME_H */

