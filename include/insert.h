// NOME: Murilo Antonio de Oliveira Amatti. NroUSP: 17150041
// NOME: Vitor Quirino Machado. NroUSP: 16904644

#ifndef INSERT_H
#define INSERT_H

#include "register_header.h"

void write_register(FILE *arquivo, Register novoRegistro, int64_t posicao, int tamPoPs, int tamPais);

void adicionar_ao_indice_em_memoria(IndexEntry **indice, int *totalIndices, int idNovo, int64_t offsetNovo);

void insert_into(char *arquivoEntrada, char *arquivoIndice, int n);

#endif