#include "busca_local.h"
#include "construtivo.h"  // Para funções de cálculo
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

                double tempo_novo = calculateVMTime(p, p.vet_tasks[k], p.vet_machine[j])*p.vet_machine[j].slowdown;
                
                double custo_finan_novo = p.vet_machine[j].cost * p.vet_machine[j].slowdown * tempo_novo;

                Tripla vizinho;
                vizinho.vm_cost_total = custo_finan_novo;
                vizinho.vm_time_total = tempo_novo;
                
                vizinho.cost = normalizeCandidateCost(vizinho,phi, p.max_fin_cost, p.max_runtime);

                if(vizinho.cost < S_atual.vet_tripla[i].cost){
                    S_atual.vet_tripla[i].cost = vizinho.cost;
                    S_atual.vet_tripla[i].vm_cost_total = vizinho.vm_cost_total;
                    S_atual.vet_tripla[i].vm_time_total = vizinho.vm_time_total;
                    S_atual.vet_tripla[i].vm_id = p.vet_machine[j].id;
                    S_atual.vet_tripla[i].vm_slowdown = p.vet_machine[j].slowdown;
                    S_atual.cost = calculateTotalCost(S_atual);
                    S_atual.financial_cost = calculateFinancialCost(S_atual);
                    S_atual.time = calculateTotalTime(S_atual);
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
                        vizinho.task_time_total = tempo_novo;
                        vizinho.task_p_config_cost = custo_finan_novo;

                        vizinho.cost = normalizeCandidateCost(vizinho,phi, p.max_fin_cost, p.max_runtime);

                        if(vizinho.cost < S_atual.vet_tripla[i].cost){
                        S_atual.vet_tripla[i].cost = vizinho.cost;
                        S_atual.vet_tripla[i].task_p_config_cost = vizinho.task_p_config_cost;
                        S_atual.vet_tripla[i].task_time_total = vizinho.task_time_total;
                        S_atual.vet_tripla[i].config_id = p.vet_tasks[j].vet_config[k].config_id;
                        S_atual.cost = calculateTotalCost(S_atual);
                        S_atual.financial_cost = calculateFinancialCost(S_atual);
                        S_atual.time = calculateTotalTime(S_atual);
                        printf("%f          %f\n\n", S_atual.financial_cost, S_atual.time);
                        return S_atual;
                        }

                    }
                }
            }
        }
    }
    return S;
}

Solution Try_Swap_MachineToConfig(Solution S, Problem p, int idx_task, float phi) {
    Solution S_atual = S;
    int task_id = S_atual.vet_tripla[idx_task].task_id;
    for (int j = 0; j < p.vet_tasks.size(); j++) {
        if (p.vet_tasks[j].task_id == task_id) {
            for (int k = 0; k < p.vet_tasks[j].vet_config.size(); k++) {
                if (S_atual.vet_tripla[idx_task].type == 1 && S_atual.vet_tripla[idx_task].config_id == p.vet_tasks[j].vet_config[k].config_id)
                    continue;
                Tripla vizinho = S_atual.vet_tripla[idx_task];
                vizinho.type = 1;
                vizinho.config_id = p.vet_tasks[j].vet_config[k].config_id;
                vizinho.task_time_total = p.vet_tasks[j].vet_config[k].task_time_total;
                vizinho.task_p_config_cost = p.vet_tasks[j].vet_config[k].task_p_config_cost;
                vizinho.cost = normalizeCandidateCost(vizinho, phi, p.max_fin_cost, p.max_runtime);
                if (vizinho.cost < S_atual.vet_tripla[idx_task].cost) {
                    S_atual.vet_tripla[idx_task] = vizinho;
                    S_atual.cost = calculateTotalCost(S_atual);
                    S_atual.financial_cost = calculateFinancialCost(S_atual);
                    S_atual.time = calculateTotalTime(S_atual);
                    return S_atual;
                }
            }
        }
    }
    return S;
}

Solution Swap_MachineToConfig(Solution S, Problem p, float phi) {
    Solution S_atual = S;
    for (int i = 0; i < S_atual.vet_tripla.size(); i++) {
        if (S_atual.vet_tripla[i].type == 0) {
            Solution S_vizinho = Try_Swap_MachineToConfig(S_atual, p, i, phi);
            if (S_vizinho.cost < S_atual.cost) {
                return S_vizinho;
            }
        }
    }
    return S;
}

