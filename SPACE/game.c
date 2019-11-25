#include <stdio.h>
#include "gametipos.h"  //BIBLIOTECA CUSTOMIZADA (com os tipos customizados)

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

#define DISTANCIAMOBS 15    //Distancia entre mobs
#define MOBVIDA 10          //Vida dos mobs

#define MAXTIROS 30          //Max de tiros no jogo



//---------- VARIAVEIS
int INGAME = 1;         //Controla o loop principal
int TELA = 0;           //TELA ATUAL (0 - Menu, 1-Game, 2-Ranking)
int desenha = 1;        //Indica se o loop redesenha as coisas

ALLEGRO_BITMAP *gameLogo = NULL; //Logo do game

int STAGE = 1;     //INICIA NO STAGE 1 (isso que muda as imagens dos mobs e fundo)

//PONTUACAO
int PONTOSPLAYER = 0;   //Pontuação
int tirosEfetuados = 0; //CONTAGEM DE TIROS PARA CALCULAR PONTUACAO NO FINAL
//---------

Player player;          //Player
Mobs mobs;              //Grupo de Mobs
ALLEGRO_BITMAP *mobImg = NULL;  //Imagem dos Mobs
ALLEGRO_BITMAP *mobImg2 = NULL; //Imagem dos Mobs quando estiverem vida baixa


int QTDMOBS = 30;       //Qtd de mobs no Grupo
int MOBSPORFILA = 10;   //10 mobs por fila

int BOSS = 0;           //Indica se o player está contra BOSS
Mob boss;               //BOSS
int bossVida;           //Vida Max do boss(pra calculos)

int atirarLock = 0;     //Segurar seta CIMA para atirar
int taxaTiro = 10;      //Taxa de quadros para proximo tiro automatico
Tiro tiros[MAXTIROS];   //POOL DE TIROS (limita a quantidade de tiros no mapa e os tiros são reutilizados quando inativos)
ALLEGRO_BITMAP *tiroPlayer = NULL; //Imagens do tiro do player (varia de acordo com o tipo)
ALLEGRO_BITMAP *tiroMob = NULL;    //Imagem do tiro do Mob (varia de acordo com o stage)


ALLEGRO_BITMAP *bgimg = NULL;         //Background
int bgAltura;       //Altura da imagem de fundo
int bgPosY = 0;     //Usado para fazer o efeito de movimento do fundo
int bgTaxaMov = 5;    //Taxa de movimentação do fundo
int bgDirecao = 1;      //Direção de movimentação do fundo


int frameCount = 0; //CONTAGEM DE FRAMES

ALLEGRO_EVENT evento;    //Escuta os eventos

int moveDir = 0;    //DIREÇÃO DE MOVIMENTO DO PLAYER
int mobMoveDir = 1; //Direção de movimento dos mobs
int taxaAtMob = 8; //Taxa de FRAMES para que o mob atualize seu movimento
int taxaTiroMob = 20;   //Taxa de FRAMES para que o mob atire

int margemMovimento = 30;   //Pixels da Margem da tela que o player e mobs podem se movimentar.

//Ranking
char nomeJogador[70] = "";



//---------- VARIAVEIS [ALLEGRO]
ALLEGRO_DISPLAY *disp = NULL;       //TELA
ALLEGRO_TIMER *timer = NULL;        //TEMPO
ALLEGRO_EVENT_QUEUE *queue = NULL;  //FILA DE EVENTO
ALLEGRO_FONT *fonte = NULL;         //FONTE
ALLEGRO_FONT *fonte2 = NULL;        //FONTE2 (mesma de cima, só que maior)
ALLEGRO_USTR* str = NULL;           //TEXTO INPUT (ranking)
int pos;

//============================================
//METODOS (Metodos do jogo e por fim o MAIN())
//============================================

