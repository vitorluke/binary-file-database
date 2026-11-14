// NOME: Murilo Antonio de Oliveira Amatti. NroUSP: 17150041
// NOME: Vitor Quirino Machado. NroUSP: 16904644

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "../include/index.h"
#include "../include/fornecidas.h"
#include "../include/register_header.h"
#include "../include/select.h"

// Estrutura de cada registro do arquivo de índice primário (12 bytes)
typedef struct {
    int idPoPs;         // Chave primária (4 bytes)
    int64_t byteoffset; // Posição do registro no arquivo de dados (8 bytes)
} IndexEntry;

// Função de comparação para ordenar o vetor de índice de forma crescente por idPoPs
int compare_index(const void *a, const void *b)
{
    IndexEntry *regA = (IndexEntry *)a;
    IndexEntry *regB = (IndexEntry *)b;

    if (regA->idPoPs < regB->idPoPs) return -1;
    if (regA->idPoPs > regB->idPoPs) return 1;
    return 0;
}

// Funcionalidade [9]: Gera o arquivo binário de índice primário indexaPoPs
void create_index(char *arquivoEntrada, char *arquivoIndice)
{
    FILE *dados = fopen(arquivoEntrada, "rb");
    if (dados == NULL)
    {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    // Lê e valida o cabeçalho do arquivo de dados
    Header cabecalho;
    read_header(&cabecalho, dados);

    if (cabecalho.status == '0')
    {
        printf("Falha no processamento do arquivo.\n");
        fclose(dados);
        return;
    }

    FILE *idx = fopen(arquivoIndice, "wb");
    if (idx == NULL)
    {
        printf("Falha no processamento do arquivo.\n");
        fclose(dados);
        return;
    }

    // Escreve o cabeçalho inicial do índice (1 byte) com status '0'
    char status = '0';
    fwrite(&status, sizeof(char), 1, idx);

    // Vetor dinâmico para armazenar as chaves e byteoffsets
    IndexEntry *lista = NULL;
    int qtd = 0;

    Register registro;
    int64_t offsetAtual = ftell(dados); // Começa no byte 25

    // Percorre todos os registros do arquivo de dados
    while (read_register(&registro, dados))
    {
        if (registro.removido == '0')
        {
            lista = realloc(lista, (qtd + 1) * sizeof(IndexEntry));
            lista[qtd].idPoPs = registro.idPoPs;
            lista[qtd].byteoffset = offsetAtual;
            qtd++;
        }

        free_register(&registro);
        offsetAtual = ftell(dados); // Guarda o início do próximo registro
    }

    // Ordena os registros de índice de forma crescente pelo idPoPs
    if (qtd > 0)
    {
        qsort(lista, qtd, sizeof(IndexEntry), compare_index);
    }

    // Grava cada entrada do índice campo a campo (4 bytes + 8 bytes = 12 bytes)
    for (int i = 0; i < qtd; i++)
    {
        fwrite(&lista[i].idPoPs, sizeof(int), 1, idx);
        fwrite(&lista[i].byteoffset, sizeof(int64_t), 1, idx);
    }

    // Volta ao início do arquivo de índice e atualiza status para '1'
    status = '1';
    fseek(idx, 0, SEEK_SET);
    fwrite(&status, sizeof(char), 1, idx);

    free(lista);
    fclose(dados);
    fclose(idx);

    // Exibe o arquivo de índice gerado conforme especificação
    BinarioNaTela(arquivoIndice);
}