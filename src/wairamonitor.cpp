/*
Waira-monitor
It is a lightweight monitor developed for the operators of Stake Pool of Cardano
https://github.com/Eztero/Waira-monitor
GPL-3.0 License
*/

#include <iostream>
#include <string>
#include <thread>
#include <atomic>
#include "consultadatos.h"
#include "ngui.h"
#include <unistd.h>
#include <csignal>
#include "lectura_red.h"
#define VERSION_WAIRA "wairamonitor 0.7"

void categorizar_ips(std::string puertos_abiertos[], uint16_t *index_puertosabiertos,std::string ip_p2p[][2], uint16_t *index_ip_p2p,std::string ip_huerfana_salida[][2], uint16_t *index_ip_huerfana_salida,std::string ip_huerfana_entrada[][2], uint16_t *index_ip_huerfana_entrada);
void salidaforzosa_abort(int a);
void salidaforzosa_term(int a);
void tiempo_restantes_slot(const uint32_t slot_actual, uint16_t *dia,uint16_t *hora,uint16_t *min,uint16_t *seg);
void tiempo_restantes_seg(const uint64_t segundos, uint16_t *dia,uint16_t *hora,uint16_t *min,uint16_t *seg);
void cargar_ui();

std::atomic<bool> salir (false);
std::atomic<int> ktecla (0);

ngui *ventana=new ngui(0);
consultadatos *consulta=new consultadatos();

int main(){
    std::signal(SIGABRT,salidaforzosa_abort); // Se declara una funcion en caso de existir un cierre inesperado
    std::signal(SIGTERM,salidaforzosa_term); // Se declara una funcion en caso de solicitar un termino de programa
    ventana->paleta_color(1,COLOR_RED,COLOR_BLACK);  // Se configuran los colores que se usaran en el interfaz, paleta_color(index,letra,fondo)
    ventana->paleta_color(2,COLOR_CYAN,COLOR_BLACK);
    ventana->paleta_color(3,COLOR_YELLOW,COLOR_BLACK);
    ventana->paleta_color(4,COLOR_GREEN,COLOR_BLACK);
    ventana->paleta_color(6,COLOR_WHITE,COLOR_RED);

    ventana->refrescar(); //se muestran los cambios

    // Se crea la ventana de carga y se leen los datos del archivo de configuracion
    const double ventanade_carga[4]={1,1,82,15}; //(x,y,ancho,alto)
    ventana->crear_subventana(ventanade_carga[0],ventanade_carga[1],ventanade_carga[2],ventanade_carga[3],"Loading Configuration");
    if(!consulta->cargar_configuracion(ventana, &ventanade_carga[0])){
        ventana->label(ventanade_carga[0]+2,ventanade_carga[1]+13,"There are error, verify your config file \"wairamonitor.conf\"");
        ventana->label(ventanade_carga[0]+2,ventanade_carga[1]+14,"Press any key for exit");
        ventana->refrescar();
        getch();
        delete consulta;
        delete ventana;
        return 0;
    }

    // Se borra la ventana de caga
    ventana->borrar_subventana(ventanade_carga[0],ventanade_carga[1],ventanade_carga[2],ventanade_carga[3]);
    ventana->refrescar();

    std::thread hilo_ui (cargar_ui); //se crea un hilo aparte para la funcion void cargar_ui();

    // ----El bucle para las deteccion de las teclas---
    ktecla=0;
    do{
        ktecla = getch();
    }while(ktecla != 113);//pasa el valor a la variable global ktecla y lo compara
    salir=true;	 // Cambia el valor de la variable salir, para sacar del bucle a la funcion cargar_ui
    //-----

    if(hilo_ui.joinable()){ //comprueba si la funcion del thread hilo_ui se puede unir a este hilo
        hilo_ui.join();
    }

    delete consulta;
    delete ventana;

    return 0;
}

