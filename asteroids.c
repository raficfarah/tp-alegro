#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>

#define STEER 0.1
#define MAX_SPEED 20
#define TIMER_REDUCTION 0.8
#define NUM_ASTEROIDS 10
#define DISTANCE 1000
#define MAX_ASTEROID_SPEED 4
#define MAX_RADIUS 80

  
 
const float FPS = 100;  
const int SCREEN_W = 640*1.5;
const int SCREEN_H = 540;
//variavel global que armazena o tamanho do ship (quadrado que vai navegar pela tela)
const int SHIP_SIZE = 16;
const int ENEMY_SIZE = 24;
const int CHASER_SIZE = 6;

int extraMode = 0;
float rotateAngle = 0;


typedef struct Point {
	float x;
	float y;
} Point;

typedef struct Asteroid {
	Point center;
	float radius;
	float xspeed;
	float yspeed;
	int time;
	ALLEGRO_COLOR color;
} Asteroid;

typedef struct Ship {
	Point A;
	Point B;
	Point C;
	Point center;
	int alive;
	ALLEGRO_COLOR color;
	ALLEGRO_BITMAP *bmp;
	float xspeed;
	float yspeed;
	float xcomp;
	float ycomp;
	float angle;
} Ship;

float dist(float x1, float y1, float x2, float y2) {
	return sqrt(pow(x1 - x2,2) + pow(y1 - y2,2));
}

float randFloat(float max) {
	return (float)rand()/RAND_MAX * max;
}

int collision(Ship s, Asteroid a) {
	if(dist(a.center.x, a.center.y, s.center.x+s.A.x, s.center.y+s.A.y) < a.radius || dist(a.center.x, a.center.y, s.center.x+s.B.x, s.center.y+s.B.y) < a.radius || dist(a.center.x, a.center.y, s.center.x+s.C.x, s.center.y+s.C.y) < a.radius)
		return 1;
	return 0;
}

void createAsteroid(Asteroid *a) {
	int left = rand()%2;
	int offset = rand()%DISTANCE;
	a->center.x = -1*left*offset + (1-left)*(SCREEN_W+offset); 
	int top = rand()%2;
	offset = rand()%DISTANCE;
	a->center.y = -1*top*offset + (1-top)*(SCREEN_H+offset); 
	int xdestiny = rand()%SCREEN_W;
	int ydestiny = rand()%SCREEN_H;
	float distance = sqrt(pow(a->center.x-xdestiny,2) + pow(a->center.y-ydestiny,2));
	a->xspeed = (xdestiny - a->center.x)/distance * (0.5+randFloat(MAX_ASTEROID_SPEED));
	a->yspeed = (ydestiny - a->center.y)/distance * (0.5+randFloat(MAX_ASTEROID_SPEED));
	a->radius = 1+rand()%MAX_RADIUS;
	a->color = al_map_rgb(56+rand()%250, 56+rand()%250, 56+rand()%250);
	
}

int newRecord(float score, float *record) {
	FILE *arq = fopen("recorde.txt", "r");
	*record = -1;
	if(arq != NULL) {
		fscanf(arq, "%f", record);
		fclose(arq);
	}
	if(*record < score ) {
		arq = fopen("recorde.txt", "w");
		fprintf(arq, "%.2f", score);
		fclose(arq);
		return 1;
	}
	return 0;
	
}

void Rotate(Point *P,float Angle) {
    float x=P->x,y=P->y;

    P->x=(x*cos(Angle))-(y*sin(Angle));
    P->y=(y*cos(Angle))+(x*sin(Angle));
}

void RotateShip(Ship *ship, float Angle) {
	Rotate(&(ship->A), Angle);
	Rotate(&(ship->B), Angle);
	Rotate(&(ship->C), Angle);
	ship->angle = (ship->angle - Angle);
	if(ship->angle > 2*M_PI) ship->angle -= 2*M_PI;
	if(ship->angle < 0) ship->angle = 2*M_PI + ship->angle ;
	ship->xcomp = cos(ship->angle);
	ship->ycomp = -sin(ship->angle);
	printf("\nangle: %f, xcomp: %f, ycomp: %f, xspeed: %f, yspeed: %f", ship->angle, ship->xcomp, ship->ycomp, ship->xspeed, ship->yspeed);
}

