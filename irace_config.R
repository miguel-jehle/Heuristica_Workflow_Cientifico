library(irace)

# Lê os parâmetros a serem otimizados
parameters <- readParameters("parameters.txt")

# Configura o cenário do irace
scenario <- list(
  targetRunner = "target-runner.sh",
  instances = c("Entradas_Irace/005_T16_C5_D49_VM3.txt", "Entradas_Irace/010_T31_C5_D97_VM3.txt", "Entradas_Irace/030_T91_C5_D260_VM3.txt", "Entradas_Irace/040_T121_C5_D284_VM3.txt", "Entradas_Irace/050_T151_C5_D347_VM3.txt"),
  maxExperiments = 1000,
  logFile = "Resultados/irace_output.Rdata",  # Arquivo binário obrigatório
  parameters = parameters
)

# Redireciona toda saída do console para arquivo texto
sink("Resultados/irace_terminal_log.txt", split = TRUE)

# Roda o irace
irace(scenario)

# Restaura saída padrão
sink()