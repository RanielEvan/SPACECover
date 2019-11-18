#include <stdio.h>

//Biblioteca ALLEGRO
#include <allegro5/allegro5.h>          //Allegro5 (BASE)
#include <allegro5/allegro_font.h>      //Textos & Fontes
#include <allegro5/allegro_image.h>     //Imagens
#include <allegro5/allegro_ttf.h>       //Permite uso de Fontes TTF
//#include <allegro5/allegro_native_dialog.h>

//---------- MACROS
#define TLARGURA 800        //LARGURA DA TELA
#define TALTURA 600         //ALTURA DA TELA
#define FPS 60              //Frames Por Segundo

#define QTDMOBS 40          //Qtd de 30 mobs
#define MOBSPORFILA 10      //10 mobs por fila
#define DISTANCIAMOBS 15    //Distancia entre mobs
#define MOBVIDA 10          //Vida dos mobs

#define MAXTIROS 30          //Max de tiros no jogo

//--------- ESTRUTURAS E TIPOS
typedef struct {
    int posicao[2];     //Posição do Player na tela ([0] = x, [1] = y)
    int tamanho[2];     //Tamanho desse obj {[0] = LARGURA, [1] = ALTURA}
    char ativo;         //Se esta ativo/vivo ou não
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
    int posicao[2];     //Posição do Mob na tela ([0] = x, [1] = y)
    int tamanho[2];     //Tamanho desse obj {[0] = LARGURA, [1] = ALTURA}
    char ativo;         //Se esse mob está ativo ou não
    char tipo;          //Tipo do mob
    int vida;           //HP do mob
    int dano;           //Dano desse mob
    ALLEGRO_BITMAP *img;    //Imagem do Mob
} Mob;

typedef struct {
    Mob mobs[QTDMOBS];  //Mobs desse grupo
    int posicao[2];     //POSICAO DO GRUPO*
    int qtd;            //Quantidade de mobs ativos
} Mobs;

typedef struct {
    char ativo;         //Indica se está ativo ou não
    int posicao[2];     //Posição do tiro na tela ([0] = x, [1] = y)
    int tamanho[2];     //Tamanho desse obj {[0] = LARGURA, [1] = ALTURA}
    int deQuem;         //Indica de quem é esse tiro
    int dano;           //Dano desse tiro
    int speed;          //Velocidade
    ALLEGRO_BITMAP *img;        //Dependendo de quem for, muda a imagem
} Tiro;


//---------- VARIAVEIS
int INGAME = 1;         //Controla o loop principal
int TELA = 0;           //TELA ATUAL (0 - Menu, 1-Game, 2-Ranking)
int desenha = 1;        //Indica se o loop redesenha as coisas

ALLEGRO_BITMAP *gameLogo = NULL; //Logo do game
int menu = 0;      //Menu atual no Menu Principal

ALLEGRO_EVENT evento;    //Escuta os eventos

Player player;          //Player
Mobs mobs;              //Grupo de Mobs
ALLEGRO_BITMAP *mobImg = NULL;  //Imagem dos Mobs
ALLEGRO_BITMAP *mobImg2 = NULL; //Imagem dos Mobs quando estiverem vida baixa


Tiro tiros[MAXTIROS];   //POOL DE TIROS (limita a quantidade de tiros no mapa e os tiros são reutilizados quando inativos)
ALLEGRO_BITMAP *tiroPlayer = NULL; //Imagens do tiro do player (varia de acordo com o tipo)
ALLEGRO_BITMAP *tiroMob = NULL;    //Imagem do tiro do Mob (varia de acordo com o stage)


ALLEGRO_BITMAP *bgimg = NULL;         //Background
int bgAltura;       //Altura da imagem de fundo
int bgPosY = 0;     //Usado para fazer o efeito de movimento do fundo
int bgTaxaMov = 5;    //Taxa de movimentação do fundo
int bgDirecao = 1;      //Direção de movimentação do fundo


int frameCount = 0; //CONTAGEM DE FRAMES

int moveDir = 0;    //DIREÇÃO DE MOVIMENTO DO PLAYER
int mobMoveDir = 1; //Direção de movimento dos mobs
int taxaAtMob = 8; //Taxa de FRAMES para que o mob atualize seu movimento
int taxaTiroMob = 20;   //Taxa de FRAMES para que o mob atire

int margemMovimento = 30;   //Pixels da Margem da tela que o player e mobs podem se movimentar.



