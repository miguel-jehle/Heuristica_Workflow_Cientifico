#include "utils.h"
#include "construtivo.h"
#include "busca_local.h"
#include "Base.h"
#include <cstring>
#include <iostream>

#define MAX_INST 100

using namespace std;

int main(int argc, char** argv) {
    if (argc < 5) {
        printf("Uso: %s phi alpha repeticoes swap_choice\n", argv[0]);
        printf("swap_choice: 1=Swap_Machine, 2=Swap_Config, 3=Swap_MachineToConfig, 4=Swap_ConfigToMachine, 5=Swap_MachinePair, 6=Swap_ConfigPair, 7=SwapMachineConfig_Pair, 8=SwapConfigMachine_Pair\n");
        return 1;
    }
    float phi = atof(argv[1]);
    float alpha = atof(argv[2]);
    int repeticoes = atoi(argv[3]);
    int swap_choice = atoi(argv[4]);
    char instance_names[MAX_INST][128];
    int num_instances = readInstanceNames("Irace_VND/Entradas_Irace/teste.txt", instance_names, MAX_INST);
    if (num_instances < 0) {
        printf("Erro ao abrir sumario.txt\n");
        return 1;
    }
    char instance_paths[MAX_INST][256];
    if (!setupRunFolders(phi, instance_paths, &num_instances)) {
        printf("Erro ao preparar pastas de execução\n");
        return 1;
    }
    for (int i = 0; i < num_instances; i++) {
        char general_path[300], final_path[300];
        setupOutputFiles(instance_paths[i], general_path, final_path);
        processInstance(instance_names[i], general_path, final_path, alpha, phi, repeticoes, swap_choice);
    }
    return 0;
}