#include <stdio.h>
#include <locale.h>

#ifndef _PERSIS_H_
#define	_PERSIS_H_

void SalvarPontos(char *_nome, int _pont, int _tiros){

    setlocale(LC_ALL,"");

	FILE *arquivo;
	char txt[128];

    arquivo=fopen("arq.txt","w+");
    scanf("%s",arquivo);
    fflush(arquivo);

    arquivo=fopen("arq.txt","a+");
    fgets(txt,128,arquivo);
    printf("%s",txt);

}

#endif
