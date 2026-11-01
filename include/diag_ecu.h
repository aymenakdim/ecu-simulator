#ifndef DIAG_ECU_H
#define DIAG_ECU_H

#include <stdint.h>
#include "can_frame.h"
#include "uds.h"

typedef struct {
    uint32_t req_count;
    uint32_t resp_count;
    uint32_t err_count;
    uint8_t  last_resp[8];
    uint8_t  last_resp_len;
} diag_ecu_state_t;

void diag_ecu_init(diag_ecu_state_t *diag);

/* Prépare une trame CAN de requête (Testeur -> ECU) */
void diag_ecu_send_clear_dtc(diag_ecu_state_t *diag, can_frame_t *out_frame);
void diag_ecu_send_read_dtc(diag_ecu_state_t *diag, can_frame_t *out_frame);
void diag_ecu_send_read_data(diag_ecu_state_t *diag, uint16_t did, can_frame_t *out_frame);

/* Traite la trame CAN de réponse (ECU -> Testeur) */
void diag_ecu_receive_response(diag_ecu_state_t *diag, const can_frame_t *in_frame);

#endif /* DIAG_ECU_H */