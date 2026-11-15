// NOME: Murilo Antonio de Oliveira Amatti. NroUSP: 17150041
// NOME: Vitor Quirino Machado. NroUSP: 16904644

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "../include/update.h"
#include "../include/select.h"
#include "../include/insert.h"
#include "../include/delete.h"
#include "../include/fornecidas.h"
#include "../include/register_header.h"




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



void update_register(char *arquivoEntrada, char *arquivoIndice, int n){
    FILE *arquivo = fopen(arquivoEntrada, "rb+");
    FILE *arqIndice = fopen(arquivoIndice, "rb+");

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
    // Carrega o índice primário para a memória principal lendo até o fim do arquivo
    IndexEntry *indice = NULL;
    int totalIndices = 0;
    int idTemp;
    int64_t offsetTemp;

    while (fread(&idTemp, sizeof(int), 1, arqIndice) == 1 &&
           fread(&offsetTemp, sizeof(int64_t), 1, arqIndice) == 1)
    {
        indice = realloc(indice, (totalIndices + 1) * sizeof(IndexEntry));
        indice[totalIndices].idPoPs = idTemp;
        indice[totalIndices].byteoffset = offsetTemp;
        totalIndices++;
    }
    fclose(arqIndice);

    

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
            ScanQuoteString(valorCampo);

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


        Register atualizaRegistro;
        int p = 0;
        scanf("%d", &p);

        // Inicialização dos filtros da atualizacao do registro
        atualizaRegistro.idPoPs = -1;
        atualizaRegistro.nomePoPs = NULL;
        atualizaRegistro.nomePais = NULL;
        atualizaRegistro.siglaPais[0] = '@';
        int tamPoPs = 0;
        int tamPais = 0;


        // Lê os p pares (nomeCampo, valorCampo)
        for (int j = 1; j <= p; j++)
        {
            char nomeCampo[50];
            char valorCampo[1000];

            scanf("%s", nomeCampo);

            if (!strcmp(nomeCampo, "idPoPs")) {
                char valorCampo[50];
                scanf("%s", valorCampo);
                atualizaRegistro.idPoPs = atoi(valorCampo);
            } else {
                char valorCampo[1000];
                ScanQuoteString(valorCampo);
                if (!strcmp(nomeCampo, "nomePoPs")) {
                    if (strcmp(valorCampo, "NULO") == 0 || strcmp(valorCampo, "") == 0) {
                        atualizaRegistro.nomePoPs = malloc(1);
                        atualizaRegistro.nomePoPs[0] = '\0';
                        tamPoPs = strlen(atualizaRegistro.nomePoPs);
                    } else {
                        atualizaRegistro.nomePoPs = malloc(strlen(valorCampo) + 1);
                        strcpy(atualizaRegistro.nomePoPs, valorCampo);
                        tamPoPs = strlen(atualizaRegistro.nomePoPs);
                    }
                } else if (!strcmp(nomeCampo, "nomePais")) {
                    if (strcmp(valorCampo, "NULO") == 0 || strcmp(valorCampo, "") == 0) {
                        atualizaRegistro.nomePais = malloc(1);
                        atualizaRegistro.nomePais[0] = '\0';
                        tamPais = strlen(atualizaRegistro.nomePais);
                    } else {
                        atualizaRegistro.nomePais = malloc(strlen(valorCampo) + 1);
                        strcpy(atualizaRegistro.nomePais, valorCampo);
                        tamPais = strlen(atualizaRegistro.nomePais);
                    }
                } else if (!strcmp(nomeCampo, "siglaPais")) {
                    if (strcmp(valorCampo, "NULO") == 0 || strcmp(valorCampo, "") == 0) {
                        atualizaRegistro.siglaPais[0] = '$';
                        atualizaRegistro.siglaPais[1] = '$';
                    } else {
                        strncpy(atualizaRegistro.siglaPais, valorCampo, 2);
                    }
                }
            }
        }


        int encontrou = 0;

        Register registro;
        registro.nomePoPs = NULL;
        registro.nomePais = NULL;

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
                        int idAntigo = registro.idPoPs; // Guarda para o índice
                        int tamAntigo = registro.tamanhoRegistro;

                        //Atualiza o registro
                        if (atualizaRegistro.idPoPs != -1) registro.idPoPs = atualizaRegistro.idPoPs;
                        if (atualizaRegistro.nomePoPs != NULL) {
                            if (registro.nomePoPs != NULL) free(registro.nomePoPs);
                            registro.nomePoPs = malloc(strlen(atualizaRegistro.nomePoPs) + 1);
                            strcpy(registro.nomePoPs, atualizaRegistro.nomePoPs);
                        }
                        if (atualizaRegistro.nomePais != NULL) {
                            if (registro.nomePais != NULL) free(registro.nomePais);
                            registro.nomePais = malloc(strlen(atualizaRegistro.nomePais) + 1);
                            strcpy(registro.nomePais, atualizaRegistro.nomePais);
                        }
                        if (atualizaRegistro.siglaPais[0] != '@') {
                            strncpy(registro.siglaPais, atualizaRegistro.siglaPais, 2);
                        }

                        int tamPoPsFinal = (registro.nomePoPs != NULL) ? strlen(registro.nomePoPs) : 0;
                        int tamPaisFinal = (registro.nomePais != NULL) ? strlen(registro.nomePais) : 0;
                        int novoTamanhoDados = 19 + tamPoPsFinal + 1 + tamPaisFinal + 1;

                        //Verifica se cabe
                        int encaixa = (novoTamanhoDados <= tamAntigo) ? 1 : 0;
                        atualizaRegistro.tamanhoRegistro = novoTamanhoDados;


                        if(encaixa){    //Grava o registro atualizado no lugar onde ele estava
                            write_register(arquivo, registro, offset, strlen(registro.nomePoPs), strlen(registro.nomePais));
                            fseek(arquivo, offset + novoTamanhoDados, SEEK_SET);
                            char lixo = '$';
                            for(int k = atualizaRegistro.tamanhoRegistro; k < registro.tamanhoRegistro; k++){
                                fwrite(&lixo, sizeof(char), 1, arquivo);
                            }

                            // Se o ID mudou, reorganiza a chave no índice
                            if (registro.idPoPs != idAntigo) {
                                remover_do_indice_em_memoria(indice, &totalIndices, idAntigo);
                                adicionar_ao_indice_em_memoria(&indice, &totalIndices, registro.idPoPs, offset);
                            }

                        }else{  //Grava o registro onde ele caiba
                            Register antigoRegistro = registro;
                            antigoRegistro.tamanhoRegistro = tamAntigo;

                            //Deleta logicamente o registro
                            fseek(arquivo, offset, SEEK_SET);
                            antigoRegistro.removido = '1';
                            fwrite(&antigoRegistro.removido, sizeof(char), 1, arquivo);
                            
                            if (cabecalho.topoLista == -1) {
                                cabecalho.topoLista = offset;
                                fseek(arquivo, 1, SEEK_SET);
                                fwrite(&cabecalho.topoLista, sizeof(int64_t), 1, arquivo);
                            } else {
                                worstFit(arquivo, &antigoRegistro, 1, cabecalho.topoLista, offset);
                                fseek(arquivo, 1, SEEK_SET);
                                fread(&cabecalho.topoLista, sizeof(int64_t), 1, arquivo);
                            }

                            fseek(arquivo, offset + 13, SEEK_SET);
                            char lixo = '$';
                            for(int k = 13; k < registro.tamanhoRegistro; k++){
                                fwrite(&lixo, sizeof(char), 1, arquivo);
                            }

                            remover_do_indice_em_memoria(indice, &totalIndices, idAntigo);
                            cabecalho.nroRegRem++;
                            fseek(arquivo, 17, SEEK_SET); //Atualiza os removidos no disco
                            fwrite(&cabecalho.nroRegRem, sizeof(int), 1, arquivo);

                            //Insere o registro no lugar certo
                            registro.tamanhoRegistro = novoTamanhoDados;
                            int maiorRegRemovido = -1;
                            
                            if (cabecalho.topoLista != -1) {
                                fseek(arquivo, cabecalho.topoLista + 1, SEEK_SET);
                                fread(&maiorRegRemovido, sizeof(int), 1, arquivo);
                            }

                            if(maiorRegRemovido < registro.tamanhoRegistro){//Confere se nao cabe no registro removido do topo da pilha
                                write_register(arquivo, registro, cabecalho.proxByteOffset, strlen(registro.nomePoPs), strlen(registro.nomePais));

                                adicionar_ao_indice_em_memoria(&indice, &totalIndices, registro.idPoPs, cabecalho.proxByteOffset);
                                cabecalho.proxByteOffset += registro.tamanhoRegistro;
                                fseek(arquivo, 9, SEEK_SET);
                                fwrite(&cabecalho.proxByteOffset, sizeof(int64_t), 1, arquivo);
                            }else{
                                int64_t topoAntigo = cabecalho.topoLista;

                                //Altera o valor do tamanho do novo registro para o valor do registro removido
                                registro.tamanhoRegistro = maiorRegRemovido;

                                //Guarda o proximo registro removido
                                fseek(arquivo, cabecalho.topoLista + 5, SEEK_SET);
                                int64_t encadeamenteTemp = 0;
                                fread(&encadeamenteTemp, sizeof(int64_t), 1, arquivo);
                
                                write_register(arquivo, registro, cabecalho.topoLista, strlen(registro.nomePoPs), strlen(registro.nomePais));

                                fseek(arquivo, cabecalho.topoLista + novoTamanhoDados, SEEK_SET);
                                for (int k = novoTamanhoDados; k < maiorRegRemovido; k++) {
                                    fwrite(&lixo, sizeof(char), 1, arquivo);
                                }

                                adicionar_ao_indice_em_memoria(&indice, &totalIndices, registro.idPoPs, topoAntigo);

                                fseek(arquivo, 1, SEEK_SET);    //Coloca o encadeamento da pilha do registro removido no topo da pilha
                                fwrite(&encadeamenteTemp, sizeof(int64_t), 1, arquivo);
                                cabecalho.topoLista = encadeamenteTemp;
                                cabecalho.nroRegRem--;
                                fseek(arquivo, 17, SEEK_SET);
                                fwrite(&cabecalho.nroRegRem, sizeof(int), 1, arquivo);
                            }
                        }
                    }
                    free_register(&registro);
                    registro.nomePoPs = NULL;
                    registro.nomePais = NULL;
                }
            }
        }
        // Caso 2: Busca não envolve idPoPs -> utiliza busca sequencial a partir do byte 25
        else
        {
            int64_t offset = 25;
            fseek(arquivo, 25, SEEK_SET);
            while (read_register(&registro, arquivo))
            {
                if (registro.removido == '1')
                {
                    offset += registro.tamanhoRegistro;
                    continue;
                }
                int tamanhoRegistroPreFiltro = registro.tamanhoRegistro;

                if (matches_filter(&registro, &filtroBusca, buscaId, buscaNomePoPs, buscaNomePais, buscaSiglaPais))
                {
                        int idAntigo = registro.idPoPs; // Guarda para o índice
                        int tamAntigo = registro.tamanhoRegistro;

                        //Atualiza o registro
                        if (atualizaRegistro.idPoPs != -1) registro.idPoPs = atualizaRegistro.idPoPs;
                        if (atualizaRegistro.nomePoPs != NULL) {
                            if (registro.nomePoPs != NULL) free(registro.nomePoPs);
                            registro.nomePoPs = malloc(strlen(atualizaRegistro.nomePoPs) + 1);
                            strcpy(registro.nomePoPs, atualizaRegistro.nomePoPs);
                        }
                        if (atualizaRegistro.nomePais != NULL) {
                            if (registro.nomePais != NULL) free(registro.nomePais);
                            registro.nomePais = malloc(strlen(atualizaRegistro.nomePais) + 1);
                            strcpy(registro.nomePais, atualizaRegistro.nomePais);
                        }
                        if (atualizaRegistro.siglaPais[0] != '@') {
                            strncpy(registro.siglaPais, atualizaRegistro.siglaPais, 2);
                        }

                        int tamPoPsFinal = (registro.nomePoPs != NULL) ? strlen(registro.nomePoPs) : 0;
                        int tamPaisFinal = (registro.nomePais != NULL) ? strlen(registro.nomePais) : 0;
                        int novoTamanhoDados = 19 + tamPoPsFinal + 1 + tamPaisFinal + 1;

                        //Verifica se cabe
                        int encaixa = (novoTamanhoDados <= tamAntigo) ? 1 : 0;
                        atualizaRegistro.tamanhoRegistro = novoTamanhoDados;

                        if(encaixa){    //Grava o registro atualizado no lugar onde ele estava
                            write_register(arquivo, registro, offset, strlen(registro.nomePoPs), strlen(registro.nomePais));
                            fseek(arquivo, offset + novoTamanhoDados, SEEK_SET);
                            char lixo = '$';
                            for(int k = atualizaRegistro.tamanhoRegistro; k < registro.tamanhoRegistro; k++){
                                fwrite(&lixo, sizeof(char), 1, arquivo);
                            }

                            // Se o ID mudou, reorganiza a chave no índice
                            if (registro.idPoPs != idAntigo) {
                                remover_do_indice_em_memoria(indice, &totalIndices, idAntigo);
                                adicionar_ao_indice_em_memoria(&indice, &totalIndices, registro.idPoPs, offset);
                            }

                        }else{  //Grava o registro onde ele caiba
                            Register antigoRegistro = registro;
                            antigoRegistro.tamanhoRegistro = tamAntigo;

                            //Deleta logicamente o registro
                            fseek(arquivo, offset, SEEK_SET);
                            antigoRegistro.removido = '1';
                            fwrite(&antigoRegistro.removido, sizeof(char), 1, arquivo);
                            
                            if (cabecalho.topoLista == -1) {
                                cabecalho.topoLista = offset;
                                fseek(arquivo, 1, SEEK_SET);
                                fwrite(&cabecalho.topoLista, sizeof(int64_t), 1, arquivo);
                            } else {
                                worstFit(arquivo, &antigoRegistro, 1, cabecalho.topoLista, offset);
                                fseek(arquivo, 1, SEEK_SET);
                                fread(&cabecalho.topoLista, sizeof(int64_t), 1, arquivo);
                            }

                            fseek(arquivo, offset + 13, SEEK_SET);
                            char lixo = '$';
                            for(int k = 13; k < registro.tamanhoRegistro; k++){
                                fwrite(&lixo, sizeof(char), 1, arquivo);
                            }

                            remover_do_indice_em_memoria(indice, &totalIndices, idAntigo);
                            cabecalho.nroRegRem++;
                            fseek(arquivo, 17, SEEK_SET); //Atualiza os removidos no disco
                            fwrite(&cabecalho.nroRegRem, sizeof(int), 1, arquivo);

                            //Insere o registro no lugar certo
                            registro.tamanhoRegistro = novoTamanhoDados;
                            int maiorRegRemovido = -1;
                            
                            if (cabecalho.topoLista != -1) {
                                fseek(arquivo, cabecalho.topoLista + 1, SEEK_SET);
                                fread(&maiorRegRemovido, sizeof(int), 1, arquivo);
                            }

                            if(maiorRegRemovido < registro.tamanhoRegistro){//Confere se nao cabe no registro removido do topo da pilha
                                write_register(arquivo, registro, cabecalho.proxByteOffset, strlen(registro.nomePoPs), strlen(registro.nomePais));

                                adicionar_ao_indice_em_memoria(&indice, &totalIndices, registro.idPoPs, cabecalho.proxByteOffset);
                                cabecalho.proxByteOffset += registro.tamanhoRegistro;
                                fseek(arquivo, 9, SEEK_SET);
                                fwrite(&cabecalho.proxByteOffset, sizeof(int64_t), 1, arquivo);
                            }else{
                                int64_t topoAntigo = cabecalho.topoLista;

                                //Altera o valor do tamanho do novo registro para o valor do registro removido
                                registro.tamanhoRegistro = maiorRegRemovido;

                                //Guarda o proximo registro removido
                                fseek(arquivo, cabecalho.topoLista + 5, SEEK_SET);
                                int64_t encadeamenteTemp = 0;
                                fread(&encadeamenteTemp, sizeof(int64_t), 1, arquivo);
                
                                write_register(arquivo, registro, cabecalho.topoLista, strlen(registro.nomePoPs), strlen(registro.nomePais));

                                fseek(arquivo, cabecalho.topoLista + novoTamanhoDados, SEEK_SET);
                                for (int k = novoTamanhoDados; k < maiorRegRemovido; k++) {
                                    fwrite(&lixo, sizeof(char), 1, arquivo);
                                }

                                adicionar_ao_indice_em_memoria(&indice, &totalIndices, registro.idPoPs, topoAntigo);

                                fseek(arquivo, 1, SEEK_SET);    //Coloca o encadeamento da pilha do registro removido no topo da pilha
                                fwrite(&encadeamenteTemp, sizeof(int64_t), 1, arquivo);
                                cabecalho.topoLista = encadeamenteTemp;
                                cabecalho.nroRegRem--;
                                fseek(arquivo, 17, SEEK_SET);
                                fwrite(&cabecalho.nroRegRem, sizeof(int), 1, arquivo);
                            }
                        }
                }

                offset += tamanhoRegistroPreFiltro;
                fseek(arquivo, offset, SEEK_SET);

                free_register(&registro);
                registro.nomePoPs = NULL; // Limpa para a próxima iteração
                registro.nomePais = NULL;
            }
        }


        
        free(filtroBusca.nomePoPs);
        free(filtroBusca.nomePais);
    }

    cabecalho.status = '1'; //Deixa o status do cabecalho como 1
    fseek(arquivo, 0, SEEK_SET);
    fwrite(&cabecalho.status, sizeof(char), 1, arquivo);

    fclose(arquivo);

    //Reescreve o indice da memoria reordenado no arquivo
    arqIndice = fopen(arquivoIndice, "wb");

    if (arqIndice != NULL) {
        char status = '0';
        //Escreve o cabeçalho inconsistente
        fwrite(&status, sizeof(char), 1, arqIndice); 

        //Grava o vetor de índices atualizado
        for (int i = 0; i < totalIndices; i++) {
            fwrite(&(indice[i].idPoPs), sizeof(int), 1, arqIndice);
            fwrite(&(indice[i].byteoffset), sizeof(int64_t), 1, arqIndice);
        }

        //Atualiza o status para consistente e fecha
        status = '1';
        fseek(arqIndice, 0, SEEK_SET);
        fwrite(&status, sizeof(char), 1, arqIndice);
        
        fclose(arqIndice);
    }
    free(indice);



    BinarioNaTela(arquivoEntrada);
    BinarioNaTela(arquivoIndice);
}
