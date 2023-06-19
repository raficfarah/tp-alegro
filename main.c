#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
//#include <stdlib.h>

#define NUM_PRATOS 8

const float FPS = 100;  

const int SCREEN_W = 954;
const int SCREEN_H = 540;

const int POSTE_W = 5;
const int POSTE_H = 324;

int AUX = 1;

//largura do jogador
const float JOGADOR_W = 50;
const float JOGADOR_H = 100;

typedef struct Jogador {
	
	float x;
	int equilibrando;
	int mov_esq, mov_dir;
	ALLEGRO_COLOR cor;
	float vel;
	
} Jogador;

typedef struct Prato {
	float x;
	
	/* um valor entre 0 e 255, em que 0 = prato equilibrado e
	   1 = prato com maxima energia, prestes a cair */
	float energia;
	float tempoParaAparecer;
	
} Prato;

typedef struct Poste {
	int x, y;
	int position;
	int status;
	ALLEGRO_COLOR cor;
} Poste;

void desenha_cenario() {
	
	ALLEGRO_COLOR BKG_COLOR = al_map_rgb(194,0,223);
	ALLEGRO_COLOR POSTE_COLOR = al_map_rgb(255,255,255);
	//colore a tela de branco (rgb(255,255,255))
	al_clear_to_color(BKG_COLOR);
	int x;
	for (x=1; x < SCREEN_W; x++){
		if (x % 106 == 0){
			al_draw_line(x, 432, x, 108, POSTE_COLOR, 5);
		}
	}
	
}

void desenha_jogador(Jogador j) {
	
	al_draw_filled_triangle(j.x, SCREEN_H - JOGADOR_H, 
							j.x - JOGADOR_W/2, SCREEN_H,
							j.x + JOGADOR_W/2, SCREEN_H,
							j.cor);	
	
}

void atualizaJogador(Jogador *j) {
	if(j->mov_esq) {
		if(j->x - j->vel > 0)
			j->x -= j->vel;
	}
	if(j->mov_dir) {
		if(j->x + j->vel < SCREEN_W)
			j->x += j->vel;
	}	
}

void inicializaJogador(Jogador *j) {
	j->x = SCREEN_W / 2;
	j->equilibrando = 0;
	j->cor = al_map_rgb(0, 223, 15);
	j->mov_esq = 0;
	j->mov_dir = 0;
	j->vel = 1;
}


float geraTempoPrato(int i) {
	return 1;
}

void inicializaPratos(Prato pratos[]) {
	
	//CONSERTAR ESTA FUNCAO!
	
	int i;
	for(i=0; i<NUM_PRATOS; i++) {
		pratos[i].x = i*2;
		pratos[i].energia = 0;
		pratos[i].tempoParaAparecer = geraTempoPrato(i);
	}
}


void inicializaPoste(Poste poste[]){
	ALLEGRO_COLOR POSTE_COLOR = al_map_rgb(255,255,255); 
	//int aux;
	int i;
	for (i=0; i<NUM_PRATOS; i++){
		poste[i].position = (i+1)*106;

	}
	poste[i].x = POSTE_W;
	poste[i].y = POSTE_H;
	poste[i].status = 0;
	poste[i].cor = POSTE_COLOR;
}