//---------- INICIALIZACAO, CONSTRUCAO DO JOGO e do BOSS --------------
void INICIALIZAR(){

    //Inicializa o Allegro
    al_init();

    //Componentes nativos que serão utilizados
    al_install_keyboard();
    str = al_ustr_new("");  //Input de Texto
    pos = (int)al_ustr_size(str);


    //Inicializa o ADDON que usa imagens
    al_init_image_addon();

    // Inicialização do add-on para uso de fontes
    al_init_font_addon();

    // Inicialização do add-on para uso de fontes True Type
    al_init_ttf_addon();

    //Cria as coisas fundamentais do jogo
    timer = al_create_timer(1.0 / FPS);             //Tempo constante
    queue = al_create_event_queue();                //Fila de eventos
    disp = al_create_display(TLARGURA, TALTURA);    //
    fonte = al_load_font("RES/kardust.ttf", 25, 0); //Fonte (para textos)
    fonte2 = al_load_font("RES/kardust.ttf", 32, 0); //Fonte (para textos)

    //Escuta os ventos desses componentes
    al_register_event_source(queue, al_get_keyboard_event_source());    //EVENTOS DO TECLADO
    al_register_event_source(queue, al_get_display_event_source(disp)); //EVENTOS DA TELA
    al_register_event_source(queue, al_get_timer_event_source(timer));  //EVENTOS DO TIMER

    al_set_window_title(disp, "SPACE INVADERS Cover");

}

void CONSTRUIRJOGO(int novoStage){

    if(!novoStage){
        STAGE = 1;
        BOSS = 0;
        moveDir = 0;

        gameLogo = al_load_bitmap("RES/gameLogo.png");

        //ESSA PARTE SERA UMA CARGA DE UM ARQUIVO TXT QUE TEM AS CONFIGURACOES DESSE JOGADOR
        //Metodo que constroi o jogo, atribuindo valores e imagens
        player.ativo = 1;
        player.dano = 5;
        player.vida = 30;
        player.vidaMax = player.vida;

        ESCOLHAS(); //CONFIGURACAO DE COR DO PLAYER (feita no Menu Inicial)

        player.tamanho[0] = al_get_bitmap_width(player.img);    //Pega a largura em PX da imagem do player
        player.tamanho[1] = al_get_bitmap_height(player.img);   //Pega a altura em PX da imagem do player

        player.posicao[0] = TLARGURA / 2 - player.tamanho[0]/2; //Posicao X Inicial
        player.posicao[1] = TALTURA - 100;  //Posicao Y Inicial

        player.hpBar = al_load_bitmap("RES/PL/hp1.png");
        player.hp = al_load_bitmap("RES/PL/hp2.png");
    }


    //GRUPO DE MOBS
    //Configura cada mob dentro do grupo
    mobs.qtd = QTDMOBS; //Armazena qtd de mobs

    switch(STAGE){
        case 1:
            mobImg = al_load_bitmap("RES/MOB/mobA.png");  //Carrega imagem dos mobs
            mobImg2 = al_load_bitmap("RES/MOB/mobA2.png"); //Carrega imagem dos mobs, quando tiver com pouca vida
            tiroMob = al_load_bitmap("RES/MOB/tiroA.png");  //Imagem do tiro dos mobs

            bgimg = al_load_bitmap("RES/STAGES/stage1.png"); //MUDA A IMAGEM DO STAGE
            break;
        case 2:
            mobImg = al_load_bitmap("RES/MOB/mobB.png");  //Carrega imagem dos mobs
            mobImg2 = al_load_bitmap("RES/MOB/mobB2.png"); //Carrega imagem dos mobs, quando tiver com pouca vida
            tiroMob = al_load_bitmap("RES/MOB/tiroC.png");  //Imagem do tiro dos mobs

            bgimg = al_load_bitmap("RES/STAGES/stage2.png"); //MUDA A IMAGEM DO STAGE
            break;
    }


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
    bgAltura = al_get_bitmap_height(bgimg);

}

void INICIARBOSS(){
    BOSS = 1;

    switch(STAGE){
        case 1:
            // BOSS DO STAGE 1
            boss.dano = 10;
            boss.vida = 100;

            mobImg = al_load_bitmap("RES/MOB/bossA.png");
            mobImg2 = al_load_bitmap("RES/MOB/bossA2.png");
            tiroMob = al_load_bitmap("RES/MOB/tiroB.png");
            break;
        case 2:
            // BOSS DO STAGE 2
            boss.dano = 15;
            boss.vida = 200;

            mobImg = al_load_bitmap("RES/MOB/bossB.png");
            mobImg2 = al_load_bitmap("RES/MOB/bossB2.png");
            tiroMob = al_load_bitmap("RES/MOB/tiroD.png");
            break;
    }

    bossVida = boss.vida; //Armazena a vida max desse boss, para calculos.
    boss.ativo = 1; //ATIVO!!!

    boss.img = mobImg;
    boss.tamanho[0] = al_get_bitmap_width(mobImg);
    boss.tamanho[1] = al_get_bitmap_height(mobImg);

    boss.posicao[0] = (TLARGURA / 2) - boss.tamanho[0]/2;
    boss.posicao[1] = 100;

}



