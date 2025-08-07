#include "utils.h"
#include "construtivo.h"  // Para funções de cálculo
#include "Base.h"
#include "busca_local.h"  // Para funções de busca local
#include <sys/resource.h>
#include <cstring>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

using namespace std;

void getCPUTime(double *seg_CPU_total, double *seg_sistema_total) {
    long seg_CPU, seg_sistema, mseg_CPU, mseg_sistema;
    struct rusage ptempo;

    getrusage(0,&ptempo);

    seg_CPU = ptempo.ru_utime.tv_sec;
    mseg_CPU = ptempo.ru_utime.tv_usec;
    seg_sistema = ptempo.ru_stime.tv_sec;
    mseg_sistema = ptempo.ru_stime.tv_usec;

    *seg_CPU_total     = (seg_CPU + 0.000001 * mseg_CPU);
    *seg_sistema_total = (seg_sistema + 0.000001 * mseg_sistema);
}

Problem readData(const char* arquivo) {
    FILE * fp = fopen(arquivo, "r");
    if(!fp) exit(1);
    char temp[200], c, temp2[200], temp3[200];
    int n;
    Problem p;

    ////////////////////////////////////////////////////////

    fscanf(fp, "%199[^\n]", temp); // Lê o primeiro header
    fscanf(fp, "%d %d %d %d %d %d %lf %lf ", &p.tasks, &p.config, &p.data, &p.vms, &p.buckets, &p.bucket_ranges, &p.max_runtime, &p.max_fin_cost);

    ////////////////////////////////////////////////////////

    fscanf(fp, "%199[^\n]", temp); // Lê o segundo header
    Tasks t_aux; 
    
    for(int i = 0; i < p.tasks; i++) {
        fscanf(fp, "%d %d %d %lf %d ", &t_aux.task_id, &t_aux.activity_id, &t_aux.type, &t_aux.vm_cpu_time, &t_aux.n_input);
        
        fscanf(fp, "%c", &c); // lê o "["
        for(int j = 0; j < t_aux.n_input; j++) {
            fscanf(fp, "%d", &n); // lê o número
            t_aux.id_inputs.push_back(n);
            fscanf(fp, "%c", &c); // lê o "," ou "]"
        }

        fscanf(fp, "%d ", &t_aux.n_output);
        fscanf(fp, "%c", &c); // lê o "["
        for(int j = 0; j < t_aux.n_output; j++) {
            fscanf(fp, "%d", &n); // lê o número
            t_aux.id_outputs.push_back(n);
            fscanf(fp, "%c ", &c); // lê o "," ou "]"
        }

        p.vet_tasks.push_back(t_aux);
        t_aux.id_inputs.clear();
        t_aux.id_outputs.clear();
    }

    ///////////////////////////////////////////////////////////////////////

    fscanf(fp, "%199[^\n]", temp); // Lê o terceiro header

    for(int i = 0; i < p.data; i++) {
        Data d_aux;
        fscanf(fp, "%d %d %s %s %d %d %s ", &d_aux.data_id, &d_aux.data_size, temp2, temp3, &d_aux.is_static, &d_aux.n_source_devices, d_aux.device_id);
        if(!strstr(temp2, "None")) d_aux.read_time = atof(temp2);
        else d_aux.read_time = 0.0;
        if(!strstr(temp3, "None")) d_aux.write_time = atof(temp3);
        else d_aux.write_time = 0.0;
        p.vet_data.push_back(d_aux);
    }

    ///////////////////////////////////////////////////////////////////////

    fscanf(fp, "%199[^\n]", temp); // Lê o quarto header (novo formato de VMs)
    Machine m_aux;
    for(int i = 0; i < p.vms; i++) {
        fscanf(fp, "%d %lf %lf %d %d ", &m_aux.id, &m_aux.slowdown, &m_aux.cost, &m_aux.storage, &m_aux.bandwith);
        p.vet_machine.push_back(m_aux);
    }

    ///////////////////////////////////////////////////////////////////////

    fscanf(fp, "%199[^\n]", temp); // Lê o quinto header (task configurations)
    for(int i = 0; i < p.tasks; i++) {
        for(int j = 0; j < p.config; j++) {
            Task_config tc_aux;
            fscanf(fp, "%d %d %d %Lf %lf %lf %lf %lf %lf %d ",
                  &p.vet_tasks[i].task_id, &p.vet_tasks[i].activity_id, &tc_aux.config_id,
                  &tc_aux.task_p_config_cost, &tc_aux.task_time_total, &tc_aux.task_time_init,
                  &tc_aux.task_time_cpu, &tc_aux.task_time_read, &tc_aux.task_time_write,
                  &tc_aux.task_count);
            p.vet_tasks[i].vet_config.push_back(tc_aux);
        }
    }

    ///////////////////////////////////////////////////////////////////////

    fscanf(fp, "%199[^\n]", temp); // Lê o sexto header (bucket ranges)
    Bucket b_aux;
    for(int i = 0; i < p.bucket_ranges; i++) {
        fscanf(fp, "%d %lf %lf %f", &b_aux.bucket_id, &b_aux.lim_inf, &b_aux.lim_sup, &b_aux.cost);
        p.vet_buckets.push_back(b_aux);
    } 

    fclose(fp);
    return p;
}

