#include "utils.h"
#include "construtivo.h"
#include "busca_local.h"
#include "Base.h"
#include <cstring>
#include <iostream>

#define MAX_INST 100

using namespace std;

int main(int argc, char** argv) {
    if (argc < 4) {
        printf("Uso: %s phi alpha repeticoes\n", argv[0]);
        return 1;
    }
    float phi = atof(argv[1]);
    float alpha = atof(argv[2]);
    int repeticoes = atoi(argv[3]);
    char instance_names[MAX_INST][128];
    int num_instances = readInstanceNames("Instancias/sumario.txt", instance_names, MAX_INST);
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
        processInstance(instance_names[i], general_path, final_path, alpha, phi, repeticoes);
    }
    return 0;
}