//---------- VARIAVEIS [ALLEGRO]
ALLEGRO_DISPLAY *disp = NULL;       //TELA
ALLEGRO_TIMER *timer = NULL;        //TEMPO
ALLEGRO_EVENT_QUEUE *queue = NULL;  //FILA DE EVENTO
ALLEGRO_FONT *font = NULL;          //FONTE



//---------- METODOS (Metodos do jogo e por fim o MAIN())
void INICIALIZAR(){

    //Inicializa o Allegro
    al_init();

    //Componentes nativos que serão utilizados
    al_install_keyboard();

    //Inicializa o ADDON que usa imagens
    al_init_image_addon();

    //Cria as coisas fundamentais do jogo
    timer = al_create_timer(1.0 / FPS);            //Tempo constante
    queue = al_create_event_queue();                //Fila de eventos
    disp = al_create_display(TLARGURA, TALTURA);    //Tela
    font = al_create_builtin_font();                //Fonte (para textos)

    //Escuta os ventos desses componentes
    al_register_event_source(queue, al_get_keyboard_event_source());    //EVENTOS DO TECLADO
    al_register_event_source(queue, al_get_display_event_source(disp)); //EVENTOS DA TELA
    al_register_event_source(queue, al_get_timer_event_source(timer));  //EVENTOS DO TIMER

    al_set_window_title(disp, "SPACE INVADERS Cover");

}

void CONSTRUIRJOGO(){


    moveDir = 0;
    gameLogo = al_load_bitmap("RES/gameLogo.png");


    //ESSA PARTE SERA UMA CARGA DE UM ARQUIVO TXT QUE TEM AS CONFIGURACOES DESSE JOGADOR
    //Metodo que constroi o jogo, atribuindo valores e imagens
    player.ativo = 1;
//    player.tipo = 'A';
//    player.tipoTiro = 'A';
    player.dano = 5;
    player.vida = 20;
    player.vidaMax = player.vida;

    ESCOLHAS();
//    switch(player.tipo){    //Muda a imagem da nave, dependendo do tipo
//        case 'A':
//            player.img = al_load_bitmap("RES/PL/naveA.png");
//            break;
//        case 'B':
//            player.img = al_load_bitmap("RES/PL/naveB.png");
//            break;
//        case 'C':
//            player.img = al_load_bitmap("RES/PL/naveC.png");
//            break;
//    }
//
//    switch(player.tipoTiro){    //Muda a imagem da nave, dependendo do tipo
//        case 'A':
//            tiroPlayer = al_load_bitmap("RES/PL/tiroA.png");
//            break;
//        case 'B':
//            tiroPlayer = al_load_bitmap("RES/PL/tiroB.png");
//            break;
//        case 'C':
//            tiroPlayer = al_load_bitmap("RES/PL/tiroC.png");
//            break;
//    }

    player.posicao[0] = TLARGURA / 2;
    player.posicao[1] = TALTURA - 100;
    player.tamanho[0] = al_get_bitmap_width(player.img);
    player.tamanho[1] = al_get_bitmap_height(player.img);

    player.hpBar = al_load_bitmap("RES/PL/hp1.png");
    player.hp = al_load_bitmap("RES/PL/hp2.png");



    //GRUPO DE MOBS
    //Configura cada mob dentro do grupo
    mobs.qtd = QTDMOBS; //Armazena qtd de mobs

    mobImg = al_load_bitmap("RES/MOB/mobA.png");  //Carrega imagem dos mobs
    mobImg2 = al_load_bitmap("RES/MOB/mobA2.png"); //Carrega imagem dos mobs, quando tiver com pouca vida
    tiroMob = al_load_bitmap("RES/MOB/tiroA.png");  //Imagem do tiro dos mobs

    int i;
    for (i=0; i<QTDMOBS; i++){

        mobs.mobs[i].ativo = 1;
        mobs.mobs[i].vida = MOBVIDA;
        mobs.mobs[i].dano = 5;
        mobs.mobs[i].img = mobImg;
        mobs.mobs[i].tamanho[0] = al_get_bitmap_width(mobs.mobs[i].img);
        mobs.mobs[i].tamanho[1] = al_get_bitmap_height(mobs.mobs[i].img);
    }

    //Isso faz centralizar o grupo de mobs na tela
    mobs.posicao[0] = (TLARGURA - MOBSPORFILA * (mobs.mobs[0].tamanho[0] + DISTANCIAMOBS)) / 2;
    mobs.posicao[1] = 40; //Distancia de CIMA


    //FUNDO
    bgimg = al_load_bitmap("RES/STAGES/stage1.png");
    bgAltura = al_get_bitmap_height(bgimg);

}



