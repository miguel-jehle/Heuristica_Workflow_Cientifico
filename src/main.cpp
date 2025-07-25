#include "utils.h"
#include "construtivo.h"
#include "busca_local.h"
#include "Base.h"
#include <cstring>
#include <iostream>

using namespace std;

int main(int argc, char** argv) {
    if (argc < 6) {
        printf("Uso: %s phi alpha repeticoes swap_choice instancia_path\n", argv[0]);
        printf("swap_choice: 1=Swap_Machine, 2=Swap_Config, 3=Swap_MachineToConfig, 4=Swap_ConfigToMachine, 5=Swap_MachinePair, 6=Swap_ConfigPair, 7=SwapMachineConfig_Pair, 8=SwapConfigMachine_Pair\n");
        return 1;
    }
    float phi = atof(argv[1]);
    float alpha = atof(argv[2]);
    int repeticoes = atoi(argv[3]);
    int swap_choice = atoi(argv[4]);
    const char* instancia_path = argv[5];
    char general_path[300], final_path[300];
    //setupOutputFiles(instancia_path, general_path, final_path);
    processInstance(instancia_path, general_path, final_path, alpha, phi, repeticoes, swap_choice);
    return 0;
}
