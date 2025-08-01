#pragma once
#include "Base.h"
#include <stdio.h>

void getCPUTime(double *seg_CPU_total, double *seg_sistema_total);
Problem readData(const char* arquivo);
void printProblem(Problem p);
void writeSolution(Solution S, FILE * fl);
void setupOutputFiles(const char* caminho_base, char* caminho_geral, char* caminho_final);
Solution GRASP_VND(Problem p, float alpha, float phi, int repeticoes, int seed, double& tempo_exec);
void processInstance(const char* nome_instancia, const char* caminho_geral, const char* caminho_final, float alpha, float phi, int repeticoes);
int setupRunFolders(float phi, char caminhos_instancias[][256], int* n_instancias);
int readInstanceNames(const char* filename, char instance_names[][128], int max_instances);