#ifndef UDS_H
#define UDS_H

#include <stdint.h>

/* IDs CAN dédiés au Diagnostic (Standard OBD-II) */
#define CAN_ID_DIAG_REQ   0x7E0U  /* Testeur -> ECU */
#define CAN_ID_DIAG_RESP  0x7E8U  /* ECU -> Testeur */

/* UDS Service Identifiers (SID) */
#define UDS_SID_CLEAR_DTC          0x14U
#define UDS_SID_READ_DTC           0x19U
#define UDS_SID_READ_DATA_BY_ID    0x22U

/* UDS Negative Response Codes (NRC) */
#define UDS_NRC_SERVICE_NOT_SUPPORTED 0x11U

#define UDS_POSITIVE_RESP_OFFSET      0x40U

/* Structure neutre pour transporter un payload UDS */
typedef struct {
    uint8_t data[8]; /* Assez grand pour une Single Frame */
    uint8_t len;
} uds_payload_t;

#endif /* UDS_H */