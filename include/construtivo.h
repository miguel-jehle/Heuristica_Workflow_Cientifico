#pragma once
#include "Base.h"

Solution Construtivo(float alpha, float phi, int* seed, Problem p);
vector<Tasks> atualizaTarefasViaveis(vector<Tasks> TarefasViaveis, vector<Tasks> vet_tasks, vector<Data> vet_data);
vector<Data> atualizaDisponibilidade(vector<Data> vet_data, vector<Tasks> vet_task, Tripla candidato);
bool verificaDisponibilidade(int id, vector<Data> vet_data);
vector<Tripla> ordenaCusto(vector<Tripla> LC);
vector<Tripla> criaLRC(vector <Tripla> LC, int corte);
Tripla selecionaAleatoriamente(vector<Tripla> LRC, int* seed);
vector<Tasks> removeTarefa(vector<Tasks> TarefasViaveis, Tripla candidato);
vector<Tripla> normalizaCustos(vector <Tripla> LC, float phi, double max_fin_cost, double max_runtime);
vector<Tasks> atualizaEscolhida(vector<Tasks> vet_tasks, Tripla candidato);
double calculaCustoTotal(Solution S);
double calculaTempoTotal(Solution S);
double calculaTempoVM(Problem p, Tasks task, Machine machine);
double calculaCustoFin(Solution S);
double calculaMaxFinCost(Problem p);
double calculaMaxRuntime(Problem p);
double normalizaUmCusto(Tripla T, float phi, double max_fin_cost, double max_runtine);
void printaSolucao(Solution S);