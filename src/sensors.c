#include "sensors.h"

void sensors_clamp(sensor_inputs_t *s) {
    if (!s) return;

    if (s->rpm < RPM_MIN) s->rpm = RPM_MIN;
    if (s->rpm > RPM_MAX) s->rpm = RPM_MAX;

    if (s->coolant_temp_c < COOLANT_MIN_C) s->coolant_temp_c = COOLANT_MIN_C;
    if (s->coolant_temp_c > COOLANT_MAX_C) s->coolant_temp_c = COOLANT_MAX_C;

    if (s->pedal_pct < PEDAL_MIN_PCT) s->pedal_pct = PEDAL_MIN_PCT;
    if (s->pedal_pct > PEDAL_MAX_PCT) s->pedal_pct = PEDAL_MAX_PCT;

    if (s->vehicle_speed_kph < SPEED_MIN_KPH) s->vehicle_speed_kph = SPEED_MIN_KPH;
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