// NOME: Murilo Antonio de Oliveira Amatti. NroUSP: 17150041
// NOME: Vitor Quirino Machado. NroUSP: 16904644

#ifndef DELETE_H
#define DELETE_H

#include "register_header.h"

void delete_register(char *arquivoEntrada, char *arquivoIndice, int n);

void remover_do_indice_em_memoria(IndexEntry *indice, int *totalIndices, int idRemovido);

void worstFit(FILE *arquivo, Register *registro, int64_t posEndereco, int64_t posRegistro, int64_t offset);

#endif