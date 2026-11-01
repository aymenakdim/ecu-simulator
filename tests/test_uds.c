#include <stdio.h>
#include <string.h>
#include "engine_ecu.h"
#include "diag_ecu.h"
#include "uds_server.h"
#include "uds.h"

static int g_pass = 0, g_fail = 0;

#define CHECK(cond, msg) do {                                   \
    if (cond) { g_pass++; }                                     \
    else { g_fail++; printf("  [FAIL] %s\n", msg); }            \
} while (0)

int main(void) {
    printf("=== test_uds ===\n");

    engine_ecu_state_t ecu;
    engine_ecu_init(&ecu);
    engine_ecu_add_dtc(&ecu, DTC_COOLANT_OVERHEAT); /* 0x0217 */

    diag_ecu_state_t diag;
    diag_ecu_init(&diag);

    can_frame_t req, resp;
    uint8_t uds_req[8], uds_resp[8];
    uint8_t resp_len;

    /* 1. Lire les DTC */
    diag_ecu_send_read_dtc(&diag, &req);
    CHECK(req.id == CAN_ID_DIAG_REQ, "req ID 0x7E0");
    
    /* Simulation du transport : le serveur extrait le payload UDS de la trame CAN */
    memcpy(uds_req, &req.data[1], req.data[0] & 0x0F);
    CHECK(uds_server_process(&ecu, uds_req, req.data[0] & 0x0F, uds_resp, &resp_len) == 1, "server processed");

    /* L'ECU encapsule la réponse UDS dans une trame CAN de réponse */
    memset(&resp, 0, sizeof(resp));
    resp.id = CAN_ID_DIAG_RESP;
    resp.dlc = CAN_DLC;
    resp.data[0] = resp_len & 0x0F;
    memcpy(&resp.data[1], uds_resp, resp_len);

    /* Le testeur reçoit la réponse */
    diag_ecu_receive_response(&diag, &resp);
    CHECK(diag.resp_count == 1, "diag received 1 response");
    CHECK(diag.last_resp[0] == (UDS_SID_READ_DTC + UDS_POSITIVE_RESP_OFFSET), "Positive resp for DTC");
    CHECK(diag.last_resp[2] == 1, "1 DTC reported");
    CHECK(diag.last_resp[3] == 0x02 && diag.last_resp[4] == 0x17, "DTC is P0217");

    /* 2. Lire les données moteur (DID F101) */
    diag_ecu_send_read_data(&diag, 0xF101, &req);
    memcpy(uds_req, &req.data[1], req.data[0] & 0x0F);
    uds_server_process(&ecu, uds_req, req.data[0] & 0x0F, uds_resp, &resp_len);

    memset(&resp, 0, sizeof(resp));
    resp.id = CAN_ID_DIAG_RESP;
    resp.dlc = CAN_DLC;
    resp.data[0] = resp_len & 0x0F;
    memcpy(&resp.data[1], uds_resp, resp_len);

    diag_ecu_receive_response(&diag, &resp);
    CHECK(diag.last_resp[0] == 0x62, "Positive resp for Read Data (0x62)");
    CHECK(diag.last_resp[1] == 0xF1 && diag.last_resp[2] == 0x01, "DID F101 echoed");

    /* 3. Effacer les DTC (Clear) */
    diag_ecu_send_clear_dtc(&diag, &req);
    memcpy(uds_req, &req.data[1], req.data[0] & 0x0F);
    uds_server_process(&ecu, uds_req, req.data[0] & 0x0F, uds_resp, &resp_len);
    
    CHECK(ecu.dtc_count == 0, "ECU DTCs cleared");
    CHECK(uds_resp[0] == (UDS_SID_CLEAR_DTC + UDS_POSITIVE_RESP_OFFSET), "Positive resp for Clear");

    /* 4. Requête invalide (Negative Response) */
    uds_req[0] = 0x99; /* SID inconnu */
    uds_server_process(&ecu, uds_req, 1, uds_resp, &resp_len);
    CHECK(uds_resp[0] == 0x7F, "Negative response SID");
    CHECK(uds_resp[1] == 0x99, "Echoed bad SID");
    CHECK(uds_resp[2] == UDS_NRC_SERVICE_NOT_SUPPORTED, "NRC 0x11");

    printf("\n=== Results: %d passed, %d failed ===\n", g_pass, g_fail);
    return (g_fail == 0) ? 0 : 1;
}