void printProblem(Problem p) {
    printf(" #<#tasks> <#config> <#data> <#vms> <#buckets> <#bucket_ranges> <max_running_time> <max_financial_cost>\n");
    printf(" %d     %d     %d     %d     %d     %d     %lf    %lf\n\n", p.tasks,p.config, p.data, p.vms, p.buckets, p.bucket_ranges, p.max_runtime, p.max_fin_cost);

    printf(" #<task_id> <activity_id> <task_type__0-VM__1-FX> <vm_cpu_time> <n_input> [<id_input>...] <n_output> [<id_output>...]\n");
    for(int i = 0; i < p.tasks; i++) {
        printf(" %d     %d     %d     %lf     %d     [", p.vet_tasks[i].task_id,p.vet_tasks[i].activity_id,p.vet_tasks[i].type,p.vet_tasks[i].vm_cpu_time,p.vet_tasks[i].n_input);
        for(int j = 0; j < p.vet_tasks[i].n_input; j++) printf(" %d",p.vet_tasks[i].id_inputs[j]);
        printf(" ]     %d     [",p.vet_tasks[i].n_output);
        for(int j = 0; j < p.vet_tasks[i].n_output; j++) printf(" %d",p.vet_tasks[i].id_outputs[j]);
        printf(" ]");
        printf("\n");
    }
    printf("\n");

    printf(" #<data_id> <data_size> <read_time_avg> <write_time_avg> <is_static> <n_source_devices> [<device_id>...]\n");
    for(int i = 0; i < p.data; i++) {
        printf(" %d     %d     %lf     %lf     %d     %d     %s\n", p.vet_data[i].data_id, p.vet_data[i].data_size, p.vet_data[i].read_time,p.vet_data[i].write_time, p.vet_data[i].is_static, p.vet_data[i].n_source_devices, p.vet_data[i].device_id);
    }
    printf("\n");

    // Updated VM section to match new format
    printf(" #<vm_id> <cpu_slowdown> <cost_per_second> <storage_mb> <bandwidth_mbps>\n");
    for(int i = 0; i < p.vms; i++) {
        printf(" %d     %.5lf     %.10lf     %d     %d\n", 
              p.vet_machine[i].id, 
              p.vet_machine[i].slowdown, 
              p.vet_machine[i].cost,
              p.vet_machine[i].storage,
              p.vet_machine[i].bandwith);
    }
    printf("\n");

    printf(" #<task_id> <activity_id> <config_id> <task_p_config_cost> <task_time_total> <task_time_init> <task_time_cpu> <task_time_read> <task_time_write> <task_count>\n");
    for(int i = 0; i < p.tasks; i++) {
        for(int j = 0; j < p.config; j++) {
            printf(" %d     %d     %d   %Lf  %f     %f     %f     %f     %f     %d\n", 
                  p.vet_tasks[i].task_id, 
                  p.vet_tasks[i].activity_id, 
                  p.vet_tasks[i].vet_config[j].config_id,
                  p.vet_tasks[i].vet_config[j].task_p_config_cost,
                  p.vet_tasks[i].vet_config[j].task_time_total, 
                  p.vet_tasks[i].vet_config[j].task_time_init, 
                  p.vet_tasks[i].vet_config[j].task_time_cpu, 
                  p.vet_tasks[i].vet_config[j].task_time_read, 
                  p.vet_tasks[i].vet_config[j].task_time_write, 
                  p.vet_tasks[i].vet_config[j].task_count);
        }
    }
    printf("\n");

    printf(" #<bucket_range_id> <size1_gb> <size2_gb> <cost_per_gb>\n");
    for(int i = 0; i < p.bucket_ranges; i++) {
        printf(" %d     %.0f     %.0f    %f\n", p.vet_buckets[i].bucket_id,p.vet_buckets[i].lim_inf,p.vet_buckets[i].lim_sup, p.vet_buckets[i].cost);
    }

    return;
}

