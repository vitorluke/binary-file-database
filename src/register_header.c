
#include "../include/register_header.h"

void read_header(Header *cabecalho, FILE *arquivo)
{
    fread(&cabecalho->status, sizeof(char), 1, arquivo);
    fread(&cabecalho->topoLista, sizeof(int64_t), 1, arquivo);
    fread(&cabecalho->proxByteOffset, sizeof(int64_t), 1, arquivo);
    fread(&cabecalho->nroRegRem, sizeof(int), 1, arquivo);
    fread(&cabecalho->nroPoPs, sizeof(int), 1, arquivo);
}

void read_register(Register *registro, FILE *arquivo)
{
    fread(&registro->removido, sizeof(char), 1, arquivo);
    fread(&registro->tamanhoRegistro, sizeof(int), 1, arquivo);
    fread(&registro->encadeamentoLista, sizeof(int64_t), 1, arquivo);

    fread(&registro->idPoPs, sizeof(int), 1, arquivo);
    fread(registro->siglaPais, sizeof(char), 2, arquivo);

    // Os campos nomePoPs e nomePais são variáveis.
    // A leitura será feita até encontrar o delimitador '|'.
}
