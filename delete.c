#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "func.h"
#include "fornecidas.h"

void delete_register(char *arquivoEntrada,int n){
    FILE *arquivo = fopen(arquivoEntrada, "rb+");

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


    Register filtroBusca; //Criação dos filtros

    int m = 0;  //Quantidade de pares de campos
    for(int i = 1; i <= n; i++){    //Percorre as exigencias da busca
        
        m = 0;
        scanf("%d", &m);

        filtroBusca.unidadeMedida = '@'; //Inicializa o filtro
        filtroBusca.velocidade =  -2;
        filtroBusca.idPopsConectado =  -2;
        filtroBusca.idPoPs =  -2;

        for(int j = 1; j <= m; j++){    //Percorre os campos e os valores de cada exigencia da busca
            char nomeCampo[20];
            char valorCampo[20];

            scanf("%s", nomeCampo);
            scanf("%s", valorCampo);

            if(!strcmp(nomeCampo, "unidadeMedida")){ //Coloca no filtro os campos
                filtroBusca.unidadeMedida = valorCampo[0];
            }else if(!strcmp(nomeCampo, "velocidade")){
                filtroBusca.velocidade =  atoi(valorCampo);
            }else if(!strcmp(nomeCampo, "idPoPsConectado")){
                filtroBusca.idPopsConectado =  atoi(valorCampo);
            }else if(!strcmp(nomeCampo, "idPoPs")){
                filtroBusca.idPoPs =  atoi(valorCampo);
            }
        }
        


    int encontrou = 0;
    int passouFiltro = 0;   //=0 significa que passou, =1 significa que nao passou


    fseek(arquivo, 17, SEEK_SET);   //Volta para o final do cabeçalho

    int posByte = 17;   //Posicao do primeiro registro

    /* Enquanto for possível ler o primeiro campo do registro
    Continuamos percorrendo o arquivo*/
    while(fread(&registro.removido,sizeof(registro.removido),1,arquivo) == 1)
    {
        posByte++;
        fread(&registro.encadeamentoPilha,sizeof(registro.encadeamentoPilha),1,arquivo);
        posByte += 4;
        fread(&registro.idPoPs,sizeof(registro.idPoPs),1,arquivo);
        posByte += 4;
        fread(&registro.idPopsConectado,sizeof(registro.idPopsConectado),1,arquivo);
        posByte += 4;
        fread(&registro.velocidade,sizeof(registro.velocidade),1,arquivo);
        posByte += 4;
        fread(&registro.unidadeMedida,sizeof(registro.unidadeMedida),1,arquivo);
        posByte++;

        // Registro logicamente removido
        if ( registro.removido == '1')
        {
            continue;
        }
        
        passouFiltro = 0;

            //Aqui ocorre o filtro de cada registro
            if(filtroBusca.idPoPs != -2 && registro.idPoPs != filtroBusca.idPoPs){
                passouFiltro = 1;
                continue;
            }
            if(filtroBusca.velocidade != -2 && registro.velocidade != filtroBusca.velocidade){
                passouFiltro = 1;
                continue;
            }
            if(filtroBusca.idPopsConectado != -2 && registro.idPopsConectado != filtroBusca.idPopsConectado){
                passouFiltro = 1;
                continue;
            }
            if(filtroBusca.unidadeMedida != '@' && registro.unidadeMedida != filtroBusca.unidadeMedida){
                passouFiltro = 1;
                continue;
            }

        

        if(passouFiltro == 1) continue;

        encontrou = 1;
        int posRemovido = posByte - 18;
        int rrnRemovido = (posRemovido - 17)/18;
        fseek(arquivo, posRemovido, SEEK_SET); 
        fwrite("1", sizeof(char), 1, arquivo);
        fwrite(&cabecalho.topoPilha, sizeof(int), 1, arquivo);
        fseek(arquivo, 1, SEEK_SET);
        fwrite(&rrnRemovido, sizeof(int), 1, arquivo);
        fseek(arquivo, 9, SEEK_SET);
        cabecalho.nroRegRem++;
        fwrite(&cabecalho.nroRegRem, sizeof(int), 1, arquivo);
        fseek(arquivo, posRemovido + 5, SEEK_SET);
        for(int h = 0; h <= 12; h++){
            fwrite("$", sizeof(char), 1, arquivo);
        }
        cabecalho.topoPilha = rrnRemovido;
    
    }
        //Caso não encontre registro válido
    if(encontrou == 0){
        printf("Registro inexistente.\n");
    }
    }

    


    fclose(arquivo);

    BinarioNaTela(arquivoEntrada);
}