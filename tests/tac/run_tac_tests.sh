#!/usr/bin/env bash
#
# Compara o TAC gerado pelo compilador com os arquivos de saida esperada (golden files).
#
# Uso: run_tac_tests.sh <caminho-do-binario-compilador>
#
# Para cada arquivo casos/*.cmm, roda o compilador em modo --tac, captura a saida e
# compara, via diff, com o esperado/<nome>.tac.expected correspondente.
# Sai com codigo 0 se todos baterem; != 0 (imprimindo o diff) na primeira divergencia.

set -u

COMPILADOR="${1:-}"
if [ -z "$COMPILADOR" ]; then
    echo "Uso: $0 <caminho-do-binario-compilador>" >&2
    exit 2
fi
if [ ! -x "$COMPILADOR" ]; then
    echo "Erro: binario do compilador nao encontrado/executavel: $COMPILADOR" >&2
    exit 2
fi

# Roda a partir do diretorio do proprio script (casos/ e esperado/ sao relativos a ele).
cd "$(dirname "$0")" || exit 2

CASOS_DIR="casos"
ESPERADO_DIR="esperado"

falhas=0
total=0

for caso in "$CASOS_DIR"/*.cmm; do
    [ -e "$caso" ] || continue
    total=$((total + 1))
    nome="$(basename "$caso" .cmm)"
    esperado="$ESPERADO_DIR/$nome.tac.expected"

    if [ ! -f "$esperado" ]; then
        echo "[FALHA] $nome: arquivo esperado ausente ($esperado)" >&2
        falhas=$((falhas + 1))
        continue
    fi

    obtido="$(mktemp)"
    "$COMPILADOR" --tac "$caso" -o "$obtido" >/dev/null 2>&1

    if diff -u "$esperado" "$obtido" > /tmp/tac_diff.$$ 2>&1; then
        echo "[OK]    $nome"
    else
        echo "[FALHA] $nome: TAC gerado difere do esperado"
        cat /tmp/tac_diff.$$
        falhas=$((falhas + 1))
    fi
    rm -f "$obtido" /tmp/tac_diff.$$
done

echo "-----------------------------------------"
echo "Casos: $total | Falhas: $falhas"

[ "$falhas" -eq 0 ]
