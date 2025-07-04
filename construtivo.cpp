#include "Base.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <algorithm>
#include "bibrand.c"
#include <sys/resource.h> 
#include <unistd.h>  // Adicione no topo

using namespace std;

//========================================================================================================================================

vector<Tasks> atualizaTarefasViaveis(vector<Tasks> TarefasViaveis, vector<Tasks> vet_tasks,vector<Data> vet_data);  //FUNÇÃO IMPLEMENTADA!

vector<Data> atualizaDisponibilidade(vector<Data> vet_data, vector<Tasks> vet_task ,Tripla candidato); //FUNÇÃO IMPLEMENTADA!

bool verificaDisponibilidade(int id, vector<Data> vet_data); //FUNÇÃO IMPLEMENTADA!

vector<Tripla> ordenaCusto(vector<Tripla> LC);//FUNÇÃO IMPLEMENTADA!

vector<Tripla> criaLRC(vector <Tripla> LC, int corte);//FUNÇÃO IMPLEMENTADA!

Tripla selecionaAleatoriamente(vector<Tripla> LRC, int* seed);//FUNÇÃO IMPLEMENTADA!

vector<Tasks> removeTarefa(vector<Tasks> TarefasViaveis, Tripla candidato);  //FUNÇÃO IMPLEMENTADA!

vector<Tripla> normalizaCustos( vector <Tripla> LC, float phi, double max_fin_cost, double max_runtime); //FUNÇÃO IMPLEMENTADA!

vector<Tasks> atualizaEscolhida(vector<Tasks> vet_tasks, Tripla candidato); //FUNÇÃO IMPLEMENTADA!

double calculaCustoTotal(Solution S);

double calculaTempoTotal(Solution S);

double calculaTempoVM(Problem p, Tasks task, Machine machine);

double calculaCustoFin(Solution S);

//========================================================================================================================================

Solution Construtivo(float alpha, float phi ,int* seed,Problem p){
    Solution S; // S <-- { }
    vector<Tasks> TarefasViaveis;                               // Cria a lista de tasks que podem ser executadas.
    TarefasViaveis = atualizaTarefasViaveis(TarefasViaveis,p.vet_tasks,p.vet_data);    // Atualiza neste caso serve para gerá-la pela primeira vez
    Tripla aux;
    
    while(!TarefasViaveis.empty()){ //Enquanto S nao esta completo faça
        //Fazer AQUI a verificacao, nas TV, as VMs, se uma esta sendo utilizada nessa iteracao, 
        vector<Tripla> LC; //LC <-- { }

        //Não sei se este preenchimento está de fato correto, mas creio que sim.
        for(int i = 0; i < TarefasViaveis.size(); i++){
            for(int j = 0; j < TarefasViaveis[i].vet_config.size(); j++){
                aux.task_id = TarefasViaveis[i].task_id;
                aux.config_id = TarefasViaveis[i].vet_config[j].config_id;
                aux.task_p_config_cost = TarefasViaveis[i].vet_config[j].task_p_config_cost;
                aux.task_time_total = TarefasViaveis[i].vet_config[j].task_time_total;
                if(aux.task_time_total > 900) continue;
                aux.type = 1;
                LC.push_back(aux);
            }
        }

        //Aqui devo inserir as VM na LC.
        for(int i = 0; i < TarefasViaveis.size(); i++){
            for(int j = 0; j < p.vet_machine.size(); j++){
                aux.task_id = TarefasViaveis[i].task_id;
                aux.vm_id = p.vet_machine[j].id;
                aux.vm_slowdown = p.vet_machine[j].slowdown;
                aux.vm_time_total = calculaTempoVM(p,TarefasViaveis[i],p.vet_machine[j]); 
                aux.vm_cost_total = p.vet_machine[j].cost * aux.vm_time_total; //Verificar se o slowdown considera ja o tempo de leitura e escrita
                aux.vm_cpu_time = TarefasViaveis[i].vm_cpu_time;
                aux.type = 0;
                LC.push_back(aux);
            }
        }

        //Bloco correspondente ao algortimo original
        LC = normalizaCustos(LC, phi,p.max_fin_cost, p.max_runtime);
        LC = ordenaCusto(LC);
        int corte = LC.size() * alpha;
        if (!corte) corte++;
        vector<Tripla> LRC = criaLRC(LC, corte);
        Tripla candidato = selecionaAleatoriamente(LRC,seed);
        //Aqui faço a verificação da maquina previamente comprada por meio do campo temp_rest.

        //Para todas as VM's contratadas que tem um tempo restante diferente de zero, se existe alguma tarefa (candidato) que cabe dentro deste intervalo restante.
        S.vet_tripla.push_back(candidato);

        //Ações necessaárias para adaptar o construtivo para esse problema.
        p.vet_data = atualizaDisponibilidade(p.vet_data,p.vet_tasks, candidato); //Atualiza a disponibilidade dos dados
        //TarefasViaveis = removeTarefa(TarefasViaveis, candidato); //Por conta dessa remoção, para não quebrar a lógica, precisei criar uma flag para verificar se a tarefa 
        p.vet_tasks = atualizaEscolhida(p.vet_tasks, candidato); //Atualiza a flag de escolhida
        TarefasViaveis.clear(); //Limpa a lista de tarefas viáveis
        TarefasViaveis = atualizaTarefasViaveis(TarefasViaveis,p.vet_tasks,p.vet_data); //Atualiza a lista de tarefas viáveis
    }
    S.cost_fin = calculaCustoFin(S);
    S.cost = calculaCustoTotal(S);
    S.time = calculaTempoTotal(S);
    return S;
}

