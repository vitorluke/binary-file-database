#include <stdio.h>
#include <stdlib.h>
#include "func.h"

void select_table(char *nomeArquivo)
{
    FILE *arquivo = fopen(nomeArquivo, "rb");

    if (arquivo == NULL)
    {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    //Criação do cabecalho
    Header cabecalho;

    //Leitura do cabecalho
    fread(&cabecalho.status, sizeof(cabecalho.status),1,arquivo);
    fread(&cabecalho.topoPilha,sizeof(cabecalho.topoPilha),1,arquivo);
    fread(&cabecalho.proxRRN,sizeof(cabecalho.proxRRN),1,arquivo);
    fread(&cabecalho.nroRegRem,sizeof(cabecalho.nroRegRem),1,arquivo);
    fread(&cabecalho.nroPares,sizeof(cabecalho.nroPares),1,arquivo);

    //Criação do registro
    Register registro;

    int encontrou = 0;
    /* Enquanto for possível ler o primeiro campo do registro
    Continuamos percorrendo o arquivo*/
    while(fread(&registro.removido,sizeof(registro.removido),1,arquivo) == 1)
    {
        fread(&registro.encadeamentoPilha,sizeof(registro.encadeamentoPilha),1,arquivo);
        fread(&registro.idPoPs,sizeof(registro.idPoPs),1,arquivo);
        fread(&registro.idPopsConectado,sizeof(registro.idPopsConectado),1,arquivo);
        fread(&registro.velocidade,sizeof(registro.velocidade),1,arquivo);
        fread(&registro.unidadeMedida,sizeof(registro.unidadeMedida),1,arquivo);

        // Registro logicamente removido não imprime
        if ( registro.removido == '1')
        {
            continue;
        }

        encontrou = 1;

        printf("%d %d ", registro.idPoPs, registro.idPopsConectado);


        //Condições
        if (registro.velocidade == -1)
        {
            printf("NULO ");
        }
        else
        {
            printf("%d ", registro.velocidade);
        }

        if (registro.unidadeMedida == '$')
        {
            printf("NULO\n");
        }
        else
        {
            printf("%c\n", registro.unidadeMedida);
        }
    
    }

    fclose(arquivo);

    //Caso não encontre registro válido
    if(encontrou == 0){
        printf("Registro inexistente.\n");
    }

}