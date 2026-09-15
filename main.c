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
    int func_option;

    fgets(buffer,sizeof(buffer), stdin);
    for(int i = 0; buffer[i] != '\0'; i++)if(buffer[i]<32)buffer[i]='\0';
    
    sscanf(buffer, "%d %s %s", &func_option, arquivoEntrada, arquivoSaida);
    switch(func_option){
        case 1:
          func1(arquivoEntrada,arquivoSaida);
          break;
        case 2:
          func2(arquivoEntrada);
          break;
        case 3:
          func3(arquivoEntrada);
          break;
        case 4:
          func4(arquivoEntrada);
          break;
        case 5:
          func5(arquivoEntrada);
          break;
        case 6:
          func6(arquivoEntrada);
          break;
        case 7:
          func7(arquivoEntrada);
          break;
        default:
          printf("ERROR: func_option out of range");
          return -1;
    }
    
    return 0;
}