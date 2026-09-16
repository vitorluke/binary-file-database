#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "func.h"
#define BUFFER 40

int main()
{
    //Seleção de Funcionalidade
    char buffer[BUFFER];
    char arquivoEntrada[BUFFER];
    char arquivoSaida[BUFFER];
    int func_option,n,rnn;

    fgets(buffer,sizeof(buffer), stdin);
    for(int i = 0; buffer[i] != '\0'; i++)if(buffer[i]<32)buffer[i]='\0';
    
    sscanf(buffer, "%d", &func_option);
    switch(func_option){
        case 1:
          sscanf(buffer,"%d %s %s", &func_option, arquivoEntrada, arquivoSaida);
          create_table(arquivoEntrada,arquivoSaida);
      
          break;
        case 2:
          sscanf(buffer,"%d %s", &func_option, arquivoEntrada);
          select_table(arquivoEntrada);

          break;
        case 3:
          sscanf(buffer,"%d %s %d", &func_option, arquivoEntrada, &n);
          printf("3\n");

          break;
        case 4:
          sscanf(buffer,"%d %s %d", &func_option, arquivoEntrada, &rnn);
          busca_rrn(arquivoEntrada,rnn);
          
          break;
        case 5:
          sscanf(buffer,"%d %s %d", &func_option, arquivoEntrada, &n);
          printf("5\n");

          break;
        case 6:
          sscanf(buffer,"%d %s %d", &func_option, arquivoEntrada, &n);
          printf("6\n");

          break;
        case 7:
          sscanf(buffer,"%d %s %d", &func_option, arquivoEntrada, &n);
          printf("7\n");
     
          break;
        default:
          printf("ERROR: func_option out of range");
          return -1;
    }
    
    return 0;
}