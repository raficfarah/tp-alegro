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


//largura do jogador
const float JOGADOR_W = 100;
const float JOGADOR1_H = 50;
const float JOGADOR2_H = 110;

typedef struct Jogador {

	float x;
	float tiroH;
	float jH;
	int tiro;
	int sentido;
	ALLEGRO_COLOR cor;

} Jogador;



void desenha_cenario() {
	ALLEGRO_COLOR BKG_COLOR = al_map_rgb(153,20,153);
	//ALLEGRO_COLOR POSTE_COLOR = al_map_rgb(255,255,255);
	//colore a tela de branco (rgb(255,255,255))
	al_clear_to_color(BKG_COLOR);
}

void inicializaJogadores(Jogador j[]) {
	int i;
	for(i=0; i<2; i++){
		if(i==0){
			j[i].x = 0;
			j[i].sentido = 1;
			j[i].cor = al_map_rgb(0, 223, 15);
			j[i].jH = JOGADOR1_H;
			j[i].tiroH = JOGADOR1_H;
		} else {
			j[i].x = SCREEN_W;
			j[i].sentido = -1;
			j[i].cor = al_map_rgb(34, 0, 255);
			j[i].jH = JOGADOR2_H;
			j[i].tiroH = JOGADOR2_H;
		}
	j[i].tiro = 0;

	}
}

void desenhaJogadores(Jogador j[]){
	int i;
	for(i=0; i<2; i++){
		
		if(i==0){
			al_draw_filled_triangle(j[i].x, JOGADOR1_H/2,
							j[i].x, JOGADOR1_H + JOGADOR1_H/2,
							j[i].x + j[i].sentido*(JOGADOR_W), JOGADOR1_H,
							j[i].cor);
		} else {
			al_draw_filled_triangle(j[i].x, JOGADOR1_H/2 + JOGADOR1_H + 10,
							j[i].x, JOGADOR1_H*2 + JOGADOR1_H/2 + 10,
							j[i].x + j[i].sentido*(JOGADOR_W), JOGADOR2_H,
							j[i].cor);
		}
	}
}

void desenhaTiro(Jogador j){
	if (j.sentido == 1){
		al_draw_filled_circle(j.x+JOGADOR_W, j.tiroH, 5, j.cor);
	} else {
		al_draw_filled_circle(j.x-JOGADOR_W, j.tiroH, 5, j.cor);
	}
}


void atualizaJogadores(Jogador j[]){
	int i, odd;
	float h1, h2;

	for(i=0; i<2; i++){
		if (j[i].x > 954 || j[i].x < 0) {
			odd = rand()%3+1;
			if (odd > 0){
				printf("\ntroca\n");
				h1 = j[0].tiroH;
				j[0].tiroH = j[1].tiroH;
				j[1].tiroH = h1;
			}
			j[i].sentido *= -1;
			
		}
		j[i].x += j[i].sentido;
		
		if (i==0){
			if (j[i].tiro == 1){
				desenhaTiro(j[i]);
				j[i].tiroH *= 1.03;
			}
		} else {
			if (j[i].tiro == 1){
				desenhaTiro(j[i]);
				j[i].tiroH *= 1.03;
			}
		}
	}
}


// void drawScore(char *txt, float score, ALLEGRO_FONT *fonte){
// 	sprintf(txt, "%.2f", score);
// 	al_draw_text(
// 		fonte, al_map_rgb(255,255,255), SCREEN_W-30, SCREEN_H - 30, ALLEGRO_ALIGN_RIGHT, txt
// 	);
	//al_flip_display();
//}
//
// int newRecord(float score, float *record) {
// 	FILE *arq = fopen("recorde.txt", "r");
// 	*record = -1;
// 	if (arq != NULL) {
// 		fscanf(arq, "%f", record);
// 		fclose(arq);
// 	}
// 	if (*record < score) {
// 		arq = fopen("recorde.txt", "w");
// 		fprintf(arq, "%.2f", score);
// 		fclose(arq);
// 		return 1;
// 	}
// 	return 0;
// }


