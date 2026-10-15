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
