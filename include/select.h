// NOME: Murilo Antonio de Oliveira Amatti. NroUSP: 17150041
// NOME: Vitor Quirino Machado. NroUSP: 16904644

#ifndef SELECT_H
#define SELECT_H

#include "register_header.h"



// Exibe os campos de um registro formatados na saída padrão
void print_register(Register *registro);

// Libera a memória alocada dinamicamente para os campos variáveis de um registro
void free_register(Register *registro);

// Funcionalidade [10]: Recupera e exibe todos os registros válidos do arquivo de dados
void select_table(char *nomeArquivo);

// Funcionalidade [11]: Recupera e exibe registros que satisfazem critérios de busca (com índice primário para idPoPs)
void select_where(char *arquivoEntrada, char *arquivoIndice, int n);

#endif