void ATUALIZARPLAYER(){

    if(moveDir < 0){
        if(player.posicao[0] > margemMovimento)
            player.posicao[0] -= 5;
    } else if (moveDir > 0) {
        if(player.posicao[0] < TLARGURA - player.tamanho[0] - margemMovimento)
            player.posicao[0] += 5;
    }

}

void CAUSARDANO(int emQuem, int mobId, int dano){

    if(emQuem == 1){ //PLAYER
        player.vida -= dano;    //Causa dano no player
        if(player.vida <= 0){

            //GAME OVER..
            TELA = 0;
        }

    } else { //MOB
        mobs.mobs[mobId].vida -= dano;  //Causa dano a este mob.

        if(mobs.mobs[mobId].vida <= MOBVIDA/2){ //Se a vida do mob ficar menor ou igual a metade..
            mobs.mobs[mobId].img = mobImg2; //Muda a imagem do mob
        }
        if(mobs.mobs[mobId].vida <= 0){ //Se a vida chegar a 0
            mobs.mobs[mobId].ativo = 0; //Mob fica inativo (morto)

            mobs.qtd--; //Decrementa no contador
            if(mobs.qtd <= 0){

                TELA = 0;
                //CONCLUIRSTAGE();
            }
        }
    }

}


void ATUALIZARFUNDO(){

    if(frameCount % bgTaxaMov == 0){

        //printf("%d %d\n", bgPosY, al_get_bitmap_height(bgimg));
        if(bgPosY == -bgAltura + TALTURA){
            bgDirecao = 1;
        } else if(bgPosY == 0) {
            bgDirecao = -1;
        }

        bgPosY += 1 * bgDirecao;
    }


}

int tr, mb;
void ATUALIZARTIROS(){

    for(tr = 0; tr< MAXTIROS; tr++){

        if(tiros[tr].ativo){

            if(tiros[tr].deQuem == 1){

                //TIROS DO PLAYER
                tiros[tr].posicao[1] -= tiros[tr].speed;    //Move o tiro

                //CHECA SE O TIRO SE CHOCOU COM ALGUM MOB
                for(mb=0; mb < QTDMOBS; mb++){
                    if(mobs.mobs[mb].ativo){
                        if
                        (tiros[tr].posicao[0] >= mobs.mobs[mb].posicao[0]
                        && tiros[tr].posicao[0] + tiros[tr].tamanho[0] <= mobs.mobs[mb].posicao[0] + mobs.mobs[mb].tamanho[0]
                        && tiros[tr].posicao[1] >= mobs.mobs[mb].posicao[1]
                        && tiros[tr].posicao[1] + tiros[tr].tamanho[1] <= mobs.mobs[mb].posicao[1]+ mobs.mobs[mb].tamanho[1]
                        ) //Checa se "HITBOX" do tiro bateu em algum MOB
                        {
                            CAUSARDANO(0, mb, tiros[tr].dano); //Causa dano no MOB
                            tiros[tr].ativo = 0; //Desativa esse tiro
                            break;
                        }
                    }
                }

            } else {

                //TIROS DO MOB
                tiros[tr].posicao[1] += tiros[tr].speed;    //Move o tiro

                //CHECA SE O TIRO SE CHOCOU COM PLAYER
                if
                (tiros[tr].posicao[0] >= player.posicao[0]
                && tiros[tr].posicao[0] + tiros[tr].tamanho[0] <= player.posicao[0] + player.tamanho[0]
                && tiros[tr].posicao[1] >= player.posicao[1]
                && tiros[tr].posicao[1] + tiros[tr].tamanho[1] <= player.posicao[1] + player.tamanho[1]
                ) //Checa se "HITBOX" do tiro bateu em algum MOB
                {
                    CAUSARDANO(1, 0, tiros[tr].dano); //Causa dano no MOB
                    tiros[tr].ativo = 0; //Desativa esse tiro
                    break;
                }
            }

            //Desativa esse tiro caso passe dos limites da tela
            if(tiros[tr].posicao[1] < 0 || tiros[tr].posicao[1] > TALTURA){
                tiros[tr].ativo = 0;
            }
        }
    }

}


