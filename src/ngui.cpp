#include "ngui.h"

ngui::ngui(uint8_t enable_cursor){
	setlocale(LC_ALL, "");
	initscr();
	keypad(stdscr,TRUE); //habilita el uso del teclado
	cbreak();
	noecho();
	start_color(); //habilita el uso de color
	if(enable_cursor==1 || enable_cursor==2){
	curs_set(enable_cursor);  //habilita el cursor
	}
	else{
	curs_set(0);  //deshabilita el cursor		
	}
	
	init_pair(31, COLOR_WHITE, COLOR_WHITE); //RESERVADA SLIDER1 (fondo blanco slider verde)
	init_pair(32, COLOR_GREEN, COLOR_GREEN);//RESERVADA SLIDER1 (fondo blanco slider verde)
	init_pair(33, COLOR_BLACK, COLOR_WHITE);
	}

ngui::~ngui(){
	endwin();
	std::cout<<"Close Windows"<<std::endl;
	}

void ngui::crear_ventantaprincipal(std::string titulo, std::string nombre_pie_ventana){
	for(int a=0;a<(LINES);a++){//crea una linea vertical (inicio_y,inicio_x,caracter,ancho) //IZQUIERDA
		mvprintw(a,0,"\u2502");
		}
	for(int a=0;a<(LINES);a++){//crea una linea vertical (inicio_y,inicio_x,caracter,ancho) //DERECHA
		mvprintw(a,COLS-1,"\u2502");
		}
	for(int a=0;a<(COLS);a++){//crea una linea horizontal (inicio_y,inicio_x,caracter,ancho) //ARRIBA
		mvprintw(0,a,"\u2500");
		}
	for(int a=0;a<(COLS);a++){//crea una linea horizontal (inicio_y,inicio_x,caracter,ancho) //ABAJO
		mvprintw(LINES-1,a,"\u2500");
		}	
	mvprintw(0,0,"\u250C"); //arriba izquierda
	mvprintw(0,COLS-1,"\u2510"); //arriba derecha
	mvprintw(LINES-1,0,"\u2514"); //abajo izquierda
	mvprintw(LINES-1,COLS-1,"\u2518"); //abajo derecha 

	mvprintw(0,COLS*0.5-(titulo.length()/2),"| %s |",titulo.c_str());
	mvprintw(LINES-1,(COLS*0.85)-(nombre_pie_ventana.length()/2),"| %s |",nombre_pie_ventana.c_str());

	
	//refresh();
	
	}
	
void ngui::crear_ventantaprincipal(std::string titulo){
	for(int a=0;a<(LINES);a++){//crea una linea vertical (inicio_y,inicio_x,caracter,ancho) //IZQUIERDA
		mvprintw(a,0,"\u2502");
		}
	for(int a=0;a<(LINES);a++){//crea una linea vertical (inicio_y,inicio_x,caracter,ancho) //DERECHA
		mvprintw(a,COLS-1,"\u2502");
		}
	for(int a=0;a<(COLS);a++){//crea una linea horizontal (inicio_y,inicio_x,caracter,ancho) //ARRIBA
		mvprintw(0,a,"\u2500");
		}
	for(int a=0;a<(COLS);a++){//crea una linea horizontal (inicio_y,inicio_x,caracter,ancho) //ABAJO
		mvprintw(LINES-1,a,"\u2500");
		}	
	mvprintw(0,0,"\u250C"); //arriba izquierda
	mvprintw(0,COLS-1,"\u2510"); //arriba derecha
	mvprintw(LINES-1,0,"\u2514"); //abajo izquierda
	mvprintw(LINES-1,COLS-1,"\u2518"); //abajo derecha 
	
	mvprintw(0,COLS*0.5-(titulo.length()/2),"| %s |",titulo.c_str());
	
	
	//refresh();
	
	}
	
void ngui::crear_ventantaprincipal(std::string titulo, uint atributo_titulo){
	for(int a=0;a<(LINES);a++){//crea una linea vertical (inicio_y,inicio_x,caracter,ancho) //IZQUIERDA
		mvprintw(a,0,"\u2502");
		}
	for(int a=0;a<(LINES);a++){//crea una linea vertical (inicio_y,inicio_x,caracter,ancho) //DERECHA
		mvprintw(a,COLS-1,"\u2502");
		}
	for(int a=0;a<(COLS);a++){//crea una linea horizontal (inicio_y,inicio_x,caracter,ancho) //ARRIBA
		mvprintw(0,a,"\u2500");
		}
	for(int a=0;a<(COLS);a++){//crea una linea horizontal (inicio_y,inicio_x,caracter,ancho) //ABAJO
		mvprintw(LINES-1,a,"\u2500");
		}	
	mvprintw(0,0,"\u250C"); //arriba izquierda
	mvprintw(0,COLS-1,"\u2510"); //arriba derecha
	mvprintw(LINES-1,0,"\u2514"); //abajo izquierda
	mvprintw(LINES-1,COLS-1,"\u2518"); //abajo derecha 
	
	attron(atributo_titulo);
	mvprintw(0,COLS*0.5-(titulo.length()/2),"| %s |",titulo.c_str());
	attroff(atributo_titulo);

	
	//refresh();
	}
	
