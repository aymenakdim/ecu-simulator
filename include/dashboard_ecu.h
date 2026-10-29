#ifndef DASHBOARD_ECU_H
#define DASHBOARD_ECU_H

#include <stdint.h>
#include <stdbool.h>
#include "can_frame.h"

/* État interne du Dashboard.
 * Représente ce que le combiné affiche à l'utilisateur.
 * Séparé du décodage brut : on pourrait ajouter filtrage,
 * lissage, gestion de timeout, etc. */
typedef struct {
    engine_status_decoded_t last;      /* dernière trame décodée valide */
    bool                    has_data;  /* au moins une trame reçue     */
    uint32_t                rx_count;  /* nb trames valides reçues     */
    uint32_t                err_count; /* nb trames rejetées           */
} dashboard_ecu_state_t;

void dashboard_ecu_init(dashboard_ecu_state_t *dash);

/* Traite une trame CAN entrante.
 * Retourne 1 si la trame a été acceptée, 0 sinon. */
int dashboard_ecu_receive(dashboard_ecu_state_t *dash,
                          const can_frame_t *frame);

/* Affiche l'état courant sur stdout (couche I/O séparée
 * du traitement pour rester testable). */
void dashboard_ecu_print(const dashboard_ecu_state_t *dash);

#endif /* DASHBOARD_ECU_H */