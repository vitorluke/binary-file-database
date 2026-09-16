#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "func.h"
#include "fornecidas.h"

#define BUFFER 100

void create_table(char *arquivoEntrada, char *arquivoSaida)
{
    //tentativa de abertura do arquivo .csv
    FILE *arquivo_entrada = fopen(arquivoEntrada, "rt");

    char linha[BUFFER];
    

    if( arquivo_entrada == NULL)
    {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    //Criação do arquivo binário
    FILE *arquivo_saida = fopen(arquivoSaida,"wb");

    if(arquivo_saida == NULL)
    {
        printf("Falha no processamento do arquivo.\n");
    }

    //Criação do Cabeçalho
    Header cabecalho;
    cabecalho.status = '0';
    cabecalho.topoPilha = -1;
    cabecalho.proxRRN = 0;
    cabecalho.nroRegRem = 0;
    cabecalho.nroPares = 0;

    //Escrita inicial do header
    fwrite(&cabecalho.status,sizeof(cabecalho.status), 1, arquivo_saida);
    fwrite(&cabecalho.topoPilha, sizeof(cabecalho.topoPilha), 1, arquivo_saida);
    fwrite(&cabecalho.proxRRN,sizeof(cabecalho.proxRRN), 1, arquivo_saida);
    fwrite(&cabecalho.nroRegRem, sizeof(cabecalho.nroRegRem), 1, arquivo_saida);
    fwrite(&cabecalho.nroPares, sizeof(cabecalho.nroPares), 1, arquivo_saida);

    //captura da primeira linha do csv
    fgets(linha, sizeof(linha), arquivo_entrada);

    int i = 0;

    while(fgets(linha, sizeof(linha), arquivo_entrada) != NULL)
    {
        i++;

        Register registro;

        registro.removido = '0';
        registro.encadeamentoPilha = -1;

        char *campo;

        campo = strtok(linha, ",");//campo é idPops
        registro.idPoPs = atoi(campo);

        campo = strtok(NULL, ",");//campo é idPopsConectado
        registro.idPopsConectado = atoi(campo);

        campo = strtok(NULL, ",");//campo é velocidade
        registro.velocidade = atoi(campo);

        campo = strtok(NULL, ",");//campo é unidade de medida
        registro.unidadeMedida = campo[0];

        fwrite(&registro.removido,sizeof(registro.removido), 1, arquivo_saida);
        fwrite(&registro.encadeamentoPilha,sizeof(registro.encadeamentoPilha), 1,arquivo_saida);
        fwrite(&registro.idPoPs,sizeof(registro.idPoPs),1,arquivo_saida);
        fwrite(&registro.idPopsConectado,sizeof(registro.idPopsConectado),1,arquivo_saida);
        fwrite(&registro.velocidade,sizeof(registro.velocidade),1,arquivo_saida);
        fwrite(&registro.unidadeMedida,sizeof(registro.unidadeMedida),1,arquivo_saida);

    }

    cabecalho.status = '1';
    cabecalho.proxRRN = i;
    cabecalho.nroPares = i;
    fseek(arquivo_saida,0,SEEK_SET);
    fwrite(&cabecalho.status,sizeof(cabecalho.status),1,arquivo_saida);
    fwrite(&cabecalho.topoPilha, sizeof(cabecalho.topoPilha), 1, arquivo_saida);
    fwrite(&cabecalho.proxRRN,sizeof(cabecalho.proxRRN), 1, arquivo_saida);
    fwrite(&cabecalho.nroRegRem, sizeof(cabecalho.nroRegRem), 1, arquivo_saida);
    fwrite(&cabecalho.nroPares, sizeof(cabecalho.nroPares), 1, arquivo_saida);

    fclose(arquivo_entrada);
    fclose(arquivo_saida);
    

    BinarioNaTela(arquivoSaida);

}