// NOME: Murilo Antonio de Oliveira Amatti. NroUSP: 17150041
// NOME: Vitor Quirino Machado. NroUSP: 16904644

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "../include/table.h"
#include "../include/fornecidas.h"
#include "../include/register_header.h"

// Escreve o cabeçalho de 25 bytes no início do arquivo binário
void write_header(Header *cab, FILE *bin)
{
    fseek(bin, 0, SEEK_SET);
    fwrite(&cab->status, sizeof(char), 1, bin);
    fwrite(&cab->topoLista, sizeof(int64_t), 1, bin);
    fwrite(&cab->proxByteOffset, sizeof(int64_t), 1, bin);
    fwrite(&cab->nroRegRem, sizeof(int), 1, bin);
    fwrite(&cab->nroPoPs, sizeof(int), 1, bin);
}

// Funcionalidade [8]: Lê CSV e gera arquivo binário
void create_table(char *arquivoEntrada, char *arquivoSaida)
{
    FILE *csv = fopen(arquivoEntrada, "r");
    FILE *bin = fopen(arquivoSaida, "wb");

    if (csv == NULL || bin == NULL)
    {
        if (csv) fclose(csv);
        if (bin) fclose(bin);
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    // Cabeçalho inicial (status '0' enquanto escreve)
    Header cab = {'0', -1, 25, 0, 0};
    write_header(&cab, bin);

    char linha[1024];
    fgets(linha, sizeof(linha), csv); // Pula a primeira linha (cabeçalho do CSV)

    char delim = '|';

    while (fgets(linha, sizeof(linha), csv) != NULL)
    {
        if (linha[0] == '\n' || linha[0] == '\r' || linha[0] == '\0')
            continue;

        // Separa as 5 colunas da linha: [0]=indice, [1]=idPoPs, [2]=nomePoPs, [3]=nomePais, [4]=siglaPais
        char campos[5][512] = {"", "", "", "", ""};
        int col = 0, pos = 0;

        for (int i = 0; linha[i] != '\0' && linha[i] != '\n' && linha[i] != '\r'; i++)
        {
            if (linha[i] == ',')
            {
                campos[col][pos] = '\0';
                col++;
                pos = 0;
            }
            else
            {
                campos[col][pos++] = linha[i];
            }
        }
        campos[col][pos] = '\0';

        // Ignora campos[0] e mapeia as 4 colunas de dados
        char *strId = campos[1];
        char *nomePoPs = campos[2];
        char *nomePais = campos[3];
        char *strSigla = campos[4];

        // Prepara os campos do registro
        char removido = '0';
        int64_t encadeamento = -1;
        int idPoPs = (strlen(strId) == 0) ? -1 : atoi(strId);

        char siglaPais[2] = {'$', '$'};
        if (strlen(strSigla) >= 2)
        {
            siglaPais[0] = strSigla[0];
            siglaPais[1] = strSigla[1];
        }

        int tamPoPs = strlen(nomePoPs);
        int tamPais = strlen(nomePais);
        int tamanhoRegistro = 19 + tamPoPs + 1 + tamPais + 1;

        // Grava campo a campo
        fwrite(&removido, sizeof(char), 1, bin);
        fwrite(&tamanhoRegistro, sizeof(int), 1, bin);
        fwrite(&encadeamento, sizeof(int64_t), 1, bin);
        fwrite(&idPoPs, sizeof(int), 1, bin);
        fwrite(siglaPais, sizeof(char), 2, bin);

        fwrite(nomePoPs, sizeof(char), tamPoPs, bin);
        fwrite(&delim, sizeof(char), 1, bin);

        fwrite(nomePais, sizeof(char), tamPais, bin);
        fwrite(&delim, sizeof(char), 1, bin);

        cab.proxByteOffset += tamanhoRegistro;
        cab.nroPoPs++;
    }

    // Atualiza cabeçalho final com status '1'
    cab.status = '1';
    write_header(&cab, bin);

    fclose(csv);
    fclose(bin);

    BinarioNaTela(arquivoSaida);
}