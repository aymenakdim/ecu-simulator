# Automotive ECU Network & Diagnostic Simulator

Simulation logicielle d'un réseau d'ECUs automobiles en C : bus CAN, diagnostic UDS, détection de fautes (DTC) et tests unitaires.

## Statut

- [x] Session 1 : ECU moteur virtuel + trame CAN
- [x] Session 2 : Dashboard ECU (décodage + round-trip)
- [x] Session 3 : Bus CAN multi-ECUs & Injection de fautes
- [x] Session 4 : Gestion avancée des DTC & Timeout
- [x] Session 5 : Diagnostic UDS sur ISO-TP (Single Frame)

## Architecture

| Module | Rôle |
|--------|------|
| `sensors` | Lecture, validation et saturation (clamp) des capteurs simulés |
| `engine_ecu` | Logique de contrôle moteur, gestion des DTC (P0217, P0219) |
| `can_frame` | Encodage et décodage des trames CAN (ID 0x100) |
| `dashboard_ecu` | Décodage, affichage, compteurs d'erreurs et Timeout (stale) |
| `can_bus` | File FIFO (32 trames) simulant le médium physique partagé |
| `fault_injector` | Outil de test : corruption de bits, ID inconnus, DLC invalide |
| `uds_server` | Répondeur UDS (SID 0x14 Clear DTC, 0x19 Read DTC, 0x22 Read Data) |
| `diag_ecu` | Client de diagnostic (formate les requêtes/réponses ISO-TP) |
| `main` | Orchestration de la simulation avec temps virtuel (`now_ms`) |

## Résumé des Sessions

### Sessions 1 & 2 : ECU Moteur & Dashboard
- Simulation capteurs avec validation.
- Encodage/Décodage Big-Endian avec facteurs d'échelle (RPM, Temp, Vitesse).
- Détection des surchauffes (DTC P0217) et surrégimes (DTC P0219).
- **53 tests unitaires** (Round-trip encode/decode avec tolérance).

### Session 3 : Réseau & Robustesse
- **Bus CAN FIFO** : Découplage émetteur/récepteur pour simuler le broadcast.
- **Timeout Monitoring** : Détection des données obsolètes (`is_stale`) après 500 ms sans réception.
- **Fault Injection** : Test de la tolérance aux pannes (trames corrompues, ID inconnus).
- **381 tests unitaires** ajoutés (Bus, Timeout, Fautes).

### Sessions 4 & 5 : Diagnostic UDS (ISO 14229)
- Implémentation du protocole de diagnostic standard automobile.
- Encapsulation **ISO-TP Single Frame** (le 1er octet définit la longueur).
- Services supportés : `0x14` (Effacer DTC), `0x19` (Lire DTC), `0x22` (Lire données temps réel).
- Gestion des réponses négatives (NRC).
- **20 tests unitaires** ajoutés (Cycle complet Requête -> Bus -> Réponse).

*Total : 454 tests automatisés.*

## Compilation & Tests

```bash
mkdir -p build

# 1. Compiler l'application principale
gcc -Wall -Wextra -Wpedantic -std=c11 -Iinclude src/*.c -o build/ecu_sim.exe
./build/ecu_sim.exe

# 2. Compiler et lancer TOUS les tests
gcc -Wall -Wextra -Wpedantic -std=c11 -Iinclude src/can_frame.c src/engine_ecu.c src/sensors.c tests/test_roundtrip.c -o build/test_roundtrip.exe && ./build/test_roundtrip.exe
gcc -Wall -Wextra -Wpedantic -std=c11 -Iinclude src/can_bus.c src/can_frame.c src/engine_ecu.c src/sensors.c tests/test_can_bus.c -o build/test_can_bus.exe && ./build/test_can_bus.exe
gcc -Wall -Wextra -Wpedantic -std=c11 -Iinclude src/can_frame.c src/engine_ecu.c src/sensors.c src/dashboard_ecu.c tests/test_dashboard_timeout.c -o build/test_dashboard_timeout.exe && ./build/test_dashboard_timeout.exe
gcc -Wall -Wextra -Wpedantic -std=c11 -Iinclude src/can_frame.c src/engine_ecu.c src/sensors.c src/dashboard_ecu.c src/fault_injector.c tests/test_fault_injection.c -o build/test_fault_injection.exe && ./build/test_fault_injection.exe
gcc -Wall -Wextra -Wpedantic -std=c11 -Iinclude src/can_frame.c src/engine_ecu.c src/sensors.c src/uds_server.c src/diag_ecu.c tests/test_uds.c -o build/test_uds.exe && ./build/test_uds.exe
