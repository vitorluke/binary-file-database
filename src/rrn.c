#include <stdio.h>
#include <stdlib.h>
#include "../include/rrn.h"

void busca_rrn(char *nomeArquivo, int rrn) {

    FILE *arquivo;

    Register registro;

    //Criação do cabecalho
    Header cabecalho;

    arquivo = fopen(nomeArquivo, "rb");

    if (arquivo == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }


    //Leitura do cabecalho
    read_header(&cabecalho, arquivo);

    if(cabecalho.status == 0){  //Verifica se o arquivo tinha terminado corretamente sua ultima alteração
        printf("Arquivo corrompido!");
        return;
    }

    // Vai diretamente para o registro desejado
    fseek(arquivo, 17 + rrn * 18, SEEK_SET);

    // Lê o registro
    if (fread(&registro.removido, sizeof(registro.removido), 1, arquivo) != 1 ||
        fread(&registro.encadeamentoPilha, sizeof(registro.encadeamentoPilha), 1, arquivo) != 1 ||
        fread(&registro.idPoPs, sizeof(registro.idPoPs), 1, arquivo) != 1 ||
        fread(&registro.idPopsConectado, sizeof(registro.idPopsConectado), 1, arquivo) != 1 ||
        fread(&registro.velocidade, sizeof(registro.velocidade), 1, arquivo) != 1 ||
        fread(&registro.unidadeMedida, sizeof(registro.unidadeMedida), 1, arquivo) != 1) {

        fclose(arquivo);
        printf("Registro inexistente.\n");
        return;
    }

    fclose(arquivo);

    // Registro logicamente removido
    if (registro.removido == '1') {
        printf("Registro inexistente.\n");
        return;
    }

    // Imprime os campos
    printf("%d ", registro.idPoPs);
    printf("%d ", registro.idPopsConectado);

    if (registro.velocidade == -1)
        printf("NULO ");
    else
        printf("%d ", registro.velocidade);

    if (registro.unidadeMedida == '$')
        printf("NULO\n");
    else
        printf("\"%c\"\n", registro.unidadeMedida);
}