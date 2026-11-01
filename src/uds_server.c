#include "uds_server.h"
#include <string.h>

int uds_server_process(engine_ecu_state_t *ecu, 
                       const uint8_t *req, uint8_t req_len, 
                       uint8_t *resp, uint8_t *resp_len) {
    if (!ecu || !req || req_len == 0 || !resp || !resp_len) return 0;

    uint8_t sid = req[0];

    /* --- Service 0x14 : Clear Diagnostic Information --- */
    if (sid == UDS_SID_CLEAR_DTC) {
        engine_ecu_clear_dtc(ecu);
        resp[0] = UDS_SID_CLEAR_DTC + UDS_POSITIVE_RESP_OFFSET;
        *resp_len = 1;
        return 1;
    }

    /* --- Service 0x19 : Read DTC Information --- */
    if (sid == UDS_SID_READ_DTC) {
        resp[0] = UDS_SID_READ_DTC + UDS_POSITIVE_RESP_OFFSET;
        resp[1] = 0x02; /* Subfunction: report DTC by status mask */
        resp[2] = ecu->dtc_count; /* Nombre de DTC */
        
        uint8_t idx = 3;
        for (uint8_t i = 0; i < ecu->dtc_count && idx < 7; i++) {
            resp[idx++] = (uint8_t)(ecu->dtc_list[i] >> 8);   /* DTC High */
            resp[idx++] = (uint8_t)(ecu->dtc_list[i] & 0xFF); /* DTC Low  */
            resp[idx++] = 0x08; /* Status: Confirmed */
        }
        *resp_len = idx;
        return 1;
    }

    /* --- Service 0x22 : Read Data By Identifier --- */
    if (sid == UDS_SID_READ_DATA_BY_ID && req_len >= 3) {
        uint16_t did = ((uint16_t)req[1] << 8) | req[2];
        
        /* DID F101 : Données moteur (RPM sur 2 octets, Temp sur 1 octet) */
        if (did == 0xF101) {
            resp[0] = UDS_SID_READ_DATA_BY_ID + UDS_POSITIVE_RESP_OFFSET;
            resp[1] = req[1];
            resp[2] = req[2];
            resp[3] = (uint8_t)(ecu->rpm >> 8);
            resp[4] = (uint8_t)(ecu->rpm & 0xFF);
            resp[5] = (uint8_t)ecu->coolant_temp_c;
            *resp_len = 6;
            return 1;
        }
    }

    /* --- Service non supporté (Negative Response) --- */
    resp[0] = 0x7F; /* Negative Response SID */
    resp[1] = sid;
    resp[2] = UDS_NRC_SERVICE_NOT_SUPPORTED;
    *resp_len = 3;
    return 1;
}