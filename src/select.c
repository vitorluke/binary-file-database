// NOME: Murilo Antonio de Oliveira Amatti. NroUSP: 17150041
// NOME: Vitor Quirino Machado. NroUSP: 16904644

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "../include/select.h"
#include "../include/fornecidas.h"
#include "../include/register_header.h"

// Estrutura auxiliar para armazenar uma entrada do arquivo de índice primário (12 bytes)
typedef struct {
    int idPoPs;         // Chave primária de busca (4 bytes)
    int64_t byteoffset; // Byte offset do registro no arquivo de dados (8 bytes)
} IndexEntry;

/*
 * Procedimento: print_register
 * Objetivo: Exibir os campos de um registro na ordem especificada (idPoPs, nomePoPs, nomePais, siglaPais).
 * Parâmetros:
 *   - registro: ponteiro para a estrutura Register contendo os dados lidos.
 */
void print_register(Register *registro)
{
    // Exibe idPoPs (ou NULO caso seja -1)
    if (registro->idPoPs == -1)
        printf("NULO ");
    else
        printf("%d ", registro->idPoPs);

    // Exibe nomePoPs (string vazia indica campo variável nulo)
    if (registro->nomePoPs == NULL || strlen(registro->nomePoPs) == 0)
        printf("NULO ");
    else
        printf("%s ", registro->nomePoPs);

    // Exibe nomePais (string vazia indica campo variável nulo)
    if (registro->nomePais == NULL || strlen(registro->nomePais) == 0)
        printf("NULO ");
    else
        printf("%s ", registro->nomePais);

    // Exibe siglaPais (preenchido com '$' quando nulo)
    if (registro->siglaPais[0] == '$')
        printf("NULO\n");
    else
        printf("%c%c\n", registro->siglaPais[0], registro->siglaPais[1]);
}

/*
 * Procedimento: free_register
 * Objetivo: Liberar a memória alocada dinamicamente para as strings variáveis do registro.
 * Parâmetros:
 *   - registro: ponteiro para a estrutura Register.
 */
void free_register(Register *registro)
{
    if (registro->nomePoPs != NULL)
    {
        free(registro->nomePoPs);
        registro->nomePoPs = NULL;
    }
    if (registro->nomePais != NULL)
    {
        free(registro->nomePais);
        registro->nomePais = NULL;
    }
}

/*
 * Função: matches_filter
 * Objetivo: Verificar se um registro lido satisfaz todos os critérios de busca informados.
 * Retorno: 1 se satisfaz todos os critérios, 0 caso contrário.
 */
static int matches_filter(Register *reg, Register *filtro, int buscaId, int buscaNomePoPs, int buscaNomePais, int buscaSiglaPais)
{
    if (buscaId && reg->idPoPs != filtro->idPoPs)
        return 0;

    if (buscaNomePoPs)
    {
        if (strcmp(filtro->nomePoPs, "") == 0) // Critério busca valor NULO
        {
            if (strlen(reg->nomePoPs) != 0)
                return 0;
        }
        else if (strcmp(reg->nomePoPs, filtro->nomePoPs) != 0)
            return 0;
    }

    if (buscaNomePais)
    {
        if (strcmp(filtro->nomePais, "") == 0) // Critério busca valor NULO
        {
            if (strlen(reg->nomePais) != 0)
                return 0;
        }
        else if (strcmp(reg->nomePais, filtro->nomePais) != 0)
            return 0;
    }

    if (buscaSiglaPais)
    {
        if (strcmp(filtro->siglaPais, "") == 0) // Critério busca valor NULO
        {
            if (reg->siglaPais[0] != '$')
                return 0;
        }
        else if (strncmp(reg->siglaPais, filtro->siglaPais, 2) != 0)
            return 0;
    }

    return 1;
}

/*
 * Função: binary_search_index
 * Objetivo: Realizar busca binária no vetor de índice primário ordenado por idPoPs.
 * Retorno: O byteoffset correspondente no arquivo de dados, ou -1 se não encontrado.
 */
static int64_t binary_search_index(IndexEntry *indice, int total, int idBuscado)
{
    int esq = 0;
    int dir = total - 1;

    while (esq <= dir)
    {
        int meio = esq + (dir - esq) / 2;

        if (indice[meio].idPoPs == idBuscado)
            return indice[meio].byteoffset;
        else if (indice[meio].idPoPs < idBuscado)
            esq = meio + 1;
        else
            dir = meio - 1;
    }

    return -1;
}

/*
 * Procedimento: select_table (Funcionalidade [10])
 * Objetivo: Percorrer sequencialmente o arquivo de dados e exibir todos os registros não removidos.
 * Parâmetros:
 *   - nomeArquivo: caminho do arquivo binário de dados PoPs.
 */
