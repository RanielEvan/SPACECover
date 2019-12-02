#include <stdio.h>
#include <locale.h>

#ifndef _PERSIS_H_
#define	_PERSIS_H_

FILE *arquivo;
char rankingAtual[250]; //Ranking do arquivo...
char *ranks[5];

char *rankNomes[5];
int rankPontos[5];

char novoRanking[250]; //Novo Ranking a ser salvo...



void SalvarPontuacao(char * nome, int pontuacao)
{
    setlocale(LC_ALL,"");

    char temp[50];
    sprintf(temp, "-%d %s\n", nome, pontuacao);

//    LerRanking();
//    EscreverRanking();

}

void EscreverRanking(char * ranking){

    arquivo=fopen("rank.pqp", "w+");
    fprintf(arquivo, ranking);
    fflush(arquivo);
}


void LerRanking(){

    int i, o, rk = 0;
    char *sep;

    //Abre e le o arquivo
    arquivo=fopen("rank.pqp", "a+");
    fgets(rankingAtual, 250, arquivo);
    printf(rankingAtual);

    //Separa os Ranks, e armazena na lista de Rankings..
    sep = strtok(rankingAtual, "-"); //Char Separador
    while (sep != NULL)
    {
        printf ("\n%s",sep);
        ranks[rk] = sep;
        rk++;

        sep = strtok (NULL, "-");
    }

    for(i=0; i<5; i++){ //Para cada Rank

        //Variaveis temporarias
        int nomeInd = 0;
        char *nomeJog[40];
        char *pts[10];

        //Vamos separar os PONTOS do Nome
        for(o=0; o<50;o++){

            if(ranks[i][o] == '\0'){ //Fim da string? Acaba esse loop.

                strncpy(nomeJog, ranks[i]+nomeInd, o); //Armazena a parte do nome...
                rankNomes[i] = nomeJog; //Salva o nome no array de nomes.

                //printf("\n\t%s = %d", rankNomes[i], rankPontos[i]);
                break;
            }

            if(ranks[i][o] == ' '){ //SEPARA OS PONTOS DO NOME

                strncpy(pts, ranks[i], o); //Copia a parte numerica em outra string...
                //printf(temp);
                rankPontos[i] = atoi(pts); //Converte a string para NUMERO e armazena no array de pontos
                //printf("%d\n", rankPontos[i]);

                nomeInd = o+1; //Salva a posicao do inicio do nome
            }

        }

    }

}


#endif