Solution Try_Swap_ConfigToMachine(Solution S, Problem p, int idx_task, float phi) {
    Solution S_atual = S;
    int task_id = S_atual.vet_tripla[idx_task].task_id;
    for (int j = 0; j < p.vet_tasks.size(); j++) {
        if (p.vet_tasks[j].task_id == task_id) {
            for (int k = 0; k < p.vet_machine.size(); k++) {
                if (S_atual.vet_tripla[idx_task].type == 0 && S_atual.vet_tripla[idx_task].vm_id == p.vet_machine[k].id)
                    continue;
                Tripla vizinho = S_atual.vet_tripla[idx_task];
                vizinho.type = 0;
                vizinho.vm_id = p.vet_machine[k].id;
                vizinho.vm_slowdown = p.vet_machine[k].slowdown;
                vizinho.vm_time_total = calculateVMTime(p, p.vet_tasks[j], p.vet_machine[k]);
                vizinho.vm_cost_total = p.vet_machine[k].cost * p.vet_machine[k].slowdown * vizinho.vm_time_total;
                vizinho.cost = normalizeCandidateCost(vizinho, phi, p.max_fin_cost, p.max_runtime);
                if (vizinho.cost < S_atual.vet_tripla[idx_task].cost) {
                    S_atual.vet_tripla[idx_task] = vizinho;
                    S_atual.cost = calculateTotalCost(S_atual);
                    S_atual.financial_cost = calculateFinancialCost(S_atual);
                    S_atual.time = calculateTotalTime(S_atual);
                    return S_atual;
                }
            }
        }
    }
    return S;
}

Solution Swap_ConfigToMachine(Solution S, Problem p, float phi) {
    Solution S_atual = S;
    for (int i = 0; i < S_atual.vet_tripla.size(); i++) {
        if (S_atual.vet_tripla[i].type == 1) {
            Solution S_vizinho = Try_Swap_ConfigToMachine(S_atual, p, i, phi);
            if (S_vizinho.cost < S_atual.cost) {
                return S_vizinho;
            }
        }
    }
    return S;
}

Solution SwapMachine_Pair(Solution S, Problem p, float phi) {
    Solution S_atual = S;
    int n = S_atual.vet_tripla.size();
    for (int i = 0; i < n; i++) {
        if (S_atual.vet_tripla[i].type != 0) continue;
        for (int j = i + 1; j < n; j++) {
            if (S_atual.vet_tripla[j].type != 0) continue;
            // Só troca se as VMs forem diferentes
            if (S_atual.vet_tripla[i].vm_id == S_atual.vet_tripla[j].vm_id) continue;
            Solution S_vizinho = S_atual;
            // Troca as VMs das duas tasks
            int vm_i = S_vizinho.vet_tripla[i].vm_id;
            int vm_j = S_vizinho.vet_tripla[j].vm_id;
            double slowdown_i = S_vizinho.vet_tripla[i].vm_slowdown;
            double slowdown_j = S_vizinho.vet_tripla[j].vm_slowdown;
            S_vizinho.vet_tripla[i].vm_id = vm_j;
            S_vizinho.vet_tripla[i].vm_slowdown = slowdown_j;
            S_vizinho.vet_tripla[j].vm_id = vm_i;
            S_vizinho.vet_tripla[j].vm_slowdown = slowdown_i;
            // Recalcula tempos e custos das duas tasks
            int idx_task_i = -1, idx_task_j = -1;
            for (int k = 0; k < p.vet_tasks.size(); k++) {
                if (p.vet_tasks[k].task_id == S_vizinho.vet_tripla[i].task_id) idx_task_i = k;
                if (p.vet_tasks[k].task_id == S_vizinho.vet_tripla[j].task_id) idx_task_j = k;
            }
            if (idx_task_i == -1 || idx_task_j == -1) continue;
            S_vizinho.vet_tripla[i].vm_time_total = calculateVMTime(p, p.vet_tasks[idx_task_i], p.vet_machine[S_vizinho.vet_tripla[i].vm_id]);
            S_vizinho.vet_tripla[i].vm_cost_total = p.vet_machine[S_vizinho.vet_tripla[i].vm_id].cost * p.vet_machine[S_vizinho.vet_tripla[i].vm_id].slowdown * S_vizinho.vet_tripla[i].vm_time_total;
            S_vizinho.vet_tripla[i].cost = normalizeCandidateCost(S_vizinho.vet_tripla[i], phi, p.max_fin_cost, p.max_runtime);
            S_vizinho.vet_tripla[j].vm_time_total = calculateVMTime(p, p.vet_tasks[idx_task_j], p.vet_machine[S_vizinho.vet_tripla[j].vm_id]);
            S_vizinho.vet_tripla[j].vm_cost_total = p.vet_machine[S_vizinho.vet_tripla[j].vm_id].cost * p.vet_machine[S_vizinho.vet_tripla[j].vm_id].slowdown * S_vizinho.vet_tripla[j].vm_time_total;
            S_vizinho.vet_tripla[j].cost = normalizeCandidateCost(S_vizinho.vet_tripla[j], phi, p.max_fin_cost, p.max_runtime);
            // Atualiza custos totais
            S_vizinho.cost = calculateTotalCost(S_vizinho);
            S_vizinho.financial_cost = calculateFinancialCost(S_vizinho);
            S_vizinho.time = calculateTotalTime(S_vizinho);
            if (S_vizinho.cost < S_atual.cost) {
                return S_vizinho;
            }
        }
    }
    return S;
}