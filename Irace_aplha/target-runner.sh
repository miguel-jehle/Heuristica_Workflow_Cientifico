#!/bin/bash

# Nova ordem dos parâmetros identificada nos logs:
ID_EXPERIMENTO="$1"    # Não utilizado (mas necessário para captura)
ID_CONFIG="$2"          # Não utilizado
SEED="$3"               # Não utilizado
INSTANCIA="$4"          # Nome da instância (CORRIGIDO!)
ALPHA="$5"              # Parâmetro otimizado (CORRIGIDO!)

# Verificação crítica de caminho
if [ ! -f "$INSTANCIA" ]; then
    echo "ERRO: Instância $INSTANCIA não encontrada!" >&2
    exit 1
fi

# Execução com parâmetros na ordem CORRETA identificada manualmente:
RESULTADO=$(./main "$INSTANCIA" 1 0.05 "$ALPHA" 200 2>&1)

# Filtragem robusta (considera notação científica e negativos):
NUMERICO=$(echo "$RESULTADO" | grep -oE '[-+]?[0-9]*\.?[0-9]+([eE][-+]?[0-9]+)?')

# Validação rigorosa:
if [ -z "$NUMERICO" ]; then
    echo "ERRO: Saída inválida! Entrada: $INSTANCIA Alpha: $ALPHA" >&2
    echo "Saída bruta: $RESULTADO" >&2
    exit 1
fi

echo "$NUMERICO"