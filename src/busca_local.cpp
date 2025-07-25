#include "busca_local.h"
#include "construtivo.h"  // Para funções de cálculo
#include "Base.h"

Solution Swap_Machine(Solution S, Problem p, float phi){
    Solution S_atual = S;
    for(int i = 0; i < S_atual.vet_tripla.size(); i++){
        if(S_atual.vet_tripla[i].type == 0){
            for(int j = 0; j < p.vet_machine.size(); j++){
                if(S_atual.vet_tripla[i].vm_id == p.vet_machine[j].id) continue;
                
                // Usa o índice direto
                int k = S_atual.vet_tripla[i].task_index;
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
            int idx_task = S_atual.vet_tripla[i].task_index;
            for(int k = 0; k < p.vet_tasks[idx_task].vet_config.size(); k++){
                if(p.vet_tasks[idx_task].vet_config[k].config_id == S_atual.vet_tripla[i].config_id) continue;
                double tempo_novo = p.vet_tasks[idx_task].vet_config[k].task_time_total;
                double custo_finan_novo = p.vet_tasks[idx_task].vet_config[k].task_p_config_cost;
                Tripla vizinho;
                vizinho.task_time_total = tempo_novo;
                vizinho.task_p_config_cost = custo_finan_novo;
                vizinho.cost = normalizeCandidateCost(vizinho,phi, p.max_fin_cost, p.max_runtime);
                if(vizinho.cost < S_atual.vet_tripla[i].cost){
                    S_atual.vet_tripla[i].cost = vizinho.cost;
                    S_atual.vet_tripla[i].task_p_config_cost = vizinho.task_p_config_cost;
                    S_atual.vet_tripla[i].task_time_total = vizinho.task_time_total;
                    S_atual.vet_tripla[i].config_id = p.vet_tasks[idx_task].vet_config[k].config_id;
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

Solution Try_Swap_MachineToConfig(Solution S, Problem p, int idx_task, float phi) {
    Solution S_atual = S;
    int idx = S_atual.vet_tripla[idx_task].task_index;
    for (int k = 0; k < p.vet_tasks[idx].vet_config.size(); k++) {
        if (S_atual.vet_tripla[idx_task].type == 1 && S_atual.vet_tripla[idx_task].config_id == p.vet_tasks[idx].vet_config[k].config_id)
            continue;
        Tripla vizinho = S_atual.vet_tripla[idx_task];
        vizinho.type = 1;
        vizinho.config_id = p.vet_tasks[idx].vet_config[k].config_id;
        vizinho.task_time_total = p.vet_tasks[idx].vet_config[k].task_time_total;
        vizinho.task_p_config_cost = p.vet_tasks[idx].vet_config[k].task_p_config_cost;
        vizinho.cost = normalizeCandidateCost(vizinho, phi, p.max_fin_cost, p.max_runtime);
        if (vizinho.cost < S_atual.vet_tripla[idx_task].cost) {
            S_atual.vet_tripla[idx_task] = vizinho;
            S_atual.cost = calculateTotalCost(S_atual);
            S_atual.financial_cost = calculateFinancialCost(S_atual);
            S_atual.time = calculateTotalTime(S_atual);
            return S_atual;
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
    int idx = S_atual.vet_tripla[idx_task].task_index;
    for (int k = 0; k < p.vet_machine.size(); k++) {
        if (S_atual.vet_tripla[idx_task].type == 0 && S_atual.vet_tripla[idx_task].vm_id == p.vet_machine[k].id)
            continue;
        Tripla vizinho = S_atual.vet_tripla[idx_task];
        vizinho.type = 0;
        vizinho.vm_id = p.vet_machine[k].id;
        vizinho.vm_slowdown = p.vet_machine[k].slowdown;
        vizinho.vm_time_total = calculateVMTime(p, p.vet_tasks[idx], p.vet_machine[k]);
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

Solution Swap_MachinePair(Solution S, Problem p, float phi) {
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
            int idx_task_i = S_vizinho.vet_tripla[i].task_index;
            int idx_task_j = S_vizinho.vet_tripla[j].task_index;
            S_vizinho.vet_tripla[i].vm_time_total = calculateVMTime(p, p.vet_tasks[idx_task_i], p.vet_machine[S_vizinho.vet_tripla[i].vm_id]);
            S_vizinho.vet_tripla[i].vm_cost_total = p.vet_machine[S_vizinho.vet_tripla[i].vm_id].cost * p.vet_machine[S_vizinho.vet_tripla[i].vm_id].slowdown * S_vizinho.vet_tripla[i].vm_time_total;
            S_vizinho.vet_tripla[i].cost = normalizeCandidateCost(S_vizinho.vet_tripla[i], phi, p.max_fin_cost, p.max_runtime);
            S_vizinho.vet_tripla[j].vm_time_total = calculateVMTime(p, p.vet_tasks[idx_task_j], p.vet_machine[S_vizinho.vet_tripla[j].vm_id]);
            S_vizinho.vet_tripla[j].vm_cost_total = p.vet_machine[S_vizinho.vet_tripla[j].vm_id].cost * p.vet_machine[S_vizinho.vet_tripla[j].vm_id].slowdown * S_vizinho.vet_tripla[j].vm_time_total;
            S_vizinho.vet_tripla[j].cost = normalizeCandidateCost(S_vizinho.vet_tripla[j], phi, p.max_fin_cost, p.max_runtime);
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

Solution Swap_ConfigPair(Solution S, Problem p, float phi) {
    Solution S_atual = S;
    int n = S_atual.vet_tripla.size();
    for (int i = 0; i < n; i++) {
        if (S_atual.vet_tripla[i].type != 1) continue;
        for (int j = i + 1; j < n; j++) {
            if (S_atual.vet_tripla[j].type != 1) continue;
            // Só troca se as configs forem diferentes
            if (S_atual.vet_tripla[i].config_id == S_atual.vet_tripla[j].config_id) continue;
            Solution S_vizinho = S_atual;
            // Troca as configs das duas tasks
            int config_i = S_vizinho.vet_tripla[i].config_id;
            int config_j = S_vizinho.vet_tripla[j].config_id;
            S_vizinho.vet_tripla[i].config_id = config_j;
            S_vizinho.vet_tripla[j].config_id = config_i;
            int idx_task_i = S_vizinho.vet_tripla[i].task_index;
            int idx_task_j = S_vizinho.vet_tripla[j].task_index;
            Task_config* conf_i = NULL;
            Task_config* conf_j = NULL;
            for (int c = 0; c < p.vet_tasks[idx_task_i].vet_config.size(); c++) {
                if (p.vet_tasks[idx_task_i].vet_config[c].config_id == S_vizinho.vet_tripla[i].config_id) conf_i = &p.vet_tasks[idx_task_i].vet_config[c];
            }
            for (int c = 0; c < p.vet_tasks[idx_task_j].vet_config.size(); c++) {
                if (p.vet_tasks[idx_task_j].vet_config[c].config_id == S_vizinho.vet_tripla[j].config_id) conf_j = &p.vet_tasks[idx_task_j].vet_config[c];
            }
            if (!conf_i || !conf_j) continue;
            S_vizinho.vet_tripla[i].task_time_total = conf_i->task_time_total;
            S_vizinho.vet_tripla[i].task_p_config_cost = conf_i->task_p_config_cost;
            S_vizinho.vet_tripla[i].cost = normalizeCandidateCost(S_vizinho.vet_tripla[i], phi, p.max_fin_cost, p.max_runtime);
            S_vizinho.vet_tripla[j].task_time_total = conf_j->task_time_total;
            S_vizinho.vet_tripla[j].task_p_config_cost = conf_j->task_p_config_cost;
            S_vizinho.vet_tripla[j].cost = normalizeCandidateCost(S_vizinho.vet_tripla[j], phi, p.max_fin_cost, p.max_runtime);
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

// ===================== SWAP MACHINE CONFIG PAIR ===================== //
// Tenta trocar o tipo de execução entre uma task VM e uma task FX (função/configuração)
// Para cada par (VM, FX), troca VM pela config da FX e FX pela VM da VM
// Recalcula custos e tempos das duas tasks trocadas
// Retorna a primeira solução melhor encontrada
Solution SwapMachineConfig_Pair(Solution S, Problem p, float phi) {
    Solution S_atual = S;
    int n = S_atual.vet_tripla.size();
    for (int i = 0; i < n; i++) {
        // Só considera tasks rodando em VM
        if (S_atual.vet_tripla[i].type != 0) continue;
        for (int j = 0; j < n; j++) {
            // Só considera tasks rodando em Config
            if (S_atual.vet_tripla[j].type != 1) continue;
            // Troca VM da i pela Config do j e vice-versa
            Solution S_vizinho = S_atual;
            // Salva ids originais
            int vm_i = S_vizinho.vet_tripla[i].vm_id;
            double slowdown_i = S_vizinho.vet_tripla[i].vm_slowdown;
            int config_j = S_vizinho.vet_tripla[j].config_id;
            // Atualiza tipos e ids
            S_vizinho.vet_tripla[i].type = 1;
            S_vizinho.vet_tripla[i].config_id = config_j;
            S_vizinho.vet_tripla[j].type = 0;
            S_vizinho.vet_tripla[j].vm_id = vm_i;
            S_vizinho.vet_tripla[j].vm_slowdown = slowdown_i;
            // -------- Recalcula custos e tempos para i (agora FX) -------- //
            int idx_task_i = S_vizinho.vet_tripla[i].task_index;
            int conf_idx_i = -1;
            for (int c = 0; c < p.vet_tasks[idx_task_i].vet_config.size(); c++) {
                if (p.vet_tasks[idx_task_i].vet_config[c].config_id == config_j) {
                    conf_idx_i = c;
                    break;
                }
            }
            if (conf_idx_i == -1) continue;
            // Atualiza tempo e custo da configuração
            S_vizinho.vet_tripla[i].task_time_total = p.vet_tasks[idx_task_i].vet_config[conf_idx_i].task_time_total;
            S_vizinho.vet_tripla[i].task_p_config_cost = p.vet_tasks[idx_task_i].vet_config[conf_idx_i].task_p_config_cost;
            S_vizinho.vet_tripla[i].cost = normalizeCandidateCost(S_vizinho.vet_tripla[i], phi, p.max_fin_cost, p.max_runtime);
            // -------- Recalcula custos e tempos para j (agora VM) -------- //
            int idx_task_j = S_vizinho.vet_tripla[j].task_index;
            S_vizinho.vet_tripla[j].vm_time_total = calculateVMTime(p, p.vet_tasks[idx_task_j], p.vet_machine[vm_i]);
            S_vizinho.vet_tripla[j].vm_cost_total = p.vet_machine[vm_i].cost * p.vet_machine[vm_i].slowdown * S_vizinho.vet_tripla[j].vm_time_total;
            S_vizinho.vet_tripla[j].cost = normalizeCandidateCost(S_vizinho.vet_tripla[j], phi, p.max_fin_cost, p.max_runtime);
            // -------- Atualiza custos totais da solução -------- //
            S_vizinho.cost = calculateTotalCost(S_vizinho);
            S_vizinho.financial_cost = calculateFinancialCost(S_vizinho);
            S_vizinho.time = calculateTotalTime(S_vizinho);
            // Se encontrou solução melhor, retorna
            if (S_vizinho.cost < S_atual.cost) {
                return S_vizinho;
            }
        }
    }
    return S;
}

// ===================== SWAP CONFIG MACHINE PAIR ===================== //
// Tenta trocar o tipo de execução entre uma task FX (função/configuração) e uma task VM
// Para cada par (FX, VM), troca Config da FX pela VM do VM e vice-versa
// Recalcula custos e tempos das duas tasks trocadas
// Retorna a primeira solução melhor encontrada
Solution SwapConfigMachine_Pair(Solution S, Problem p, float phi) {
    Solution S_atual = S;
    int n = S_atual.vet_tripla.size();
    for (int i = 0; i < n; i++) {
        // Só considera tasks rodando em Config
        if (S_atual.vet_tripla[i].type != 1) continue;
        for (int j = 0; j < n; j++) {
            // Só considera tasks rodando em VM
            if (S_atual.vet_tripla[j].type != 0) continue;
            // Troca Config da i pela VM do j e vice-versa
            Solution S_vizinho = S_atual;
            // Salva ids originais
            int config_i = S_vizinho.vet_tripla[i].config_id;
            int vm_j = S_vizinho.vet_tripla[j].vm_id;
            double slowdown_j = S_vizinho.vet_tripla[j].vm_slowdown;
            // Atualiza tipos e ids
            S_vizinho.vet_tripla[i].type = 0;
            S_vizinho.vet_tripla[i].vm_id = vm_j;
            S_vizinho.vet_tripla[i].vm_slowdown = slowdown_j;
            S_vizinho.vet_tripla[j].type = 1;
            S_vizinho.vet_tripla[j].config_id = config_i;
            // -------- Recalcula custos e tempos para i (agora VM) -------- //
            int idx_task_i = S_vizinho.vet_tripla[i].task_index;
            for (int k = 0; k < p.vet_tasks.size(); k++) {
                if (p.vet_tasks[k].task_id == S_vizinho.vet_tripla[i].task_id) idx_task_i = k;
            }
            if (idx_task_i == -1) continue;
            S_vizinho.vet_tripla[i].vm_time_total = calculateVMTime(p, p.vet_tasks[idx_task_i], p.vet_machine[vm_j]);
            S_vizinho.vet_tripla[i].vm_cost_total = p.vet_machine[vm_j].cost * p.vet_machine[vm_j].slowdown * S_vizinho.vet_tripla[i].vm_time_total;
            S_vizinho.vet_tripla[i].cost = normalizeCandidateCost(S_vizinho.vet_tripla[i], phi, p.max_fin_cost, p.max_runtime);
            // -------- Recalcula custos e tempos para j (agora FX) -------- //
            int idx_task_j = S_vizinho.vet_tripla[j].task_index;
            int conf_idx_j = -1;
            if (idx_task_j != -1) {
                for (int c = 0; c < p.vet_tasks[idx_task_j].vet_config.size(); c++) {
                    if (p.vet_tasks[idx_task_j].vet_config[c].config_id == config_i) {
                        conf_idx_j = c;
                        break;
                    }
                }
            }
            if (conf_idx_j == -1) continue;
            S_vizinho.vet_tripla[j].task_time_total = p.vet_tasks[idx_task_j].vet_config[conf_idx_j].task_time_total;
            S_vizinho.vet_tripla[j].task_p_config_cost = p.vet_tasks[idx_task_j].vet_config[conf_idx_j].task_p_config_cost;
            S_vizinho.vet_tripla[j].cost = normalizeCandidateCost(S_vizinho.vet_tripla[j], phi, p.max_fin_cost, p.max_runtime);
            // -------- Atualiza custos totais da solução -------- //
            S_vizinho.cost = calculateTotalCost(S_vizinho);
            S_vizinho.financial_cost = calculateFinancialCost(S_vizinho);
            S_vizinho.time = calculateTotalTime(S_vizinho);
            // Se encontrou solução melhor, retorna
            if (S_vizinho.cost < S_atual.cost) {
                return S_vizinho;
            }
        }
    }
    return S;
}

void VND(Solution& solucao, Problem p, float phi, FILE* fs, const char* nome_instancia, double custo_medio, double tempo_medio) {
    Solution S_atual = solucao;
    Solution S_melhor = solucao;
    bool melhoria = true;

    while (melhoria) {
        melhoria = false;

        // 1. SwapConfig (2)
        S_atual = Swap_Config(S_melhor, p, phi);
        if (S_atual.cost < S_melhor.cost) {
            S_melhor = S_atual;
            melhoria = true;
            continue;
        }

        // 2. SwapMachine (1)
        S_atual = Swap_Machine(S_melhor, p, phi);
        if (S_atual.cost < S_melhor.cost) {
            S_melhor = S_atual;
            melhoria = true;
            continue;
        }

        // 3. SwapConfigToMachine (4)
        S_atual = Swap_ConfigToMachine(S_melhor, p, phi);
        if (S_atual.cost < S_melhor.cost) {
            S_melhor = S_atual;
            melhoria = true;
            continue;
        }

        // 4. SwapConfigMachine_Pair (8)
        S_atual = SwapConfigMachine_Pair(S_melhor, p, phi);
        if (S_atual.cost < S_melhor.cost) {
            S_melhor = S_atual;
            melhoria = true;
            continue;
        }

        // 5. SwapMachineConfig_Pair (7)
        S_atual = SwapMachineConfig_Pair(S_melhor, p, phi);
        if (S_atual.cost < S_melhor.cost) {
            S_melhor = S_atual;
            melhoria = true;
            continue;
        }

        // 6. SwapConfig_Pair (6)
        S_atual = Swap_ConfigPair(S_melhor, p, phi);
        if (S_atual.cost < S_melhor.cost) {
            S_melhor = S_atual;
            melhoria = true;
            continue;
        }

        // 7. SwapMachine_Pair (5)
        S_atual = Swap_MachinePair(S_melhor, p, phi);
        if (S_atual.cost < S_melhor.cost) {
            S_melhor = S_atual;
            melhoria = true;
            continue;
        }

        // 8. Swap_MachineToConfig (3)
        S_atual = Swap_MachineToConfig(S_melhor, p, phi);
        if (S_atual.cost < S_melhor.cost) {
            S_melhor = S_atual;
            melhoria = true;
            continue;
        }
    }

    // Atualiza a solução de entrada com a melhor encontrada
    solucao = S_melhor;

    // Registra no arquivo de saída
    fprintf(fs, "----------------------------VND FINAL RESULT----------------------------------------------\n");
    fprintf(fs, "%s\t %f\t %f\t %f\t %f\t %f\n", 
            nome_instancia, solucao.financial_cost, solucao.time, solucao.cost, custo_medio, tempo_medio);
}