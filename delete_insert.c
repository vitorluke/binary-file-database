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

    //Atualiza o status do cabeçalho para 0
    fseek(arquivo, 0, SEEK_SET);
    fwrite("0", sizeof(char), 1, arquivo);

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

        int posRemovido = posByte - 18; //Posicao do registro removido
        int rrnRemovido = (posRemovido - 17)/18;    //rrn do registro removido

        fseek(arquivo, posRemovido, SEEK_SET); //Apontar para o inicio do registro a ser removido
        fwrite("1", sizeof(char), 1, arquivo);  //Dizer que aquele registro foi removido no byte de removido
        fwrite(&cabecalho.topoPilha, sizeof(int), 1, arquivo);  //Dizer qual o registro removido anterior da pilha

        fseek(arquivo, 1, SEEK_SET);    //Apontar para o topo da pilha
        fwrite(&rrnRemovido, sizeof(int), 1, arquivo);  //Dizer qual o rrn do registro removido para o topo da pilha

        fseek(arquivo, 9, SEEK_SET);    //Apontar para o numero de registros removidos
        cabecalho.nroRegRem++;
        fwrite(&cabecalho.nroRegRem, sizeof(int), 1, arquivo);  //Dizer a nova quantidade de registros removidos

        fseek(arquivo, posRemovido + 5, SEEK_SET);  //Apontar para os bytes de campos do registro removido
        for(int h = 0; h <= 12; h++){
            fwrite("$", sizeof(char), 1, arquivo);  //Apagar os dados do registro removido
        }
        cabecalho.topoPilha = rrnRemovido;  //Atualiza na memoria RAM o topo da pilha
    
    }
    }

    
    //Atualiza o status do cabeçalho para 1
    fseek(arquivo, 0, SEEK_SET);
    fwrite("1", sizeof(char), 1, arquivo);
    
    fclose(arquivo);

    BinarioNaTela(arquivoEntrada);
}


void insert_into(char *arquivoEntrada,int n){
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

    //Atualiza o status do cabeçalho para 0
    fseek(arquivo, 0, SEEK_SET);
    fwrite("0", sizeof(char), 1, arquivo);

    //Criação do registro
    Register registro;


    Register registroAdicionado; //Criação do registro a ser adicionado

    for(int i = 1; i <= n; i++){    //Percorre os registros a serem adicionados
        
        registroAdicionado.encadeamentoPilha = -1;
        registroAdicionado.removido = '0';
        registroAdicionado.unidadeMedida = '@'; //Inicializa o registro a ser adicionado
        registroAdicionado.velocidade =  -2;
        registroAdicionado.idPopsConectado =  -2;
        registroAdicionado.idPoPs =  -2;

        for(int j = 1; j <= 4; j++){    //Percorre os campos e os valores de cada registro a ser adicionado
            char valorCampo[20];

            
            scanf("%s", valorCampo);
            

            if(j == 1){ //Coloca no registro adicionado os campos
                if(!strcmp(valorCampo, "NULO")){
                    registroAdicionado.idPoPs = -1;
                    continue;
                }
                registroAdicionado.idPoPs = atoi(valorCampo);
            }else if(j == 2){
                if(!strcmp(valorCampo, "NULO")){
                    registroAdicionado.idPopsConectado = -1;
                    continue;
                }
                registroAdicionado.idPopsConectado =  atoi(valorCampo);
            }else if(j == 3){
                if(!strcmp(valorCampo, "NULO")){
                    registroAdicionado.velocidade = -1;
                    continue;
                }
                registroAdicionado.velocidade =  atoi(valorCampo);
            }else if(j == 4){
                if(!strcmp(valorCampo, "NULO")){
                    registroAdicionado.unidadeMedida = '$';
                    continue;
                }
                registroAdicionado.unidadeMedida =  valorCampo[1];
            }
        }

        if(cabecalho.nroRegRem == 0){   //Confere se tem algum registro removido
            fseek(arquivo, 17 + cabecalho.proxRRN * 18, SEEK_SET);  //Vai até o final do arquivo

            fwrite(&registroAdicionado.removido,sizeof(registro.removido), 1, arquivo); //Escreve os dados do novo registro
            fwrite(&registroAdicionado.encadeamentoPilha,sizeof(registro.encadeamentoPilha), 1,arquivo);
            fwrite(&registroAdicionado.idPoPs,sizeof(registro.idPoPs),1,arquivo);
            fwrite(&registroAdicionado.idPopsConectado,sizeof(registro.idPopsConectado),1,arquivo);
            fwrite(&registroAdicionado.velocidade,sizeof(registro.velocidade),1,arquivo);
            fwrite(&registroAdicionado.unidadeMedida,sizeof(registro.unidadeMedida),1,arquivo);

            fseek(arquivo, 5, SEEK_SET);    //Atualiza o proximo RRN
            cabecalho.proxRRN++;
            fwrite(&cabecalho.proxRRN, sizeof(cabecalho.proxRRN), 1, arquivo);


        }else{
            int posRemovido = cabecalho.topoPilha;  //Salva o rrn do registro removido do topo da pilha
            fseek(arquivo, 17 + cabecalho.topoPilha * 18 + 1, SEEK_SET);    //Vai até o encadeamento do registro removido do topo da pilha
            fread(&cabecalho.topoPilha, sizeof(cabecalho.topoPilha), 1, arquivo);   //Le o encadeamento da pilha e coloca no cabeçalho em RAM

            fseek(arquivo, 17 + posRemovido * 18, SEEK_SET);    //vai até o inicio do registro removido
            fwrite(&registroAdicionado.removido, sizeof(registroAdicionado.removido), 1, arquivo);  //Rescreve o novo registro
            fwrite(&registroAdicionado.encadeamentoPilha, sizeof(registroAdicionado.encadeamentoPilha), 1, arquivo);
            fwrite(&registroAdicionado.idPoPs,sizeof(registro.idPoPs),1,arquivo);
            fwrite(&registroAdicionado.idPopsConectado,sizeof(registro.idPopsConectado),1,arquivo);
            fwrite(&registroAdicionado.velocidade,sizeof(registro.velocidade),1,arquivo);
            fwrite(&registroAdicionado.unidadeMedida,sizeof(registro.unidadeMedida),1,arquivo);

            fseek(arquivo, 1, SEEK_SET);    //Vai ate o topo da pilha
            fwrite(&cabecalho.topoPilha, sizeof(cabecalho.topoPilha), 1, arquivo);  //Escreve o novo topo da pilha de removidos

            cabecalho.nroRegRem = cabecalho.nroRegRem - 1;  //Atualiza o numero de registros removidos
            fseek(arquivo, 9, SEEK_SET);
            fwrite(&cabecalho.nroRegRem, sizeof(cabecalho.nroRegRem), 1, arquivo);

        }
    }

    
    //Atualiza o status do cabeçalho para 1
    fseek(arquivo, 0, SEEK_SET);
    fwrite("1", sizeof(char), 1, arquivo);

    fclose(arquivo);

    BinarioNaTela(arquivoEntrada);

}

