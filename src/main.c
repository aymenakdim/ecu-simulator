#include <stdio.h>
#include <windows.h>  /* Pour Sleep() visuel */
#include <string.h>

#include "engine_ecu.h"
#include "can_frame.h"
#include "dashboard_ecu.h"
#include "can_bus.h"
#include "fault_injector.h"
#include "diag_ecu.h"
#include "uds.h"
#include "uds_server.h"



int main(void) {
    /* ── Initialisation de tous les modules ── */
    engine_ecu_state_t ecu;
    engine_ecu_init(&ecu);

    dashboard_ecu_state_t dash;
    dashboard_ecu_init(&dash);

    diag_ecu_state_t diag;
    diag_ecu_init(&diag);

    can_bus_t bus;
    can_bus_init(&bus);

    sensor_inputs_t in = {
        .rpm = 0, .coolant_temp_c = 20,
        .pedal_pct = 0, .vehicle_speed_kph = 0
    };

    uint32_t now_ms = 0;

    printf("=== Automotive ECU Network Simulator (Session 3) ===\n");
    printf("Cycle | ms   | RPM  | T(C) | DTC | Evenement / Trame\n");
    printf("------+------+------+------+-----+-----------------------------------\n");

    /* ── Boucle de simulation (100 cycles de 100ms = 10 secondes) ── */
    for (int t = 0; t < 100; t++) {
        now_ms += 100;
        
        /* 1. Mise à jour des capteurs et de l'Engine ECU */
        in.pedal_pct       = (t < 50) ? (uint8_t)(t * 2) : 100U;
        in.rpm             = (uint16_t)(in.pedal_pct * 60);
        in.coolant_temp_c  = (int16_t)(20 + t);
        engine_ecu_update(&ecu, &in);

        /* 2. L'Engine ECU prépare sa trame */
        can_frame_t engine_frame = engine_encode_can(&ecu, in.vehicle_speed_kph);

        /* ────────────────────────────────────────────────────────────
         * SCÉNARIO 1 : INJECTION DE FAUTES
         * ──────────────────────────────────────────────────────────── */
        
        // A. Trames perdues (Drop) entre t=20 et t=25 (600ms sans trame)
        // Le Dashboard devrait passer en mode [STALE]
        if (t >= 20 && t <= 25) {
            printf("%5d | %4u | %4u | %4d | %3u | [FAULT] Trame perdue (Drop)\n",
                   t, now_ms, ecu.rpm, ecu.coolant_temp_c, ecu.dtc_count);
        } 
         // B. Trame corrompue à t=30 (on modifie le DLC pour tromper le Dashboard)
        else if (t == 30) {
            printf("%5d | %4u | %4u | %4d | %3u | [FAULT] Trame corrompue (DLC invalide)\n",
                   t, now_ms, ecu.rpm, ecu.coolant_temp_c, ecu.dtc_count);
            fault_make_bad_dlc(&engine_frame, 4); // On force le DLC à 4
            can_bus_send(&bus, &engine_frame);
        }
        // C. Émission normale
        else {
            printf("%5d | %4u | %4u | %4d | %3u | CAN ID=0x%03X\n",
                   t, now_ms, ecu.rpm, ecu.coolant_temp_c, ecu.dtc_count, engine_frame.id);
            can_bus_send(&bus, &engine_frame);
        }

        /* ────────────────────────────────────────────────────────────
         * SCÉNARIO 2 : DIAGNOSTIC UDS
         * ──────────────────────────────────────────────────────────── */
        
        // À t=40, le technicien branche la valise et demande les DTC
        if (t == 40) {
            printf("      |      |      |      |     | [DIAG] Requete Read DTC envoyee\n");
            can_frame_t req_frame;
            diag_ecu_send_read_dtc(&diag, &req_frame);
            can_bus_send(&bus, &req_frame);
        }
        // À t=50, le technicien efface les DTC
        if (t == 50) {
            printf("      |      |      |      |     | [DIAG] Requete Clear DTC envoyee\n");
            can_frame_t req_frame;
            diag_ecu_send_clear_dtc(&diag, &req_frame);
            can_bus_send(&bus, &req_frame);
        }

        /* ────────────────────────────────────────────────────────────
         * ROUTAGE DU BUS CAN (Le rôle du "réseau")
         * ──────────────────────────────────────────────────────────── */
        can_frame_t rx;
        while (can_bus_receive(&bus, &rx)) {
            // 1. Trames pour le Dashboard (ID 0x100)
            if (rx.id == CAN_ID_ENGINE_STATUS) {
                dashboard_ecu_receive_at(&dash, &rx, now_ms);
            }
            // 2. Requêtes de diagnostic pour l'Engine ECU (ID 0x7E0)
            else if (rx.id == CAN_ID_DIAG_REQ) {
                uint8_t uds_req[8], uds_resp[8];
                uint8_t req_len = rx.data[0] & 0x0F;
                uint8_t resp_len = 0;
                
                memcpy(uds_req, &rx.data[1], req_len);
                
                // L'Engine ECU traite la requête
                uds_server_process(&ecu, uds_req, req_len, uds_resp, &resp_len);
                
                // L'Engine ECU renvoie la réponse sur le bus
                can_frame_t resp_frame;
                memset(&resp_frame, 0, sizeof(resp_frame));
                resp_frame.id  = CAN_ID_DIAG_RESP;
                resp_frame.dlc = CAN_DLC;
                resp_frame.data[0] = resp_len & 0x0F;
                memcpy(&resp_frame.data[1], uds_resp, resp_len);
                can_bus_send(&bus, &resp_frame);
            }
            // 3. Réponses de diagnostic pour le Testeur (ID 0x7E8)
            else if (rx.id == CAN_ID_DIAG_RESP) {
                diag_ecu_receive_response(&diag, &rx);
            }
        }

        /* Mise à jour du Timeout du Dashboard à chaque cycle */
        dashboard_ecu_tick(&dash, now_ms);

        /* Affichage de l'état du Dashboard à chaque cycle */
        dashboard_ecu_print(&dash);
        
        Sleep(50); /* Ralenti l'affichage pour la démo visuelle */
    }

    /* ── Bilan de fin de simulation ── */
    printf("\n=== Bilan Reseau ===\n");
    printf("Bus CAN    : %u emises, %u recues, %u drops\n", 
           bus.tx_total, bus.rx_total, bus.drop_full);
    printf("Dashboard  : %u rx, %u err\n", dash.rx_count, dash.err_count);
    printf("Diag ECU   : %u req, %u resp, %u err\n", 
           diag.req_count, diag.resp_count, diag.err_count);
           
    if (diag.resp_count > 0) {
        printf("Derniere reponse Diag (SID=0x%02X)\n", diag.last_resp[0]);
    }

    return 0;
}