#include "utils.h"
#include "construtivo.h"
#include "busca_local.h"
#include "Base.h"
#include <cstring>
#include <iostream>

using namespace std;

int main(int argc, char** argv) {
    if (argc < 5) {
        cerr << "Uso: " << argv[0] << " caminho_resp phi alpha repeticoes\n";
        return 1;
    }

    char caminho_geral[200], caminho_final[200];
    setupOutputFiles(argv[1], caminho_geral, caminho_final);

    FILE* fp = fopen("Instancias/teste.txt", "r");
    if (!fp) {
        cerr << "Erro ao abrir arquivo de instâncias\n";
        return 1;
    }

    char nome_arquivo[200];
    while (fscanf(fp, "%s", nome_arquivo) == 1) {
        processInstance(
            nome_arquivo,
            caminho_geral, 
            caminho_final, 
            atof(argv[3]),
            atof(argv[2]),
            atoi(argv[4])
        );
    }

    fclose(fp);
    return 0;
}