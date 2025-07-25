library(irace)

# Lê os parâmetros a serem otimizados
parameters <- readParameters("parameters.txt")

# Lista de instâncias reais
instancias <- c(
  "Irace_VND/Entradas_Irace/005_T16_C5_D49_VM3.txt",
  "Irace_VND/Entradas_Irace/010_T31_C5_D97_VM3.txt",
  "Irace_VND/Entradas_Irace/030_T91_C5_D260_VM3.txt",
  "Irace_VND/Entradas_Irace/040_T121_C5_D284_VM3.txt",
  "Irace_VND/Entradas_Irace/050_T151_C5_D347_VM3.txt"
)

# Configura o cenário do irace
scenario <- list(
  targetRunner = "target-runner.sh",
  maxExperiments = 500,
  logFile = "Resultados/irace_output.Rdata",  # Arquivo binário obrigatório
  parameters = parameters,
  instances = instancias,
  parallel = 5,
  loadBalancing = TRUE,
  deterministic = FALSE,
  firstTest = 5,
  minNbSurvival = 1,     # Permite maior eliminação
  eachTest = 5           # Testa mais configurações por vez
)

# Redireciona toda saída do console para arquivo texto
sink("Resultados/irace_terminal_log.txt", split = TRUE)

# Roda o irace
irace(scenario)

# Restaura saída padrão
sink()

# Mostra o top 8 melhores swaps após a execução
# topN <- 8
# load("Resultados/irace_output.Rdata")
# cat("\nTop", topN, "configurações (swap_choice) com menor custo médio:\n")
# ordem <- order(iraceResults$allConfigurations$cost)
# print(iraceResults$allConfigurations[ordem, ][1:topN, ])