void update(char *arquivoEntrada,int n){
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

    //Atualiza o status do cabeçalho para 0
    fseek(arquivo, 0, SEEK_SET);
    fwrite("0", sizeof(char), 1, arquivo);

    //Criação do registro
    Register registro;


    Register filtroBusca; //Criação dos filtros
    Register novoRegistro;

    int m = 0;  //Quantidade de pares de campos do filtro
    int p = 0;  //Quantidade de pares de campos do novo registro
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
                filtroBusca.unidadeMedida = valorCampo[1];
            }else if(!strcmp(nomeCampo, "velocidade")){
                filtroBusca.velocidade =  atoi(valorCampo);
            }else if(!strcmp(nomeCampo, "idPoPsConectado")){
                filtroBusca.idPopsConectado =  atoi(valorCampo);
            }else if(!strcmp(nomeCampo, "idPoPs")){
                filtroBusca.idPoPs =  atoi(valorCampo);
            }
        }

        p = 0;
        scanf("%d", &p);

        novoRegistro.unidadeMedida = '@'; //Inicializa o novo registro
        novoRegistro.velocidade =  -2;
        novoRegistro.idPopsConectado =  -2;
        novoRegistro.idPoPs =  -2;

        for(int j = 1; j <= p; j++){    //Percorre os campos e os valores de cada registro novo
            char nomeCampo[20];
            char valorCampo[20];

            scanf("%s", nomeCampo);
            scanf("%s", valorCampo);

            if(!strcmp(nomeCampo, "unidadeMedida")){ //Coloca no novo registro os campos
                if(!strcmp(valorCampo, "NULO")){
                    novoRegistro.unidadeMedida = '$';
                    continue;
                }
                novoRegistro.unidadeMedida = valorCampo[1];
            }else if(!strcmp(nomeCampo, "velocidade")){
                if(!strcmp(valorCampo, "NULO")){
                    novoRegistro.velocidade = -1;
                    continue;
                }
                novoRegistro.velocidade =  atoi(valorCampo);
            }else if(!strcmp(nomeCampo, "idPoPsConectado")){
                if(!strcmp(valorCampo, "NULO")){
                    novoRegistro.idPopsConectado = -1;
                    continue;
                }
                novoRegistro.idPopsConectado =  atoi(valorCampo);
            }else if(!strcmp(nomeCampo, "idPoPs")){
                if(!strcmp(valorCampo, "NULO")){
                    novoRegistro.idPoPs = -1;
                    continue;
                }
                novoRegistro.idPoPs =  atoi(valorCampo);
            }
        }
        


    int encontrou = 0;
    int passouFiltro = 0;   //=0 significa que passou, =1 significa que nao passou


    fseek(arquivo, 17, SEEK_SET);   //Volta para o final do cabeçalho

    int posByte = 17;   //Posicao de onde esta lendo

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

        //Aqui atualiza o registro localizado na busca
            if(novoRegistro.idPoPs != -2){
                fseek(arquivo, posByte - 13, SEEK_SET);
                fwrite(&novoRegistro.idPoPs, sizeof(novoRegistro.idPoPs), 1, arquivo);
            }
            if(novoRegistro.idPopsConectado != -2){
                fseek(arquivo, posByte - 9, SEEK_SET);
                fwrite(&novoRegistro.idPopsConectado, sizeof(novoRegistro.idPopsConectado), 1, arquivo);
            }
            if(novoRegistro.velocidade != -2){
                fseek(arquivo, posByte - 5, SEEK_SET);
                fwrite(&novoRegistro.velocidade, sizeof(novoRegistro.velocidade), 1, arquivo);
            }
            if(novoRegistro.unidadeMedida != '@'){
                fseek(arquivo, posByte - 1, SEEK_SET);
                fwrite(&novoRegistro.unidadeMedida, sizeof(novoRegistro.unidadeMedida), 1, arquivo);
            }

        fseek(arquivo, posByte, SEEK_SET);  //Volta a apontar para o final do registro analisado


    
    }
    }

    
    //Atualiza o status do cabeçalho para 1
    fseek(arquivo, 0, SEEK_SET);
    fwrite("1", sizeof(char), 1, arquivo);

    fclose(arquivo);

    BinarioNaTela(arquivoEntrada);
}