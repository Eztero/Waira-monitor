#ifndef NGUI_H
#define NGUI_H
#include <ncurses.h>
#include <iostream>
#include <clocale>
//#include <unistd.h>

//LINES COLS

/*
ATRIBUTOS PARA LABEL

A_NORMAL         Visualizacion normal (sin resaltar)
A_STANDOUT       Mejor modo del terminal para resaltado
A_UNDERLINE      Subrayado
A_REVERSE        Video inverso
A_BLINK          Intermitente
A_DIM            Mitad de brillo
A_BOLD           Brillo extra o negrita
A_PROTECT        Modo protegido
A_INVIS          En modo Invisible o en blanco
A_ALTCHARSET     Juego de caracteres alternativo
A_CHARTEXT       Mascara de bits para extraer el caracter
COLOR_PAIR(n)    Par de color numero n

COLORES

COLOR_BLACK   0
COLOR_RED     1
COLOR_GREEN   2
COLOR_YELLOW  3
COLOR_BLUE    4
COLOR_MAGENTA 5
COLOR_CYAN    6
COLOR_WHITE   7

DETECTAR REDIMENCION TERMINAL
Se usa KEY_RESIZE para detectar las señales de redimención de Terminal

key = getch();
if (key == KEY_RESIZE) {}

 */
class ngui{
	
	public:
	ngui(uint8_t enable_cursor);
	~ngui();
	void crear_subventana(uint32_t x,uint32_t y,uint32_t ancho,uint32_t alto);
    void crear_subventana(uint32_t x,uint32_t y,uint32_t ancho,uint32_t alto,std::string nombre);
    void crear_subventana(uint32_t x,uint32_t y,uint32_t ancho,uint32_t alto,std::string nombre,std::string comentario_pieventana);
    void borrar_subventana(uint32_t x,uint32_t y,uint32_t ancho,uint32_t alto);
    void slider_horizontal(uint32_t x,uint32_t y,uint32_t ancho,uint32_t v_max,uint32_t valor);
    void slider_vertical(uint32_t x,uint32_t y,uint32_t alto,uint32_t v_max,uint32_t valor);
    void crear_ventantaprincipal(std::string titulo);
    void crear_ventantaprincipal(std::string titulo,uint atributo_titulo);
    void crear_ventantaprincipal(std::string titulo,std::string nombre_pie_ventana);
    void crear_ventantaprincipal(std::string titulo,uint atributo_titulo, std::string nombre_pie_ventana,uint atributo_pie_ventana);
    void label(uint32_t x,uint32_t y,const char *texto);
    void label(uint32_t x,uint32_t y,const char *texto,const char *s);
    void label(uint32_t x,uint32_t y,const char *texto,uint atributo);
    void paleta_color(short index,short color_letra, short color_fondo);
    void ventana_terminal_xy(int *x,int *y);
    void posicion_cursor(int *x, int *y);
	void crear_linea_horizontal(uint32_t x,uint32_t y,uint32_t ancho,char caracter);
	void refrescar(); //imprime los caracteres en la ventana
	
	private:
	
	
	
	};
#endif
