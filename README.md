# Flexe: Heurística para Workflow Científico em Ambientes Híbridos

Este projeto implementa a heurística **Flexe** para otimização de workflows científicos em ambientes híbridos, onde cada tarefa pode ser alocada tanto em Máquinas Virtuais (VMs) quanto em Funções na Nuvem (serverless). Este é um conceito inovador que explora a flexibilidade de execução entre recursos tradicionais e computação serverless.

## Objetivo
Flexe busca alocar tarefas de um workflow científico em diferentes máquinas e configurações (VMs ou Funções), minimizando o custo financeiro e o tempo total de execução. O algoritmo utiliza técnicas de busca local e multistart para encontrar soluções eficientes em ambientes híbridos.

## Principais funcionalidades
- **Leitura de instâncias**: O programa lê arquivos de instância listados em `Instancias/sumario.txt`.
- **Criação automática de pastas de resultados**: Para cada execução, uma pasta é criada em `Resultados` com o padrão de data/hora e valor de phi.
- **Execução da heurística Flexe**: Para cada instância, são aplicadas heurísticas construtivas e buscas locais (Swap Machine, Swap Config, Swap MachineToConfig, Swap ConfigToMachine), permitindo alternância entre VMs e Funções na Nuvem.
- **Resultados organizados**: Para cada instância, são gerados arquivos de resultado geral e final, contendo os custos e tempos das soluções encontradas.

## Como executar
Compile o projeto e execute:
```
./seu_programa phi alpha repetitions
```
- `phi`: Peso para normalização dos custos.
- `alpha`: Parâmetro de aleatoriedade da heurística construtiva.
- `repetitions`: Número de repetições do multistart.

Os resultados serão salvos automaticamente em subpastas de `Resultados`.

## Estrutura do código
- `src/`: Código-fonte principal.
- `include/`: Cabeçalhos das funções e estruturas.
- `Instancias/`: Instâncias de teste e sumário.
- `Resultados/`: Resultados organizados por execução e instância.

## Sobre
Este repositório foi criado para armazenar o desenvolvimento da heurística Flexe durante a iniciação científica, focando em otimização de workflows científicos em ambientes híbridos (VMs e Funções na Nuvem).
