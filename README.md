# Automotive ECU Network & Diagnostic Simulator

[![CI - Build & Test](https://github.com/aymenakdim/ecu-simulator/actions/workflows/ci.yml/badge.svg)](https://github.com/aymenakdim/ecu-simulator/actions/workflows/ci.yml)

Simulation logicielle d'un réseau d'ECUs automobiles en C : bus CAN, diagnostic UDS, détection de fautes (DTC), tests unitaires et pipeline CI/CD.

## Statut du Projet

- [x] Session 1 : ECU moteur virtuel et trame CAN
- [x] Session 2 : Dashboard ECU (décodage + round-trip)
- [x] Session 3 : Bus CAN simulé multi-ECUs & Injection de fautes
- [x] Session 4 : Gestion avancée des DTC & Timeout
- [x] Session 5 : Diagnostic UDS sur ISO-TP (Single Frame)
- [x] Session 6 : Tests automatisés (Intégration C/Python avec pytest)
- [x] Session 7 : Intégration CMake & Refactoring de l'architecture
- [x] Session 8 : Documentation, CI/CD (GitHub Actions) et finalisation

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

### Sessions 1 à 5 : Logique Métier & Réseau
- Simulation capteurs, encodage/décodage Big-Endian.
- Bus CAN FIFO et détection des données obsolètes (`is_stale`).
- Implémentation UDS (ISO 14229) : `0x14` (Clear DTC), `0x19` (Read DTC), `0x22` (Read Data).
- **454 tests unitaires C** validés.

### Sessions 6 à 8 : Industrialisation & CI/CD
- **CMake & Ninja** : Génération de build portable et rapide.
- **CTest** : Lancement automatisé des exécutables de test C.
- **pytest** : Couche d'intégration Python (exécute les binaires C et vérifie les codes de retour).
- **pytest-html** : Génération de rapports de test visuels (HTML).
- **GitHub Actions** : Pipeline d'intégration continue (CI) déclenché à chaque `push` (compilation + CTest + pytest sur machine Windows vierge).

## Compilation & Tests

Le projet utilise **CMake** et **Ninja** pour une compilation standardisée.

### 1. Compiler le projet
```bash
cmake -B build -G Ninja
cmake --build build