void writeSolution(Solution S, FILE * fl){
fprintf(fl, "<task_id>\t <FX/VM id>\t <Custo_fin>\t <Tempo_workflow>\t <Custo_normalizado>\n");
    for(int i = 0; i < S.vet_tripla.size(); i++){
        if(S.vet_tripla[i].type == 0){
            fprintf(fl, "%d\t %d\t %.9f\t %.9f\t %.9f  VM\n", S.vet_tripla[i].task_id, S.vet_tripla[i].vm_id, S.vet_tripla[i].vm_cost_total, S.vet_tripla[i].vm_time_total, S.vet_tripla[i].cost);
        }
        else{
            fprintf(fl, "%d\t %d\t %.9f\t %.9f\t %.9f  FX\n", S.vet_tripla[i].task_id, S.vet_tripla[i].config_id, S.vet_tripla[i].task_p_config_cost, S.vet_tripla[i].task_time_total, S.vet_tripla[i].cost);
        }
    }
    return;
}

void setupOutputFiles(const char* caminho_base, char* caminho_geral, char* caminho_final) {
    // Construir caminho para res_geral.txt
    strcpy(caminho_geral, caminho_base);
    strcat(caminho_geral, "/res_geral.txt");

    // Construir caminho para res_final.txt
    strcpy(caminho_final, caminho_base);
    strcat(caminho_final, "/res_final.txt");

    // Criar/limpar arquivo res_final.txt
    FILE* fs = fopen(caminho_final, "w");
    if (!fs) {
        cerr << "Erro ao criar arquivo: " << caminho_final << endl;
        exit(1);
    }
    fprintf(fs, "<Instância> \t <Custo_Financeiro_Melhor> \t <Tempo_Melhor> \t <Custo_Normalizado_Melhor> \t <Custo_Normalizado_Médio> \t <Tempo_CPU_Médio>\n \n");
    fclose(fs);

    // Criar/limpar arquivo res_geral.txt
    FILE* fl = fopen(caminho_geral, "w");
    if (!fl) {
        cerr << "Erro ao criar arquivo: " << caminho_geral << endl;
        exit(1);
    }
    fprintf(fl, "<Instância> \t <Semente> \t <Melhor_Custo> \t <Tempo_CPU>\t <Custo_Financeiro> \t <Tempo_Workflow>\n");
    fclose(fl);

    return;
}

Solution GRASP_VND(Problem p, float alpha, float phi, int repeticoes, int seed, double& tempo_exec) {
    double inicio_cpu, inicio_sistema, fim_cpu, fim_sistema;
    Solution melhor_sol;
    float melhor_custo = __FLT_MAX__;

    getCPUTime(&inicio_cpu, &inicio_sistema);

    for (int i = 0; i < repeticoes; i++) {
        Solution sol = constructiveHeuristic(alpha, phi, &seed, p);
        VND(melhor_sol, p, phi);
        if (sol.cost < melhor_custo) {
            melhor_custo = sol.cost;
            melhor_sol = sol;
        }
    }

    getCPUTime(&fim_cpu, &fim_sistema);
    
    tempo_exec = fim_cpu - inicio_cpu;
    
    return melhor_sol;
}

