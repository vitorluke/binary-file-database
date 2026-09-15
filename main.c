#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define IN "arquivoEntrada.csv"
#define OUT "arquivoSaida.bin"
#define BUFFER 40

int main()
{
    //Seleção de Funcionalidade
    char buffer[BUFFER];
    int func_option;

    fgets(buffer,sizeof(buffer), stdin);
    for(int i = 0; buffer[i] != '\0'; i++)if(buffer[i]<32)buffer[i]='\0';
    func_option = atoi(buffer);
    
    switch(func_option){
        case 1:
          break;
        case 2:
          break;
        case 3:
          break;
        case 4:
          break;
        case 5:
          break;
        case 6:
          break;
        case 7:
          break;
        default:
          printf("ERROR: func_option out of range");
          return -1;
    }
    
    return 0;
}