void drawAsteroid(Asteroid *a) {
	a->center.x += a->xspeed;
	a->center.y += a->yspeed;
	al_draw_filled_circle(a->center.x, a->center.y, a->radius, a->color);
	if(fabs(a->center.x) > SCREEN_W*1.5 || fabs(a->center.x) < -SCREEN_W*0.5 || fabs(a->center.y) > SCREEN_H*1.5 || fabs(a->center.y) < - SCREEN_H*0.5)
		createAsteroid(a);
}

void drawShip(Ship *ship) {
	
	ship->center.x += STEER*ship->xspeed;
	ship->center.y += STEER*ship->yspeed;

	if(ship->center.x > SCREEN_W)
		ship->center.x = 0;
	else if(ship->center.x < 0)
		ship->center.x = SCREEN_W;
	if(ship->center.y > SCREEN_H)
		ship->center.y = 0;
	else if(ship->center.y < 0)
		ship->center.y = SCREEN_H;
	
	al_draw_filled_triangle(ship->center.x + ship->A.x, ship->center.y + ship->A.y, ship->center.x + ship->B.x, ship->center.y + ship->B.y, ship->center.x + ship->C.x, ship->center.y + ship->C.y, ship->color);
}

void updateSpeed (float *speed, float delta) {
	if((delta > 0 && *speed < MAX_SPEED) || (delta < 0 && *speed > -MAX_SPEED))
		*speed = *speed + delta;
}

 
int main(int argc, char **argv){
	
	if(argc > 1) {
		if(strcmp(argv[1],"extras") == 0)
			extraMode = 1;
	}
	
	srand(time(NULL));
	
	
	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;
	Ship ship;
	int playing = 1;

	int i,j;
	float score=0;
	char my_score[10];
	int steering = 0;
	
   
	//----------------------- rotinas de inicializacao ---------------------------------------
   if(!al_init()) {
      fprintf(stderr, "failed to initialize allegro!\n");
      return -1;
   }
   
    if(!al_init_primitives_addon()){
		fprintf(stderr, "failed to initialize primitives!\n");
        return -1;
    }
 
   timer = al_create_timer(1.0 / FPS);
   if(!timer) {
      fprintf(stderr, "failed to create timer!\n");
      return -1;
   }
 
   display = al_create_display(SCREEN_W, SCREEN_H);
   if(!display) {
      fprintf(stderr, "failed to create display!\n");
      al_destroy_timer(timer);
      return -1;
   }
   
	//inicializa o modulo allegro que carrega as fontes
    al_init_font_addon();
	//inicializa o modulo allegro que entende arquivos tff de fontes
    al_init_ttf_addon();

	//carrega o arquivo arial.ttf da fonte Arial e define que sera usado o tamanho 32 (segundo parametro)
    ALLEGRO_FONT *size_32 = al_load_font("arial.ttf", 32, 1);   
 
	ALLEGRO_COLOR BKG_COLOR = al_map_rgb(0,0,0);
   
   al_install_keyboard();
   
	//ship
	ALLEGRO_COLOR scolors[2];
	scolors[0] = al_map_rgb(0, 255, 0);
	scolors[1] = al_map_rgb(0, 0, 255);
	
	ship.alive = 1;
	ship.center.x = (SCREEN_W)/2.0;
	ship.center.y = (SCREEN_H)/2.0;
	ship.A.x = 0;
	ship.A.y = -20;
	ship.B.x = 10;
	ship.B.y = 20;
	ship.C.x = -10;
	ship.C.y = 20;
	ship.angle = M_PI/2;
	ship.xcomp = cos(ship.angle);
	ship.ycomp = -sin(ship.angle);	
	ship.xspeed = 0;
	ship.yspeed = 0;
	ship.color = scolors[0];
	
	//asteroids
	Asteroid asteroids[NUM_ASTEROIDS];
	for(i=0; i<NUM_ASTEROIDS; i++)
		createAsteroid(&asteroids[i]);
	   
//al_draw_filled_triangle(float x1, float y1, float x2, float y2, float x3, float y3, ALLEGRO_COLOR color);
	   
    
	//avisa o allegro que agora eu quero modificar as propriedades da tela
   al_set_target_bitmap(al_get_backbuffer(display));
	//colore a tela de preto (rgb(0,0,0))
   al_clear_to_color(BKG_COLOR);   
 
	//cria a fila de eventos
   event_queue = al_create_event_queue();
   if(!event_queue) {
      fprintf(stderr, "failed to create event_queue!\n");
      al_destroy_display(display);
      al_destroy_timer(timer);
      return -1;
   }
 
	//registra na fila de eventos que eu quero identificar quando a tela foi alterada
   al_register_event_source(event_queue, al_get_display_event_source(display));
	//registra na fila de eventos que eu quero identificar quando o tempo alterou de t para t+1
   al_register_event_source(event_queue, al_get_timer_event_source(timer));
   //registra que a fila de eventos deve detectar quando uma tecla for pressionada no teclado
   al_register_event_source(event_queue, al_get_keyboard_event_source());   
   
  

	//reinicializa a tela
   al_flip_display();
	//inicia o temporizador
   al_start_timer(timer);

	//enquanto playing for verdadeiro, faca:
   while(playing) {
      ALLEGRO_EVENT ev;
	  //espera por um evento e o armazena na variavel de evento ev
      al_wait_for_event(event_queue, &ev);
	  

	
	//se o tipo do evento for uma tecla pressionada
	if(ev.type == ALLEGRO_EVENT_KEY_DOWN) {
		//verifica qual tecla foi
		switch(ev.keyboard.keycode) {
			//se a tecla for o W
			case ALLEGRO_KEY_W:
				steering = 1;
			break;
			case ALLEGRO_KEY_S:
				//steering = -1;
			break;
			case ALLEGRO_KEY_A:
				rotateAngle = -M_PI/40;
			break;
			case ALLEGRO_KEY_D:
				rotateAngle = M_PI/40;
				
			break;
			
		}		

	}	  
	else if(ev.type == ALLEGRO_EVENT_KEY_UP) {
		//verifica qual tecla foi
		switch(ev.keyboard.keycode) {
			//se a tecla for o W
			case ALLEGRO_KEY_W:
				steering = 0;
			break;
			case ALLEGRO_KEY_S:
				//steering = 0;
			break;
			case ALLEGRO_KEY_A:
				rotateAngle = 0;
			break;
			case ALLEGRO_KEY_D:
				rotateAngle = 0;
				
			break;
			
		}		

	}	  
		
		//se o tipo de evento for um evento do temporizador, ou seja, se o tempo passou de t para t+1
    else if(ev.type == ALLEGRO_EVENT_TIMER) {
		
		//limpo a tela
		al_clear_to_color(BKG_COLOR);
		//desenho o ship nas novas posicoes x e y

		if(steering != 0) {
			updateSpeed(&ship.xspeed, steering*ship.xcomp);
			updateSpeed(&ship.yspeed, steering*ship.ycomp);
		}
		
		if(rotateAngle != 0)
			RotateShip(&ship, rotateAngle);
		drawShip(&ship);
		for(i=0; i<NUM_ASTEROIDS; i++) {
			drawAsteroid(&asteroids[i]);
		}
		 
		
		//SCORE
		score = al_get_timer_count(timer)/FPS;
		sprintf(my_score, "%.2f", score);
        //imprime o texto armazenado em my_text na posicao x=10,y=10 e com a cor rgb(128,200,30)
        al_draw_text(size_32, al_map_rgb(255, 0, 0), SCREEN_W-100, 20, 0, my_score);
		 
		 

		//reinicializo a tela
		 al_flip_display();
		 
		 for(i=0; i<NUM_ASTEROIDS; i++) {
			if(collision(ship, asteroids[i])) {
				al_rest(2);
				playing = 0;
				break;
			}
		}
		 

    }
	//se o tipo de evento for o fechamento da tela (clique no x da janela)
	else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
		//interrompe o while(1)
		break;
	}
	
  } //fim do while
  

	
	char my_text[20];	
		
	//colore toda a tela de preto
	al_clear_to_color(al_map_rgb(230,240,250));
	//imprime o texto armazenado em my_text na posicao x=10,y=10 e com a cor rgb(128,200,30)
	sprintf(my_text, "Score: %.2f", score);
	al_draw_text(size_32, al_map_rgb(200, 0, 30), SCREEN_W/3, SCREEN_H/2, 0, my_text);
	float record;
	if(newRecord(score, &record)) {
		al_draw_text(size_32, al_map_rgb(200, 20, 30), SCREEN_W/3, 100+SCREEN_H/2, 0, "NEW RECORD!");
	}
	else {
		sprintf(my_text, "Record: %.2f", record);
		al_draw_text(size_32, al_map_rgb(0, 200, 30), SCREEN_W/3, 100+SCREEN_H/2, 0, my_text);
	}

	
	//reinicializa a tela
	al_flip_display();	
    al_rest(1);		
   
	//procedimentos de fim de jogo (fecha a tela, limpa a memoria, etc)
	
  
   al_destroy_timer(timer);
   al_destroy_display(display);
   al_destroy_event_queue(event_queue);
 
   return 0;
}