void processInstance(const char* nome_instancia, const char* caminho_geral, const char* caminho_final, float alpha, float phi, int repeticoes) {
    char caminho_completo[200];
    strcpy(caminho_completo, "Instancias/");
    strcat(caminho_completo, nome_instancia);

    Problem p = readData(caminho_completo);
    p.max_fin_cost = calculateMaxFinancialCost(p);
    p.max_runtime = calculateMaxRuntime(p);
    //printProblem(p);

    double custo_medio = 0.0;
    double tempo_medio = 0.0;
    Solution melhor_solucao;
    double melhor_custo = __FLT_MAX__;
    double custo_financeiro_melhor = 0.0;
    double tempo_melhor = 0.0;

    // Loop principal de sementes (originalmente 1 iteração)
    for (int i = 0; i < 10; i++) {
        int seed = i + 1;
        double tempo_exec;
        Solution sol = GRASP_VND(p, alpha, phi, repeticoes, seed, tempo_exec);
        
        custo_medio += sol.cost;
        tempo_medio += tempo_exec;

        if (sol.cost < melhor_custo) {
            melhor_custo = sol.cost;
            custo_financeiro_melhor = sol.financial_cost;
            tempo_melhor = sol.time;
            melhor_solucao = sol;
        }

        // Escrever resultados gerais
        FILE* fl = fopen(caminho_geral, "a");
        fprintf(fl, "%s\t %d\t %.15f\t %f\t %f\t %f\n", 
                nome_instancia, seed, sol.cost, tempo_exec, sol.financial_cost, sol.time);
        writeSolution(sol, fl);
        fprintf(fl, "========================================================================\n");
        fclose(fl);
    }

    custo_medio /= 1.0;
    tempo_medio /= 1.0;

    // Escrever resultados finais
    FILE* fs = fopen(caminho_final, "a");
    fprintf(fs, "%s\t %f\t %f\t %f\t %f\t %f\n", 
            nome_instancia, custo_financeiro_melhor, tempo_melhor, melhor_custo, custo_medio, tempo_medio);
    
    fprintf(fs, "====================================================================\n\n\n");
    fclose(fs);
}

int setupRunFolders(float phi, char caminhos_instancias[][256], int* n_instancias) {
    FILE* fsum = fopen("Instancias/teste.txt", "r");
    if (!fsum) return 0;
    char nome_inst[128];
    int count = 0;
    while (fscanf(fsum, "%127s", nome_inst) == 1) {
        strcpy(caminhos_instancias[count], nome_inst);
        count++;
    }
    fclose(fsum);
    *n_instancias = count;
    // Data/hora
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    char pasta_base[256];
    snprintf(pasta_base, sizeof(pasta_base), "Resultados/A%d_M%d_D%d_H%d_m%d_P%.2f",t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, phi);
    mkdir("Resultados", 0777);
    mkdir(pasta_base, 0777);
    // Cria arquivo de contexto vazio
    char caminho_contexto[300];
    snprintf(caminho_contexto, sizeof(caminho_contexto), "%s/contexto_execucao.txt", pasta_base);
    FILE* fcontext = fopen(caminho_contexto, "w");
    if (fcontext) fclose(fcontext);
    for (int i = 0; i < count; i++) {
        char pasta_inst[300];
        snprintf(pasta_inst, sizeof(pasta_inst), "%s/%s", pasta_base, caminhos_instancias[i]);
        mkdir(pasta_inst, 0777);
        strcpy(caminhos_instancias[i], pasta_inst);
    }
    return 1;
}

int readInstanceNames(const char* filename, char instance_names[][128], int max_instances) {
    FILE* f = fopen(filename, "r");
    if (!f) return -1;
    int count = 0;
    while (count < max_instances && fscanf(f, "%127s", instance_names[count]) == 1) {
        count++;
    }
    fclose(f);
    return count;
}