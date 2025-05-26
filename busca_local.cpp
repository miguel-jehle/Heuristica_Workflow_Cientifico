#include "Base.h"

double calculaTempoVM1(Problem p, Tasks task, Machine machine){
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
    tempo += task.vm_cpu_time;
    return tempo; 
}

double calculaCustoTotal1(Solution S){
    double custo = 0;
    for(int i = 0; i < S.vet_tripla.size(); i++){
        custo += S.vet_tripla[i].final_cost; 
    }
    return custo;
}

Solution Swap_Machine(Solution S, Problem p){
    double custo_atual = S.cost;

    while (true){
        Solution melhor_vizinho;
        melhor_vizinho.cost = custo_atual;

        for(int i = 0; i < S.vet_tripla.size(); i++){ //Para cada tarefa da solução
            for(int j = 0; j < p.vet_machine.size(); j++){ //Para cada VM possível
                if(melhor_vizinho.vet_tripla[i].vm_id != p.vet_machine[j].id){ //Verifico se a VM não eh a mesma
                    Solution vizinho = S;
                    vizinho.vet_tripla[i].vm_id = p.vet_machine[j].id;
                    vizinho.vet_tripla[i].vm_slowdown = p.vet_machine[j].slowdown;
                    vizinho.vet_tripla[i].vm_time_total = calculaTempoVM1(p, p.vet_tasks[vizinho.vet_tripla[i].task_id], p.vet_machine[j]);
                    vizinho.vet_tripla[i].vm_cost_total = p.vet_machine[j].cost * p.vet_machine[j].slowdown * vizinho.vet_tripla[i].vm_time_total;
                    vizinho.cost = calculaCustoTotal1(vizinho);

                    if(vizinho.cost < melhor_vizinho.cost){ //Se o custo do vizinho for melhor que o melhor vizinho encontrado
                        melhor_vizinho = vizinho; //Atualiza o melhor vizinho
                    }
                }
            }
        }
        if(melhor_vizinho.cost >= custo_atual){ //Se o melhor vizinho não for melhor que o custo atual, sai do loop
            return S; //Retorna a solução original
        }
        S = melhor_vizinho; //Atualiza a solução com o melhor vizinho encontrado
        custo_atual = S.cost; //Atualiza o custo atual
    }

}