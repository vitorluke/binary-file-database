#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "../include/delete.h"
#include "../include/select.h"
#include "../include/fornecidas.h"



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
 * Função: remover_do_indice_em_memoria
 * Objetivo: Localiza e remove um ID do vetor de índices, deslocando os demais elementos.
 */
void remover_do_indice_em_memoria(IndexEntry *indice, int *totalIndices, int idRemovido) {
    int pos = -1;
    int esq = 0, dir = *totalIndices - 1;

    // Busca binária para achar o índice no array
    while (esq <= dir) {
        int meio = esq + (dir - esq) / 2;
        if (indice[meio].idPoPs == idRemovido) {
            pos = meio;
            break;
        } else if (indice[meio].idPoPs < idRemovido) {
            esq = meio + 1;
        } else {
            dir = meio - 1;
        }
    }

    // Se encontrou, desloca os elementos 1 posição para a esquerda
    if (pos != -1) {
        for (int i = pos; i < *totalIndices - 1; i++) {
            indice[i] = indice[i + 1];
        }
        (*totalIndices)--; // Reduz o tamanho lógico do vetor
    }
}

//Algoritmo WorstFit para o encadeamento da lista de dados removidos
void worstFit(FILE *arquivo, Register *registro, int64_t posEndereco, int64_t posRegistro, int64_t offset){
    int tamRegistro = 0;
    fseek(arquivo, posRegistro + 1, SEEK_SET);
    fread(&tamRegistro, sizeof(int), 1, arquivo);
    if(tamRegistro <= registro->tamanhoRegistro){
        registro->encadeamentoLista = posRegistro;
        fseek(arquivo, offset + 5, SEEK_SET);
        fwrite(&(registro->encadeamentoLista), sizeof(int64_t), 1, arquivo);

        fseek(arquivo, posEndereco, SEEK_SET);
        fwrite(&offset, sizeof(int64_t), 1, arquivo);
    }
    else{
        int64_t encadeamentoLista;
        fread(&encadeamentoLista, sizeof(int64_t), 1, arquivo);
        if(encadeamentoLista == -1){
            fseek(arquivo, posRegistro + 5, SEEK_SET);
            fwrite(&offset, sizeof(int64_t), 1, arquivo);
        }
        else{
            worstFit(arquivo, registro, posRegistro + 5, encadeamentoLista, offset);
        }
    }


}

void delete_register(char *arquivoEntrada, char *arquivoIndice, int n){
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



                        //Deleta logicamente o registro
                        fseek(arquivo, offset, SEEK_SET);
                        registro.removido = '1';
                        fwrite(&registro.removido, sizeof(char), 1, arquivo);

                        if(cabecalho.topoLista == -1){  //Confere se tem algum registro ja removido
                            cabecalho.topoLista = offset;
                            fseek(arquivo, 1, SEEK_SET);    //Coloca a posicao do registro no topo da lista
                            fwrite(&cabecalho.topoLista, sizeof(int64_t), 1, arquivo);

                        }
                        else{

                            worstFit(arquivo, &registro, 1, cabecalho.topoLista, offset);

                            fseek(arquivo, 1, SEEK_SET);    //Atualiza na memoria o topo da lista 
                            fread(&cabecalho.topoLista, sizeof(int64_t), 1, arquivo);

                        }
                        fseek(arquivo, offset + 13, SEEK_SET);
                        char lixo = '$';
                        for(int i = 13; i < registro.tamanhoRegistro; i++){
                            fwrite(&lixo, sizeof(char), 1, arquivo);
                        }

                        //Atualiza o cabecalho
                        cabecalho.nroRegRem += 1;
                        cabecalho.nroPoPs -= 1;
                        fseek(arquivo, 17, SEEK_SET);
                        fwrite(&cabecalho.nroRegRem, sizeof(int), 1, arquivo);
                        fwrite(&cabecalho.nroPoPs, sizeof(int), 1, arquivo);

                        //Remove o indice do index na memoria
                        remover_do_indice_em_memoria(indice, &totalIndices, registro.idPoPs);
                    }

                    free_register(&registro);
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
                    free_register(&registro);
                    continue;
                }

                if (matches_filter(&registro, &filtroBusca, buscaId, buscaNomePoPs, buscaNomePais, buscaSiglaPais))
                {


                        //Deleta logicamente o registro
                        fseek(arquivo, offset, SEEK_SET);
                        registro.removido = '1';
                        fwrite(&registro.removido, sizeof(char), 1, arquivo);

                        if(cabecalho.topoLista == -1){  //Confere se tem algum registro ja removido
                            cabecalho.topoLista = offset;
                            fseek(arquivo, 1, SEEK_SET);    //Coloca a posicao do registro no topo da lista
                            fwrite(&cabecalho.topoLista, sizeof(int64_t), 1, arquivo);

                        }
                        else{

                            worstFit(arquivo, &registro, 1, cabecalho.topoLista, offset);

                            fseek(arquivo, 1, SEEK_SET);    //Atualiza na memoria o topo da lista 
                            fread(&cabecalho.topoLista, sizeof(int64_t), 1, arquivo);

                        }
                        fseek(arquivo, offset + 13, SEEK_SET);
                        char lixo = '$';
                        for(int i = 13; i < registro.tamanhoRegistro; i++){
                            fwrite(&lixo, sizeof(char), 1, arquivo);
                        }

                        //Atualiza o cabecalho
                        cabecalho.nroRegRem += 1;
                        cabecalho.nroPoPs -= 1;
                        fseek(arquivo, 17, SEEK_SET);
                        fwrite(&cabecalho.nroRegRem, sizeof(int), 1, arquivo);
                        fwrite(&cabecalho.nroPoPs, sizeof(int), 1, arquivo);

                        //Remove o indice do index na memoria
                        remover_do_indice_em_memoria(indice, &totalIndices, registro.idPoPs);
                }
                offset += registro.tamanhoRegistro;
                fseek(arquivo, offset, SEEK_SET);

                free_register(&registro);
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