int main(int argc, char **argv){

	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_TIMER *timer;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;

	// float score = 0;
	// char my_score[100];



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
	Jogador j[2];
	inicializaJogadores(j);


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

			atualizaJogadores(j);

			//desenhaScore(newScore);

			desenhaJogadores(j);

			//SCORE
			//score = al_get_timer_count(timer)/FPS;
			//drawScore(my_score, score, size_32);

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
    }

			// if(ev.keyboard.keycode == ALLEGRO_KEY_A) {
			// 	jogador.mov_esq = 1;
				// int i;
				// for (i=0; i<NUM_PRATOS; i++){
				// 	poste[i].status = 0;
				// }

			// else if(ev.keyboard.keycode == ALLEGRO_KEY_D) {
			// 	jogador.mov_dir = 1;
				// int i;
			// 	for (i=0; i<NUM_PRATOS; i++){
			// 		poste[i].status = 0;
			// 	}

			if(ev.keyboard.keycode == ALLEGRO_KEY_SPACE){
				if(j[0].tiro == 1) {
					j[0].tiro = 0;
					j[0].tiroH = JOGADOR1_H;	
				}
				//j[0].tiro = 0;
				j[0].tiro = 1;
			}

			if(ev.keyboard.keycode == ALLEGRO_KEY_ENTER){
				if(j[1].tiro == 1) {
					j[1].tiro = 0;
					j[1].tiroH = JOGADOR2_H;	
				}
				//j[0].tiro = 0;
				j[1].tiro = 1;
			}


			// else if(ev.keyboard.keycode == ALLEGRO_KEY_P) {
			// 	int i;
			// 	for (i=0; i<NUM_PRATOS; i++){
			// 		pratos[i].cheat = 1;
			// 	}
			// }

		//se o tipo de evento for um soltar de uma tecla
		// else if(ev.type == ALLEGRO_EVENT_KEY_UP) {
    //
		// 	if(ev.keyboard.keycode == ALLEGRO_KEY_A) {
		// 		jogador.mov_esq = 0;
		// 	}
		// 	else if(ev.keyboard.keycode == ALLEGRO_KEY_D) {
		// 		jogador.mov_dir = 0;
		// 	}
		// 	else if(ev.keyboard.keycode == ALLEGRO_KEY_SPACE) {
		// 		int i;
		// 		for (i=0; i<NUM_PRATOS; i++){
		// 			poste[i].status = 0;
		// 		}
		// 	}
		// 	else if(ev.keyboard.keycode == ALLEGRO_KEY_P) {
		// 		int i;
		// 		for (i=0; i<NUM_PRATOS; i++){
		// 			pratos[i].cheat = 0;
		// 		}
		// 	}
		// }

	// } //playing = 0;

	// char scoretxt[20];
  //
	// //pinta a tela de branco.
	// al_clear_to_color(al_map_rgb(255,255,255));
	// // atribui o score a scoretxt, e imprime no meio da tela.
	// sprintf(scoretxt, "Pontuação: %.2f", score);
	// al_draw_text(size_32, al_map_rgb (0,0,0), SCREEN_W/3, SCREEN_H/2, 0, scoretxt);


	// float record;
	// if (newRecord(score, &record)) {
	// 	al_draw_text(
	// 		size_32,
	// 		al_map_rgb(255, 0, 0),
	// 		SCREEN_W/3,
	// 		100+SCREEN_H/2,
	// 		0,
	// 		"NOVO RECORDE!"
	// 		);
	// } else {
	// 	sprintf(scoretxt, "Recorde: %.2f", record);
	// 	al_draw_text(size_32, al_map_rgb(0, 200, 30), SCREEN_W/3, 100+SCREEN_H/2, 0, scoretxt);
	// }
  }


	al_flip_display();
	al_rest(2);


	al_destroy_timer(timer);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);


	return 0;
}
