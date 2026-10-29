# Journal de développement

## Session 1 — 2026-10-15 ✅

### Réalisé
- Structure projet : include/ src/ build/
- Module sensors : validation + clamp des capteurs
- Module engine_ecu : logique de contrôle + gestion DTC
- Module can_frame : encodage trame CAN 8 octets (ID 0x100)
- main.c : boucle de simulation 10 Hz, 100 cycles
- DTC surchauffe 0x0217 déclenché à 106°C
- Git initialisé + README

### Concepts appris
- Fixed-width types (uint16_t, int16_t) pour la portabilité
- Header guards (#ifndef/#define/#endif)
- Encapsulation avec static
- Codage big-endian + échelles (0.25, 0.01) dans une trame CAN
- Anti-doublon DTC

### Problèmes rencontrés
- MSYS2 : gcc/cmake non trouvés → mauvais shell (MSYS au lieu de MINGW64)
- CMake : erreur 1723 → DLL VC++ Redist manquantes
- Linker : undefined reference to WinMain → main.c vide/non sauvegardé

### À explorer en Session 2
- Décodage de trame CAN (côté réception)
- Dashboard ECU qui affiche les valeurs décodées
- Vérifier cohérence encodage/décodage (round-trip test)

### Questions ouvertes
- Comment gérer plusieurs ECUs sur le même bus ?
- Faut-il un bus CAN simulé par file de messages ?

## Session 2 — 2026-10-30 ✅

### Réalisé
Dashboard ECU : réception, validation et décodage de la trame CAN 0x100
Extension du module can_frame : ajout de engine_status_decoded_t et can_decode_engine_status
Séparation de la logique et de l'I/O dans le Dashboard (dashboard_ecu_receive vs print)
Tests unitaires : tests/test_roundtrip.c avec 53 checks (cas nominaux, bornes, quantification)
Validation de l'intégration : 100 cycles, rx=100 / err=0
Mise à jour du README et création des tags Git (v1.0-session1, v2.0-session2)

Concepts appris
Symétrie encode/decode (principe du DBC) : garder le format au même endroit
Quantification du signal : perte de précision due aux facteurs d'échelle et gestion de la tolérance dans les tests
Différence entre erreur de compilation (unknown type) et erreur d'édition de liens (undefined reference)
Validation défensive : rejet des trames avec mauvais ID ou mauvais DLC
Gestion des branches Git (git pull --rebase) et utilisation des Tags

Problèmes rencontrés
Linker : undefined reference to can_decode_engine_status → fonction non implémentée dans le .c
Compilation : unknown type name 'can_frame_t' → oubli des #include lors d'un copier-coller
Compilation : redefinition of 'can_decode_engine_status' → doublon de fonction dans le fichier
Git : push rejeté (branches divergentes) → résolu avec git config pull.rebase true et git pull

À explorer en Session 3
Simulation d'un bus CAN (file de messages entre plusieurs ECUs)
Injection de fautes (trames perdues, corrompues, ID inconnu)
Ajout du Diagnostic ECU (requêtes UDS basiques)
Gestion du timeout dans le Dashboard (détection de trames obsolètes / stale)

Questions ouvertes
Comment gérer l'arbitrage et les priorités sur le bus CAN simulé ?
Faut-il simuler les ACK et le CRC pour être plus proche du niveau physique ?