void ngui::crear_ventantaprincipal(std::string titulo, uint atributo_titulo, std::string nombre_pie_ventana,uint atributo_pie_ventana){
	for(int a=0;a<(LINES);a++){//crea una linea vertical//IZQUIERDA
		mvprintw(a,0,"\u2502");
		}
	for(int a=0;a<(LINES);a++){//crea una linea vertical//DERECHA
		mvprintw(a,COLS-1,"\u2502");
		}
	for(int a=0;a<(COLS);a++){//crea una linea horizontal //ARRIBA
		mvprintw(0,a,"\u2500");
		}
	for(int a=0;a<(COLS);a++){//crea una linea horizontal //ABAJO
		mvprintw(LINES-1,a,"\u2500");
		}	
	mvprintw(0,0,"\u250C"); //arriba izquierda
	mvprintw(0,COLS-1,"\u2510"); //arriba derecha
	mvprintw(LINES-1,0,"\u2514"); //abajo izquierda
	mvprintw(LINES-1,COLS-1,"\u2518"); //abajo derecha 
	
	attron(atributo_titulo);
	mvprintw(0,COLS*0.5-(titulo.length()/2),"| %s |",titulo.c_str());
	attroff(atributo_titulo);
	attron(atributo_pie_ventana);
	mvprintw(LINES-1,(COLS*0.85)-(nombre_pie_ventana.length()/2),"| %s |",nombre_pie_ventana.c_str());
	attroff(atributo_pie_ventana);
	
	//refresh();
	}

void ngui::crear_linea_horizontal(uint32_t x,uint32_t y,uint32_t ancho, char caracter){
	mvhline(y,x,caracter,ancho);
	//refresh();
	}

void ngui::slider_horizontal(uint32_t x,uint32_t y,uint32_t ancho,uint32_t v_max,uint32_t valor){
	uint32_t a = (valor*ancho/v_max);
	if(a>ancho){
		a=ancho;
	}
	attron(COLOR_PAIR(31));
	mvhline(y,x,' ',ancho); //crea una linea horizontal (inicio_y,inicio_x,caracter,ancho) //ARRIBA
	attroff(COLOR_PAIR(31));
	attron(COLOR_PAIR(32));
	mvhline(y,x,' ',a); //crea una linea horizontal (inicio_y,inicio_x,caracter,ancho) //ARRIBA
	attroff(COLOR_PAIR(32));
	//refresh();
	
	}
	
void ngui::slider_vertical(uint32_t x,uint32_t y,uint32_t alto,uint32_t v_max,uint32_t valor){
	
	attron(COLOR_PAIR(32));
	mvvline(y,x,' ',alto); //crea una linea vertical (inicio_y,inicio_x,caracter,alto) //ARRIBA
	mvvline(y,x+1,' ',alto); //crea una linea vertical (inicio_y,inicio_x,caracter,alto) //ARRIBA
	attroff(COLOR_PAIR(32));
	attron(COLOR_PAIR(31));
	mvvline(y,x,' ',(alto-(valor*alto/v_max))); //crea una linea vertical (inicio_y,inicio_x,caracter,alto) //ARRIBA
	mvvline(y,x+1,' ',(alto-(valor*alto/v_max))); //crea una linea vertical (inicio_y,inicio_x,caracter,alto) //ARRIBA
	attroff(COLOR_PAIR(31));
	//refresh();
	}

void ngui::crear_subventana(uint32_t x,uint32_t y,uint32_t ancho,uint32_t alto,std::string nombre){
	
	for(uint32_t a=y;a<(y+alto);a++){//crea una linea vertical (inicio_y,inicio_x,caracter,ancho) //IZQUIERDA
		mvprintw(a,x,"\u2502");
		}
	for(uint32_t a=y;a<(y+alto);a++){//crea una linea vertical (inicio_y,inicio_x,caracter,ancho) //DERECHA
		mvprintw(a,x+ancho,"\u2502");
		}
	for(uint32_t a=x;a<(x+ancho);a++){//crea una linea horizontal (inicio_y,inicio_x,caracter,ancho) //ARRIBA
		mvprintw(y,a,"\u2500");
		}
	for(uint32_t a=x;a<(x+ancho);a++){//crea una linea horizontal (inicio_y,inicio_x,caracter,ancho) //ABAJO
		mvprintw(y+alto,a,"\u2500");
		}	
	mvprintw(y,x,"\u250C"); //arriba izquierda
	mvprintw(y,x+ancho,"\u2510"); //arriba derecha
	mvprintw(y+alto,x,"\u2514"); //abajo izquierda
	mvprintw(y+alto,x+ancho,"\u2518"); //abajo derecha 
	
	//mvhline(y,x,'-',ancho); //crea una linea horizontal (inicio_y,inicio_x,caracter,ancho) //ARRIBA
	//mvhline((y+alto),x,'-',ancho); //crea una linea horizontal (inicio_y,inicio_x,caracter,ancho) //ABAJO
	//mvvline(y,x,'|',alto); //crea una linea horizontal (inicio_y,inicio_x,caracter,ancho) //IZQUIERDA
	//mvvline(y,(x+ancho),'|',alto); //crea una linea horizontal (inicio_y,inicio_x,caracter,ancho)  //DERECHA
	
	//mvaddch(y,x,'.');
	//mvaddch(y+alto,x,'*');
	//mvaddch(y,x+ancho,'.');
	//mvaddch(y+alto,x+ancho,'*');
	
	if(nombre!=" "){
	int a =(ancho/2)-((nombre.length()/2)+2);
	attron(A_BOLD);
	mvprintw(y,x+a," %s ",nombre.c_str());
	attroff(A_BOLD);	
}
	//refresh();
	}



