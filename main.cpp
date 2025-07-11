#include "busca_local.cpp"
#include "Base.h"


void Tempo_CPU_Sistema(double *seg_CPU_total, double *seg_sistema_total)
{
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

void printa_dados(Problem p) {
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

Problem le_dados(const char* arquivo) {
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

void escreveSolucao(Solution S, FILE * fl){
fprintf(fl, "<task_id>\t <FX/VM id>\t <Custo_fin>\t <Tempo_workflow>\t <Custo_normalizado>\n");
    for(int i = 0; i < S.vet_tripla.size(); i++){
        if(S.vet_tripla[i].type == 0){
            fprintf(fl, "%d\t %d\t %.9f\t %.9f\t %.9f  VM\n", S.vet_tripla[i].task_id, S.vet_tripla[i].vm_id, S.vet_tripla[i].vm_cost_total, S.vet_tripla[i].vm_time_total, S.vet_tripla[i].final_cost);
        }
        else{
            fprintf(fl, "%d\t %d\t %.9f\t %.9f\t %.9f  FX\n", S.vet_tripla[i].task_id, S.vet_tripla[i].config_id, S.vet_tripla[i].task_p_config_cost, S.vet_tripla[i].task_time_total, S.vet_tripla[i].final_cost);
        }
    }
    return;
}

int main(int argc, char** argv){// caminho_resp phi alpha repeticoes
    double s_CPU_inicial = 0.0, s_total_inicial = 0.0;
    double s_CPU_final = 0.0, s_total_final = 0.0;

    float phi = atof(argv[2]);
    float alpha = atof(argv[3]);
    int repeticoes = atoi(argv[4]);
    int seed;

    char caminho_resp_geral[200];
    strcpy(caminho_resp_geral, argv[1]);
    strcat(caminho_resp_geral, "/res_geral.txt");

    char caminho_resp_final[200];
    strcpy(caminho_resp_final, argv[1]);
    strcat(caminho_resp_final, "/res_final.txt");

    FILE * fs = fopen(caminho_resp_final, "w");
    if(!fs) exit(1);
    fprintf(fs, "<Instância> \t <Custo_Financeiro_Melhor> \t <Tempo_Melhor> \t <Custo_Normalizado_Melhor> \t <Custo_Normalizado_Médio> \t <Tempo_CPU_Médio>\n \n");
    fclose(fs);

    FILE * fl = fopen(caminho_resp_geral, "w");
    if(!fl) exit(1);
    fprintf(fl, "<Instância> \t <Semente> \t <Melhor_Custo> \t <Tempo_CPU>\t <Custo_Financeiro> \t <Tempo_Workflow>\n");
    fclose(fl);

    FILE * fp = fopen("Instancias/teste.txt", "r");
    if(!fp) exit(1);
    char arquivo[200];

    while(!feof(fp)){
        fscanf(fp,"%s", arquivo);
        char caminho[200];
        strcpy(caminho, "Instancias/");
        strcat(caminho, arquivo);

        Problem p;
        p = le_dados(caminho);
        p.max_fin_cost = calculaMaxFinCost(p);
        p.max_runtime = calculaMaxRuntime(p);
        printa_dados(p);
        
        double custo_médio = 0.0;
        double tempo_CPU_médio = 0.0;


        Solution S_melhor;
        double custo_financeiro_melhor = 0.0;
        double tempo_melhor = 0.0;
        double melhor_custo = __FLT_MAX__;

        for(int i = 0; i < 1; i++){

            Solution S, otima;
            float cost_otima = __FLT_MAX__;
            seed = i+1;

            Tempo_CPU_Sistema(&s_CPU_inicial, &s_total_inicial);

            for(int i = 0; i < repeticoes; i++){ //MULTISTART
                S = Construtivo(alpha, phi, &seed ,p);
                if(S.cost < cost_otima){
                    cost_otima = S.cost;
                    otima = S;
                }
            }

            Tempo_CPU_Sistema(&s_CPU_final, &s_total_final);
            
            custo_médio += cost_otima;
            tempo_CPU_médio += s_CPU_final - s_CPU_inicial;

            if(cost_otima < melhor_custo) {
                melhor_custo = cost_otima;
                custo_financeiro_melhor = otima.cost_fin;
                tempo_melhor = otima.time;
                S_melhor = otima;
            }

            FILE * fl = fopen(caminho_resp_geral, "a");
            if(!fl) exit(1);
            fprintf(fl, "%s\t %d\t %.15f\t %f\t %f\t %f\n", arquivo, i+1, cost_otima, s_CPU_final - s_CPU_inicial, otima.cost_fin, otima.time);
            escreveSolucao(otima, fl);
            fprintf(fl, "========================================================================\n");
            fclose(fl);
        }

        custo_médio /= 10.0;
        tempo_CPU_médio /= 10.0;

        FILE * fs = fopen(caminho_resp_final, "a");
        if(!fs) exit(1);
        fprintf(fs, "%s\t %f\t %f\t %f\t %f\t %f\n", arquivo, custo_financeiro_melhor, tempo_melhor, melhor_custo, custo_médio, tempo_CPU_médio);
        fclose(fs);

        Solution S_bl = S_melhor;

        do{
            S_bl = Swap_Machine(S_bl, p, phi);
            if(S_bl.cost < S_melhor.cost){
                S_melhor = S_bl;
            }
            else break;
        }
        while(1);

        fs = fopen(caminho_resp_final, "a");
        if(!fs) exit(1);
        fprintf(fs, "----------------------------SWAP MACHINE----------------------------------------------\n");
        fprintf(fs, "%s\t %f\t %f\t %f\t %f\t %f\n", arquivo, S_melhor.cost_fin, S_melhor.time, S_melhor.cost, custo_médio, tempo_CPU_médio);
        fclose(fs);

        do{
            S_bl = Swap_Config(S_bl, p, phi);
            if(S_bl.cost < S_melhor.cost){
                S_melhor = S_bl;
            }
            else break;
        }
        while(1);

        fs = fopen(caminho_resp_final, "a");
        if(!fs) exit(1);
        fprintf(fs, "----------------------------SWAP CONFIG----------------------------------------------\n");
        fprintf(fs, "%s\t %f\t %f\t %f\t %f\t %f\n", arquivo, S_melhor.cost_fin, S_melhor.time, S_melhor.cost, custo_médio, tempo_CPU_médio);
        fclose(fs);

        do{
            S_bl = Swap_MachineToConfig(S_bl, p, phi);
            if(S_bl.cost < S_melhor.cost){
                S_melhor = S_bl;
            }
            else break;
        }
        while(1);

        fs = fopen(caminho_resp_final, "a");
        if(!fs) exit(1);
        fprintf(fs, "----------------------------SWAP MACHINE TO CONFIG----------------------------------------------\n");
        fprintf(fs, "%s\t %f\t %f\t %f\t %f\t %f\n", arquivo, S_melhor.cost_fin, S_melhor.time, S_melhor.cost, custo_médio, tempo_CPU_médio);
        fclose(fs);
        
        do{
            S_bl = Swap_ConfigToMachine(S_bl, p, phi);
            if(S_bl.cost < S_melhor.cost){
                S_melhor = S_bl;
            }
            else break;
        }
        while(1);

        fs = fopen(caminho_resp_final, "a");
        if(!fs) exit(1);
        fprintf(fs, "----------------------------SWAP CONFIG TO MACHINE----------------------------------------------\n");
        fprintf(fs, "%s\t %f\t %f\t %f\t %f\t %f\n", arquivo, S_melhor.cost_fin, S_melhor.time, S_melhor.cost, custo_médio, tempo_CPU_médio);
        fprintf(fs, "====================================================================\n \n \n");
        fclose(fs);
    }
    fclose(fp);
    return 0;
}




// ===========================================================================================================================================

// int main(int argc, char** argv){
//     double s_CPU_inicial = 0.0, s_total_inicial = 0.0;
//     double s_CPU_final = 0.0, s_total_final = 0.0;

//     int seed = atoi(argv[2]);

//     Problem p;
//     p = le_dados(argv[1]);
//     //printa_dados(p);

//     Solution S, otima;
//     float cost_otima = __FLT_MAX__;

//     Tempo_CPU_Sistema(&s_CPU_inicial, &s_total_inicial);

//     for(int i = 0; i < atoi(argv[5]); i++){ //MULTISTART
//         S = Construtivo(atof(argv[4]), atof(argv[3]), &seed ,p);
//         if(S.cost < cost_otima){
//             cost_otima = S.cost;
//             otima = S;
//         }
//     }

//     Tempo_CPU_Sistema(&s_CPU_final, &s_total_final);

//     printf ("Tempo de CPU total = %.9f\n", s_CPU_final - s_CPU_inicial);
//     printf ("Tempo total =  %.9f\n", s_total_final-s_total_inicial);
//     printaSolucao(otima);
//     return 0;
// }

// ===========================================================================================================================================