#include "construtivo.cpp"
#include "Base.h"

Solution Swap_Machine(Solution S, Problem p, float phi){
    Solution S_atual = S;
    for(int i = 0; i < S_atual.vet_tripla.size(); i++){
        if(S_atual.vet_tripla[i].type == 0){
            for(int j = 0; j < p.vet_machine.size(); j++){
                if(S_atual.vet_tripla[i].vm_id == p.vet_machine[j].id) continue;
                
                int k;
                for(k = 0; k < p.vet_tasks.size(); k++){
                    if (p.vet_tasks[k].task_id == S_atual.vet_tripla[i].task_id) break;
                }

                double tempo_novo = calculaTempoVM(p, p.vet_tasks[k], p.vet_machine[j])*p.vet_machine[j].slowdown;
                
                double custo_finan_novo = p.vet_machine[j].cost * p.vet_machine[j].slowdown * tempo_novo;

                Tripla vizinho;
                vizinho.vm_cost_total = custo_finan_novo;
                vizinho.vm_time_total = tempo_novo;
                
                vizinho.final_cost = normalizaUmCusto(vizinho,phi, p.max_fin_cost, p.max_runtime);

                if(vizinho.final_cost < S_atual.vet_tripla[i].final_cost){
                    S_atual.vet_tripla[i].final_cost = vizinho.final_cost;
                    S_atual.vet_tripla[i].vm_cost_total = vizinho.vm_cost_total;
                    S_atual.vet_tripla[i].vm_time_total = vizinho.vm_time_total;
                    S_atual.vet_tripla[i].vm_id = p.vet_machine[j].id;
                    S_atual.vet_tripla[i].vm_slowdown = p.vet_machine[j].slowdown;
                    S_atual.cost = calculaCustoTotal(S_atual);
                    S_atual.cost_fin = calculaCustoFin(S_atual);
                    S_atual.time = calculaTempoTotal(S_atual);
                    return S_atual;
                }
            }
        }
    }
    return S;
}

Solution Swap_Config(Solution S, Problem p, float phi){
    Solution S_atual = S;
    for(int i = 0; i < S_atual.vet_tripla.size(); i++){
        if(S_atual.vet_tripla[i].type == 1){ //Se for FX
            for(int j = 0; j < p.vet_tasks.size(); j++){
               if(p.vet_tasks[j].task_id == S_atual.vet_tripla[i].task_id){
                    for(int k = 0; k < p.vet_tasks[j].vet_config.size(); k++){
                        if(p.vet_tasks[j].vet_config[k].config_id == S_atual.vet_tripla[i].config_id) continue;

                        double tempo_novo = p.vet_tasks[j].vet_config[k].task_time_total;

                        double custo_finan_novo = p.vet_tasks[j].vet_config[k].task_p_config_cost;

                        Tripla vizinho;
                        vizinho.task_p_config_cost = custo_finan_novo;
                        vizinho.task_time_total = tempo_novo;

                        vizinho.final_cost = normalizaUmCusto(vizinho,phi, p.max_fin_cost, p.max_runtime);

                        if(vizinho.final_cost < S_atual.vet_tripla[i].final_cost){
                        S_atual.vet_tripla[i].final_cost = vizinho.final_cost;
                        S_atual.vet_tripla[i].task_p_config_cost = vizinho.task_p_config_cost;
                        S_atual.vet_tripla[i].task_time_total = vizinho.task_time_total;
                        S_atual.vet_tripla[i].config_id = p.vet_tasks[j].vet_config[k].config_id;
                        S_atual.cost = calculaCustoTotal(S_atual);
                        S_atual.cost_fin = calculaCustoFin(S_atual);
                        S_atual.time = calculaTempoTotal(S_atual);
                        return S_atual;
                        }

                    }
                }
            }
        }
    }
    return S;
}





























// Solution Swap_Machine(Solution S, Problem p, float phi){
//     double custo_atual = S.cost;

//     while (true){
//         Solution melhor_vizinho = S;
//         melhor_vizinho.cost = custo_atual;

//         for(int i = 0; i < S.vet_tripla.size(); i++){ //Para cada tarefa da solução
//             if(S.vet_tripla[i].type == 1){ 
//                 for(int j = 0; j < p.vet_machine.size(); j++){ //Para cada VM possível
//                     if(S.vet_tripla[i].vm_id != p.vet_machine[j].id){ //Verifico se a VM não eh a mesma
//                         Solution vizinho = S;
//                         vizinho.vet_tripla[i].vm_id = p.vet_machine[j].id; //Troca a VM da tarefa
//                         vizinho.vet_tripla[i].vm_slowdown = p.vet_machine[j].slowdown; //Atualiza o slowdown da VM
//                         vizinho.vet_tripla[i].vm_time_total = calculaTempoVM(p, p.vet_tasks[vizinho.vet_tripla[i].task_id], p.vet_machine[j]); //Calcula o tempo total da VM
//                         vizinho.vet_tripla[i].vm_cost_total = p.vet_machine[j].cost * p.vet_machine[j].slowdown * vizinho.vet_tripla[i].vm_time_total; //Calcula o custo total da VM
//                         vizinho.vet_tripla[i].type = 1; //Atualiza o tipo da tarefa para VM
//                         vizinho.vet_tripla = normalizaCustos(vizinho.vet_tripla, phi, p.max_fin_cost, p.max_runtime); //Normaliza os custos da solução vizinha
//                         vizinho.cost = calculaCustoTotal(vizinho); //Calcula o custo total da solução vizinha


//                         if(vizinho.cost < melhor_vizinho.cost){ //Se o custo do vizinho for melhor que o melhor vizinho encontrado
//                             melhor_vizinho = vizinho; //Atualiza o melhor vizinho

//                         }
//                     }
//                 }
//             }
//         }
//         if(melhor_vizinho.cost > custo_atual){ //Se o melhor vizinho não for melhor que o custo atual, sai do loop
//             return S; //Retorna a solução original
//         }
//         S = melhor_vizinho; //Atualiza a solução com o melhor vizinho encontrado
//         custo_atual = S.cost; //Atualiza o custo atual
//     }

// }