void atualizaPoste(Poste poste[], Jogador *j) {
	int i;
	for (i=0; i<NUM_PRATOS; i++){
		if (poste[i].status != 0) {
			poste[i].cor = al_map_rgb(255,0,0);
		al_draw_line(poste[i].position, 432, poste[i].position, 108, poste[i].cor, 5);
		} 
	}
}

 
int main(int argc, char **argv){
	
	ALLEGRO_DISPLAY *display = NULL;	
	ALLEGRO_TIMER *timer = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	
	//inicializa o Allegro
	if(!al_init()) {
		fprintf(stderr, "failed to initialize allegro!\n");
		return -1;
	}
	
	//cria um temporizador que incrementa uma unidade a cada 1.0/FPS segundos
    timer = al_create_timer(1.0 / FPS);
    if(!timer) {
		fprintf(stderr, "failed to create timer!\n");
		return -1;
	}	
	
    //inicializa o módulo de primitivas do Allegro
    if(!al_init_primitives_addon()){
		fprintf(stderr, "failed to initialize primitives!\n");
        return -1;
    }	
	
	//inicializa o modulo que permite carregar imagens no jogo
	if(!al_init_image_addon()){
		fprintf(stderr, "failed to initialize image module!\n");
		return -1;
	}	
	
	//cria uma tela com dimensoes de SCREEN_W, SCREEN_H pixels
	display = al_create_display(SCREEN_W, SCREEN_H);
	if(!display) {
		fprintf(stderr, "failed to create display!\n");
		al_destroy_timer(timer);
		return -1;
	}	
	
	//instala o teclado
	if(!al_install_keyboard()) {
		fprintf(stderr, "failed to install keyboard!\n");
		return -1;
	}
	
	//inicializa o modulo allegro que carrega as fontes
	al_init_font_addon();

	//inicializa o modulo allegro que entende arquivos tff de fontes
	if(!al_init_ttf_addon()) {
		fprintf(stderr, "failed to load tff font module!\n");
		return -1;
	}
	
	//carrega o arquivo arial.ttf da fonte Arial e define que sera usado o tamanho 32 (segundo parametro)
    ALLEGRO_FONT *size_32 = al_load_font("arial.ttf", 32, 1);   
	if(size_32 == NULL) {
		fprintf(stderr, "font file does not exist or cannot be accessed!\n");
	}	
	
	
 	//cria a fila de eventos
	event_queue = al_create_event_queue();
	if(!event_queue) {
		fprintf(stderr, "failed to create event_queue!\n");
		al_destroy_display(display);
		al_destroy_timer(timer);
		return -1;
	}	
	
	//registra na fila os eventos de tela (ex: clicar no X na janela)
	al_register_event_source(event_queue, al_get_display_event_source(display));
	//registra na fila os eventos de tempo: quando o tempo altera de t para t+1
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	//registra na fila os eventos de teclado (ex: pressionar uma tecla)
	al_register_event_source(event_queue, al_get_keyboard_event_source()); 	
	
	
	//JOGADOR
	Jogador jogador;
	inicializaJogador(&jogador);
	
	//PRATOS
	Prato pratos[NUM_PRATOS];
	inicializaPratos(pratos);

	//POSTES
	Poste poste[NUM_PRATOS];
	inicializaPoste(poste);
	
	int playing=1;
	
	//inicia o temporizador
	al_start_timer(timer);	
	
	while(playing) {
		
		ALLEGRO_EVENT ev;
		//espera por um evento e o armazena na variavel de evento ev
		al_wait_for_event(event_queue, &ev);
		
		//se o tipo de evento for um evento do temporizador, ou seja, se o tempo passou de t para t+1
		if(ev.type == ALLEGRO_EVENT_TIMER) {

		
			desenha_cenario();
			
			atualizaJogador(&jogador);
			
			atualizaPoste(poste, &jogador);

			desenha_jogador(jogador);	

			//atualiza a tela (quando houver algo para mostrar)
			al_flip_display();
			
			if(al_get_timer_count(timer)%(int)FPS == 0)
				printf("\n%d segundos se passaram...", (int)(al_get_timer_count(timer)/FPS));
		}
		//se o tipo de evento for o fechamento da tela (clique no x da janela)
		else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			playing = 0;
		}		
		//se o tipo de evento for um pressionar de uma tecla
		else if(ev.type == ALLEGRO_EVENT_KEY_DOWN) {
			//imprime qual tecla foi
			//printf("\ncodigo tecla: %d", ev.keyboard.keycode);
			
			if(ev.keyboard.keycode == ALLEGRO_KEY_A) {
				jogador.mov_esq = 1;
				int i;
				for (i=0; i<NUM_PRATOS; i++){
					if(poste[i].status == 1){
						poste[i].status = 0;
					}
				}
			}
			else if(ev.keyboard.keycode == ALLEGRO_KEY_D) {
				jogador.mov_dir = 1;
				int i;
				for (i=0; i<NUM_PRATOS; i++){
					if(poste[i].status == 1){
						poste[i].status = 0;
					}
				}
			}
			else if(ev.keyboard.keycode == ALLEGRO_KEY_SPACE){
				int i;
				for (i=0; i<NUM_PRATOS; i++){
					if(jogador.x >= poste[i].position - 4 && jogador.x <= poste[i].position + 4){
						poste[i].status = 1;
					}
				}
			}
		}
		//se o tipo de evento for um soltar de uma tecla
		else if(ev.type == ALLEGRO_EVENT_KEY_UP) {

			if(ev.keyboard.keycode == ALLEGRO_KEY_A) {
				jogador.mov_esq = 0;
			}
			else if(ev.keyboard.keycode == ALLEGRO_KEY_D) {
				jogador.mov_dir = 0;
			}
			else if(ev.keyboard.keycode == ALLEGRO_KEY_SPACE) {
				int i;
				for (i=0; i<NUM_PRATOS; i++){
					poste[i].status = 0;
				}
			}
		}		
	}

	al_destroy_timer(timer);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);
	
 
	return 0;
}
