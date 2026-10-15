#include "sensors.h"

void sensors_clamp(sensor_inputs_t *s) {
    if (!s) return;

    /* RPM est uint16_t : pas besoin de vérifier < 0,
       on ne borne que le maximum */
    if (s->rpm > RPM_MAX) s->rpm = RPM_MAX;

    /* coolant_temp_c est int16_t (signé) :
       on vérifie bien les deux bornes */
    if (s->coolant_temp_c < COOLANT_MIN_C) s->coolant_temp_c = COOLANT_MIN_C;
    if (s->coolant_temp_c > COOLANT_MAX_C) s->coolant_temp_c = COOLANT_MAX_C;

    /* pedal_pct est uint8_t : on ne borne que le maximum */
    if (s->pedal_pct > PEDAL_MAX_PCT) s->pedal_pct = PEDAL_MAX_PCT;

    /* vehicle_speed_kph est uint16_t : on ne borne que le maximum */
    if (s->vehicle_speed_kph > SPEED_MAX_KPH) s->vehicle_speed_kph = SPEED_MAX_KPH;
}

int sensors_validate(const sensor_inputs_t *s) {
    if (!s) return 0;

    if (s->rpm > RPM_MAX) return 0;
    if (s->coolant_temp_c < COOLANT_MIN_C || s->coolant_temp_c > COOLANT_MAX_C) return 0;
    if (s->pedal_pct > PEDAL_MAX_PCT) return 0;
    if (s->vehicle_speed_kph > SPEED_MAX_KPH) return 0;

    return 1;
}