#ifndef UDS_SERVER_H
#define UDS_SERVER_H

#include "uds.h"
#include "engine_ecu.h"

/* Traite une requête UDS et remplit la réponse.
 * Retourne 1 si le traitement est OK (réponse générée), 0 si erreur interne. */
int uds_server_process(engine_ecu_state_t *ecu, 
                       const uint8_t *req, uint8_t req_len, 
                       uint8_t *resp, uint8_t *resp_len);

#endif /* UDS_SERVER_H */