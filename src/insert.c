#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "../include/insert.h"
#include "../include/fornecidas.h"


// Estrutura auxiliar para armazenar uma entrada do arquivo de índice primário (12 bytes)
typedef struct {
    int idPoPs;         // Chave primária de busca (4 bytes)
    int64_t byteoffset; // Byte offset do registro no arquivo de dados (8 bytes)
} IndexEntry;

/*
 * Função: adicionar_ao_indice_em_memoria
 * Objetivo: Realoca o vetor de índices e insere o novo ID na posição 
 *           correta para manter a ordenação crescente.
 */
void adicionar_ao_indice_em_memoria(IndexEntry **indice, int *totalIndices, int idNovo, int64_t offsetNovo) {
    // Realoca a memória para acomodar o novo elemento
    *indice = realloc(*indice, (*totalIndices + 1) * sizeof(IndexEntry));
    
    // Começa do último elemento e vai "puxando" para a direita os IDs que são maiores que o novo
    int i = *totalIndices - 1;
    while (i >= 0 && (*indice)[i].idPoPs > idNovo) {
        (*indice)[i + 1] = (*indice)[i];
        i--;
    }
    
    // Salva o novo registro na posição correta que ficou vazia
    (*indice)[i + 1].idPoPs = idNovo;
    (*indice)[i + 1].byteoffset = offsetNovo;
    
    // Incrementa o contador lógico do vetor
    (*totalIndices)++;
}

void write_register(FILE *arquivo, Register novoRegistro, int64_t posicao, int tamPoPs, int tamPais){
    char delim = '|';


    fseek(arquivo, posicao, SEEK_SET);

    // Grava campo a campo no arquivo
    fwrite(&novoRegistro.removido, sizeof(char), 1, arquivo);
    fwrite(&novoRegistro.tamanhoRegistro, sizeof(int), 1, arquivo);
    fwrite(&novoRegistro.encadeamentoLista, sizeof(int64_t), 1, arquivo);
    fwrite(&novoRegistro.idPoPs, sizeof(int), 1, arquivo);
    fwrite(novoRegistro.siglaPais, sizeof(char), 2, arquivo);

    fwrite(novoRegistro.nomePoPs, sizeof(char), tamPoPs, arquivo);
    fwrite(&delim, sizeof(char), 1, arquivo);

    fwrite(novoRegistro.nomePais, sizeof(char), tamPais, arquivo);
    fwrite(&delim, sizeof(char), 1, arquivo);
}



