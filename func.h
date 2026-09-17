#ifndef FUNC_H
#define FUNC_H

#include <stdio.h>

//Struct do Cabeçalho
typedef struct {
    char status;
    int topoPilha;
    int proxRRN;
    int nroRegRem;
    int nroPares;
} Header;

typedef struct {
    char removido;
    int encadeamentoPilha;
    int idPoPs;
    int idPopsConectado;
    int velocidade;
    char unidadeMedida;
} Register;

void create_table(char *arquivoEntrada, char *arquivoSaida);
void select_table(char *arquivoEntrada);
void select_where(char *arquivoEntrada,int n);
void busca_rrn(char *arquivoEntrada,int rnn);
void delete_register(char *arquivoEntrada,int n);
void insert_into(char *arquivoEntrada,int n);
void update(char *arquivoEntrada,int n);

#endif