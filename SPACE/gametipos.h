
#ifndef _GAMETIPOS_H_
#define	_GAMETIPOS_H_


#include <stdio.h>

//Biblioteca ALLEGRO
#include <allegro5/allegro5.h>          //Allegro5 (BASE)
#include <allegro5/allegro_image.h>     //Imagens

//--------- ESTRUTURAS E TIPOS
typedef struct {
    int posicao[2];     //Posi��o do Player na tela ([0] = x, [1] = y)
    int tamanho[2];     //Tamanho desse obj {[0] = LARGURA, [1] = ALTURA}
    char ativo;         //Se esta ativo/vivo ou n�o
    char tipo;          //Tipo da Nave
    char tipoTiro;      //Tipo do Tiro
    int vida;           //HP Atual
    int vidaMax;        //Hp max (para calculos)
    int dano;           //Dano do tiro
    ALLEGRO_BITMAP *img;    //Imagem do player
    ALLEGRO_BITMAP *hpBar;  //Imagem Moldura do HP
    ALLEGRO_BITMAP *hp;     //Imagem do Hp mesmo
} Player;

typedef struct {
    int posicao[2];     //Posi��o do Mob na tela ([0] = x, [1] = y)
    int tamanho[2];     //Tamanho desse obj {[0] = LARGURA, [1] = ALTURA}
    char ativo;         //Se esse mob est� ativo ou n�o
    char tipo;          //Tipo do mob
    int vida;           //HP do mob
    int dano;           //Dano desse mob
    ALLEGRO_BITMAP *img;    //Imagem do Mob
} Mob;

typedef struct {
    Mob mobs[100];      //Mobs desse grupo (At� 100 mobs)
    int posicao[2];     //POSICAO DO GRUPO*
    int qtd;            //Quantidade de mobs ativos
} Mobs;

typedef struct {
    char ativo;         //Indica se est� ativo ou n�o
    int posicao[2];     //Posi��o do tiro na tela ([0] = x, [1] = y)
    int tamanho[2];     //Tamanho desse obj {[0] = LARGURA, [1] = ALTURA}
    int deQuem;         //Indica de quem � esse tiro
    int dano;           //Dano desse tiro
    int speed;          //Velocidade
    ALLEGRO_BITMAP *img;        //Dependendo de quem for, muda a imagem
} Tiro;


#endif
