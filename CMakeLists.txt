cmake_minimum_required(VERSION 3.15)
project(ecu_simulator C)

# 1. Configuration du compilateur C
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra)

# 2. Dossier des headers
include_directories(include)

# 3. La bibliothèque "core" (tous les .c métier SAUF main.c)
add_library(core STATIC
    src/can_bus.c
    src/can_frame.c
    src/dashboard_ecu.c
    src/diag_ecu.c
    src/engine_ecu.c
    src/fault_injector.c
    src/sensors.c
    src/uds_server.c
)

# 4. L'exécutable principal (le simulateur)
add_executable(simulator src/main.c)
target_link_libraries(simulator PRIVATE core)

# 5. Activation des tests
enable_testing()

# 6. Création automatique des exécutables de test
# On boucle sur tes 5 fichiers de test pour ne pas répéter le code
set(TEST_NAMES
    test_can_bus
    test_dashboard_timeout
    test_fault_injection
    test_roundtrip
    test_uds
)

foreach(test_name ${TEST_NAMES})
    add_executable(${test_name} tests/${test_name}.c)
    target_link_libraries(${test_name} PRIVATE core)
    # Déclaration du test auprès de CTest
    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()