
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/select.h"
#include "../include/fornecidas.h"
#include "../include/register_header.h"

// Imprime um registro
void print_register(Register *registro)
{
    printf("%d ", registro->idPoPs);

    if (strcmp(registro->nomePoPs, "$") == 0)
        printf("NULO ");
    else
        printf("%s ", registro->nomePoPs);

    if (strcmp(registro->nomePais, "$") == 0)
        printf("NULO ");
    else
        printf("%s ", registro->nomePais);

    if (registro->siglaPais[0] == '$')
        printf("NULO\n");
    else
        printf("%c%c\n", registro->siglaPais[0], registro->siglaPais[1]);
}

// Libera as strings do registro
void free_register(Register *registro)
{
    free(registro->nomePoPs);
    free(registro->nomePais);
}

void select_table(char *nomeArquivo)
{
    FILE *arquivo = fopen(nomeArquivo, "rb");

    if (arquivo == NULL)
    {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    // Criação do cabeçalho
    Header cabecalho;

    // Leitura do cabeçalho
    read_header(&cabecalho, arquivo);

    if (cabecalho.status == '0')
    {
        printf("Falha no processamento do arquivo.\n");
        fclose(arquivo);
        return;
    }

    // Criação do registro
    Register registro;

    int encontrou = 0;

    // Percorre os registros do arquivo
    while (read_register(&registro, arquivo))
    {
        // Registro logicamente removido não imprime
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

    // Caso não encontre registro válido
    if (encontrou == 0)
    {
        printf("Registro inexistente.\n");
    }
}

void select_where(char *arquivoEntrada, int n)
{
    FILE *arquivo = fopen(arquivoEntrada, "rb");

    if (arquivo == NULL)
    {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    // Criação do cabeçalho
    Header cabecalho;

    // Leitura do cabeçalho
    read_header(&cabecalho, arquivo);

    if (cabecalho.status == '0')
    {
        printf("Falha no processamento do arquivo.\n");
        fclose(arquivo);
        return;
    }

    // Criação do registro
    Register registro;

    // Percorre as buscas
    for (int i = 1; i <= n; i++)
    {
        Register filtroBusca;

        int m = 0;

        scanf("%d", &m);

        // Inicialização dos filtros
        filtroBusca.idPoPs = -1;

        filtroBusca.nomePoPs = NULL;
        filtroBusca.nomePais = NULL;

        filtroBusca.siglaPais[0] = '@';

        int buscaId = 0;
        int buscaNomePoPs = 0;
        int buscaNomePais = 0;
        int buscaSiglaPais = 0;

        // Leitura dos campos e valores da busca
        for (int j = 1; j <= m; j++)
        {
            char nomeCampo[50];
            char valorCampo[1000];

            scanf("%s", nomeCampo);
            scan_quote_string(valorCampo);

            if (!strcmp(nomeCampo, "idPoPs"))
            {
                filtroBusca.idPoPs = atoi(valorCampo);
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

        // Volta para o início dos registros
        fseek(arquivo, 25, SEEK_SET);

        // Percorre os registros
        while (read_register(&registro, arquivo))
        {
            // Registro logicamente removido não imprime
            if (registro.removido == '1')
            {
                free_register(&registro);
                continue;
            }

            int passouFiltro = 1;

            // Verificação dos filtros
            if (buscaId && registro.idPoPs != filtroBusca.idPoPs)
                passouFiltro = 0;

            if (buscaNomePoPs)
            {
                if (strcmp(filtroBusca.nomePoPs, "") == 0)
                {
                    if (strcmp(registro.nomePoPs, "$") != 0)
                        passouFiltro = 0;
                }
                else if (strcmp(registro.nomePoPs, filtroBusca.nomePoPs) != 0)
                    passouFiltro = 0;
            }

            if (buscaNomePais)
            {
                if (strcmp(filtroBusca.nomePais, "") == 0)
                {
                    if (strcmp(registro.nomePais, "$") != 0)
                        passouFiltro = 0;
                }
                else if (strcmp(registro.nomePais, filtroBusca.nomePais) != 0)
                    passouFiltro = 0;
            }

            if (buscaSiglaPais)
            {
                if (strcmp(filtroBusca.siglaPais, "") == 0)
                {
                    if (registro.siglaPais[0] != '$')
                        passouFiltro = 0;
                }
                else if (strncmp(registro.siglaPais, filtroBusca.siglaPais, 2) != 0)
                    passouFiltro = 0;
            }

            if (passouFiltro)
            {
                encontrou = 1;
                print_register(&registro);
            }

            free_register(&registro);
        }

        // Caso não encontre registro válido
        if (encontrou == 0)
        {
            printf("Registro inexistente.\n");
        }

        free(filtroBusca.nomePoPs);
        free(filtroBusca.nomePais);

        if (i < n)
            printf("\n");
    }

    fclose(arquivo);
}
