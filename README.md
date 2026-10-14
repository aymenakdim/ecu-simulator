# Automotive ECU Network & Diagnostic Simulator

Simulation logicielle d'un réseau d'ECUs automobiles : bus CAN,
diagnostic DTC, et à terme UDS + tests automatisés.

## Statut

- [x] Session 1 : ECU moteur virtuel + trame CAN
- [ ] Session 2 : Dashboard ECU (décodage)
- [ ] Session 3 : Bus CAN multi-ECUs
- [ ] Session 4 : Gestion DTC avancée
- [ ] Session 5 : UDS sur ISO-TP
- [ ] Session 6 : Tests automatisés pytest

## Architecture

| Module | Rôle |
|--------|------|
| `sensors` | Lecture et validation des capteurs simulés |
| `engine_ecu` | Logique de contrôle moteur + DTC |
| `can_frame` | Encodage trame CAN (8 octets) |
| `main` | Boucle principale 10 Hz |

## Build

```bash
gcc -Wall -Wextra -std=c11 -Iinclude \
    src/main.c src/engine_ecu.c src/sensors.c src/can_frame.c \
    -o build/ecu_sim.exe
./build/ecu_sim.exe
git add README.md
git commit -m "docs: add README"
