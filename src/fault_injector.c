#include "fault_injector.h"

int fault_corrupt_frame(can_frame_t *frame, uint8_t byte_idx, uint8_t mask) {
    if (!frame || byte_idx >= CAN_DLC) return 0;
    
    /* XOR inverse les bits à 1 dans le masque */
    frame->data[byte_idx] ^= mask;
    return 1;
}

int fault_make_unknown_id(can_frame_t *frame, uint32_t bad_id) {
    if (!frame) return 0;
    frame->id = bad_id;
    return 1;
}

int fault_make_bad_dlc(can_frame_t *frame, uint8_t bad_dlc) {
    if (!frame || bad_dlc >= CAN_DLC) return 0;
    frame->dlc = bad_dlc;
    return 1;
}