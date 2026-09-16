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
void func3(char *arquivoEntrada,int n);
void busca_rrn(char *arquivoEntrada,int rnn);
void func5(char *arquivoEntrada,int n);
void func6(char *arquivoEntrada,int n);
void func7(char *arquivoEntrada,int n);


#endif