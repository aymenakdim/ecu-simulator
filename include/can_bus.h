#ifndef CAN_BUS_H
#define CAN_BUS_H

#include <stdint.h>
#include <stdbool.h>
#include "can_frame.h"

#define CAN_BUS_QUEUE_SIZE 32U

/* Bus CAN simulé : file FIFO bornée de trames.
 * Représente le médium partagé. Les ECUs émettent (send),
 * les récepteurs consomment (receive). */
typedef struct {
    can_frame_t frames[CAN_BUS_QUEUE_SIZE];
    uint32_t    head;      /* index de lecture                 */
    uint32_t    tail;      /* index d'écriture (prochaine libre)*/
    uint32_t    count;     /* nb de trames en file             */

    /* Statistiques cumulées (jamais remises à 0 par clear) */
    uint32_t    tx_total;  /* trames acceptées par le bus       */
    uint32_t    rx_total;  /* trames retirées du bus            */
    uint32_t    drop_full; /* send refusés car file pleine      */
} can_bus_t;

void     can_bus_init(can_bus_t *bus);

/* Enfile une trame. Retourne 1 si OK, 0 si NULL ou file pleine. */
int      can_bus_send(can_bus_t *bus, const can_frame_t *frame);

/* Défile la prochaine trame. Retourne 1 si OK, 0 si NULL ou file vide. */
int      can_bus_receive(can_bus_t *bus, can_frame_t *out);

uint32_t can_bus_count(const can_bus_t *bus);

/* Vide la file (les compteurs cumulés tx_total/rx_total/drop_full
 * restent, comme un reset de buffer sans reset de stats). */
void     can_bus_clear(can_bus_t *bus);

#endif /* CAN_BUS_H */