void ATUALIZARMOBS(){

    if(frameCount % taxaAtMob == 0){
        mobs.posicao[0] += (8 * mobMoveDir);

        if(mobs.posicao[0] + MOBSPORFILA * (mobs.mobs[0].tamanho[0] + DISTANCIAMOBS) > TLARGURA - margemMovimento){
            printf("ESQUERDA\t");
            mobMoveDir = -1;
            mobs.posicao[1] += mobs.mobs[0].tamanho[1] / 2;
        } else if(mobs.posicao[0] < margemMovimento){
            mobMoveDir = 1;
            printf("DIREITA\t");
            mobs.posicao[1] += mobs.mobs[0].tamanho[1] / 2;
        }

        int i;
        for (i=0; i<QTDMOBS; i++){

            if(i==0){
                mobs.mobs[i].posicao[0] = mobs.posicao[0];
                mobs.mobs[i].posicao[1] = mobs.posicao[1];
            } else {

                if(i % MOBSPORFILA == 0){
                    mobs.mobs[i].posicao[1] = mobs.mobs[i-1].posicao[1] + mobs.mobs[i-1].tamanho[1] + DISTANCIAMOBS;
                    mobs.mobs[i].posicao[0] = mobs.mobs[0].posicao[0];
                } else {
                    mobs.mobs[i].posicao[0] = mobs.mobs[i-1].posicao[0] + mobs.mobs[i-1].tamanho[0] + DISTANCIAMOBS;
                    mobs.mobs[i].posicao[1] = mobs.mobs[i-1].posicao[1];
                }
            }
        }


        if(frameCount % taxaTiroMob == 0){

            frameCount = 0;
            //Pega um numero aleatorio entre 1 e QTDMOBS, pra saber qual mob vai atirar..
            int mobSorteado;

            while (1){
                mobSorteado = (rand() % (0 - QTDMOBS + 1)) + 0;
                if(mobs.mobs[mobSorteado].ativo){
                    MOBATIRAR(mobs.mobs[mobSorteado]);
                    break;
                }
            }
        }

    }

}


void PLAYERATIRAR(){

    int i;
    for(i=0; i< MAXTIROS; i++){
        if(tiros[i].ativo == 0){

            //printf("ATIROU!");
            tiros[i].ativo = 1;     //Ativa esse tiro
            tiros[i].dano = player.dano;    //Informa o dano do tiro (dano do player)
            tiros[i].deQuem = 1;    //Indica que o tiro é do player (pra poder causar dano nos mobs)
            tiros[i].speed = 10;    //Velocidade desse tiro

            tiros[i].img = tiroPlayer;  //Imagem de tiro que ja foi carregado

            tiros[i].tamanho[0] = al_get_bitmap_width(tiroPlayer);  //Largura do projetil
            tiros[i].tamanho[1] = al_get_bitmap_height(tiroPlayer); //Altura do projetil

            tiros[i].posicao[0] = player.posicao[0] + (player.tamanho[0] / 2);   //Mesma posicao X do player (centralizado)
            tiros[i].posicao[1] = player.posicao[1];    //Um pouco mais acima da posicao Y do player

            break;
        }

    }


}

void MOBATIRAR(Mob mob){
    int i;
    for(i=0; i<MAXTIROS; i++){
        if(tiros[i].ativo == 0){
            //printf("ATIROU!");
            tiros[i].ativo = 1;     //Ativa esse tiro
            tiros[i].dano = mob.dano;    //Informa o dano do tiro (dano do mob)
            tiros[i].deQuem = 2;    //Indica que o tiro é do mob (pra poder causar dano no player)
            tiros[i].speed = 5;    //Velocidade desse tiro

            tiros[i].img = tiroMob;  //Imagem de tiro que ja foi carregado

            tiros[i].tamanho[0] = al_get_bitmap_width(tiroPlayer);  //Largura do projetil
            tiros[i].tamanho[1] = al_get_bitmap_height(tiroPlayer); //Altura do projetil

            tiros[i].posicao[0] = mob.posicao[0] + (mob.tamanho[0] / 2);   //Mesma posicao X do mob (centralizado)
            tiros[i].posicao[1] = mob.posicao[1] + mob.tamanho[1];    //Um pouco mais abaixo da posicao Y do mob

            break;

        }
    }
}


