#pragma once
#include "Base.h"

Solution constructiveHeuristic(float alpha, float phi, int* seed, Problem p);
vector<Tasks> updateFeasibleTasks(vector<Tasks> TarefasViaveis, vector<Tasks> vet_tasks, vector<Data> vet_data);
vector<Data> updateDataAvailability(vector<Data> vet_data, vector<Tasks> vet_task, Tripla candidato);
bool checkAvailability(int id, vector<Data> vet_data);
vector<Tripla> sortByCost(vector<Tripla> LC);
vector<Tripla> createRCL(vector <Tripla> LC, int corte);
Tripla selectRandomCandidate(vector<Tripla> LRC, int* seed);
vector<Tasks> removeTask(vector<Tasks> TarefasViaveis, Tripla candidato);
vector<Tripla> normalizeCosts(vector <Tripla> LC, float phi, double max_fin_cost, double max_runtime);
vector<Tasks> markTaskAsChosen(vector<Tasks> vet_tasks, Tripla candidato);
double calculateTotalCost(Solution S);
double calculateTotalTime(Solution S);
double calculateVMTime(Problem p, Tasks task, Machine machine);
double calculateFinancialCost(Solution S);
double calculateMaxFinancialCost(Problem p);
double calculateMaxRuntime(Problem p);
double normalizeCandidateCost(Tripla T, float phi, double max_fin_cost, double max_runtine);
void printSolution(Solution S);