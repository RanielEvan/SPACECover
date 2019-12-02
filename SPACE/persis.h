#include <stdio.h>
#include <locale.h>

#ifndef _PERSIS_H_
#define	_PERSIS_H_

FILE *arquivo;

char rankingAtual[250]; //Ranking do arquivo...
char *ranks[5]; //Destrinchando o arquivo...

typedef struct {
    int posicao;
    char nome[40];
    int pontos;
} pRank;

pRank ranking[5];




void SalvarPontuacao(char * nome, int pontuacao)
{
    setlocale(LC_ALL,"");

    //Primeiro vamos LÊ o Ranking do Arquivo ja gerado..
    LerRanking();


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
    printf("ARQUIVO RAW:\n%s\n", rankingAtual);


    //Separa os Ranks, e armazena na lista de Rankings..
    sep = strtok(rankingAtual, "-"); //Char Separador
    while (sep != NULL)
    {
        //printf ("\n%s",sep);
        ranks[rk] = sep;
        rk++;

        sep = strtok (NULL, "-");
    }

    printf("\n\t%d Rankings Gravados\n", rk);


    int maiorPonto = 0, menorPonto = 999; //Armazena o maior/menor ponto... Usaremos para organizar o podium

    //Leitura e armazenagem na ESTRUTURA
    for(i=0; i<rk; i++){ //Para cada Rank

        //Variaveis temporarias
        int nomeInd = 0;
        char nomeJog[40];
        char *pts[10];

        //Vamos separar os PONTOS do Nome
        for(o=0; o<50;o++){

            if(ranks[i][o] == '\0'){ //Fim da string? Acaba esse loop.

                strncpy(nomeJog, ranks[i]+nomeInd, o); //Armazena a parte do nome...
                //ranking[i].nome = nomeJog; //Salva o nome no array de nomes.

                sprintf(ranking[i].nome, "%s", nomeJog);

                if(ranking[i].pontos > maiorPonto)
                    maiorPonto = ranking[i].pontos; //Armazena o maior ponto.
                else if(ranking[i].pontos < menorPonto && ranking[i].pontos > 0){
                    menorPonto = ranking[i].pontos; //Armazena o menor ponto.
                }

                //printf("\n\t%s = %d", ranking[i].nome, ranking[i].pontos);
                break;
            }

            if(ranks[i][o] == ' '){ //SEPARA OS PONTOS DO NOME

                strncpy(pts, ranks[i], o); //Copia a parte numerica em outra string...
                ranking[i].pontos = atoi(pts); //Converte a string para NUMERO e armazena no array de pontos

                nomeInd = o+1; //Salva a posicao do inicio do nome
            }
        }
    }


    int pos = rk; //Começamos na posicao 5
    for(i=0; i<= maiorPonto;i++){ //Fazemos um LOOP começando de 0 Pontos, até o ponto maximo. Com isso vamos setando as posições

        for(o=0; o<rk; o++){
            if(ranking[o].pontos == i){
                ranking[o].posicao = pos;
                pos--;
                printf("\n\t %d \t %s \t %d pontos", ranking[o].posicao, ranking[o].nome, ranking[o].pontos);
                break;
            }
        }
    }
}


#endif
