#include "../include/register_header.h"

void read_header(Header *cabecalho, FILE *arquivo)
{
    fread(&cabecalho->status, sizeof(cabecalho->status),1,arquivo);
    fread(&cabecalho->topoPilha,sizeof(cabecalho->topoPilha),1,arquivo);
    fread(&cabecalho->proxRRN,sizeof(cabecalho->proxRRN),1,arquivo);
    fread(&cabecalho->nroRegRem,sizeof(cabecalho->nroRegRem),1,arquivo);
    fread(&cabecalho->nroPares,sizeof(cabecalho->nroPares),1,arquivo);
}

void read_register(Register *registro, FILE *arquivo)
{
    fread(&registro->encadeamentoPilha,sizeof(registro->encadeamentoPilha),1,arquivo);
    fread(&registro->idPoPs,sizeof(registro->idPoPs),1,arquivo);
    fread(&registro->idPopsConectado,sizeof(registro->idPopsConectado),1,arquivo);
    fread(&registro->velocidade,sizeof(registro->velocidade),1,arquivo);
    fread(&registro->unidadeMedida,sizeof(registro->unidadeMedida),1,arquivo);
}