void cargar_ui(){
    //---Array de datos para TCP
    std::string tcp_dec[540][5];
    std::string ip_p2p[100][2]; //[index][ip,puerto_local]
    std::string puerto_escucha[50];
    std::string ip_huerfana_salida[100][2];
    std::string ip_huerfana_entrada[100][2];

    //---index para TCP---------
    uint16_t index_puerto_escucha;
    uint16_t index_ip_p2p;
    uint16_t index_ip_huerfana_salida;
    uint16_t index_ip_huerfana_entrada;

    //---buffer para post borrado para TCP---
    uint16_t buff_index_ip_huerfana_entrada=0;
    uint16_t buff_index_ip_huerfana_salida=0;
    uint16_t buff_index_ip_p2p=0;
    uint16_t buff_index_puerto_escucha=0;

    enum strlength{  //ASIGNAR CANTIDAD A espacios[], actual 22
        epoca,
        slot_epoca,
        dias_restante_kes,
        memoria,
        transacciones_mempool,
        peers,
        forks,
        bloques_asignados,
        bloques_creados,
        bloques_perdidos,
        slots_perdidos,
        pledge,
        stake_total,
        stake_activo,
        delegadores,
        saturacion,
        esp_version,
        esp_local_nodo_version,
        cpu,
        threads_nodo,
        bloque_size,
        bloque_delay,
    };

    uint16_t espacios[22];
    for(int a=0;a<18;a++){ //establece todos los espacios en 1
        espacios[a]=1;
    }
    bool acceso_prometheus, acceso_github, acceso_adapools, acceso_versionnodo, switchmain, switchpeer, switchabout;
    std::string version, estado, local_nodo_version;
    uint32_t tbuf,txmin, txmax=1;  //variables usadas para los calculos del mempool
    uint16_t hora,dia,min,seg,conteo_nodo,conteo_github,conteo_adapools;
    switchmain=true;
    switchpeer=false;
    switchabout=false;
    conteo_nodo=0;
    conteo_github=0;
    conteo_adapools=0;
    double pos_x=COLS*0.25;
    //double pos_y=LINES*0.3;
    double pos_ancho, pos_ancho2,pos_ancho3 ;//pos_alto;

    //---Ventana de Loading Data---
    ventana->label(2,2,"Loading Data ",A_BOLD); //ventana carga slider
    ventana->slider_horizontal(15,2,10,5,0); //ventana carga slider
    ventana->refrescar();
    uint32_t puerto_n =consulta->puerto_nodo();
    char cbuff[std::to_string(puerto_n).length()+1];
    strcpy(cbuff, std::to_string(puerto_n).c_str());  //se pasa el puerto a un char[array]
    ventana->slider_horizontal(15,2,10,5,1); //ventana carga slider
    ventana->refrescar();
    if(consulta->actualizar_datos(&puerto_n)){acceso_prometheus=true;}else{acceso_prometheus=false;}
    ventana->slider_horizontal(15,2,10,5,2); //ventana carga slider
    ventana->refrescar();
    if(consulta->github_habilitado()){if(consulta->github(&version,&estado)){acceso_github=true;}else{acceso_github=false;}}else{acceso_github=false;}
    ventana->slider_horizontal(15,2,10,5,3); //ventana carga slider
    ventana->refrescar();
    if(consulta->adapools_habilitado()){if(consulta->actualizar_adapools()){acceso_adapools=true;}else{acceso_adapools=false;}}else{acceso_adapools=false;}
    ventana->slider_horizontal(15,2,10,5,4); //ventana carga slider
    ventana->refrescar();
    if(consulta->version_nodo(&local_nodo_version)){acceso_versionnodo=true;}else{acceso_versionnodo=false;}
    ventana->slider_horizontal(15,2,10,5,5); //ventana carga slider
    ventana->refrescar();

    //Se borra el "Loading"
    mvhline(2,2,' ',COLS/2);

    //Se calculan las dimenciones de las ventanas
    ventana->crear_ventantaprincipal(consulta->poolnamew(),A_BOLD,VERSION_WAIRA,A_BOLD);
    ventana->label(1,LINES-2,"Press \"q\" for Quit, \"m\" for Main ,\"p\" for Peer,\"a\" for About");
    pos_ancho=(COLS*0.3);
    if(pos_ancho<31){ ////calculo el ancho minimo para ventana_estadisticapool_node
        pos_ancho=31;
    }
    double ventana_estadisticapool_node[4]={1,1,pos_ancho,17};  //Dimensiones para la ventana de estadistica

    double ventana_tagversion[4]={1,19,pos_ancho,4};  //Dimensiones para la ventana de tags

    pos_x=(COLS-(COLS*0.3)-2);
    pos_ancho2=(COLS*0.3);

    if(pos_ancho2<28){ ////calculo el ancho minimo para ventana_stake
        pos_ancho2=28;
    }
    double ventana_stake[4]={pos_x,1,pos_ancho2,9}; //Dimensiones para la ventana de  stake

    double ventana_mined[4]={pos_x,11,pos_ancho2,5}; //Dimensiones para la ventana de forge

    pos_ancho3=(pos_x-(5+pos_ancho));
    if(pos_ancho3<27){ ////calculo el ancho minimo para ventana_memoria
        pos_ancho3=27;
    }
    double ventana_memoria[4]={(3+pos_ancho),1,pos_ancho3,4};  //Dimensiones para la ventana de memoria

    double ventana_trx[4]={(3+pos_ancho),6,pos_ancho3,5};  //Dimensiones para la ventana de transacciones

    double ventana_peer[4]={(3+pos_ancho),12,(pos_x-(5+pos_ancho)),2};  //Dimensiones para la ventana de peer

    double ventana_blockchain[4]={(3+pos_ancho),15,pos_ancho3,3};  //Dimensiones para la ventana de blockchain

    //---Dimenciones de las ventanas TCP-----------
    double alto=LINES-4;
    double ventana_puertos_abiertos[4]={1,1,16,alto};
    double ventana_interconectados[4]={18,1,25,alto};
    double ventana_huerfano_entrada[4]={48,1,25,alto};
    double ventana_huerfano_salida[4]={74,1,25,alto};

    if(consulta->kes_actual()==0){
        ventana->crear_subventana(ventana_estadisticapool_node[0],ventana_estadisticapool_node[1],ventana_estadisticapool_node[2],ventana_estadisticapool_node[3],"Relay");
    }else{
        ventana->crear_subventana(ventana_estadisticapool_node[0],ventana_estadisticapool_node[1],ventana_estadisticapool_node[2],ventana_estadisticapool_node[3],"Producer");
    }
    //Se crean las ventanas
    ventana->crear_subventana(ventana_memoria[0],ventana_memoria[1],ventana_memoria[2],ventana_memoria[3],"Node process");
    ventana->crear_subventana(ventana_trx[0],ventana_trx[1],ventana_trx[2],ventana_trx[3],"Transactions");
    ventana->crear_subventana(ventana_peer[0],ventana_peer[1],ventana_peer[2],ventana_peer[3],"Peers");
    ventana->crear_subventana(ventana_blockchain[0],ventana_blockchain[1],ventana_blockchain[2],ventana_blockchain[3],"Blockchain");

    //Se completa las ventanas con sus nombres
    if(acceso_prometheus){
        ventana->label(ventana_estadisticapool_node[0]+1,ventana_estadisticapool_node[1]+1,"Epoch: ");
        ventana->label(ventana_estadisticapool_node[0]+1,ventana_estadisticapool_node[1]+2,"Slot: ");
        ventana->label(ventana_estadisticapool_node[0]+1,ventana_estadisticapool_node[1]+3,"Block number: ");
        ventana->label(ventana_estadisticapool_node[0]+1,ventana_estadisticapool_node[1]+4,"Block size: ");
        ventana->label(ventana_estadisticapool_node[0]+1,ventana_estadisticapool_node[1]+5,"Block delay: ");
        ventana->label(ventana_estadisticapool_node[0]+1,ventana_estadisticapool_node[1]+10,"Total Slot: ");
        if(consulta->kes_actual()!=0){
            ventana->label(ventana_estadisticapool_node[0]+1,ventana_estadisticapool_node[1]+11,"Actual KES period: ");
            ventana->label((ventana_estadisticapool_node[0]+ventana_estadisticapool_node[2]*0.5-13),ventana_estadisticapool_node[1]+13,"Operational Certificate", A_UNDERLINE | A_BOLD);
            ventana->label(ventana_estadisticapool_node[0]+1,ventana_estadisticapool_node[1]+14,"Remaining days for expire: ");
            ventana->label(ventana_estadisticapool_node[0]+1,ventana_estadisticapool_node[1]+15,"Created in KES period: ");
            ventana->label(ventana_estadisticapool_node[0]+1,ventana_estadisticapool_node[1]+16,"Expire in KES period: ");
        }else{
            ventana->label((ventana_estadisticapool_node[0]+ventana_estadisticapool_node[2]*0.5-13),ventana_estadisticapool_node[1]+13,"-No information about KES-", A_UNDERLINE | A_BOLD);
        }

        ventana->label(ventana_memoria[0]+1,ventana_memoria[1]+1,"RAM (Used):");
        ventana->label(ventana_memoria[0]+1,ventana_memoria[1]+2,"CPU:");
        ventana->label(ventana_memoria[0]+1,ventana_memoria[1]+3,"Threads:");

        ventana->label(ventana_trx[0]+1,ventana_trx[1]+1,"In mempool: ");
        ventana->label((ventana_trx[0]+ventana_trx[2]-11),ventana_trx[1]+1,"Max: ");
        ventana->label(ventana_trx[0]+1,ventana_trx[1]+4,"Total Processed: ");

        ventana->label(ventana_peer[0]+1,ventana_peer[1]+1,"Connected: ");

        ventana->label(ventana_blockchain[0]+1,ventana_blockchain[1]+1,"Density: ");
        ventana->label(ventana_blockchain[0]+1,ventana_blockchain[1]+2,"Fork: ");



    }else{
        ventana->label((ventana_estadisticapool_node[0]+2),ventana_estadisticapool_node[1]+7,"Can't be accessed");
        ventana->label((ventana_estadisticapool_node[0]+2),ventana_estadisticapool_node[1]+8,"to port %s",cbuff);
        ventana->label(ventana_memoria[0]+1,ventana_memoria[1]+1,"Can't be accessed");
        ventana->label(ventana_trx[0]+1,ventana_trx[1]+1,"Can't be accessed");
        ventana->label(ventana_peer[0]+1,ventana_peer[1]+1,"Can't be accessed");
        ventana->label(ventana_blockchain[0]+1,ventana_blockchain[1]+1,"Can't be accessed");
        ventana->label(ventana_mined[0]+1,ventana_mined[1]+2,"Can't be accessed");
    }
    if(acceso_github){
        //Crea ventana tag
        ventana->crear_subventana(ventana_tagversion[0],ventana_tagversion[1],ventana_tagversion[2],ventana_tagversion[3],"Tag version","Consulted to github");
        ventana->label(ventana_tagversion[2]*0.3,ventana_tagversion[1]+1,"version");
        ventana->label(ventana_tagversion[2]*0.6,ventana_tagversion[1]+1,"status");
        ventana->label(ventana_tagversion[0]+1,ventana_tagversion[1]+2,"Github");
        ventana->label(ventana_tagversion[0]+1,ventana_tagversion[1]+3,"Local");
    }else{
        if(consulta->github_habilitado()){
            //Crea ventana tag
            ventana->crear_subventana(ventana_tagversion[0],ventana_tagversion[1],ventana_tagversion[2],ventana_tagversion[3],"Tag version","Consulted to github");
            ventana->label(ventana_tagversion[2]*0.3,ventana_tagversion[1]+2,"Can't be accessed to Github");
            ventana->label(ventana_tagversion[0]+1,ventana_tagversion[1]+3,"Local");
        }
        else{
            //Crea ventana tag
            ventana_tagversion[3]=3;
            ventana->crear_subventana(ventana_tagversion[0],ventana_tagversion[1],ventana_tagversion[2],ventana_tagversion[3],"Tag version");
            ventana->label(ventana_tagversion[0]+1,ventana_tagversion[1]+2,"Local");
        }
        ventana->label(ventana_tagversion[2]*0.3,ventana_tagversion[1]+1,"version");
        ventana->label(ventana_tagversion[2]*0.6,ventana_tagversion[1]+1,"status");

    }
    if(acceso_adapools){ //si pudo conectar con el servidor
        //Se crean las ventana de stake y mined
        ventana->crear_subventana(ventana_stake[0],ventana_stake[1],ventana_stake[2],ventana_stake[3],"Stake", "Consulted to adapools");

        if(consulta->kes_actual()!=0){ //se crea la ventana forge solo cuando detecta el kes del productor
            ventana->crear_subventana(ventana_mined[0],ventana_mined[1],ventana_mined[2],ventana_mined[3],"Forge");
        }

        ventana->label(ventana_stake[0]+1,ventana_stake[1]+1,"Pledge: ");
        ventana->label(ventana_stake[0]+1,ventana_stake[1]+2,"Total Stake: ");
        ventana->label(ventana_stake[0]+1,ventana_stake[1]+3,"Live Stake: ");
        ventana->label(ventana_stake[0]+1,ventana_stake[1]+4,"Delegator: ");
        ventana->label((ventana_stake[0]+ventana_stake[2]*0.5-6),ventana_stake[1]+6,"Saturation:");
    }else{ //si no pudo conectar
        if(consulta->adapools_habilitado()){ // pero, la opcion esta habilitada
            //Se crean las ventana de stake y mined
            ventana->crear_subventana(ventana_stake[0],ventana_stake[1],ventana_stake[2],ventana_stake[3],"Stake", "Consulted to adapools");

            if(consulta->kes_actual()!=0){ //se crea la ventana forge solo cuando detecta el kes del productor
                ventana->crear_subventana(ventana_mined[0],ventana_mined[1],ventana_mined[2],ventana_mined[3],"Forge");
            }

            ventana->label(ventana_stake[0]+1,ventana_stake[1]+4,"Can't be accessed to Adapools");
        }
        else{ //si esta desahabilitada
            ventana_mined[1]=1; //se desplaza la ventana forge a la posicion y=1
            ventana->crear_subventana(ventana_mined[0],ventana_mined[1],ventana_mined[2],ventana_mined[3],"Forge");
        }
    }
    //despues de crear las ventana stake y mined, asigno las letras
    if(consulta->kes_actual()!=0){
        ventana->label(ventana_mined[0]+1,ventana_mined[1]+1,"Blocks lead: ");
        ventana->label(ventana_mined[0]+1,ventana_mined[1]+2,"Blocks minted: ");
        ventana->label(ventana_mined[0]+1,ventana_mined[1]+3,"Blocks missed: ");
        ventana->label(ventana_mined[0]+1,ventana_mined[1]+4,"Slots missed: ");
    }

    ventana->refrescar();

    while(!salir){  //este es el bucle de la funcion cargar_ui
        switch (ktecla){
        case KEY_RESIZE:{
            ventana->borrar_subventana(0,0,COLS,LINES); //Borra la ventana principal, junto con todas
            ventana->refrescar();

            pos_ancho=(COLS*0.3);
            if(pos_ancho<31){ ////calculo el ancho minimo para ventana_estadisticapool_node
                pos_ancho=31;
            }
            ventana_estadisticapool_node[0]=1;//Dimensiones para la ventana de estadistica
            ventana_estadisticapool_node[1]=1;
            ventana_estadisticapool_node[2]=pos_ancho;
            ventana_estadisticapool_node[3]=17;

            ventana_tagversion[0]=1;//Dimensiones para la ventana de tags
            ventana_tagversion[1]=19;
            ventana_tagversion[2]=pos_ancho;
            ventana_tagversion[3]=4;

            pos_x=(COLS-(COLS*0.3)-2);
            pos_ancho2=(COLS*0.3);
            if(pos_ancho2<28){ ////calculo el ancho minimo para ventana_stake
                pos_ancho2=28;
            }
            ventana_stake[0]=pos_x;//Dimensiones para la ventana de  stake
            ventana_stake[1]=1;
            ventana_stake[2]=pos_ancho2;
            ventana_stake[3]=9;

            ventana_mined[0]=pos_x;//Dimensiones para la ventana de forge
            ventana_mined[1]=11;
            ventana_mined[2]=pos_ancho2;
            ventana_mined[3]=5;

            pos_ancho3=(pos_x-(5+pos_ancho));
            if(pos_ancho3<27){ ////calculo el ancho minimo para ventana_memoria
                pos_ancho3=27;
            }
            ventana_memoria[0]=(3+pos_ancho);//Dimensiones para la ventana de memoria
            ventana_memoria[1]=1;
            ventana_memoria[2]=pos_ancho3;
            ventana_memoria[3]=4;

            ventana_trx[0]=(3+pos_ancho);//Dimensiones para la ventana de transacciones
            ventana_trx[1]=6;
            ventana_trx[2]=pos_ancho3;
            ventana_trx[3]=5;


            ventana_peer[0]=(3+pos_ancho);//Dimensiones para la ventana de peer
            ventana_peer[1]=12;
            ventana_peer[2]=pos_ancho3;
            ventana_peer[3]=2;

            ventana_blockchain[0]=(3+pos_ancho);//Dimensiones para la ventana de blockchain
            ventana_blockchain[1]=15;
            ventana_blockchain[2]=pos_ancho3;
            ventana_blockchain[3]=3;

            //---Dimenciones de las ventanas TCP-----------
            double alto=LINES-4;
            ventana_puertos_abiertos[3]=alto;
            ventana_interconectados[3]=alto;
            ventana_huerfano_entrada[3]=alto;
            ventana_huerfano_salida[3]=alto;

            ventana->crear_ventantaprincipal(consulta->poolnamew(),A_BOLD,VERSION_WAIRA,A_BOLD);
            ventana->label(1,LINES-2,"Press \"q\" for Quit, \"m\" for Main ,\"p\" for Peer,\"a\" for About");
            if(switchpeer==true && switchabout==false && switchmain==false){
                //---Crea las ventanas
                ventana->crear_subventana(ventana_interconectados[0],ventana_interconectados[1],ventana_interconectados[2],ventana_interconectados[3],"Interconected \u2B81","TCP established");
                ventana->crear_subventana(ventana_huerfano_entrada[0],ventana_huerfano_entrada[1],ventana_huerfano_entrada[2],ventana_huerfano_entrada[3],"Lonely input \u2B63","TCP established");
                ventana->crear_subventana(ventana_huerfano_salida[0],ventana_huerfano_salida[1],ventana_huerfano_salida[2],ventana_huerfano_salida[3],"Lonely output \u2B61","TCP established");
                ventana->crear_subventana(ventana_puertos_abiertos[0],ventana_puertos_abiertos[1],ventana_puertos_abiertos[2],ventana_puertos_abiertos[3],"Listen port \u2B26","All TCP ports");
            }
            if(switchpeer==false && switchabout==false && switchmain==true){
                if(consulta->kes_actual()==0){
                    ventana->crear_subventana(ventana_estadisticapool_node[0],ventana_estadisticapool_node[1],ventana_estadisticapool_node[2],ventana_estadisticapool_node[3],"Relay");
                }else{
                    ventana->crear_subventana(ventana_estadisticapool_node[0],ventana_estadisticapool_node[1],ventana_estadisticapool_node[2],ventana_estadisticapool_node[3],"Producer");
                }
                ventana->crear_subventana(ventana_memoria[0],ventana_memoria[1],ventana_memoria[2],ventana_memoria[3],"Node process");
                ventana->crear_subventana(ventana_trx[0],ventana_trx[1],ventana_trx[2],ventana_trx[3],"Transactions");
                ventana->crear_subventana(ventana_peer[0],ventana_peer[1],ventana_peer[2],ventana_peer[3],"Peers");
                ventana->crear_subventana(ventana_blockchain[0],ventana_blockchain[1],ventana_blockchain[2],ventana_blockchain[3],"Blockchain");

                //Se completa las ventanas con sus nombres
                if(acceso_prometheus){
                    ventana->label(ventana_estadisticapool_node[0]+1,ventana_estadisticapool_node[1]+1,"Epoch: ");
                    ventana->label(ventana_estadisticapool_node[0]+1,ventana_estadisticapool_node[1]+2,"Slot: ");
                    ventana->label(ventana_estadisticapool_node[0]+1,ventana_estadisticapool_node[1]+3,"Block number: ");
                    ventana->label(ventana_estadisticapool_node[0]+1,ventana_estadisticapool_node[1]+4,"Block size: ");
                    ventana->label(ventana_estadisticapool_node[0]+1,ventana_estadisticapool_node[1]+5,"Block delay: ");
                    ventana->label(ventana_estadisticapool_node[0]+1,ventana_estadisticapool_node[1]+10,"Total Slot: ");
                    if(consulta->kes_actual()!=0){
                        ventana->label(ventana_estadisticapool_node[0]+1,ventana_estadisticapool_node[1]+11,"Actual KES period: ");
                        ventana->label((ventana_estadisticapool_node[0]+ventana_estadisticapool_node[2]*0.5-13),ventana_estadisticapool_node[1]+13,"Operational Certificate", A_UNDERLINE | A_BOLD);
                        ventana->label(ventana_estadisticapool_node[0]+1,ventana_estadisticapool_node[1]+14,"Remaining days for expire: ");

                        ventana->label(ventana_estadisticapool_node[0]+1,ventana_estadisticapool_node[1]+15,"Created in KES period: ");
                        ventana->label(ventana_estadisticapool_node[0]+1,ventana_estadisticapool_node[1]+16,"Expire in KES period: ");
                    }else{
                        ventana->label((ventana_estadisticapool_node[0]+ventana_estadisticapool_node[2]*0.5-13),ventana_estadisticapool_node[1]+13,"-No information about KES-", A_UNDERLINE | A_BOLD);
                    }

                    ventana->label(ventana_memoria[0]+1,ventana_memoria[1]+1,"RAM (Used):");
                    ventana->label(ventana_memoria[0]+1,ventana_memoria[1]+2,"CPU:");
                    ventana->label(ventana_memoria[0]+1,ventana_memoria[1]+3,"Threads:");

                    ventana->label(ventana_trx[0]+1,ventana_trx[1]+1,"In mempool: ");
                    ventana->label((ventana_trx[0]+ventana_trx[2]-11),ventana_trx[1]+1,"Max: ");
                    ventana->label(ventana_trx[0]+1,ventana_trx[1]+4,"Total Processed: ");

                    ventana->label(ventana_peer[0]+1,ventana_peer[1]+1,"Connected: ");

                    ventana->label(ventana_blockchain[0]+1,ventana_blockchain[1]+1,"Density: ");
                    ventana->label(ventana_blockchain[0]+1,ventana_blockchain[1]+2,"Fork: ");


                }else{
                    ventana->label((ventana_estadisticapool_node[0]+2),ventana_estadisticapool_node[1]+7,"Can't be accessed");
                    ventana->label((ventana_estadisticapool_node[0]+2),ventana_estadisticapool_node[1]+8,"to port %s",cbuff);
                    ventana->label(ventana_memoria[0]+1,ventana_memoria[1]+1,"Can't be accessed");
                    ventana->label(ventana_trx[0]+1,ventana_trx[1]+1,"Can't be accessed");
                    ventana->label(ventana_peer[0]+1,ventana_peer[1]+1,"Can't be accessed");
                    ventana->label(ventana_blockchain[0]+1,ventana_blockchain[1]+1,"Can't be accessed");
                    ventana->label(ventana_mined[0]+1,ventana_mined[1]+2,"Can't be accessed");
                }
                if(acceso_github){
                    //Crea ventana tag
                    ventana->crear_subventana(ventana_tagversion[0],ventana_tagversion[1],ventana_tagversion[2],ventana_tagversion[3],"Tag version","Consulted to github");
                    ventana->label(ventana_tagversion[2]*0.3,ventana_tagversion[1]+1,"version");
                    ventana->label(ventana_tagversion[2]*0.6,ventana_tagversion[1]+1,"status");
                    ventana->label(ventana_tagversion[0]+1,ventana_tagversion[1]+2,"Github");
                    ventana->label(ventana_tagversion[0]+1,ventana_tagversion[1]+3,"Local");
                }else{
                    if(consulta->github_habilitado()){
                        //Crea ventana tag
                        ventana->crear_subventana(ventana_tagversion[0],ventana_tagversion[1],ventana_tagversion[2],ventana_tagversion[3],"Tag version","Consulted to github");
                        ventana->label(ventana_tagversion[2]*0.3,ventana_tagversion[1]+2,"Can't be accessed to Github");
                        ventana->label(ventana_tagversion[0]+1,ventana_tagversion[1]+3,"Local");

                    }
                    else{
                        //Crea ventana tag
                        ventana_tagversion[3]=3;
                        ventana->crear_subventana(ventana_tagversion[0],ventana_tagversion[1],ventana_tagversion[2],ventana_tagversion[3],"Tag version");
                        ventana->label(ventana_tagversion[0]+1,ventana_tagversion[1]+2,"Local");
                    }
                    ventana->label(ventana_tagversion[2]*0.3,ventana_tagversion[1]+1,"version");
                    ventana->label(ventana_tagversion[2]*0.6,ventana_tagversion[1]+1,"status");

                }
                if(acceso_adapools){
                    ventana->crear_subventana(ventana_stake[0],ventana_stake[1],ventana_stake[2],ventana_stake[3],"Stake", "Consulted to adapools");
                    if(consulta->kes_actual()!=0){ //se crea la ventana forge solo cuando detecta el kes del productor
                        ventana->crear_subventana(ventana_mined[0],ventana_mined[1],ventana_mined[2],ventana_mined[3],"Forge");
                    }
                    ventana->label(ventana_stake[0]+1,ventana_stake[1]+1,"Pledge: ");
                    ventana->label(ventana_stake[0]+1,ventana_stake[1]+2,"Total Stake: ");
                    ventana->label(ventana_stake[0]+1,ventana_stake[1]+3,"Live Stake: ");
                    ventana->label(ventana_stake[0]+1,ventana_stake[1]+4,"Delegator: ");
                    ventana->label((ventana_stake[0]+ventana_stake[2]*0.5-6),ventana_stake[1]+6,"Saturation:");
                }else{
                    if(consulta->adapools_habilitado()){
                        //Se crean las ventana de stake y mined
                        ventana->crear_subventana(ventana_stake[0],ventana_stake[1],ventana_stake[2],ventana_stake[3],"Stake", "Consulted to adapools");
                        if(consulta->kes_actual()!=0){ //se crea la ventana forge solo cuando detecta el kes del productor
                            ventana->crear_subventana(ventana_mined[0],ventana_mined[1],ventana_mined[2],ventana_mined[3],"Forge");
                        }
                        ventana->label(ventana_stake[0]+1,ventana_stake[1]+4,"Can't be accessed to Adapools");
                    }else{
                        if(consulta->kes_actual()!=0){ //se crea la ventana forge solo cuando detecta el kes del productor
                            ventana_mined[1]=1;
                            ventana->crear_subventana(ventana_mined[0],ventana_mined[1],ventana_mined[2],ventana_mined[3],"Forge");
                        }
                    }
                }
                //Despues de crear las ventanas stake y mined asigno las letras
                if(consulta->kes_actual()!=0){ //se crea la ventana forge solo cuando detecta el kes del productor
                    ventana->label(ventana_mined[0]+1,ventana_mined[1]+1,"Blocks lead: ");
                    ventana->label(ventana_mined[0]+1,ventana_mined[1]+2,"Blocks minted: ");
                    ventana->label(ventana_mined[0]+1,ventana_mined[1]+3,"Blocks missed: ");
                    ventana->label(ventana_mined[0]+1,ventana_mined[1]+4,"Slots missed: ");
                }
                //switchmain=true;
                //switchpeer=false;
                //switchabout=false;
                ventana->refrescar();
            }
            ktecla=0;
        };break;
        case 109:{  //tecla m
            if(switchmain==false &&(switchpeer=true || switchabout==true)){
                ventana->borrar_subventana(0,0,COLS,LINES);
                ventana->refrescar();
                ventana->crear_ventantaprincipal(consulta->poolnamew(),A_BOLD,VERSION_WAIRA,A_BOLD);
                ventana->label(1,LINES-2,"Press \"q\" for Quit, \"m\" for Main ,\"p\" for Peer,\"a\" for About");
                if(consulta->kes_actual()==0){
                    ventana->crear_subventana(ventana_estadisticapool_node[0],ventana_estadisticapool_node[1],ventana_estadisticapool_node[2],ventana_estadisticapool_node[3],"Relay");
                }else{
                    ventana->crear_subventana(ventana_estadisticapool_node[0],ventana_estadisticapool_node[1],ventana_estadisticapool_node[2],ventana_estadisticapool_node[3],"Producer");
                }
                ventana->crear_subventana(ventana_memoria[0],ventana_memoria[1],ventana_memoria[2],ventana_memoria[3],"Node process");
                ventana->crear_subventana(ventana_trx[0],ventana_trx[1],ventana_trx[2],ventana_trx[3],"Transactions");
                ventana->crear_subventana(ventana_peer[0],ventana_peer[1],ventana_peer[2],ventana_peer[3],"Peers");
                ventana->crear_subventana(ventana_blockchain[0],ventana_blockchain[1],ventana_blockchain[2],ventana_blockchain[3],"Blockchain");

                //Se completa las ventanas con sus nombres
                if(acceso_prometheus){
                    ventana->label(ventana_estadisticapool_node[0]+1,ventana_estadisticapool_node[1]+1,"Epoch: ");
                    ventana->label(ventana_estadisticapool_node[0]+1,ventana_estadisticapool_node[1]+2,"Slot: ");
                    ventana->label(ventana_estadisticapool_node[0]+1,ventana_estadisticapool_node[1]+3,"Block number: ");
                    ventana->label(ventana_estadisticapool_node[0]+1,ventana_estadisticapool_node[1]+4,"Block size: ");
                    ventana->label(ventana_estadisticapool_node[0]+1,ventana_estadisticapool_node[1]+5,"Block delay: ");
                    ventana->label(ventana_estadisticapool_node[0]+1,ventana_estadisticapool_node[1]+10,"Total Slot: ");
                    if(consulta->kes_actual()!=0){
                        ventana->label(ventana_estadisticapool_node[0]+1,ventana_estadisticapool_node[1]+11,"Actual KES period: ");
                        ventana->label((ventana_estadisticapool_node[0]+ventana_estadisticapool_node[2]*0.5-13),ventana_estadisticapool_node[1]+13,"Operational Certificate", A_UNDERLINE | A_BOLD);
                        ventana->label(ventana_estadisticapool_node[0]+1,ventana_estadisticapool_node[1]+14,"Remaining days for expire: ");

                        ventana->label(ventana_estadisticapool_node[0]+1,ventana_estadisticapool_node[1]+15,"Created in KES period: ");
                        ventana->label(ventana_estadisticapool_node[0]+1,ventana_estadisticapool_node[1]+16,"Expire in KES period: ");
                    }else{
                        ventana->label((ventana_estadisticapool_node[0]+ventana_estadisticapool_node[2]*0.5-13),ventana_estadisticapool_node[1]+13,"-No information about KES-", A_UNDERLINE | A_BOLD);
                    }

                    ventana->label(ventana_memoria[0]+1,ventana_memoria[1]+1,"RAM (Used):");
                    ventana->label(ventana_memoria[0]+1,ventana_memoria[1]+2,"CPU:");
                    ventana->label(ventana_memoria[0]+1,ventana_memoria[1]+3,"Threads:");

                    ventana->label(ventana_trx[0]+1,ventana_trx[1]+1,"In mempool: ");
                    ventana->label((ventana_trx[0]+ventana_trx[2]-11),ventana_trx[1]+1,"Max: ");
                    ventana->label(ventana_trx[0]+1,ventana_trx[1]+4,"Total Processed: ");

                    ventana->label(ventana_peer[0]+1,ventana_peer[1]+1,"Connected: ");

                    ventana->label(ventana_blockchain[0]+1,ventana_blockchain[1]+1,"Density: ");
                    ventana->label(ventana_blockchain[0]+1,ventana_blockchain[1]+2,"Fork: ");



                }else{
                    ventana->label((ventana_estadisticapool_node[0]+2),ventana_estadisticapool_node[1]+7,"Can't be accessed");
                    ventana->label((ventana_estadisticapool_node[0]+2),ventana_estadisticapool_node[1]+8,"to port %s",cbuff);
                    ventana->label(ventana_memoria[0]+1,ventana_memoria[1]+1,"Can't be accessed");
                    ventana->label(ventana_trx[0]+1,ventana_trx[1]+1,"Can't be accessed");
                    ventana->label(ventana_peer[0]+1,ventana_peer[1]+1,"Can't be accessed");
                    ventana->label(ventana_blockchain[0]+1,ventana_blockchain[1]+1,"Can't be accessed");
                    ventana->label(ventana_mined[0]+1,ventana_mined[1]+2,"Can't be accessed");
                }
                if(acceso_github){
                    //Crea ventana tag
                    ventana->crear_subventana(ventana_tagversion[0],ventana_tagversion[1],ventana_tagversion[2],ventana_tagversion[3],"Tag version","Consulted to github");
                    ventana->label(ventana_tagversion[2]*0.3,ventana_tagversion[1]+1,"version");
                    ventana->label(ventana_tagversion[2]*0.6,ventana_tagversion[1]+1,"status");
                    ventana->label(ventana_tagversion[0]+1,ventana_tagversion[1]+2,"Github");
                    ventana->label(ventana_tagversion[0]+1,ventana_tagversion[1]+3,"Local");
                }else{
                    if(consulta->github_habilitado()){
                        //Crea ventana tag
                        ventana->crear_subventana(ventana_tagversion[0],ventana_tagversion[1],ventana_tagversion[2],ventana_tagversion[3],"Tag version","Consulted to github");
                        ventana->label(ventana_tagversion[2]*0.3,ventana_tagversion[1]+2,"Can't be accessed to Github");
                        ventana->label(ventana_tagversion[0]+1,ventana_tagversion[1]+3,"Local");
                    }
                    else{
                        //Crea ventana tag
                        ventana_tagversion[3]=3;
                        ventana->crear_subventana(ventana_tagversion[0],ventana_tagversion[1],ventana_tagversion[2],ventana_tagversion[3],"Tag version");
                        ventana->label(ventana_tagversion[0]+1,ventana_tagversion[1]+2,"Local");
                    }
                    ventana->label(ventana_tagversion[2]*0.3,ventana_tagversion[1]+1,"version");
                    ventana->label(ventana_tagversion[2]*0.6,ventana_tagversion[1]+1,"status");
                }
                if(acceso_adapools){
                    ventana->crear_subventana(ventana_stake[0],ventana_stake[1],ventana_stake[2],ventana_stake[3],"Stake", "Consulted to adapools");
                    if(consulta->kes_actual()!=0){ //se crea la ventana forge solo cuando detecta el kes del productor
                        ventana->crear_subventana(ventana_mined[0],ventana_mined[1],ventana_mined[2],ventana_mined[3],"Forge");
                    }
                    ventana->label(ventana_stake[0]+1,ventana_stake[1]+1,"Pledge: ");
                    ventana->label(ventana_stake[0]+1,ventana_stake[1]+2,"Total Stake: ");
                    ventana->label(ventana_stake[0]+1,ventana_stake[1]+3,"Live Stake: ");
                    ventana->label(ventana_stake[0]+1,ventana_stake[1]+4,"Delegator: ");
                    ventana->label((ventana_stake[0]+ventana_stake[2]*0.5-6),ventana_stake[1]+6,"Saturation:");
                }
                else{
                    if(consulta->adapools_habilitado()){
                        //Se crean las ventana de stake y mined
                        ventana->crear_subventana(ventana_stake[0],ventana_stake[1],ventana_stake[2],ventana_stake[3],"Stake", "Consulted to adapools");
                        if(consulta->kes_actual()!=0){ //se crea la ventana forge solo cuando detecta el kes del productor
                            ventana->crear_subventana(ventana_mined[0],ventana_mined[1],ventana_mined[2],ventana_mined[3],"Forge");
                        }
                        ventana->label(ventana_stake[0]+1,ventana_stake[1]+4,"Can't be accessed to Adapools");
                    }else{
                        if(consulta->kes_actual()!=0){ //se crea la ventana forge solo cuando detecta el kes del productor
                            ventana_mined[1]=1;
                            ventana->crear_subventana(ventana_mined[0],ventana_mined[1],ventana_mined[2],ventana_mined[3],"Forge");
                        }
                    }
                }
                //Despues de crear las ventana stake y mined,asigno las letras
                if(consulta->kes_actual()!=0){ //se crea la ventana forge solo cuando detecta el kes del productor
                    ventana->label(ventana_mined[0]+1,ventana_mined[1]+1,"Blocks lead: ");
                    ventana->label(ventana_mined[0]+1,ventana_mined[1]+2,"Blocks minted: ");
                    ventana->label(ventana_mined[0]+1,ventana_mined[1]+3,"Blocks missed: ");
                    ventana->label(ventana_mined[0]+1,ventana_mined[1]+4,"Slots missed: ");
                }
                switchmain=true;
                switchpeer=false;
                switchabout=false;
                ventana->refrescar();
            }
            ktecla=0;
        };break;
        case 97:{ //tecla a
            if(switchabout==false && (switchmain==true || switchpeer==true)){
                ventana->borrar_subventana(0,0,COLS,LINES);
                ventana->refrescar();
                ventana->crear_ventantaprincipal(consulta->poolnamew(),A_BOLD,VERSION_WAIRA,A_BOLD);
                ventana->label(1,LINES-2,"Press \"q\" for Quit, \"m\" for Main ,\"p\" for Peer,\"a\" for About");
                ventana->label((COLS/2)-30,(LINES/2)-4,"WairaMonitor is a lightweight monitor written in c++ ");
                ventana->label((COLS/2)-30,(LINES/2)-3,"and uses ncurses for its graphical interface.");
                ventana->label((COLS/2)-30,(LINES/2)-1,"Created for Eztero, Operator of TricahuePool");
                ventana->label((COLS/2)-30,(LINES/2)-0,"Twitter: @TCHPool");
                ventana->label((COLS/2)-30,(LINES/2)+1,"Web: https://eztero.github.io/TricahuePool/index_en.html");
                switchmain=false;
                switchpeer=false;
                switchabout=true;
                ventana->refrescar();
            }
        };break;
        case 112:{ //tecla p
            if(switchpeer==false && (switchabout==true || switchmain==true)){
                ventana->borrar_subventana(0,0,COLS,LINES);
                ventana->refrescar();
                ventana->crear_ventantaprincipal(consulta->poolnamew(),A_BOLD,VERSION_WAIRA,A_BOLD);
                ventana->label(1,LINES-2,"Press \"q\" for Quit, \"m\" for Main ,\"p\" for Peer,\"a\" for About");
                //---Crea las ventanas
                ventana->crear_subventana(ventana_interconectados[0],ventana_interconectados[1],ventana_interconectados[2],ventana_interconectados[3],"Interconected \u2B81","TCP established");
                ventana->crear_subventana(ventana_huerfano_entrada[0],ventana_huerfano_entrada[1],ventana_huerfano_entrada[2],ventana_huerfano_entrada[3],"Lonely input \u2B63","TCP established");
                ventana->crear_subventana(ventana_huerfano_salida[0],ventana_huerfano_salida[1],ventana_huerfano_salida[2],ventana_huerfano_salida[3],"Lonely output \u2B61","TCP established");
                ventana->crear_subventana(ventana_puertos_abiertos[0],ventana_puertos_abiertos[1],ventana_puertos_abiertos[2],ventana_puertos_abiertos[3],"Listen port \u2B26","All TCP ports");
                switchpeer=true;
                switchmain=false;
                switchabout=false;
                ventana->refrescar();
            }
            ktecla=0;
        };break;
        default:{
            if(switchmain){
                if(acceso_prometheus){
                    //---------Datos de la ventana Producor/Relay
                    tiempo_restantes_slot(consulta->slot_epoca(),&dia,&hora,&min,&seg); //calcula cuanto falta para terminar la epoca segun el slot actual
                    //Epoch:
                    ventana->crear_linea_horizontal(ventana_estadisticapool_node[0]+10,ventana_estadisticapool_node[1]+1,espacios[epoca],' ');
                    ventana->label_uint32(ventana_estadisticapool_node[0]+10,ventana_estadisticapool_node[1]+1,consulta->epoca());
                    espacios[epoca]=std::to_string(consulta->epoca()).length();
                    //Slot:
                    ventana->crear_linea_horizontal(ventana_estadisticapool_node[0]+8,ventana_estadisticapool_node[1]+2,espacios[slot_epoca],' ');
                    ventana->label_uint32(ventana_estadisticapool_node[0]+8,ventana_estadisticapool_node[1]+2,consulta->slot_epoca());
                    espacios[slot_epoca]=std::to_string(consulta->slot_epoca()).length();
                    //Block number:
                    ventana->label_uint64(ventana_estadisticapool_node[0]+15,ventana_estadisticapool_node[1]+3,consulta->numero_bloque());
                    //Block size
                    ventana->crear_linea_horizontal(ventana_estadisticapool_node[0]+13,ventana_estadisticapool_node[1]+4,espacios[bloque_size],' ');
                    mvprintw(ventana_estadisticapool_node[1]+4,ventana_estadisticapool_node[0]+13,"%.2f Kb",consulta->bloque_kbytes());
                    espacios[bloque_size]=std::to_string(consulta->bloque_kbytes()).length()+3;
                    //Block delay
                    ventana->crear_linea_horizontal(ventana_estadisticapool_node[0]+13,ventana_estadisticapool_node[1]+5,espacios[bloque_delay],' ');
                    //ventana->label(ventana_estadisticapool_node[0]+13,ventana_estadisticapool_node[1]+5,consulta->bloque_delay());
                    mvprintw(ventana_estadisticapool_node[1]+5,ventana_estadisticapool_node[0]+13,"%s seg",consulta->bloque_delay().c_str());
                    espacios[bloque_delay]=consulta->bloque_delay().length()+4;
                    //Slider Epoca
                    mvprintw(ventana_estadisticapool_node[1]+7,ventana_estadisticapool_node[0]+ventana_estadisticapool_node[2]*0.5-14,"                              ");
                    mvprintw(ventana_estadisticapool_node[1]+7,ventana_estadisticapool_node[0]+ventana_estadisticapool_node[2]*0.5-14,"Epoch ends in: %id:%ih:%im:%is",dia,hora,min,seg);
                    ventana->slider_horizontal(ventana_estadisticapool_node[0]+2,ventana_estadisticapool_node[1]+8,ventana_estadisticapool_node[2]-3,EPOCHLENGTH,consulta->slot_epoca());
                    //ventana->label(ventana_estadisticapool_node[0]+1,ventana_estadisticapool_node[1]+8,"Total Slot: ");
                    ventana->label_uint64(ventana_estadisticapool_node[0]+14,ventana_estadisticapool_node[1]+10,consulta->totalslot());
                    //----Datos KES
                    if(consulta->kes_certificado()!=0){
                        //Actual KES period:
                        ventana->label_uint32(ventana_estadisticapool_node[0]+21,ventana_estadisticapool_node[1]+11,consulta->kes_actual());
                        //Remaining days for expire:
                        tbuf=consulta->dias_restante_kes();
                        if(tbuf>20){
                            ventana->crear_linea_horizontal(ventana_estadisticapool_node[0]+28,ventana_estadisticapool_node[1]+14,espacios[dias_restante_kes],' ');
                            ventana->label_uint32(ventana_estadisticapool_node[0]+28,ventana_estadisticapool_node[1]+14,tbuf);
                            espacios[dias_restante_kes]=std::to_string(tbuf).length();
                        }
                        else if(tbuf>5 && tbuf<=20 ){
                            ventana->crear_linea_horizontal(ventana_estadisticapool_node[0]+28,ventana_estadisticapool_node[1]+14,espacios[dias_restante_kes],' ');
                            ventana->label(ventana_estadisticapool_node[0]+28,ventana_estadisticapool_node[1]+14,std::to_string(tbuf).c_str(),COLOR_PAIR(3) | A_BOLD);
                            espacios[dias_restante_kes]=std::to_string(tbuf).length();
                        }
                        else{
                            ventana->crear_linea_horizontal(ventana_estadisticapool_node[0]+28,ventana_estadisticapool_node[1]+14,espacios[dias_restante_kes],' ');
                            ventana->label(ventana_estadisticapool_node[0]+28,ventana_estadisticapool_node[1]+14,std::to_string(tbuf).c_str(), COLOR_PAIR(1) | A_BOLD | A_BLINK);
                            espacios[dias_restante_kes]=std::to_string(tbuf).length();
                        }
                        //Created in KES period:
                        ventana->label_uint32(ventana_estadisticapool_node[0]+25,ventana_estadisticapool_node[1]+15,consulta->kes_certificado());
                        //Expire in KES period:
                        ventana->label_uint32(ventana_estadisticapool_node[0]+24,ventana_estadisticapool_node[1]+16,consulta->kes_termino_certificado());

                    }else{
                        ventana->label((ventana_estadisticapool_node[0]+ventana_estadisticapool_node[2]*0.5-13),ventana_estadisticapool_node[1]+13,"-No information about KES-", A_UNDERLINE | A_BOLD);
                    }
                    //--------------

                    //------Datos de la ventana Node process"
                    //RAM (Used):
                    ventana->crear_linea_horizontal(ventana_memoria[0]+13,ventana_memoria[1]+1,espacios[memoria],' ');
                    ventana->label_uint32(ventana_memoria[0]+13,ventana_memoria[1]+1,consulta->memoria());
                    espacios[memoria]=std::to_string(consulta->memoria()).length();
                    ventana->label(ventana_memoria[0]+14+espacios[memoria],ventana_memoria[1]+1,"MB");
                    espacios[memoria]=(espacios[memoria])+3;
                    //CPU:
                    ventana->slider_horizontal(ventana_memoria[0]+5,ventana_memoria[1]+2,ventana_memoria[2]-11,100,consulta->ticks_porciento());
                    ventana->crear_linea_horizontal(ventana_memoria[0]+(ventana_memoria[2]-5),ventana_memoria[1]+2,espacios[cpu],' ');
                    ventana->label_uint16(ventana_memoria[0]+(ventana_memoria[2]-5),ventana_memoria[1]+2,consulta->ticks_porciento());
                    espacios[cpu]=std::to_string(consulta->ticks_porciento()).length();
                    ventana->label(ventana_memoria[0]+(ventana_memoria[2]-5)+espacios[cpu],ventana_memoria[1]+2,"\uFF05");
                    espacios[cpu]=(espacios[cpu])+1;
                    //Threads
                    ventana->crear_linea_horizontal(ventana_memoria[0]+10,ventana_memoria[1]+3,espacios[threads_nodo],' ');
                    ventana->label_uint32(ventana_memoria[0]+10,ventana_memoria[1]+3,consulta->hilos_nodo());
                    espacios[threads_nodo]=std::to_string(consulta->hilos_nodo()).length();
                    //---------------


                    //-----Datos de la ventana Transactions
                    txmin=consulta->transacciones_mempool();
                    if(txmin>txmax){
                        txmax=txmin;
                    }
                    //In mempool:
                    ventana->crear_linea_horizontal(ventana_trx[0]+13,ventana_trx[1]+1,espacios[transacciones_mempool],' ');
                    ventana->label_uint32(ventana_trx[0]+13,ventana_trx[1]+1,txmin);
                    espacios[transacciones_mempool]=std::to_string(txmin).length();
                    //Max:
                    ventana->label_uint32((ventana_trx[0]+ventana_trx[2]-6),ventana_trx[1]+1,txmax);
                    ventana->slider_horizontal(ventana_trx[0]+2,ventana_trx[1]+2,ventana_trx[2]-3,txmax,txmin);
                    //Total Processed:
                    ventana->label_uint64(ventana_trx[0]+18,ventana_trx[1]+4,consulta->transacciones_procesadas());
                    //----------------

                    //----Datos de la ventana Peers
                    //Connected:
                    ventana->crear_linea_horizontal(ventana_peer[0]+12,ventana_peer[1]+1,espacios[peers],' ');
                    ventana->label_uint16(ventana_peer[0]+12,ventana_peer[1]+1,consulta->peers());
                    espacios[peers]=std::to_string(consulta->peers()).length();
                    //----------------

                    //----Datos de la Ventana BlockChain
                    //Density:
                    ventana->label(ventana_blockchain[0]+9,ventana_blockchain[1]+1,consulta->densidad_str().c_str());
                    ventana->label(ventana_blockchain[0]+10+consulta->densidad_str().length(),ventana_blockchain[1]+1,"\uFF05");
                    //ventana->label(ventana_blockchain[0]+10+std::to_string(consulta->densidad()).length(),ventana_blockchain[1]+1,"\uFF05");
                    //Fork:
                    ventana->crear_linea_horizontal(ventana_blockchain[0]+7,ventana_blockchain[1]+2,espacios[forks],' ');
                    ventana->label_uint32(ventana_blockchain[0]+7,ventana_blockchain[1]+2,consulta->forks());
                    espacios[forks]=std::to_string(consulta->forks()).length();
                    //-----------------

                    //--- Datos de la Ventana Forge
                    if(consulta->kes_actual()!=0){
                        //Blocks lead:
                        ventana->crear_linea_horizontal(ventana_mined[0]+13,ventana_mined[1]+1,espacios[bloques_asignados],' ');
                        ventana->label(ventana_mined[0]+13,ventana_mined[1]+1,std::to_string(consulta->bloques_asignados()).c_str());
                        espacios[bloques_asignados]=std::to_string(consulta->bloques_asignados()).length();
                        //Blocks minted:
                        ventana->crear_linea_horizontal(ventana_mined[0]+15,ventana_mined[1]+2,espacios[bloques_creados],' ');
                        if(consulta->bloques_creados()==0){
                            ventana->label(ventana_mined[0]+15,ventana_mined[1]+2,std::to_string(consulta->bloques_creados()).c_str());
                        }
                        else{
                            ventana->label(ventana_mined[0]+15,ventana_mined[1]+2,std::to_string(consulta->bloques_creados()).c_str(),COLOR_PAIR(4));
                        }
                        espacios[bloques_creados]=std::to_string(consulta->bloques_creados()).length();
                        //Blocks missed:
                        ventana->crear_linea_horizontal(ventana_mined[0]+15,ventana_mined[1]+3,espacios[bloques_perdidos],' ');
                        if(consulta->bloques_perdidos()==0){
                            ventana->label(ventana_mined[0]+15,ventana_mined[1]+3,std::to_string(consulta->bloques_perdidos()).c_str());
                        }
                        else{
                            ventana->label(ventana_mined[0]+15,ventana_mined[1]+3,std::to_string(consulta->bloques_perdidos()).c_str(),COLOR_PAIR(1));
                        }
                        espacios[bloques_perdidos]=std::to_string(consulta->bloques_perdidos()).length();
                        //Slot missed
                        ventana->crear_linea_horizontal(ventana_mined[0]+14,ventana_mined[1]+4,espacios[slots_perdidos],' ');
                        if(consulta->slot_perdidos()==0){
                            ventana->label(ventana_mined[0]+14,ventana_mined[1]+4,std::to_string(consulta->slot_perdidos()).c_str());
                        }
                        else{
                            ventana->label(ventana_mined[0]+14,ventana_mined[1]+4,std::to_string(consulta->slot_perdidos()).c_str(),COLOR_PAIR(3));
                        }
                        espacios[slots_perdidos]=std::to_string(consulta->slot_perdidos()).length();

                    }
                    //-----------------

                    //Up time
                    tiempo_restantes_seg(consulta->uptimens(),&dia,&hora,&min,&seg);
                    mvprintw(LINES-1,(COLS/2)-14,"                              ");
                    attron(COLOR_PAIR(2) | A_BOLD);
                    mvprintw(LINES-1,(COLS/2)-14,"Node up time: %id:%ih:%im:%is",dia,hora,min,seg);
                    attroff(COLOR_PAIR(2) | A_BOLD);
                }

                if(acceso_github){
                    ventana->crear_linea_horizontal(ventana_tagversion[2]*0.3,ventana_tagversion[1]+2,espacios[esp_version],' ');
                    ventana->label(ventana_tagversion[2]*0.3,ventana_tagversion[1]+2,version.c_str());
                    espacios[esp_version]=version.length();
                    if(estado=="release"){
                        ventana->label(ventana_tagversion[2]*0.6,ventana_tagversion[1]+2,"           ");
                        ventana->label(ventana_tagversion[2]*0.6,ventana_tagversion[1]+2,"release",COLOR_PAIR(4) | A_BOLD);
                    }
                    else if(estado=="prerelease"){
                        ventana->label(ventana_tagversion[2]*0.6,ventana_tagversion[1]+2,"           ");
                        ventana->label(ventana_tagversion[2]*0.6,ventana_tagversion[1]+2,"prerelease",COLOR_PAIR(3) | A_BOLD);
                    }
                    else if(estado=="draft"){
                        ventana->label(ventana_tagversion[2]*0.6,ventana_tagversion[1]+2,"           ");
                        ventana->label(ventana_tagversion[2]*0.6,ventana_tagversion[1]+2,"draft",COLOR_PAIR(1) | A_BOLD);
                    }
                    else { //en el caso que no encuentre ninguno de esos estados, muestra un aviso de error
                        ventana->label(ventana_tagversion[2]*0.6,ventana_tagversion[1]+2,"           ");
                        ventana->label(ventana_tagversion[2]*0.6,ventana_tagversion[1]+2,"Error",COLOR_PAIR(1) | A_BOLD);
                    }
                }

                if(acceso_versionnodo){
                    if(consulta->github_habilitado()){
                        ventana->crear_linea_horizontal(ventana_tagversion[2]*0.3,ventana_tagversion[1]+3,espacios[esp_local_nodo_version],' ');
                        ventana->label(ventana_tagversion[2]*0.3,ventana_tagversion[1]+3,local_nodo_version.c_str());
                        espacios[esp_local_nodo_version]=local_nodo_version.length();
                        if(local_nodo_version.length()==0){
                            ventana->label(ventana_tagversion[2]*0.6,ventana_tagversion[1]+3,"         ");
                            ventana->label(ventana_tagversion[2]*0.6,ventana_tagversion[1]+3,"Error", COLOR_PAIR(6) | A_BOLD | A_BLINK);
                        }
                        else if(version==local_nodo_version || (version!=local_nodo_version && estado!="release")){ //la segunda condicion es para no considerar un prerelease o un draft
                            ventana->label(ventana_tagversion[2]*0.6,ventana_tagversion[1]+3,"         ");
                            ventana->label(ventana_tagversion[2]*0.6,ventana_tagversion[1]+3,"updated",COLOR_PAIR(4) | A_BOLD);
                            }
                        else if(estado == "error"){ //si la consulta a github da un error y no determina el tag
                            ventana->label(ventana_tagversion[2]*0.6,ventana_tagversion[1]+3,"         ");
                            ventana->label(ventana_tagversion[2]*0.6,ventana_tagversion[1]+3,"Local",COLOR_PAIR(4) | A_BOLD);
                        }
                        else{
                            ventana->label(ventana_tagversion[2]*0.6,ventana_tagversion[1]+3,"         ");
                            ventana->label(ventana_tagversion[2]*0.6,ventana_tagversion[1]+3,"outdated", COLOR_PAIR(6) | A_BOLD | A_BLINK);
                        }
                    }else{
                        ventana->crear_linea_horizontal(ventana_tagversion[2]*0.3,ventana_tagversion[1]+2,espacios[esp_local_nodo_version],' ');
                        ventana->label(ventana_tagversion[2]*0.3,ventana_tagversion[1]+2,local_nodo_version.c_str());
                        if(local_nodo_version.length()==0){
                            ventana->label(ventana_tagversion[2]*0.6,ventana_tagversion[1]+2,"         ");
                            ventana->label(ventana_tagversion[2]*0.6,ventana_tagversion[1]+2,"Error", COLOR_PAIR(6) | A_BOLD | A_BLINK);
                        }else{
                            ventana->label(ventana_tagversion[2]*0.6,ventana_tagversion[1]+2,"Local",COLOR_PAIR(4) | A_BOLD);
                        }
                    }
                }else{ventana->label(ventana_tagversion[2]*0.3,ventana_tagversion[1]+3,"Disable");}

                if(acceso_adapools){
                    //ventana->crear_subventana(ventana_stake[0],ventana_stake[1],ventana_stake[2],ventana_stake[3],"Stake", "Consulted to adapools");
                    //ventana->label(ventana_stake[0]+1,ventana_stake[1]+1,"Pledge: ");
                    ventana->crear_linea_horizontal(ventana_stake[0]+9,ventana_stake[1]+1,2+espacios[pledge],' ');
                    ventana->label_uint64(ventana_stake[0]+9,ventana_stake[1]+1,consulta->pledge());
                    espacios[pledge]=std::to_string(consulta->pledge()).length();
                    ventana->label(ventana_stake[0]+10+espacios[pledge],ventana_stake[1]+1,"\u20B3");
                    //ventana->label(ventana_stake[0]+1,ventana_stake[1]+2,"Total Stake: ");
                    ventana->crear_linea_horizontal(ventana_stake[0]+14,ventana_stake[1]+2,2+espacios[stake_total],' ');
                    ventana->label_uint64(ventana_stake[0]+14,ventana_stake[1]+2,consulta->stake_total());
                    espacios[stake_total]=std::to_string(consulta->stake_total()).length();
                    ventana->label(ventana_stake[0]+15+espacios[stake_total],ventana_stake[1]+2,"\u20B3");
                    //ventana->label(ventana_stake[0]+1,ventana_stake[1]+3,"Live Stake: ");
                    ventana->crear_linea_horizontal(ventana_stake[0]+13,ventana_stake[1]+3,2+espacios[stake_activo],' ');
                    ventana->label_uint64(ventana_stake[0]+13,ventana_stake[1]+3,consulta->stake_activo());
                    espacios[stake_activo]=std::to_string(consulta->stake_activo()).length();
                    ventana->label(ventana_stake[0]+14+espacios[stake_activo],ventana_stake[1]+3,"\u20B3");
                    //ventana->label(ventana_stake[0]+1,ventana_stake[1]+4,"Delegator: ");
                    ventana->crear_linea_horizontal(ventana_stake[0]+12,ventana_stake[1]+4,2+espacios[delegadores],' ');
                    ventana->label_uint32(ventana_stake[0]+12,ventana_stake[1]+4,consulta->delegadores());
                    espacios[delegadores]=std::to_string(consulta->delegadores()).length();
                    ventana->label(ventana_stake[0]+13+espacios[delegadores],ventana_stake[1]+4,"\uC6C3");
                    //ventana->label((ventana_stake[0]+ventana_stake[2]*0.5-6),ventana_stake[1]+6,"Saturation:");
                    ventana->crear_linea_horizontal((ventana_stake[0]+ventana_stake[2]*0.5+6),ventana_stake[1]+6,2+espacios[saturacion],' ');
                    ventana->label_uint16((ventana_stake[0]+ventana_stake[2]*0.5+6),ventana_stake[1]+6,consulta->saturacion());
                    espacios[saturacion]=std::to_string(consulta->saturacion()).length();
                    ventana->label((ventana_stake[0]+ventana_stake[2]*0.5+7+espacios[saturacion]),ventana_stake[1]+6,"\uFF05");
                    ventana->slider_horizontal(ventana_stake[0]+2,ventana_stake[1]+7,ventana_stake[2]-3,100,(consulta->saturacion()));

                }
                ventana->refrescar();

            }
            else if(switchpeer){
                //---Consulta los datos locales TCP
                categorizar_ips(&puerto_escucha[0],&index_puerto_escucha,&ip_p2p[0],&index_ip_p2p,&ip_huerfana_salida[0],&index_ip_huerfana_salida,&ip_huerfana_entrada[0],&index_ip_huerfana_entrada);

                //---Imprime los datos en sus respectivas ventanas
                uint16_t numero_maximo_filas=LINES-5;
                //seccion IPs Interconectadas
                ventana->borrar_subventana(ventana_interconectados[0]+1,ventana_interconectados[1]+1,23,buff_index_ip_p2p-1);
                if(index_ip_p2p>numero_maximo_filas){index_ip_p2p=numero_maximo_filas;}
                for(uint16_t a=0;a<index_ip_p2p;a++){
                    mvprintw(ventana_interconectados[1]+(a+1),ventana_interconectados[0]+1,"%s : %s",ip_p2p[a][0].c_str(),ip_p2p[a][1].c_str());
                }
                buff_index_ip_p2p=index_ip_p2p;
                //seccion IPs de Entrada
                ventana->borrar_subventana(ventana_huerfano_entrada[0]+1,ventana_huerfano_entrada[1]+1,23,buff_index_ip_huerfana_entrada-1);
                if(index_ip_huerfana_entrada>numero_maximo_filas){index_ip_huerfana_entrada=numero_maximo_filas;}
                for(uint16_t a=0;a<index_ip_huerfana_entrada;a++){
                    mvprintw(ventana_huerfano_entrada[1]+(a+1),ventana_huerfano_entrada[0]+1,"%s : %s",ip_huerfana_entrada[a][0].c_str(),ip_huerfana_entrada[a][1].c_str());
                }
                buff_index_ip_huerfana_entrada=index_ip_huerfana_entrada;
                //seccion IPs de Salida
                ventana->borrar_subventana(ventana_huerfano_salida[0]+1,ventana_huerfano_salida[1]+1,23,buff_index_ip_huerfana_salida-1);
                if(index_ip_huerfana_salida>numero_maximo_filas){index_ip_huerfana_salida=numero_maximo_filas;}
                for(uint16_t a=0;a<index_ip_huerfana_salida;a++){
                    mvprintw(ventana_huerfano_salida[1]+(a+1),ventana_huerfano_salida[0]+1,"%s : %s",ip_huerfana_salida[a][0].c_str(),ip_huerfana_salida[a][1].c_str());
                }
                buff_index_ip_huerfana_salida=index_ip_huerfana_salida;
                //seccion puertos escuchas
                ventana->borrar_subventana(ventana_puertos_abiertos[0]+1,ventana_puertos_abiertos[1]+1,ventana_puertos_abiertos[2]-2,buff_index_puerto_escucha-1);
                if(index_puerto_escucha>numero_maximo_filas){index_puerto_escucha=numero_maximo_filas;}
                for(uint16_t a=0;a<index_puerto_escucha;a++){
                    mvprintw(ventana_puertos_abiertos[1]+(a+1),ventana_puertos_abiertos[0]+1,"%s",puerto_escucha[a].c_str());
                }
                buff_index_puerto_escucha=index_puerto_escucha;
                //---Refresca la pantalla
                ventana->refrescar();
            }
        };break;
        }
        conteo_nodo++;
        conteo_github++;
        conteo_adapools++;
        if(conteo_nodo>2){
            consulta->actualizar_datos(&puerto_n);
            conteo_nodo=0;
        }
        if(conteo_github>7200){
            if(consulta->github_habilitado()){
                consulta->github(&version,&estado);
                //if(consulta->github(&version,&estado)){acceso_github=true;}else{acceso_github=false;}
            }
            conteo_github=0;
        }
        if(conteo_adapools>1200){
            if(consulta->adapools_habilitado()){
                consulta->actualizar_adapools();
                //if(consulta->actualizar_adapools()){acceso_adapools=true;}else{acceso_adapools=false;}
            }
            conteo_adapools=0;
        }
        usleep(500000); //medio seg el refresco
    }

}

