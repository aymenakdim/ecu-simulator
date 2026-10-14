#ifndef SENSORS_H
#define SENSORS_H

#include <stdint.h>

/* Plages réalistes pour un moteur essence */
#define RPM_MIN         0U
#define RPM_MAX         8000U
#define COOLANT_MIN_C   (-40)
#define COOLANT_MAX_C   150
#define PEDAL_MIN_PCT   0U
#define PEDAL_MAX_PCT   100U
#define SPEED_MIN_KPH   0U
#define SPEED_MAX_KPH   240U

/* État d'une lecture capteurs à un instant donné */
typedef struct {
    uint16_t rpm;               /* tours/minute  */
    int16_t  coolant_temp_c;    /* °C            */
    uint8_t  pedal_pct;         /* 0 à 100 %     */
    uint16_t vehicle_speed_kph; /* km/h          */
} sensor_inputs_t;

/* Sature les valeurs hors plage */
void sensors_clamp(sensor_inputs_t *s);

/* Retourne 1 si la lecture est cohérente, 0 sinon */
int sensors_validate(const sensor_inputs_t *s);

#endif /* SENSORS_H */