void ngui::crear_subventana(uint32_t x,uint32_t y,uint32_t ancho,uint32_t alto,std::string nombre,std::string comentario_pieventana){
	for(uint32_t a=y;a<(y+alto);a++){//crea una linea vertical //IZQUIERDA
		mvprintw(a,x,"\u2502");
		}
	for(uint32_t a=y;a<(y+alto);a++){//crea una linea vertical  //DERECHA
		mvprintw(a,x+ancho,"\u2502");
		}
	for(uint32_t a=x;a<(x+ancho);a++){//crea una linea horizontal //ARRIBA
		mvprintw(y,a,"\u2500");
		}
	for(uint32_t a=x;a<(x+ancho);a++){//crea una linea horizontal //ABAJO
		mvprintw(y+alto,a,"\u2500");
		}	
	mvprintw(y,x,"\u250C"); //arriba izquierda
	mvprintw(y,x+ancho,"\u2510"); //arriba derecha
	mvprintw(y+alto,x,"\u2514"); //abajo izquierda
	mvprintw(y+alto,x+ancho,"\u2518"); //abajo derecha 
	
	if(nombre!=" " && comentario_pieventana!=" "){
	int a =(ancho/2)-((nombre.length()/2)+2);
	attron(A_BOLD);
	mvprintw(y,x+a," %s ",nombre.c_str());	
	attroff(A_BOLD);
	a =(ancho)-((comentario_pieventana.length())+3);
	mvprintw(y+alto,x+a," %s ",comentario_pieventana.c_str());	

}
	//refresh();
	}


void ngui::crear_subventana(uint32_t x,uint32_t y,uint32_t ancho,uint32_t alto){
	for(uint32_t a=y;a<(y+alto);a++){//crea una linea vertical  //IZQUIERDA
		mvprintw(a,x,"\u2502");
		}
	for(uint32_t a=y;a<(y+alto);a++){//crea una linea vertical //DERECHA
		mvprintw(a,x+ancho,"\u2502");
		}
	for(uint32_t a=x;a<(x+ancho);a++){//crea una linea horizontal //ARRIBA
		mvprintw(y,a,"\u2500");
		}
	for(uint32_t a=x;a<(x+ancho);a++){//crea una linea horizontal //ABAJO
		mvprintw(y+alto,a,"\u2500");
		}	
	mvprintw(y,x,"\u250C"); //arriba izquierda
	mvprintw(y,x+ancho,"\u2510"); //arriba derecha
	mvprintw(y+alto,x,"\u2514"); //abajo izquierda
	mvprintw(y+alto,x+ancho,"\u2518"); //abajo derecha 
	//refresh();
}

void ngui::borrar_subventana(uint32_t x,uint32_t y,uint32_t ancho,uint32_t alto){
	uint32_t b,a=y;
	b= alto+y+1;
	while(a<b){
    mvhline(a,x,' ',ancho+1); //borra por linea
    a++;
    //refresh();
}
}

void ngui::label_uint64(uint32_t x,uint32_t y,const uint64_t s){
	
	mvprintw(y,x,"%llu",s);
	//refresh();
	}

void ngui::label_uint32(uint32_t x,uint32_t y,const uint32_t s){
	
	mvprintw(y,x,"%lu",s);
	//refresh();
	}

void ngui::label_uint16(uint32_t x,uint32_t y,const uint16_t s){
	
	mvprintw(y,x,"%u",s);
	//refresh();
	}
	
void ngui::label(uint32_t x,uint32_t y,const char *texto){
	mvprintw(y,x,texto);
	//refresh();
	}
	
void ngui::label(uint32_t x,uint32_t y,const char *texto,const char *s){
	
	mvprintw(y,x,texto,s);
	//refresh();
	}
	
void ngui::label(uint32_t x,uint32_t y,const char *texto,uint atributo){
	attron(atributo);
	mvprintw(y,x,texto);
	attroff(atributo);
	//refresh();
	}

void ngui::paleta_color(short index,short color_letra, short color_fondo){
	init_pair(index, color_letra, color_fondo);
	}

void ngui::ventana_terminal_xy(int *x,int *y){
	getmaxyx(stdscr,*y,*x);
	}
	
void ngui::posicion_cursor(int *x, int *y){
	getyx(stdscr,*y,*x);
	}
	
void ngui::refrescar(){
	refresh();
	}
