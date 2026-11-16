import os
import subprocess
import pytest

# On remonte de tests/integration vers la racine, puis vers build/
BUILD_DIR = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", "build"))

# Liste de tes exécutables générés par CMake
TEST_EXECUTABLES = [
    "test_can_bus.exe",
    "test_dashboard_timeout.exe",
    "test_fault_injection.exe",
    "test_roundtrip.exe",
    "test_uds.exe"
]

@pytest.mark.parametrize("exe_name", TEST_EXECUTABLES)
def test_c_executable(exe_name):
    """
    Lance un exécutable C de test et vérifie qu'il passe avec succès.
    """
    exe_path = os.path.join(BUILD_DIR, exe_name)
    
    # 1. Vérifier que l'exécutable existe (le build a bien tourné)
    assert os.path.exists(exe_path), f"Exécutable introuvable : {exe_path}. As-tu lancé 'cmake --build build' ?"
    
    # 2. Lancer l'exécutable et capturer la sortie
    result = subprocess.run([exe_path], capture_output=True, text=True)
    
    # 3. Vérifier le code de retour (0 = succès dans nos tests C)
    assert result.returncode == 0, \
        f"{exe_name} a échoué (code {result.returncode}).\nSortie :\n{result.stdout}\nErreurs :\n{result.stderr}"
    
    # 4. Vérifier la chaîne de caractères attendue (sécurité supplémentaire)
    assert "0 failed" in result.stdout, \
        f"{exe_name} n'a pas rapporté '0 failed'.\nSortie :\n{result.stdout}"
    
    # Optionnel : afficher le résultat dans le rapport HTML
    print(f"\n[{exe_name}] -> {result.stdout.strip().splitlines()[-1]}")