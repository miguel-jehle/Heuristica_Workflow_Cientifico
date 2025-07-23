#!/bin/bash

# Nova ordem dos parâmetros identificada nos logs:
ID_EXPERIMENTO="$1"    # Não utilizado (mas necessário para captura)
ID_CONFIG="$2"          # Não utilizado
SEED="$3"               # Não utilizado
INSTANCIA="$4"          # Ignorado
SWAP_CHOICE="$5"

ALPHA=0.3486
BEST=-999999999
for PHI in 0.05 0.50 0.95; do
    RESULTADO=$(cd .. && ./main "$PHI" "$ALPHA" 1 "$SWAP_CHOICE" 2>&1)
    # Para cada linha de resultado (uma por instância), processa separadamente
    while read -r NUMERICO; do
        # Verifica se a linha é um número válido
        if [[ "$NUMERICO" =~ ^-?[0-9]*\.?[0-9]+([eE][-+]?[0-9]+)?$ ]]; then
            # Usa awk para comparação de floats
            COMPARE=$(awk -v a="$NUMERICO" -v b="$BEST" 'BEGIN {print (a > b)}')
            if [[ "$COMPARE" -eq 1 ]]; then
                BEST=$NUMERICO
            fi
        fi
    done <<< "$RESULTADO"
done

if [ "$BEST" == "-999999999" ]; then
    echo "ERRO: Nenhum resultado válido!" >&2
    exit 1
fi

echo "$BEST"