void insert_into(char *arquivoEntrada, char *arquivoIndice, int n){
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

    cabecalho.status = '0'; //Deixa o status do cabecalho como 0
    fseek(arquivo, 0, SEEK_SET);
    fwrite(&cabecalho.status, sizeof(char), 1, arquivo);

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


    

    // Insere os n novos registros
    for (int i = 1; i <= n; i++)
    {
        Register novoRegistro;

        // Inicialização do registro atual
        novoRegistro.removido = '0';
        novoRegistro.encadeamentoLista = -1;
        novoRegistro.idPoPs = -1;
        novoRegistro.nomePoPs = NULL;
        novoRegistro.nomePais = NULL;
        novoRegistro.siglaPais[0] = '@';


        //Le os valores fornecidos
        char idPoPs[1000], nomePoPs[1000], nomePais[1000], siglaPais[1000];
        
        scanf("%s", idPoPs);
        novoRegistro.idPoPs = atoi(idPoPs);

        ScanQuoteString(nomePoPs);
        if (strcmp(nomePoPs, "NULO") == 0 || strcmp(nomePoPs, "") == 0) {
            novoRegistro.nomePoPs = malloc(1);
            novoRegistro.nomePoPs[0] = '\0';
        } else {
            novoRegistro.nomePoPs = malloc(strlen(nomePoPs) + 1);
            strcpy(novoRegistro.nomePoPs, nomePoPs);
        }

        ScanQuoteString(nomePais);
        if (strcmp(nomePais, "NULO") == 0 || strcmp(nomePais, "") == 0) {
            novoRegistro.nomePais = malloc(1);
            novoRegistro.nomePais[0] = '\0';
        } else {
            novoRegistro.nomePais = malloc(strlen(nomePais) + 1);
            strcpy(novoRegistro.nomePais, nomePais);
        }

        ScanQuoteString(siglaPais);
        if (strcmp(siglaPais, "NULO") == 0 || strcmp(siglaPais, "") == 0) {
            novoRegistro.siglaPais[0] = '$';
            novoRegistro.siglaPais[1] = '$';
        } else {
            strncpy(novoRegistro.siglaPais, siglaPais, 2);
        }

        

        //Mede o tamanho do novo registro
        int tamPoPs = strlen(novoRegistro.nomePoPs);
        int tamPais = strlen(novoRegistro.nomePais);
        novoRegistro.tamanhoRegistro = 19 + tamPoPs + 1 + tamPais + 1;

        if(cabecalho.topoLista == -1){  //Confere se nao tem registro removido
            write_register(arquivo, novoRegistro, cabecalho.proxByteOffset, tamPoPs, tamPais);

            adicionar_ao_indice_em_memoria(&indice, &totalIndices, novoRegistro.idPoPs, cabecalho.proxByteOffset);

            cabecalho.proxByteOffset += novoRegistro.tamanhoRegistro;
            cabecalho.nroPoPs++;

            //Atualiza o cabecalho no arquivo
            fseek(arquivo, 9, SEEK_SET);
            fwrite(&cabecalho.proxByteOffset, sizeof(int64_t), 1, arquivo);

        }
        else{
            int maiorRegRemovido = -1;
            fseek(arquivo, cabecalho.topoLista + 1, SEEK_SET);
            fread(&maiorRegRemovido, sizeof(int), 1, arquivo);

            //Confere se no registro removido do topo da pilha nao cabe o novo registro
            if(maiorRegRemovido < novoRegistro.tamanhoRegistro){    
                write_register(arquivo, novoRegistro, cabecalho.proxByteOffset, tamPoPs, tamPais);

                adicionar_ao_indice_em_memoria(&indice, &totalIndices, novoRegistro.idPoPs, cabecalho.proxByteOffset);

                cabecalho.proxByteOffset += novoRegistro.tamanhoRegistro;
                cabecalho.nroPoPs++;

                //Atualiza o cabecalho no arquivo
                fseek(arquivo, 9, SEEK_SET);
                fwrite(&cabecalho.proxByteOffset, sizeof(int64_t), 1, arquivo);
            }
            else{
                int antigoTamReg = novoRegistro.tamanhoRegistro;
                //Altera o valor do tamanho do novo registro para o valor do registro removido
                novoRegistro.tamanhoRegistro = maiorRegRemovido;

                //Guarda o proximo registro removido
                fseek(arquivo, cabecalho.topoLista + 5, SEEK_SET);
                int64_t encadeamenteTemp = 0;
                fread(&encadeamenteTemp, sizeof(int64_t), 1, arquivo);
                
                write_register(arquivo, novoRegistro, cabecalho.topoLista, tamPoPs, tamPais);

                //Coloca o lixo de memoria do registro removido como $
                fseek(arquivo, cabecalho.topoLista + antigoTamReg, SEEK_SET);
                char lixo = '$';
                for (int k = antigoTamReg; k < maiorRegRemovido; k++) {
                    fwrite(&lixo, sizeof(char), 1, arquivo);
                }

                adicionar_ao_indice_em_memoria(&indice, &totalIndices, novoRegistro.idPoPs, cabecalho.topoLista);

                fseek(arquivo, 1, SEEK_SET);    //Coloca o encadeamento da pilha do registro removido no topo da pilha
                fwrite(&encadeamenteTemp, sizeof(int64_t), 1, arquivo);
                cabecalho.topoLista = encadeamenteTemp;

                cabecalho.nroPoPs++;

                //Atualiza o numero de registros removidos
                cabecalho.nroRegRem--;
                fseek(arquivo, 17, SEEK_SET);
                fwrite(&cabecalho.nroRegRem, sizeof(int), 1, arquivo);
            }
        }

        //Atualiza o numero de registros
        fseek(arquivo, 21, SEEK_SET);
        fwrite(&cabecalho.nroPoPs, sizeof(int), 1, arquivo);

        free(novoRegistro.nomePoPs);
        free(novoRegistro.nomePais);
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