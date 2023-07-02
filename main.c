#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>

#define NUM_PRATOS 8

//ALLEGRO_TIMER *timer = NULL;


const float FPS = 60;  

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
	int jogo;
	
} Jogador;

typedef struct Prato {
	float x;
	
	/* um valor entre 0 e 255, em que 0 = prato equilibrado e
	   1 = prato com maxima energia, prestes a cair */
	float energia;
	int status;
	int tempo;
	ALLEGRO_COLOR cor;
	int cheat;
	
} Prato;

typedef struct Poste {
	int x, y;
	int status;
	ALLEGRO_COLOR cor;

} Poste;



void desenha_cenario() {
	ALLEGRO_COLOR BKG_COLOR = al_map_rgb(153,20,153);
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
	j->vel = 1.5;
	j->jogo = 1;
}


void inicializaPratos(Prato pratos[]) {
	int i;

	srand(time(NULL));

	for(i=0; i<NUM_PRATOS; i++) {
		pratos[i].x = (i+1)*106;

		if (i == 3 || i == 4) {
			pratos[i].tempo = rand() % 4 + 6;
		} else if (i == 2 || i == 5) {
			pratos[i].tempo = rand() % 10 + 20;
		} else if (i == 1 || i == 6) {
			pratos[i].tempo = rand() % 14 + 24;
		} else if (i == 0 || i == 7) {
			pratos[i].tempo = rand() % 18 + 28;
		}
	}
	
	pratos[i].energia = 1;
	pratos[i].status = 0;
	pratos[i].cor = al_map_rgb(255,0,0);

	pratos[i].cheat = 0;
}

void desenhaPrato(Prato pratos[]){
	int i;

	for (i=0; i < NUM_PRATOS; i++){
		if (pratos[i].status == 1){
			al_draw_filled_rectangle(
				pratos[i].x - 20, 98, pratos[i].x + 20, 108,al_map_rgb(255,255-pratos[i].energia,255-pratos[i].energia)
			);
		}
		if (pratos[i].energia >= 255) {
			al_draw_filled_rectangle(
				pratos[i].x - 20, 98, pratos[i].x + 20, 108,al_map_rgb(153,20,153)
			);
			al_draw_filled_rectangle(
			pratos[i].x, 440, pratos[i].x + 40, 452,al_map_rgb(255,0,0)
			);
			al_flip_display();
			al_rest(1);
		}
	}
}

int atualizaPrato(Prato pratos[], Poste poste[],ALLEGRO_TIMER* timer){
	int i;
	
	for (i= 0; i < 8; i++){
		if ((al_get_timer_count(timer)/FPS) - pratos[i].tempo > 0){
			pratos[i].status = 1;
		}
		if (pratos[i].status == 1){
			if (
				poste[i].x == pratos[i].x && poste[i].status != 0 && pratos[i].energia >= 3
			){
				pratos[i].energia -= 2;
				//printf("\nposte %d ativado", i);
			}

			if (pratos[i].cheat == 0){
				if (pratos[i].energia < 255 /* && pratos[i].energia > 230 */){
					pratos[i].energia += 0.15;
				}
			else {
					pratos[i].energia += 0;
				}
			}
			
			desenhaPrato(pratos);

			if (pratos[i].energia >= 255) {
				desenhaPrato(pratos);
				return 0;
			}
		}		
	}
	return 1;
}


void inicializaPoste(Poste poste[]){
	ALLEGRO_COLOR BRANCO = al_map_rgb(255,255,255); 
	int i;
	for (i=0; i<NUM_PRATOS; i++){
		poste[i].x = (i+1)*106;
	}

	poste[i].y = POSTE_H;
	poste[i].status = 0;
	poste[i].cor = BRANCO;
}

void atualizaPoste(Poste poste[], Jogador *j) {
	int i;
	for (i=0; i<NUM_PRATOS; i++){
		if (poste[i].status != 0) {
			poste[i].cor = al_map_rgb(255,0,0);
			al_draw_line(poste[i].x, 108, poste[i].x, 432, poste[i].cor, 5);
		}
	}
}


void drawScore(char *txt, float score, ALLEGRO_FONT *fonte){
	sprintf(txt, "%.2f", score);
	al_draw_text(
		fonte, al_map_rgb(255,255,255), SCREEN_W-30, SCREEN_H - 30, ALLEGRO_ALIGN_RIGHT, txt
	);
	//al_flip_display();
}

