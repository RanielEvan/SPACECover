#include <stdio.h>
#include <locale.h>

#ifndef _PERSIS_H_
#define	_PERSIS_H_

typedef struct {
    int posicao;
    char nome[40];
    int pontos;
} pRank;


FILE *arquivo; //Arquivo..

char rankingAtual[250]; //String inteira do Arquivo
char *ranks[10]; //Destrinchando o arquivo...

pRank ranking[10]; //Rankings do arquivo (organizados)
pRank novoRanking; //Que será salvo..
char novoRankingStr[250] = ""; //String a ser escrita no arquivo...

int i, o, rk = 0;
char *sep;
int maiorPonto = 0, menorPonto = 999; //Armazena o maior/menor ponto... Usaremos para organizar o podium


void SalvarPontuacao(char * nome, int pontuacao)
{
    setlocale(LC_ALL,"");

    //Ler o arquivo de pontuação..
    LerRanking();

    //Depois incluimos os novos Pontos...
    EscreverRanking(nome, pontuacao);
}



void LerRanking(){

    rk = 0;

    //Abre e le o arquivo
    arquivo=fopen("rank.pqp", "a+");
    fgets(rankingAtual, 250, arquivo);
    //printf("\n\nARQUIVO RAW:\n%s\n", rankingAtual);

    //Separa os Ranks, e armazena na lista temporária de Rankings..
    sep = strtok(rankingAtual, "#"); //Char Separador
    while (sep != NULL)
    {
        //printf ("\n%s",sep);
        ranks[rk] = sep;
        rk++;

        sep = strtok (NULL, "#");
    }

    printf("\n\t%d Rankings Lidos do Arquivo\n", rk);


    //Leitura e armazenagem na ESTRUTURA
    for(i=0; i<rk; i++){ //Para cada Rank

        //Variaveis temporarias
        int nomeInd = 0;
        char nomeJog[40];
        char pts[10];

        //Vamos separar os PONTOS do Nome
        for(o=0; o<50;o++){

            if(ranks[i][o] == '\0'){ //Fim da string? Acaba esse loop.

                strncpy(nomeJog, ranks[i]+nomeInd, o); //Separa a parte do nome...

                //ranking[i].nome = nomeJog;
                sprintf(ranking[i].nome, "%s", nomeJog); //Salva o nome neste ranking

                if(ranking[i].pontos > maiorPonto)
                    maiorPonto = ranking[i].pontos; //Salva o maior ponto.
                if(ranking[i].pontos < menorPonto && ranking[i].pontos > 0){
                    menorPonto = ranking[i].pontos; //Salva o menor ponto.
                }

                //printf("\n\t%s = %d", ranking[i].nome, ranking[i].pontos);
                break;
            } else if(ranks[i][o] == '-'){ //SEPARA OS PONTOS DO NOME

                memset(pts, 0, sizeof(pts));
                strncpy(pts, ranks[i], o); //Copia a parte numerica em outra string...
                ranking[i].pontos = atoi(pts); //Converte a string para NUMERO e armazena no array de pontos

                nomeInd = o+1; //Salva a posicao do inicio do nome
            }
        }
    }

    if(rk > 5){
        rk = 5; //Limita o Ranking a 5 posicoes, poode ser de qualquer tamanho.. O game so exibe 5 no Ranking
    }

    //RANKINGS LIDOS E ORGANIZADOS...
}


void EscreverRanking(char *nome, int pts){

    //Salva o nome/pontos
    sprintf(novoRanking.nome, "%s", nome);
    //strcpy(novoRanking.nome, nome);
    novoRanking.pontos = pts;

    //INCLUI A PONTUACAO ATUAL
    sprintf(ranking[rk].nome, "%s", novoRanking.nome);
    ranking[rk].pontos = novoRanking.pontos;
    printf("\n (NOVA PONTUACAO)\n %d \t %s \t %d pontos (%d)", ranking[rk].posicao, ranking[rk].nome, ranking[rk].pontos, rk);

    int pos = 1; //Começamos na maior posicao (rk+1, pois a posicao 0 não conta no ranking)

    printf("\n\nSALVANDO PONTOS...\n");

    if(novoRanking.pontos > maiorPonto){
        maiorPonto = novoRanking.pontos; //Se ele passar o maior ponto, define como maior ponto
    } else if(novoRanking.pontos < menorPonto ){
        menorPonto = novoRanking.pontos; //Se ele tiver o menor ponto, define como menor ponto
    }

    //Mostra qual foi o maior/menor ponto processado.
    printf("\nMaior Ponto = %d\nMenor Ponto = %d", maiorPonto, menorPonto);

    for(i=maiorPonto; i>=0 ;i--){ //Fazemos um LOOP começando de 0 Pontos, até o ponto maximo. Com isso vamos setando as posições
        for(o=0; o<=rk; o++){
            if(ranking[o].pontos == i){

                ranking[o].posicao = pos;
                printf("\n %d \t %s \t %d pontos (%d)", ranking[o].posicao, ranking[o].nome, ranking[o].pontos, pos);
                pos++;

                char *tempRank[50];
                sprintf(tempRank, "%d-%s#", ranking[o].pontos, ranking[o].nome);
                strcat(novoRankingStr, tempRank);
            }
        }
    }


    //Finalmente Escreve no arquivo .pqp
    arquivo=fopen("rank.pqp", "w+");
    fprintf(arquivo, novoRankingStr);
    fflush(arquivo);


    printf("\n\n\tPONTUACAO SALVA COM SUCESSO NO ARQUIVO 'rank.pqp'\n\n");
    //RANK ESCRITO E FIM..

    LerRanking(); //Para atualizar o Ranking In Game

}



#endif
