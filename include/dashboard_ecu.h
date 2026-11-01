#ifndef DASHBOARD_ECU_H
#define DASHBOARD_ECU_H

#include <stdint.h>
#include <stdbool.h>
#include "can_frame.h"

/* Timeout par défaut : 500 ms sans trame = donnée obsolète */
#define DASHBOARD_TIMEOUT_MS 500U

typedef struct {
    engine_status_decoded_t last;      /* dernière trame décodée valide */
    bool                    has_data;  /* au moins une trame reçue     */
    uint32_t                rx_count;  
    uint32_t                err_count; 
    
    /* ← Ajouts Session 3 */
    uint32_t                last_rx_ms; /* horodatage de la dernière trame */
    bool                    is_stale;   /* true si la donnée est périmée  */
} dashboard_ecu_state_t;

void dashboard_ecu_init(dashboard_ecu_state_t *dash);

/* Ancienne API (conservée pour non-régression) */
int dashboard_ecu_receive(dashboard_ecu_state_t *dash,
                          const can_frame_t *frame);

/* ← Nouvelle API Session 3 avec injection de temps */
int dashboard_ecu_receive_at(dashboard_ecu_state_t *dash,
                             const can_frame_t *frame,
                             uint32_t now_ms);

/* ← Met à jour l'état "stale" en fonction du temps écoulé.
 * À appeler régulièrement (ex: dans la boucle principale). */
void dashboard_ecu_tick(dashboard_ecu_state_t *dash, uint32_t now_ms);

void dashboard_ecu_print(const dashboard_ecu_state_t *dash);

#endif /* DASHBOARD_ECU_H */