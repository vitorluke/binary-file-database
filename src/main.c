// NOME: Murilo Antonio de Oliveira Amatti. NroUSP: 17150041
// NOME: Vitor Quirino Machado. NroUSP: 16904644

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../include/table.h"
#include "../include/select.h"
#include "../include/index.h"
#include "../include/delete.h"
#include "../include/insert.h"
#include "../include/update.h"


#define BUFFER 512

int main()
{
    char buffer[BUFFER];
    char arquivoEntrada[BUFFER];
    char arquivoSaida[BUFFER];
    char arquivoIndice[BUFFER];
    int func_option, n;

    // Lê a linha de comando inicial da entrada padrão
    if (fgets(buffer, sizeof(buffer), stdin) == NULL)
        return 0;

    // Remove quebras de linha (\n, \r) do final da string
    for (int i = 0; buffer[i] != '\0'; i++)
        if (buffer[i] < 32)
            buffer[i] = '\0';

    sscanf(buffer, "%d", &func_option);

    switch (func_option)
    {
        case 8: // 8 arquivoEntrada.csv arquivoSaida.bin[cite: 8]
            sscanf(buffer, "%d %s %s", &func_option, arquivoEntrada, arquivoSaida);
            create_table(arquivoEntrada, arquivoSaida);
            break;

        case 9: // 9 arquivoEntrada.bin arquivoIndicePrimario.bin[cite: 9]
            sscanf(buffer, "%d %s %s", &func_option, arquivoEntrada, arquivoIndice);
            create_index(arquivoEntrada, arquivoIndice);
            break;

        case 10: // 10 arquivoEntrada.bin[cite: 11]
            sscanf(buffer, "%d %s", &func_option, arquivoEntrada);
            select_table(arquivoEntrada);
            break;

        case 11: // 11 arquivoEntrada.bin arquivoIndicePrimario.bin n[cite: 13]
            sscanf(buffer, "%d %s %s %d", &func_option, arquivoEntrada, arquivoIndice, &n);
            select_where(arquivoEntrada, arquivoIndice, n);
            break;

        case 12: // 12 arquivoEntrada.bin arquivoIndicePrimario.bin n[cite: 16]
            sscanf(buffer, "%d %s %s %d", &func_option, arquivoEntrada, arquivoIndice, &n);
            delete_register(arquivoEntrada, arquivoIndice, n);
            break;

        case 13: // 13 arquivoEntrada.bin arquivoIndicePrimario.bin n[cite: 18]
            sscanf(buffer, "%d %s %s %d", &func_option, arquivoEntrada, arquivoIndice, &n);
            insert_into(arquivoEntrada, arquivoIndice, n);
            break;

        case 14: // 14 arquivoEntrada.bin arquivoIndicePrimario.bin n[cite: 20]
            sscanf(buffer, "%d %s %s %d", &func_option, arquivoEntrada, arquivoIndice, &n);
            update_register(arquivoEntrada, arquivoIndice, n);
            break;

        default:
            printf("Falha no processamento do arquivo.\n");
            break;
    }

    return 0;
}