void categorizar_ips(std::string puertos_abiertos[], uint16_t *index_puertosabiertos,std::string ip_p2p[][2], uint16_t *index_ip_p2p,std::string ip_huerfana_salida[][2], uint16_t *index_ip_huerfana_salida,std::string ip_huerfana_entrada[][2], uint16_t *index_ip_huerfana_entrada){
    lectura_red lectura;
    *index_ip_p2p=0;
    *index_ip_huerfana_salida=0;
    *index_ip_huerfana_entrada=0;
    *index_puertosabiertos=0;
    std::string tcp_dec[540][5];
    std::string pe1[540][2]; //string[index][ip,puerto_local]
    std::string pe2[540][2]; //string[index][ip,puerto_local]
    std::string ps[540][3]; //string[index][ip,puerto_local,puerto_remoto]
    uint16_t index_pe1=0;
    uint16_t index_pe2=0;
    uint16_t index_ps=0;
    uint16_t todos=0;
    lectura.puerto_tcp(&todos,&tcp_dec[0]);
    for(uint16_t a=0; a<(lectura.cantidad_ips());a++){ //se determinan los puertos abiertos y se guardan en puertos_abiertos[]
        if(tcp_dec[a][4]=="10"){
            puertos_abiertos[*index_puertosabiertos]=tcp_dec[a][1];
            *index_puertosabiertos+=1;

        }
    }
    for(uint16_t a=0;a<*index_puertosabiertos;a++){ //se buscan las ips de entrada ligadas a los puertos abiertos y se almacenan en pe1[][] y pe2[][]
        for(uint16_t b=0; b<(lectura.cantidad_ips());b++){
            if(tcp_dec[b][4]=="1"){
                if(tcp_dec[b][1]==puertos_abiertos[a]){ //ese puerto abierto es igual al puerto tcp_dec[][]
                    pe1[index_pe1][0]=tcp_dec[b][2]; //se guarda esa ip en pe1
                    pe1[index_pe1][1]=tcp_dec[b][1]; //se guarda ese puerto en pe1
                    pe2[index_pe1][0]=tcp_dec[b][2]; //se guarda esa ip en pe2
                    pe2[index_pe1][1]=tcp_dec[b][1]; //se guarda ese puerto en pe2
                    index_pe1++;
                }
            }
        }
    }
    index_pe2=index_pe1;
    for(uint16_t a=0; a<(lectura.cantidad_ips());a++){ //se determinan las ip ESTABLISHED y se guardan en ps[][]
        if(tcp_dec[a][4]=="1"){
            ps[index_ps][0]=tcp_dec[a][2]; //se guarda esa ip en ps
            ps[index_ps][1]=tcp_dec[a][1]; //se guarda ese puerto local en ps
            ps[index_ps][2]=tcp_dec[a][3]; //se guarda ese puerto remoto en ps
            index_ps++;
        }
    }
    for(uint16_t a=0;a<*index_puertosabiertos;a++){ //a ps[][] se le quitan los puertos_abiertos[] con un ""
        for(uint16_t b=0;b<index_ps;b++){
            if(puertos_abiertos[a]==ps[b][1]){ //se comparan sus puertos
                ps[b][0]="";
                ps[b][1]="";
                ps[b][2]="";
            }
        }
    }
    for(uint16_t a=0;a<index_pe2;a++){ //a pe2[][] se dejan en "" las ips que coinciden con de ps[][]
        for(uint16_t b=0;b<index_ps;b++){
            if(pe2[a][0]==ps[b][0]){ // se comparan sus ips
                pe2[a][0]="";
                pe2[a][1]="";
            }
        }
    }
    for(uint16_t a=0;a<index_pe2;a++){ //se pasa pe2[][] a ip_huerfana_entrada[][]
        if(pe2[a][0]!=""){
            ip_huerfana_entrada[*index_ip_huerfana_entrada][0]=pe2[a][0];
            ip_huerfana_entrada[*index_ip_huerfana_entrada][1]=pe2[a][1];
            *index_ip_huerfana_entrada+=1;
        }
    }

    for(uint16_t a=0;a<index_pe1;a++){ //a ps[][] se dejan en "" las ips que coinciden con de pe1[][]
        for(uint16_t b=0;b<index_ps;b++){
            if(pe1[a][0]==ps[b][0]){ // se comparan sus ips
                ps[b][0]="";
                ps[b][1]="";
                ps[b][2]="";
            }
        }
    }
    for(uint16_t a=0;a<index_ps;a++){ //se pasa ps[][] a ip_huerfana_salida[][]
        if(ps[a][0]!=""){
            ip_huerfana_salida[*index_ip_huerfana_salida][0]=ps[a][0];
            ip_huerfana_salida[*index_ip_huerfana_salida][1]=ps[a][2];
            *index_ip_huerfana_salida+=1;
        }
    }

    for(uint16_t a=0;a<index_pe1;a++){ //a pe1[][] se dejan en "" las ips que coinciden con ip_huerfana_entrada[][]
        for(uint16_t b=0;b<*index_ip_huerfana_entrada;b++){
            if(pe1[a][0]==ip_huerfana_entrada[b][0]){ // se comparan sus ips
                pe1[a][0]="";
                pe1[a][1]="";
            }
        }
    }

    std::string buf_unaip;
    bool bool_unaip=true;
    for(uint16_t a=0;a<index_pe1;a++){ //se borran las ips repetidas en pe1[][]
        if(pe1[a][0]!=""){
            buf_unaip=pe1[a][0];
            bool_unaip=true;
            for(uint16_t b=0;b<index_pe1;b++){
                if(buf_unaip==pe1[b][0] && bool_unaip){
                    bool_unaip=false;
                }
                else if(buf_unaip==pe1[b][0]){
                    pe1[b][0]="";
                }
            }
        }
    }

    for(uint16_t a=0;a<index_pe1;a++){ //se pasa pe1[][] a ip_p2p[][]
        if(pe1[a][0]!=""){
            ip_p2p[*index_ip_p2p][0]=pe1[a][0];
            ip_p2p[*index_ip_p2p][1]=pe1[a][1];
            *index_ip_p2p+=1;
        }
    }

}

void tiempo_restantes_slot(const uint32_t slot_actual, uint16_t *dia,uint16_t *hora,uint16_t *min,uint16_t *seg){ //calcula el tiempo restante para terminar la epoca
    uint32_t slot_restantes=EPOCHLENGTH-slot_actual;
    *dia=slot_restantes/86400;  //dia "86400=1dia"
    *hora=(slot_restantes%86400)/3600;  //horas "3600=1hora"
    *min=(slot_restantes%3600)/60; //minutos "60=1min"
    *seg=(slot_restantes%3600)%60; //seg
}

void tiempo_restantes_seg(const uint64_t segundos, uint16_t *dia,uint16_t *hora,uint16_t *min,uint16_t *seg){
    *dia=segundos/86400;
    *hora=(segundos%86400)/3600;
    *min=(segundos%3600)/60;
    *seg=(segundos%3600)%60;
}

void salidaforzosa_abort(int a){
    delete consulta;
    delete ventana;
    std::cout<<"The program has closed unexpectedly.\nThis can occur due to misconfiguration in \"wairamonitor.json\" file."<<std::endl;
}

void salidaforzosa_term(int a){
    delete consulta;
    delete ventana;
    std::cout<<"The program was closed by an end sign"<<std::endl;
}
