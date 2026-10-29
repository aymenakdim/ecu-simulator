# Automotive ECU Network & Diagnostic Simulator

Simulation logicielle d'un réseau d'ECUs automobiles : bus CAN,
diagnostic DTC, et à terme UDS + tests automatisés.

## Statut

- [x] Session 1 : ECU moteur virtuel + trame CAN
- [x] Session 2 : Dashboard ECU (décodage + round-trip)
- [ ] Session 3 : Bus CAN multi-ECUs
- [ ] Session 4 : Gestion DTC avancée
- [ ] Session 5 : UDS sur ISO-TP
- [ ] Session 6 : Tests automatisés pytest

## Architecture

| Module | Rôle |
|--------|------|
| `sensors` | Lecture et validation des capteurs simulés |
| `engine_ecu` | Logique de contrôle moteur + DTC |
| `can_frame` | Encodage et décodage de la trame CAN (8 octets) |
| `dashboard_ecu` | Réception, validation et affichage des trames CAN |
| `main` | Boucle principale 10 Hz |

## Résumé des Sessions

### Session 1 : ECU moteur virtuel + trame CAN
- **Modules** : `sensors`, `engine_ecu`, `can_frame`
- **Fonctionnalités** :
  - Simulation capteurs (RPM, température, pédale, vitesse) avec clamp/validation.
  - Logique moteur : démarrage automatique, détection surchauffe (DTC P0217) et surrégime (DTC P0219).
  - Encodage de la trame CAN `0x100` (ENGINE_STATUS) : RPM big-endian (0.25 tr/min/bit), température offset +40, vitesse big-endian (0.01 km/h/bit).

### Session 2 : Dashboard ECU (décodage + round-trip)
- **Modules** : `dashboard_ecu`, extension de `can_frame`
- **Fonctionnalités** :
  - Décodage de la trame CAN `0x100` en valeurs physiques (`engine_status_decoded_t`).
  - Validation stricte : vérification de l'ID et du DLC avant décodage.
  - Affichage du Dashboard avec compteurs `rx_count` et `err_count`.
  - Test round-trip (`tests/test_roundtrip.c`) : **53 checks** (cas nominaux, bornes, quantification, rejets).

## Build

```bash
mkdir -p build

# Application principale
gcc -Wall -Wextra -std=c11 -Iinclude \
    src/main.c src/engine_ecu.c src/sensors.c src/can_frame.c src/dashboard_ecu.c \
    -o build/ecu_sim.exe

# Tests
gcc -Wall -Wextra -std=c11 -Iinclude \
    tests/test_roundtrip.c src/engine_ecu.c src/sensors.c src/can_frame.c \
    -o build/test_roundtrip.exe
