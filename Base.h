#pragma once

#include <vector>

using namespace std;

struct Bucket{
    int bucket_id;
    double lim_inf;
    double lim_sup;
    float cost;
};

struct Task_config{
    int config_id;
    double task_time_total; 
    double task_time_cpu;
    double task_time_read;
    double task_time_write;
    double task_time_init;
    long double task_p_config_cost; 
    int task_count;
};

struct Tasks{
    int task_id = -1;
    int type = -1; //0 - VM or 1 - FX
    int activity_id;
    int n_input;
    vector<int> id_inputs;
    int n_output;
    vector<int> id_outputs;
    double vm_cpu_time;
    vector<Task_config> vet_config;
    bool escolhida = false;
};

struct Data{ 
    int data_id = -1;
    int data_size;
    double read_time = -1; //Inicializado com -1 pois existe a chance de ser "None" na entrada
    double write_time = -1; //Inicializado com -1 pois existe a chance de ser "None" na entrada
    int is_static;
    int is_available = 0;
    int n_source_devices; //0 if is dynamic
    char device_id[50];
    bool disponivel = false;
    //vector<int> device_id; //Colocarei como char* pois na instância inicial, sempre é o denethor_bucket, mas fica aqui a possível mudança para um vetor de id

};

struct Machine{ 
    int id; 
    double slowdown;
    int storage;
    float cost;
    int bandwith;
    double temp_rest;
};

struct Problem{
    int tasks;
    int config;
    int data;
    int vms;
    int buckets;
    double max_runtime;
    double max_fin_cost;
    int bucket_ranges;
    vector<Tasks> vet_tasks;
    vector<Data> vet_data;
    vector<Machine> vet_machine;
    vector<Bucket> vet_buckets;
    
};

struct Tripla{
    //Infos gerais
    int task_id;
    double final_cost = 0; //Campo que faz a normalização do custo total da tarefa com relação ao custo total das tarefas e o tempo.

    //Infos para FX
    int  config_id;

    //Infos para VM
    int vm_id;
    int type; //0 - VM or 1 - FX

    //Infos auxiliares para otimização
    double task_p_config_cost;
    double task_time_total;
    double vm_cpu_time;
    double vm_cost_total;
    double vm_time_total;
    double vm_slowdown;
};

struct Solution{
    vector<Tripla> vet_tripla;
    double cost;
    double time;
    double cost_fin;
};