void select_table(char *nomeArquivo)
{
    FILE *arquivo = fopen(nomeArquivo, "rb");
    if (arquivo == NULL)
    {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    // Leitura e validação do cabeçalho
    Header cabecalho;
    read_header(&cabecalho, arquivo);

    if (cabecalho.status == '0')
    {
        printf("Falha no processamento do arquivo.\n");
        fclose(arquivo);
        return;
    }

    Register registro;
    int encontrou = 0;

    // Percorre todos os registros até o fim do arquivo
    while (read_register(&registro, arquivo))
    {
        // Ignora registros logicamente removidos
        if (registro.removido == '1')
        {
            free_register(&registro);
            continue;
        }

        encontrou = 1;
        print_register(&registro);
        free_register(&registro);
    }

    fclose(arquivo);

    if (encontrou == 0)
    {
        printf("Registro inexistente.\n");
    }
}

/*
 * Procedimento: select_where (Funcionalidade [11])
 * Objetivo: Realizar n buscas filtradas no arquivo de dados, utilizando o índice primário
 *           quando o critério incluir idPoPs e busca sequencial nos demais casos.
 * Parâmetros:
 *   - arquivoEntrada: caminho do arquivo binário de dados PoPs.
 *   - arquivoIndice: caminho do arquivo binário de índice primário indexaPoPs.
 *   - n: quantidade de buscas a serem executadas.
 */
void select_where(char *arquivoEntrada, char *arquivoIndice, int n)
{
    FILE *arquivo = fopen(arquivoEntrada, "rb");
    FILE *arqIndice = fopen(arquivoIndice, "rb");

    if (arquivo == NULL || arqIndice == NULL)
    {
        if (arquivo != NULL) fclose(arquivo);
        if (arqIndice != NULL) fclose(arqIndice);
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    // Validação do status de consistência do arquivo de dados
    Header cabecalho;
    read_header(&cabecalho, arquivo);

    // Validação do status de consistência do arquivo de índice
    char statusIndice;
    if (fread(&statusIndice, sizeof(char), 1, arqIndice) != 1 || cabecalho.status == '0' || statusIndice == '0')
    {
        printf("Falha no processamento do arquivo.\n");
        fclose(arquivo);
        fclose(arqIndice);
        return;
    }

    // Calcula a quantidade de registros no arquivo de índice (cada entrada tem 12 bytes)
    fseek(arqIndice, 0, SEEK_END);
    long tamanhoArqIndice = ftell(arqIndice);
    int totalIndices = (tamanhoArqIndice - 1) / (sizeof(int) + sizeof(int64_t));

    // Carrega o índice primário para a memória principal
    IndexEntry *indice = NULL;
    if (totalIndices > 0)
    {
        indice = malloc(totalIndices * sizeof(IndexEntry));
        fseek(arqIndice, 1, SEEK_SET);
        for (int k = 0; k < totalIndices; k++)
        {
            fread(&indice[k].idPoPs, sizeof(int), 1, arqIndice);
            fread(&indice[k].byteoffset, sizeof(int64_t), 1, arqIndice);
        }
    }
    fclose(arqIndice);

    Register registro;

    // Executa as n buscas solicitadas
    for (int i = 1; i <= n; i++)
    {
        Register filtroBusca;
        int m = 0;
        scanf("%d", &m);

        // Inicialização dos filtros da busca atual
        filtroBusca.idPoPs = -1;
        filtroBusca.nomePoPs = NULL;
        filtroBusca.nomePais = NULL;
        filtroBusca.siglaPais[0] = '@';

        int buscaId = 0;
        int buscaNomePoPs = 0;
        int buscaNomePais = 0;
        int buscaSiglaPais = 0;

        // Lê os m pares (nomeCampo, valorCampo)
        for (int j = 1; j <= m; j++)
        {
            char nomeCampo[50];
            char valorCampo[1000];

            scanf("%s", nomeCampo);
            scan_quote_string(valorCampo);

            if (!strcmp(nomeCampo, "idPoPs"))
            {
                filtroBusca.idPoPs = (strcmp(valorCampo, "") == 0) ? -1 : atoi(valorCampo);
                buscaId = 1;
            }
            else if (!strcmp(nomeCampo, "nomePoPs"))
            {
                filtroBusca.nomePoPs = malloc(strlen(valorCampo) + 1);
                strcpy(filtroBusca.nomePoPs, valorCampo);
                buscaNomePoPs = 1;
            }
            else if (!strcmp(nomeCampo, "nomePais"))
            {
                filtroBusca.nomePais = malloc(strlen(valorCampo) + 1);
                strcpy(filtroBusca.nomePais, valorCampo);
                buscaNomePais = 1;
            }
            else if (!strcmp(nomeCampo, "siglaPais"))
            {
                strcpy(filtroBusca.siglaPais, valorCampo);
                buscaSiglaPais = 1;
            }
        }

        int encontrou = 0;

        // Caso 1: Busca envolve idPoPs -> utiliza busca indexada via índice primário
        if (buscaId)
        {
            int64_t offset = binary_search_index(indice, totalIndices, filtroBusca.idPoPs);
            if (offset != -1)
            {
                fseek(arquivo, offset, SEEK_SET);
                if (read_register(&registro, arquivo))
                {
                    if (registro.removido == '0' && matches_filter(&registro, &filtroBusca, buscaId, buscaNomePoPs, buscaNomePais, buscaSiglaPais))
                    {
                        encontrou = 1;
                        print_register(&registro);
                    }
                    free_register(&registro);
                }
            }
        }
        // Caso 2: Busca não envolve idPoPs -> utiliza busca sequencial a partir do byte 25
        else
        {
            fseek(arquivo, 25, SEEK_SET);
            while (read_register(&registro, arquivo))
            {
                if (registro.removido == '1')
                {
                    free_register(&registro);
                    continue;
                }

                if (matches_filter(&registro, &filtroBusca, buscaId, buscaNomePoPs, buscaNomePais, buscaSiglaPais))
                {
                    encontrou = 1;
                    print_register(&registro);
                }

                free_register(&registro);
            }
        }

        if (encontrou == 0)
        {
            printf("Registro inexistente.\n");
        }

        free(filtroBusca.nomePoPs);
        free(filtroBusca.nomePais);
    }

    free(indice);
    fclose(arquivo);
}