//---------- ATUALIZACOES DAS COISAS -----------------

void ATUALIZARPLAYER(){

    if(moveDir < 0){
        if(player.posicao[0] > margemMovimento)
            player.posicao[0] -= 5;
    } else if (moveDir > 0) {
        if(player.posicao[0] < TLARGURA - player.tamanho[0] - margemMovimento)
            player.posicao[0] += 5;
    }

}

void ATUALIZARMOBS(){

    if(frameCount % taxaAtMob == 0){
        mobs.posicao[0] += (8 * mobMoveDir);

        if(mobs.posicao[0] + MOBSPORFILA * (mobs.mobs[0].tamanho[0] + DISTANCIAMOBS) > TLARGURA - margemMovimento){
            mobMoveDir = -1;
            mobs.posicao[1] += mobs.mobs[0].tamanho[1] / 2;
        } else if(mobs.posicao[0] < margemMovimento){
            mobMoveDir = 1;
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

        //Se os mobs chegarem na linha do player, Game ov.
        if(mobs.posicao[1] > player.posicao[1]){
            TELA = 0; //Volta pra o menu...
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

void ATUALIZARBOSS(){

    if(frameCount % taxaAtMob == 0 && boss.ativo){
        boss.posicao[0] += (10 * mobMoveDir);

        if(boss.posicao[0] + boss.tamanho[0]> TLARGURA - margemMovimento){
            mobMoveDir = -1;
            boss.posicao[1] += boss.tamanho[1] / 2;

        } else if(boss.posicao[0] < margemMovimento){
            mobMoveDir = 1;
            boss.posicao[1] += boss.tamanho[1] / 2;
        }


        //Se o boss chegar na linha do player, Game ov.
        if(boss.posicao[1] > player.posicao[1]){
            TELA = 2; //Volta pra o menu...
        }

        //Controla o tiro do boss
        if(frameCount % taxaTiroMob == 0){
            frameCount = 0;
            MOBATIRAR(boss);
        }
    } else if(boss.ativo == 0){

        if(frameCount == 120){

            if(STAGE > 2){
                TELA = 2; // FINALIZA O JOGO
            } else {
                CONSTRUIRJOGO(1);
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



//-------------- TIROS -------------------
void PLAYERATIRAR(){

    if(atirarLock && frameCount % taxaTiro == 0){
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

                tirosEfetuados++; //Contagem de Tiros..
                break;
            }
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

            tiros[i].tamanho[0] = al_get_bitmap_width(tiroMob);  //Largura do projetil
            tiros[i].tamanho[1] = al_get_bitmap_height(tiroMob); //Altura do projetil

            tiros[i].posicao[0] = mob.posicao[0] + (mob.tamanho[0] / 2);   //Mesma posicao X do mob (centralizado)
            tiros[i].posicao[1] = mob.posicao[1] + mob.tamanho[1];    //Um pouco mais abaixo da posicao Y do mob

            break;

        }
    }
}

int tr, mb;
void ATUALIZARTIROS(){

    for(tr = 0; tr< MAXTIROS; tr++){

        if(tiros[tr].ativo){

            if(tiros[tr].deQuem == 1){

                //TIROS DO PLAYER
                tiros[tr].posicao[1] -= tiros[tr].speed;    //Move o tiro

                if(BOSS && boss.ativo){ //Se estiver contra o BOSS

                    if
                    (tiros[tr].posicao[0] >= boss.posicao[0]
                    && tiros[tr].posicao[0] + tiros[tr].tamanho[0] <= boss.posicao[0] + boss.tamanho[0]
                    && tiros[tr].posicao[1] >= boss.posicao[1]
                    && tiros[tr].posicao[1] + tiros[tr].tamanho[1] <= boss.posicao[1]+ boss.tamanho[1]
                    ) //Checa se "HITBOX" do tiro bateu no BOSS
                    {
                        CAUSARDANO(2, 0, tiros[tr].dano); //Causa dano no BOSS
                        tiros[tr].ativo = 0; //Desativa esse tiro
                        break;
                    }

                } else { //Se estiver contra os MOBS ainda..

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
                }

            } else { //TIROS DO MOB/BOSS

                tiros[tr].posicao[1] += tiros[tr].speed;    //Move o tiro

                //CHECA SE O TIRO SE CHOCOU COM PLAYER
                if
                (tiros[tr].posicao[0] >= player.posicao[0]
                && tiros[tr].posicao[0] + tiros[tr].tamanho[0] <= player.posicao[0] + player.tamanho[0]
                && tiros[tr].posicao[1] >= player.posicao[1]
                && tiros[tr].posicao[1] + tiros[tr].tamanho[1] <= player.posicao[1] + player.tamanho[1]
                ) //Checa se "HITBOX" do tiro bateu em algum MOB
                {
                    CAUSARDANO(1, 0, tiros[tr].dano); //Causa dano no Player
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

void CAUSARDANO(int emQuem, int mobId, int dano){

    if(emQuem == 1){ //PLAYER
        player.vida -= dano;    //Causa dano no player
        if(player.vida <= 0){
            player.ativo = 0;
            //GAME OVER..
            frameCount = 0; //Zera para contagem
            TELA = 2;
        }

    } else if(emQuem == 0) { //MOB
        mobs.mobs[mobId].vida -= dano;  //Causa dano a este mob.

        if(mobs.mobs[mobId].vida <= MOBVIDA/2){ //Se a vida do mob ficar menor ou igual a metade..
            mobs.mobs[mobId].img = mobImg2; //Muda a imagem do mob
        }
        if(mobs.mobs[mobId].vida <= 0){ //Se a vida chegar a 0
            mobs.mobs[mobId].ativo = 0; //Mob fica inativo (morto)

            mobs.qtd--; //Decrementa no contador
            PONTOSPLAYER += 10;

            if(mobs.qtd <= 0){
                INICIARBOSS();
            }
        }

    } else if(emQuem == 2){ //BOSS

        boss.vida -= dano;  //Causa dano a este mob.

        if(boss.vida <= bossVida/2){ //Se a vida do mob ficar menor ou igual a metade..
            boss.img = mobImg2; //Muda a imagem do mob
        }
        if(boss.vida <= 0){ //Se a vida chegar a 0
            boss.ativo = 0; //Boss fica inativo (morto)

            PONTOSPLAYER += 50;

            STAGE++; //PROXIMO STAGE...

            //CONCLUIRSTAGE();
        }
    }
}



//-------------- Rotinas do MENU, GAME e ENDGAME -------------------
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
                CONSTRUIRJOGO(1);
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

        al_draw_text(fonte, al_map_rgb(255, 255, 255), TLARGURA / 2, TALTURA - 250, ALLEGRO_ALIGN_CENTER, "Seta ESQ/DIR para trocar cor do tiro");
        al_draw_text(fonte, al_map_rgb(255, 255, 255), TLARGURA / 2, TALTURA - 220, ALLEGRO_ALIGN_CENTER, "Seta CIMA/BAIXO para mudar cor da nave");
        al_draw_text(fonte2, al_map_rgb(255, 255, 255), TLARGURA / 2, TALTURA - 180, ALLEGRO_ALIGN_CENTER, "ENTER para comecar");


        //ESCOLHAS DE COR
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
        } else {
            ATUALIZARBOSS();
        }

        PLAYERATIRAR();
        ATUALIZARTIROS();

        //printf("a");
    }

    //SE FOR UM BOTAO PRESSIONADO
    else if(evento.type == ALLEGRO_EVENT_KEY_DOWN){

         //verifica qual tecla foi PRESSINADA
        switch(evento.keyboard.keycode){
            //seta para cima
            case ALLEGRO_KEY_UP:
                atirarLock = 1;
                //PLAYERATIRAR();
                break;
            //espaco
            case ALLEGRO_KEY_SPACE:
                atirarLock = 1;
                //PLAYERATIRAR();
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
                atirarLock = 0;
                //PLAYERATIRAR();
                break;
            //espaco
            case ALLEGRO_KEY_SPACE:
                atirarLock = 0;
                //PLAYERATIRAR();
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
        } else if(BOSS) {
            al_draw_bitmap(
                        boss.img,
                        boss.posicao[0],
                        boss.posicao[1],
                        0);
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

void ENDGAME(){

    al_wait_for_event(queue, &evento);   //Pronto para ouvir o proimo evento

    /* -- EVENTOS -- */
    //se o tipo do evento for o disparo de um timer
    if(evento.type == ALLEGRO_EVENT_TIMER){

        //Atualiza com o timer... Colocar animacoes aqui
        desenha = 1;

        frameCount++;   //ATUALIZA O FRAME ATUAL

        ATUALIZARFUNDO();   //Atualiza o Fundo

        //SALVARPONTOS();     //SALVA OS PONTOS DO PLAYER PARA O RANKING

    }

      //SE FOR UM BOTAO PRESSIONADO
    else if(evento.type == ALLEGRO_EVENT_KEY_DOWN){


         //verifica qual tecla foi PRESSINADA
        switch(evento.keyboard.keycode){

            case ALLEGRO_KEY_UP:

                break;

            case ALLEGRO_KEY_DOWN:

                break;

            case ALLEGRO_KEY_LEFT:

                break;

            case ALLEGRO_KEY_RIGHT:

                break;

            case ALLEGRO_KEY_ENTER:
                if(al_ustr_length(str) > 0){

                    //Salva e Mostrar Ranking
                    strcpy(nomeJogador, al_cstr(str)); //Armazena o nome informado na variavel
                    printf("%s", nomeJogador); //Mostra no console

                    CONSTRUIRJOGO(0);
                    TELA = 0;
                }
                break;

            //esc. sair=1 faz com que o programa saia do loop principal
            case ALLEGRO_KEY_ESCAPE:
                INGAME = 0;
                break;

        }

    }

    if(evento.type == ALLEGRO_EVENT_KEY_CHAR){
        if(evento.keyboard.unichar >= 32)
            {
                pos += al_ustr_append_chr(str, evento.keyboard.unichar);
            }
            else if(evento.keyboard.keycode == ALLEGRO_KEY_BACKSPACE)
            {
                if(al_ustr_prev(str, &pos))
                al_ustr_truncate(str, pos);
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

        if(player.ativo){
            al_draw_bitmap(player.img, player.posicao[0], player.posicao[1], 0); //Desenha o player
        }

        if(frameCount >= 60){

            al_draw_text(fonte2, al_map_rgb(255, 255, 255), TLARGURA / 2, 150, ALLEGRO_ALIGN_CENTER, "Fim de Jogo!");
            al_draw_text(fonte, al_map_rgb(255, 255, 255), TLARGURA / 2, 220, ALLEGRO_ALIGN_CENTER, "Digite seu nome:");

            //INPUT DE NOME
            al_draw_ustr(fonte2, al_map_rgb(253, 195, 53), TLARGURA / 2, 250, ALLEGRO_ALIGN_CENTRE, str);

            al_draw_textf(fonte, al_map_rgb(255, 255, 255), TLARGURA / 2, TALTURA - 240, ALLEGRO_ALIGN_CENTER, "Voce atirou %d vezes", tirosEfetuados);
            al_draw_textf(fonte, al_map_rgb(255, 255, 255), TLARGURA / 2, TALTURA - 200, ALLEGRO_ALIGN_CENTER, "Sua pontuacao foi: %d", PONTOSPLAYER);



        }

        al_flip_display(); //EXIBE A TELA

        desenha = 0; //Nao redesenha até acontecer alguma coisa
    }

}



//-------------- LIBERAR MEMORIA UTILIZADA -------------------
void DestruirInstancias(){

    //Esse metodo limpa a memoria das coisas instanciadas
    al_destroy_bitmap(player.img);  //Imagem do player
    al_destroy_bitmap(tiroPlayer);  //Imagem do tiro do Player

    al_destroy_bitmap(mobImg);      //Imagem do Mob
    al_destroy_bitmap(mobImg2);     //Imagem do Mob2
    al_destroy_bitmap(tiroMob);     //Imagem do Tiro do mob

    al_destroy_bitmap(bgimg);       //Fundo

    al_destroy_font(fonte);         //FONTE
    al_destroy_font(fonte2);        //FONTE2
    al_destroy_display(disp);       //TELA
    al_destroy_timer(timer);        //TIMER
    al_destroy_event_queue(queue);  //FILA DE EVENTO

}



//------------- PRINCIPAL --------------------
int main()
{
    INICIALIZAR();      //INICIALIZA OS COMPONENTES DO JOGO E ALLEGRO

    CONSTRUIRJOGO(0);    //CONSTRUI A BASE DO JOGO
    printf("Iniciando o Game\n");

    al_start_timer(timer); //Inicia temporizador

    while(INGAME)
    {
        if(TELA == 0){
            MENUPRINCIPAL();
        } else if(TELA == 1){
            GAME();
        } else if(TELA == 2){
            ENDGAME();
        }
    }

    //Destroi as instancias para liberar memoria utilizada
    DestruirInstancias();

    return 0;
}
