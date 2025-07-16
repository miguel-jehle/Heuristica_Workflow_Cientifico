#pragma once
#include "Base.h"

void Tempo_CPU_Sistema(double *seg_CPU_total, double *seg_sistema_total);
Problem le_dados(const char* arquivo);
void printa_dados(Problem p);
void escreveSolucao(Solution S, FILE * fl);