//========================================================================================================================================

vector<Tasks> removeTarefa(vector<Tasks> TarefasViaveis, Tripla candidato){  //Percorro a lista de tarefas viáveis e removo a tarefa que foi selecionada baseado no task id, pois se escolhi uma task, independente da configuração, ela não pode ser escolhida novamente.
    for(int i = 0; i < TarefasViaveis.size(); i++){
        if(TarefasViaveis[i].task_id == candidato.task_id){
            TarefasViaveis.erase(TarefasViaveis.begin() + i);
            return TarefasViaveis;
        }
    }
    return TarefasViaveis;
}

//========================================================================================================================================

Tripla selecionaAleatoriamente(vector<Tripla> LRC, int* seed){
    int aleatorio = get_rand_ij(seed,0,LRC.size()-1); //Gera um número aleatório entre 0 e o tamanho da lista LRC já o resto da divisão pelo tamanho da lista garante que o número gerado esteja dentro do intervalo da lista.
    return LRC[aleatorio];
}

//========================================================================================================================================

vector<Tripla> criaLRC(vector <Tripla> LC, int corte){
    vector<Tripla> LRC;
    for(int i = 0; i < corte; i++){
        LRC.push_back(LC[i]);
    }
    return LRC;
}

//========================================================================================================================================

vector<Tripla> ordenaCusto(vector<Tripla> LC){ 
    sort(LC.begin(), LC.end(), [](const Tripla &a, const Tripla &b) { //Os parametros do sort são o inicio e o fim do vetor e uma condição de ordenação que neste caso utilizei uma função lambda.
        return a.final_cost < b.final_cost;
    });
    return LC;
}

//========================================================================================================================================

vector<Tasks> atualizaTarefasViaveis(vector<Tasks> TarefasViaveis, vector<Tasks> vet_tasks, vector<Data> vet_data) {
    for (int i = 0; i < vet_tasks.size(); i++) {
        if (vet_tasks[i].escolhida) continue; // Se a tarefa já foi escolhida, passa para a próxima.
        bool todosDisponiveis = true;
        for (int j = 0; j < vet_tasks[i].n_input; j++) {
            if (!verificaDisponibilidade(vet_tasks[i].id_inputs[j], vet_data)) {
                todosDisponiveis = false;
                break;
            }
        }
        if (todosDisponiveis) {
            TarefasViaveis.push_back(vet_tasks[i]);
        }
    }
    return TarefasViaveis;
}

//========================================================================================================================================

bool verificaDisponibilidade(int id, vector<Data> vet_data) {
    for (int i = 0; i < vet_data.size(); i++) {
        if (vet_data[i].data_id == id) {
            if (vet_data[i].disponivel || vet_data[i].is_static) {
                return true;
            }
        }
    }
    return false;
}

//========================================================================================================================================

vector<Data> atualizaDisponibilidade(vector<Data> vet_data, vector<Tasks> vet_task, Tripla candidato){
    for(int i = 0; i < vet_task.size(); i++){
        if(vet_task[i].task_id == candidato.task_id){
            for(int j = 0; j < vet_task[i].n_output; j++){
                for(int k = 0; k < vet_data.size(); k++){
                    if(vet_data[k].data_id == vet_task[i].id_outputs[j]){
                        vet_data[k].disponivel = true;
                    }
                }
            }
        }
    }
    return vet_data;
}

//========================================================================================================================================

