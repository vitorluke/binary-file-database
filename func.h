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

void func1(char *arquivoEntrada, char *arquivoSaida);
void func2(char *arquivoEntrada);
void func3(char *arquivoEntrada);
void func4(char *arquivoEntrada);
void func5(char *arquivoEntrada);
void func6(char *arquivoEntrada);
void func7(char *arquivoEntrada);


#endif