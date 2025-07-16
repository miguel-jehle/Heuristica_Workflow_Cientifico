#include "utils.h"
#include "construtivo.h"
#include "busca_local.h"
#include "Base.h"
#include <cstring>
#include <iostream>

using namespace std;

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