vector<Tasks> atualizaEscolhida(vector<Tasks> vet_tasks, Tripla candidato){
    for(int i = 0; i < vet_tasks.size(); i++){
        if(vet_tasks[i].task_id == candidato.task_id){
            vet_tasks[i].escolhida = true;
            break;
        }
    }
    return vet_tasks;
}

//========================================================================================================================================

vector <Tripla> normalizaCustos(vector <Tripla> LC, float phi, double max_fin_cost, double max_runtine){
    for(int i = 0; i < LC.size(); i++){
        if(LC[i].type == 1){
            LC[i].final_cost = phi * (LC[i].task_time_total/max_runtine) + (1 - phi) * (LC[i].task_p_config_cost/max_fin_cost);
        }
        else{
            LC[i].final_cost = phi * (LC[i].vm_time_total/max_runtine) + (1 - phi) * (LC[i].vm_cost_total/max_fin_cost);
        }
    }
    return LC;
}

//========================================================================================================================================

void printaSolucao(Solution S){
    
    cout << S.cost << endl;
}

//========================================================================================================================================

double calculaCustoTotal(Solution S){
    double custo = 0;
    for(int i = 0; i < S.vet_tripla.size(); i++){
        custo += S.vet_tripla[i].final_cost; 
    }
    return custo;
}

//========================================================================================================================================

double calculaTempoTotal(Solution S){
    double tempo = 0;
    for(int i = 0; i < S.vet_tripla.size(); i++){
        if(S.vet_tripla[i].type == 1){
            tempo += S.vet_tripla[i].task_time_total;
        }
        else{
            tempo += S.vet_tripla[i].vm_time_total;
        }
    }
    return tempo;
}

//========================================================================================================================================

double calculaCustoFin(Solution S){
    double custo_fin = 0;
    for(int i = 0; i < S.vet_tripla.size(); i++){
        if(S.vet_tripla[i].type == 1){
            custo_fin += S.vet_tripla[i].task_p_config_cost;
        }
        else{
            custo_fin += S.vet_tripla[i].vm_cost_total;
        }
    }
    return custo_fin;
}

//=======================================================================================================================================================

double calculaTempoVM(Problem p, Tasks task, Machine machine){
    //calcular o custo sendo (VM_CPU_time + tempo de leitura + tempo saidas)
    double tempo = 0;
    for(int i = 0; i < task.id_inputs.size(); i++){
        for(int j = 0; j < p.vet_data.size(); j++){
            if(task.id_inputs[i] == p.vet_data[j].data_id){
                tempo += p.vet_data[j].read_time;                
            }
        }
    }
    for(int i = 0; i < task.id_outputs.size(); i++){
        for(int j = 0; j < p.vet_data.size(); j++){
            if(task.id_outputs[i] == p.vet_data[j].data_id){
                tempo += p.vet_data[j].write_time;
            }
        }
    }
    tempo += task.vm_cpu_time*machine.slowdown;
    return tempo; 
}

//========================================================================================================================================

double calculaMaxFinCost(Problem p) {
    double max_fin_cost_FX = 0;

    for (int i = 0; i < p.vet_tasks.size(); i++) {
        max_fin_cost_FX += p.vet_tasks[i].vet_config[4].task_p_config_cost;
    }

    double max_fin_cost_VM = 0;

    Machine max_machine = p.vet_machine.back();

    for (int i = 0; i < p.vet_tasks.size(); i++) {
        double max_machine_time = calculaTempoVM(p, p.vet_tasks[i], max_machine);
        double max_machine_cost = max_machine.cost * max_machine.slowdown * max_machine_time;
        max_fin_cost_VM += max_machine_cost;
    }

    if (max_fin_cost_VM > max_fin_cost_FX) {
        return max_fin_cost_VM;
    } else {
        return max_fin_cost_FX;
    }
}

//========================================================================================================================================

double calculaMaxRuntime(Problem p) {
    double max_runtime = 0;

    for (int i = 0; i < p.vet_tasks.size(); i++) {
        max_runtime += p.vet_tasks[i].vet_config[0].task_time_total;
    }

    return max_runtime;
}

//========================================================================================================================================

double normalizaUmCusto(Tripla T, float phi, double max_fin_cost, double max_runtine){
    if(T.type == 1){
        return phi * (T.task_time_total/max_runtine) + (1 - phi) * (T.task_p_config_cost/max_fin_cost);
    }

    return phi * (T.vm_time_total/max_runtine) + (1 - phi) * (T.vm_cost_total/max_fin_cost);
}