int escolha1 = 0, escolha2 = 0;
void ESCOLHAS(){

    if(escolha1 > 2){
        escolha1 = 0;
    } else if(escolha1 < 0){
        escolha1 = 2;
    }
    if(escolha2 > 2){
        escolha2 = 0;
    }else if(escolha2 < 0){
        escolha2 = 2;
    }

    switch(escolha1){
        case 0:
            player.tipo = 'A';
            player.img = al_load_bitmap("RES/PL/naveA.png");
            break;
        case 1:
            player.tipo = 'B';
            player.img = al_load_bitmap("RES/PL/naveB.png");
            break;
        case 2:
            player.tipo = 'C';
            player.img = al_load_bitmap("RES/PL/naveC.png");
            break;
    }

    switch(escolha2){
        case 0:
            player.tipoTiro = 'A';
            tiroPlayer = al_load_bitmap("RES/PL/tiroA.png");
            break;
        case 1:
            player.tipoTiro = 'B';
            tiroPlayer = al_load_bitmap("RES/PL/tiroB.png");
            break;
        case 2:
            player.tipoTiro = 'C';
            tiroPlayer = al_load_bitmap("RES/PL/tiroC.png");
            break;
    }

}


int MENUPRINCIPAL(){

    al_wait_for_event(queue, &evento);   //Pronto para ouvir o proimo evento

    /* -- EVENTOS -- */
    //se o tipo do evento for o disparo de um timer
    if(evento.type == ALLEGRO_EVENT_TIMER){

        //Atualiza com o timer... Colocar animacoes aqui
        desenha = 1;

        frameCount++;   //ATUALIZA O FRAME ATUAL

        ATUALIZARFUNDO();   //Atualiza o Fundo

        ESCOLHAS();

    }

    //SE FOR UM BOTAO PRESSIONADO
    else if(evento.type == ALLEGRO_EVENT_KEY_DOWN){

         //verifica qual tecla foi PRESSINADA
        switch(evento.keyboard.keycode){

            case ALLEGRO_KEY_UP:
                escolha1--; //MUDA A COR DA NAVE
                break;

            case ALLEGRO_KEY_DOWN:
                escolha1++; //MUDA A COR DA NAVE
                break;

            case ALLEGRO_KEY_LEFT:
                escolha2--; //MUDA A COR DO TIRO
                break;

            case ALLEGRO_KEY_RIGHT:
                escolha2++; //MUDA COR DO TIRO
                break;

            case ALLEGRO_KEY_ENTER:
                CONSTRUIRJOGO();
                TELA = 1;
                break;

            //esc. sair=1 faz com que o programa saia do loop principal
            case ALLEGRO_KEY_ESCAPE:
                INGAME = 0;
        }
    }

    else if(evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {

        //AO FECHAR A JANELA
        INGAME = 0;
    }


    //DESENHA AS COISAS NA TELA
    if(desenha && al_is_event_queue_empty(queue))
    {
        al_draw_bitmap(bgimg, 0, bgPosY ,0);  //Desenha o fundo...

        al_draw_scaled_bitmap(gameLogo,
                              0,0,
                              al_get_bitmap_width(gameLogo),
                              al_get_bitmap_height(gameLogo),
                              (TLARGURA - 380) / 2,
                              20,
                              380,200,0);

        if(menu == 0){
            //RAIZ DO MENU PRINCIPAL

        } else if(menu == 1){
        }

        al_draw_bitmap(player.img, player.posicao[0], player.posicao[1], 0); //Desenha o player

        al_draw_bitmap(tiroPlayer, (float)player.posicao[0] + ((float)player.tamanho[0] / 2.3), player.posicao[1] - 20, 0); //Desenha o tiro


        al_flip_display(); //EXIBE A TELA

        desenha = 0; //Nao redesenha até acontecer alguma coisa
    }

}


int mobc, tiroc; //CONTADORES
void GAME(){

    al_wait_for_event(queue, &evento);   //Pronto para ouvir o proimo evento

    /* -- EVENTOS -- */
    //se o tipo do evento for o disparo de um timer
    if(evento.type == ALLEGRO_EVENT_TIMER){

        //Atualiza com o timer... Colocar animacoes aqui
        desenha = 1;

        frameCount++;   //ATUALIZA O FRAME ATUAL

        ATUALIZARFUNDO();   //Atualiza o Fundo

        if(mobs.qtd > 0){ //Verifica se tem mobs ativos
            ATUALIZARMOBS();    //Se tiver, atualiza
        }

        ATUALIZARTIROS();

        //printf("a");
    }

    //SE FOR UM BOTAO PRESSIONADO
    else if(evento.type == ALLEGRO_EVENT_KEY_DOWN){

         //verifica qual tecla foi PRESSINADA
        switch(evento.keyboard.keycode){
            //seta para cima
            case ALLEGRO_KEY_UP:
                PLAYERATIRAR();
                break;
            //seta para baixo
            case ALLEGRO_KEY_DOWN:
                //...
                break;
            //seta para esquerda
            case ALLEGRO_KEY_LEFT:
                moveDir = -1;
                break;
            //seta para direita.
            case ALLEGRO_KEY_RIGHT:
                moveDir = 1;
                break;
            //esc. sair=1 faz com que o programa saia do loop principal
            case ALLEGRO_KEY_ESCAPE:
                INGAME = 0;
        }
    }
    else if (evento.type == ALLEGRO_EVENT_KEY_UP){

         //verifica qual tecla foi SOLTA
        switch(evento.keyboard.keycode){
            //seta para cima
            case ALLEGRO_KEY_UP:
                //ATIRA(0);
                break;

            //seta para esquerda
            case ALLEGRO_KEY_LEFT:
                moveDir = 0;
                break;
            //seta para direita.
            case ALLEGRO_KEY_RIGHT:
                moveDir = 0;
                break;
        }
    }

    else if(evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {

        //AO FECHAR A JANELA
        INGAME = 0;
    }


    if(moveDir != 0){
        ATUALIZARPLAYER();
    }



    //DESENHA AS COISAS NA TELA
    if(desenha && al_is_event_queue_empty(queue))
    {
        //al_clear_to_color(al_map_rgb(0, 0, 0)); //Deixa tela de uma cor..

        al_draw_bitmap(bgimg, 0, bgPosY ,0);  //Desenha o fundo...

        //Desenha os tiros
        for(tiroc = 0; tiroc < MAXTIROS; tiroc++){
            if(tiros[tiroc].ativo){
                al_draw_bitmap(
                   tiros[tiroc].img,
                   tiros[tiroc].posicao[0],
                   tiros[tiroc].posicao[1],
                   0);
            }
        }

        //Desenha os mobs
        if(mobs.qtd > 0){
            for(mobc = 0; mobc < QTDMOBS; mobc++){
                if(mobs.mobs[mobc].ativo){
                    al_draw_bitmap(
                        mobs.mobs[mobc].img,
                        mobs.mobs[mobc].posicao[0],
                        mobs.mobs[mobc].posicao[1],
                        0);
                }

            }
        }


        if(player.ativo){
            al_draw_bitmap(player.img, player.posicao[0], player.posicao[1], 0); //Desenha o player

            al_draw_bitmap(player.hpBar, TLARGURA - 150, TALTURA - 40, 0);  //Moldura de VIDA do player

            al_draw_scaled_bitmap(player.hp,    //VIDA DO PLAYER
                                  0,
                                  0,
                                  al_get_bitmap_width(player.hp),
                                  al_get_bitmap_height(player.hp),
                                  TLARGURA - 146,
                                  TALTURA - 36,
                                  122 * ((float)player.vida / (float)player.vidaMax),
                                  17,
                                  0);
        }

        al_flip_display(); //EXIBE A TELA

        desenha = 0; //Nao redesenha até acontecer alguma coisa
    }
}

void DestruirInstancias(){

    //Esse metodo limpa a memoria das coisas instanciadas
    al_destroy_bitmap(player.img);  //Imagem do player
    al_destroy_bitmap(tiroPlayer);  //Imagem do tiro do Player

    al_destroy_bitmap(mobImg);      //Imagem do Mob
    al_destroy_bitmap(mobImg2);     //Imagem do Mob2
    al_destroy_bitmap(tiroMob);     //Imagem do Tiro do mob

    al_destroy_bitmap(bgimg);       //Fundo

    al_destroy_font(font);          //FONTE
    al_destroy_display(disp);       //TELA
    al_destroy_timer(timer);        //TIMER
    al_destroy_event_queue(queue);  //FILA DE EVENTO

}


int main()
{

    INICIALIZAR();      //INICIALIZA OS COMPONENTES DO JOGO E ALLEGRO

    CONSTRUIRJOGO();    //CONSTRUI A BASE DO JOGO
    printf("Iniciando o Game\n");

    al_start_timer(timer); //Inicia temporizador

    while(INGAME)
    {
        if(TELA == 0){
            MENUPRINCIPAL();
        } else if(TELA == 1){
            GAME();
        } else if(TELA == 2){
//            RANKING();
        }
    }

    //Destroi as instancias para não consumir memoria
    DestruirInstancias();

    return 0;
}
