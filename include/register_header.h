// NOME: Murilo Antonio de Oliveira Amatti. NroUSP: 17150041
// NOME: Vitor Quirino Machado. NroUSP: 16904644

#ifndef REGISTER_HEADER_H
#define REGISTER_HEADER_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>

// Struct do cabeçalho do arquivo PoPs
typedef struct {
    char status;
    int64_t topoLista;// O Tipo int64_t é um tipo de dado inteiro
    // que possui exatamente 64bits de tamanho, ou seja, 8 bytes
    int64_t proxByteOffset;
    int nroRegRem;
    int nroPoPs;
} Header;

// Struct de um registro do arquivo PoPs
typedef struct {
    char removido;
    int tamanhoRegistro;
    int64_t encadeamentoLista;

    int idPoPs;
    char siglaPais[2];

    char *nomePoPs;
    char *nomePais;
} Register;

void read_header(Header *cabecalho, FILE *arquivo);
int read_register(Register *registro, FILE *arquivo);


// Estrutura auxiliar para armazenar uma entrada do arquivo de índice primário (12 bytes)
typedef struct {
    int idPoPs;         // Chave primária de busca (4 bytes)
    int64_t byteoffset; // Byte offset do registro no arquivo de dados (8 bytes)
} IndexEntry;

#endif
