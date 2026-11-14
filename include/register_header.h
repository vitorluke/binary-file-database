#ifndef REGISTER_HEADER_H
#define REGISTER_HEADER_H

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

void read_header(Header *cabecalho, FILE *arquivo);
void read_register(Register *registro, FILE *arquivo);


#endif