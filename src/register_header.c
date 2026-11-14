// NOME: Murilo Antonio de Oliveira Amatti. NroUSP: 17150041
// NOME: Vitor Quirino Machado. NroUSP: 16904644

#include "../include/register_header.h"
#include <stdlib.h>
#include <stdio.h>

//Função para ler o campo de tamanho variavel delimitador por PIPE('|')
char *read_string(FILE *arquivo)
{
    char *texto = malloc(1);
    int tamanho = 0;
    int caractere;

    // Loop de leitura
    while ((caractere = fgetc(arquivo)) != '|')
    {
        texto = realloc(texto, tamanho + 2);

        texto[tamanho] = caractere;
        tamanho++;
    }

    texto[tamanho] = '\0';

    return texto;//Retorna a string no fim da função
}

//Função de leitura de cabeçalho
void read_header(Header *cabecalho, FILE *arquivo)
{
    fread(&cabecalho->status, sizeof(char), 1, arquivo);
    fread(&cabecalho->topoLista, sizeof(int64_t), 1, arquivo);
    fread(&cabecalho->proxByteOffset, sizeof(int64_t), 1, arquivo);
    fread(&cabecalho->nroRegRem, sizeof(int), 1, arquivo);
    fread(&cabecalho->nroPoPs, sizeof(int), 1, arquivo);
}

//Função de leitura de registro
int read_register(Register *registro, FILE *arquivo)
{
    if (fread(&registro->removido, sizeof(char), 1, arquivo) != 1){
        return 0;
    }
    if (fread(&registro->tamanhoRegistro, sizeof(int), 1, arquivo) != 1){
        return 0;
    }
    //Pula o registro removido logicamente
    if(registro->removido == '1'){
        registro->nomePoPs = NULL; // Evita Segmentation Fault quando select.c chamar free_registers
        registro->nomePais = NULL;
        fseek(arquivo,registro->tamanhoRegistro - (sizeof(char) + sizeof(int)),SEEK_CUR);
        return 1;
    }
    if (fread(&registro->encadeamentoLista, sizeof(int64_t), 1, arquivo) != 1){
        return 0;
    }

    if(fread(&registro->idPoPs, sizeof(int), 1, arquivo) != 1){
        return 0;
    }
    
    if(fread(registro->siglaPais, sizeof(char), 2, arquivo) != 2){
        return 0;
    }

    // Os campos nomePoPs e nomePais são variáveis.
    // A leitura será feita até encontrar o delimitador '|'.
    registro->nomePoPs = read_string(arquivo);
    registro->nomePais = read_string(arquivo);

    return 1;
}