int newRecord(float score, float *record) {
	FILE *arq = fopen("recorde.txt", "r");
	*record = -1;
	if (arq != NULL) {
		fscanf(arq, "%f", record);
		fclose(arq);
	}
	if (*record < score) {
		arq = fopen("recorde.txt", "w");
		fprintf(arq, "%.2f", score);
		fclose(arq);
		return 1;
	}
	return 0;
}


 
int main(int argc, char **argv){
	
	ALLEGRO_DISPLAY *display = NULL;	
	ALLEGRO_TIMER *timer;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	
	float score = 0;
	char my_score[100];



	//inicializa o Allegro
	if(!al_init()) {
		fprintf(stderr, "failed to initialize allegro!\n");
		return -1;
	}
	
	//cria um temporizador que incrementa uma unidade a cada 1.0/FPS segundos
    timer = al_create_timer(1.0/FPS);
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

			//desenha prato e muda estado do jogo caso o prato caia
			playing = atualizaPrato(pratos, poste, timer);

			atualizaJogador(&jogador);
			
			atualizaPoste(poste, &jogador);

			//desenhaScore(newScore);

			desenha_jogador(jogador);
			
			//SCORE
			score = al_get_timer_count(timer)/FPS;
			drawScore(my_score, score, size_32);

			//atualiza a tela (quando houver algo para mostrar)
			al_flip_display();
			
			/* if(al_get_timer_count(timer)%(int)FPS == 0)
				printf("\n%d segundos se passaram...", (int)(al_get_timer_count(timer)/FPS)); */
		}
		//se o tipo de evento for o fechamento da tela (clique no x da janela)
		else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			playing = 0;
		}		
		//se o tipo de evento for um pressionar de uma tecla
		else if(ev.type == ALLEGRO_EVENT_KEY_DOWN) {
			//imprime qual tecla foi
			printf("\ncodigo tecla: %d", ev.keyboard.keycode);
			
			if(ev.keyboard.keycode == ALLEGRO_KEY_A) {
				jogador.mov_esq = 1;
				int i;
				for (i=0; i<NUM_PRATOS; i++){
					poste[i].status = 0;
				}
			}
			else if(ev.keyboard.keycode == ALLEGRO_KEY_D) {
				jogador.mov_dir = 1;
				int i;
				for (i=0; i<NUM_PRATOS; i++){
					poste[i].status = 0;
				}
			}
			else if(ev.keyboard.keycode == ALLEGRO_KEY_SPACE){
				int i;
				jogador.jogo = 0;
				for (i=0; i<NUM_PRATOS; i++){
					if(jogador.x >= poste[i].x - 4 && jogador.x <= poste[i].x + 4){
						if(jogador.mov_dir != 0 || jogador.mov_esq != 0){
							poste[i].status = 0;
						}
						else {
							poste[i].status = 1;
						}
					}
				}
			}
			else if(ev.keyboard.keycode == ALLEGRO_KEY_P) {
				int i;
				for (i=0; i<NUM_PRATOS; i++){
					pratos[i].cheat = 1;
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
			else if(ev.keyboard.keycode == ALLEGRO_KEY_P) {
				int i;
				for (i=0; i<NUM_PRATOS; i++){
					pratos[i].cheat = 0;
				}
			}
		}	

	} //playing = 0;
	
	char scoretxt[20];

	//pinta a tela de branco.
	al_clear_to_color(al_map_rgb(255,255,255));
	// atribui o score a scoretxt, e imprime no meio da tela.
	sprintf(scoretxt, "Pontuação: %.2f", score);
	al_draw_text(size_32, al_map_rgb (0,0,0), SCREEN_W/3, SCREEN_H/2, 0, scoretxt);

	
	float record;
	if (newRecord(score, &record)) {
		al_draw_text(
			size_32,
			al_map_rgb(255, 0, 0),
			SCREEN_W/3,
			100+SCREEN_H/2,
			0, 
			"NOVO RECORDE!"
			);
	} else {
		sprintf(scoretxt, "Recorde: %.2f", record);
		al_draw_text(size_32, al_map_rgb(0, 200, 30), SCREEN_W/3, 100+SCREEN_H/2, 0, scoretxt);
	}


	al_flip_display();
	al_rest(2);


	al_destroy_timer(timer);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);
	
 
	return 0;
}
