library(irace)

# Lê os parâmetros a serem otimizados
parameters <- readParameters("parameters.txt")

# Configura o cenário do irace
scenario <- list(
  targetRunner = "target-runner.sh",
  maxExperiments = 1000,
  logFile = "Resultados/irace_output.Rdata",  # Arquivo binário obrigatório
  parameters = parameters,
  instances = c("dummy")
)

# Redireciona toda saída do console para arquivo texto
sink("Resultados/irace_terminal_log.txt", split = TRUE)

# Roda o irace
irace(scenario)

# Restaura saída padrão
sink()

# Mostra o top 8 melhores swaps após a execução
topN <- 8
load("Resultados/irace_output.Rdata")
cat("\nTop", topN, "configurações (swap_choice) com menor custo médio:\n")
ordem <- order(iraceResults$allConfigurations$cost)
print(iraceResults$allConfigurations[ordem, ][1:topN, ])