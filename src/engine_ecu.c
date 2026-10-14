#include "engine_ecu.h"
#include <string.h>

void engine_ecu_init(engine_ecu_state_t *ecu) {
    if (!ecu) return;
    memset(ecu, 0, sizeof(*ecu));
    ecu->engine_running = false;
}

int engine_ecu_add_dtc(engine_ecu_state_t *ecu, uint16_t dtc) {
    if (!ecu || dtc == DTC_NONE) return 0;

    /* Évite les doublons */
    for (uint8_t i = 0; i < ecu->dtc_count; i++) {
        if (ecu->dtc_list[i] == dtc) return 0;
    }

    if (ecu->dtc_count >= DTC_MAX_COUNT) return 0;

    ecu->dtc_list[ecu->dtc_count++] = dtc;
    return 1;
}

void engine_ecu_clear_dtc(engine_ecu_state_t *ecu) {
    if (!ecu) return;
    memset(ecu->dtc_list, 0, sizeof(ecu->dtc_list));
    ecu->dtc_count = 0;
}

void engine_ecu_update(engine_ecu_state_t *ecu, const sensor_inputs_t *in) {
    if (!ecu || !in) return;

    /* 1. Valide et borne les entrées */
    sensor_inputs_t v = *in;
    sensors_clamp(&v);

    if (!sensors_validate(&v)) {
        engine_ecu_add_dtc(ecu, DTC_PEDAL_MISMATCH);
        return;
    }

    /* 2. Mise à jour de l'état interne */
    ecu->rpm = v.rpm;
    ecu->coolant_temp_c = v.coolant_temp_c;

    /* 3. Papillon suit la pédale */
    ecu->throttle_pct = v.pedal_pct;

    /* 4. Démarrage automatique */
    if (v.pedal_pct > 5) {
        ecu->engine_running = true;
    }

    /* 5. Détection de défauts */
    if (v.coolant_temp_c > 105) {
        engine_ecu_add_dtc(ecu, DTC_COOLANT_OVERHEAT);
    }
    if (v.rpm > 7000) {
        engine_ecu_add_dtc(ecu, DTC_RPM_OVERREV);
    }

    ecu->cycle_count++;
}