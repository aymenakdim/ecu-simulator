#ifndef FAULT_INJECTOR_H
#define FAULT_INJECTOR_H

#include <stdint.h>
#include "can_frame.h"

/* Corrompt un octet de la trame en appliquant un masque XOR (bit-flip). */
int fault_corrupt_frame(can_frame_t *frame, uint8_t byte_idx, uint8_t mask);

/* Remplace l'ID par un ID inconnu (ex: 0x999). */
int fault_make_unknown_id(can_frame_t *frame, uint32_t bad_id);

/* Modifie le DLC pour le rendre invalide (ex: 4 au lieu de 8). */
int fault_make_bad_dlc(can_frame_t *frame, uint8_t bad_dlc);

#endif /* FAULT_INJECTOR_H */