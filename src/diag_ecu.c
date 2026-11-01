#include "diag_ecu.h"
#include <string.h>
#include <stdio.h>

void diag_ecu_init(diag_ecu_state_t *diag) {
    if (!diag) return;
    memset(diag, 0, sizeof(*diag));
}

static void wrap_single_frame(const uint8_t *payload, uint8_t len, can_frame_t *out) {
    memset(out, 0, sizeof(*out));
    out->id  = CAN_ID_DIAG_REQ;
    out->dlc = CAN_DLC;
    out->data[0] = len & 0x0F; /* ISO-TP Single Frame : longueur max 7 */
    for (uint8_t i = 0; i < len && i < 7; i++) {
        out->data[i + 1] = payload[i];
    }
}

void diag_ecu_send_clear_dtc(diag_ecu_state_t *diag, can_frame_t *out_frame) {
    uint8_t req[] = { UDS_SID_CLEAR_DTC };
    wrap_single_frame(req, sizeof(req), out_frame);
    if (diag) diag->req_count++;
}

void diag_ecu_send_read_dtc(diag_ecu_state_t *diag, can_frame_t *out_frame) {
    uint8_t req[] = { UDS_SID_READ_DTC, 0x02 };
    wrap_single_frame(req, sizeof(req), out_frame);
    if (diag) diag->req_count++;
}

void diag_ecu_send_read_data(diag_ecu_state_t *diag, uint16_t did, can_frame_t *out_frame) {
    uint8_t req[] = { UDS_SID_READ_DATA_BY_ID, (uint8_t)(did >> 8), (uint8_t)(did & 0xFF) };
    wrap_single_frame(req, sizeof(req), out_frame);
    if (diag) diag->req_count++;
}

void diag_ecu_receive_response(diag_ecu_state_t *diag, const can_frame_t *in_frame) {
    if (!diag || !in_frame) return;

    if (in_frame->id != CAN_ID_DIAG_RESP || in_frame->dlc != CAN_DLC) {
        diag->err_count++;
        return;
    }

    uint8_t len = in_frame->data[0] & 0x0F;
    if (len > 0 && len <= 7) {
        diag->last_resp_len = len;
        memcpy(diag->last_resp, &in_frame->data[1], len);
        diag->resp_count++;
    } else {
        diag->err_count++;
    }
}