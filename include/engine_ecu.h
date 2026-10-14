#ifndef ENGINE_ECU_H
#define ENGINE_ECU_H

#include <stdint.h>
#include <stdbool.h>
#include "sensors.h"

/* Codes DTC (format OBD-II 16 bits) */
#define DTC_NONE             0x0000U
#define DTC_COOLANT_OVERHEAT 0x0217U  /* P0217 : surchauffe moteur        */
#define DTC_RPM_OVERREV      0x0219U  /* P0219 : surrégime moteur         */
#define DTC_PEDAL_MISMATCH   0x2135U  /* P2135 : incohérence capteur pédale */

#define DTC_MAX_COUNT 8

typedef struct {
    /* État interne ECU */
    uint16_t rpm;
    int16_t  coolant_temp_c;
    uint8_t  throttle_pct;
    bool     engine_running;

    /* Diagnostic */
    uint16_t dtc_list[DTC_MAX_COUNT];
    uint8_t  dtc_count;

    /* Compteur de cycles (utile pour tests) */
    uint32_t cycle_count;
} engine_ecu_state_t;

void engine_ecu_init(engine_ecu_state_t *ecu);
void engine_ecu_update(engine_ecu_state_t *ecu, const sensor_inputs_t *in);
int  engine_ecu_add_dtc(engine_ecu_state_t *ecu, uint16_t dtc);
void engine_ecu_clear_dtc(engine_ecu_state_t *ecu);

#endif /* ENGINE_ECU_H */