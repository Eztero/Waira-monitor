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
#define VERSION_WAIRA "wairamonitor 0.2"

void salidaforzosa(int a);
void senalsalida(int a);
void tiempo_restantes_slot(const uint32_t slot_actual, uint16_t *dia,uint16_t *hora,uint16_t *min,uint16_t *seg);
void tiempo_restantes_ns(const uint64_t nanosegundos, uint16_t *dia,uint16_t *hora,uint16_t *min,uint16_t *seg);
void cargar_ui();

std::atomic<bool> salir (false);
std::atomic<int> ktecla (0);

ngui *ventana=new ngui(0);
consultadatos *consulta=new consultadatos();

int main(){ 
    std::signal(SIGABRT,salidaforzosa);
    ventana->paleta_color(1,COLOR_RED,COLOR_BLACK);
    ventana->paleta_color(2,COLOR_CYAN,COLOR_BLACK);
    ventana->paleta_color(3,COLOR_YELLOW,COLOR_BLACK);
    ventana->paleta_color(4,COLOR_GREEN,COLOR_BLACK);
    ventana->paleta_color(6,COLOR_WHITE,COLOR_RED);
    
    
    ventana->refrescar();
    
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

    ventana->borrar_subventana(ventanade_carga[0],ventanade_carga[1],ventanade_carga[2],ventanade_carga[3]);
    ventana->refrescar();
    
    
    std::thread hilo_ui (cargar_ui); //se crea un hilo para la funcion void cargar_ui();
    ktecla=0;
    do{ 
        ktecla = getch();
    }while(ktecla != 113);//pasa el valor a la variable global ktecla y lo compara
    salir=true;	
    if(hilo_ui.joinable()){
        hilo_ui.join();
    }
    delete consulta;
    delete ventana;
    
    return 0;
}

void cargar_ui(){
    enum strlength{
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
        pledge,
        stake_total, 
        stake_activo,
        delegadores,
        saturacion,
        esp_version,
        esp_nversion,
    };
    uint16_t espacios[17];
    for(int a=0;a<17;a++){ //establece todos los espacios en 1
        espacios[a]=1;
    }
    bool acceso_prometheus, acceso_github, acceso_adapools, acceso_versionnodo, switchmain, switchpeer, switchabout;
    std::string version, estado, nversion;
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
    if(consulta->version_nodo(&nversion)){acceso_versionnodo=true;}else{acceso_versionnodo=false;}
    ventana->slider_horizontal(15,2,10,5,5); //ventana carga slider
    ventana->refrescar();
    
    //Se borra el "Loading"
    mvhline(2,2,' ',COLS/2);
    
    //Se calculan las dimenciones de las ventanas
    ventana->crear_ventantaprincipal(consulta->poolnamew(),A_BOLD,VERSION_WAIRA,A_BOLD);
    ventana->label(1,LINES-2,"Press \"q\" for Quit, \"ESC\" for Main ,\"p\" for Peer,\"a\" for About");
    pos_ancho=(COLS*0.3);
    if(pos_ancho<31){ ////calculo el ancho minimo para ventana_estadisticapool_node
        pos_ancho=31;
    }  
    double ventana_estadisticapool_node[4]={1,1,pos_ancho,16};  //Dimensiones para la ventana de estadistica
    
    double ventana_tagversion[4]={1,18,pos_ancho,5};  //Dimensiones para la ventana de tags
    
    pos_x=(COLS-(COLS*0.3)-2);
    pos_ancho2=(COLS*0.3);
    
    if(pos_ancho2<28){ ////calculo el ancho minimo para ventana_stake
        pos_ancho2=28;
    }  
    double ventana_stake[4]={pos_x,1,pos_ancho2,9}; //Dimensiones para la ventana de  stake
    
    double ventana_mined[4]={pos_x,11,pos_ancho2,4}; //Dimensiones para la ventana de forge
    
    pos_ancho3=(pos_x-(5+pos_ancho));
    if(pos_ancho3<27){ ////calculo el ancho minimo para ventana_memoria
        pos_ancho3=27;
    }  
    double ventana_memoria[4]={(3+pos_ancho),1,pos_ancho3,3};  //Dimensiones para la ventana de memoria
    
    double ventana_trx[4]={(3+pos_ancho),5,pos_ancho3,5};  //Dimensiones para la ventana de transacciones
    
    double ventana_peer[4]={(3+pos_ancho),11,(pos_x-(5+pos_ancho)),2};  //Dimensiones para la ventana de peer
    
    double ventana_blockchain[4]={(3+pos_ancho),14,pos_ancho3,3};  //Dimensiones para la ventana de blockchain
    
    if(consulta->kes_actual()==0){
        ventana->crear_subventana(ventana_estadisticapool_node[0],ventana_estadisticapool_node[1],ventana_estadisticapool_node[2],ventana_estadisticapool_node[3],"Relay");
    }else{
        ventana->crear_subventana(ventana_estadisticapool_node[0],ventana_estadisticapool_node[1],ventana_estadisticapool_node[2],ventana_estadisticapool_node[3],"Producer");
    }
    
    //Se crean las ventanas
    ventana->crear_subventana(ventana_tagversion[0],ventana_tagversion[1],ventana_tagversion[2],ventana_tagversion[3],"Tag version","Consulted to github");
    ventana->crear_subventana(ventana_memoria[0],ventana_memoria[1],ventana_memoria[2],ventana_memoria[3],"Memory");
    ventana->crear_subventana(ventana_trx[0],ventana_trx[1],ventana_trx[2],ventana_trx[3],"Transactions");
    ventana->crear_subventana(ventana_stake[0],ventana_stake[1],ventana_stake[2],ventana_stake[3],"Stake", "Consulted to adapools");
    ventana->crear_subventana(ventana_mined[0],ventana_mined[1],ventana_mined[2],ventana_mined[3],"Forge");
    ventana->crear_subventana(ventana_peer[0],ventana_peer[1],ventana_peer[2],ventana_peer[3],"Peers");
    ventana->crear_subventana(ventana_blockchain[0],ventana_blockchain[1],ventana_blockchain[2],ventana_blockchain[3],"Blockchain");
    
    //Se completa las ventanas con sus nombres
    if(acceso_prometheus){
        ventana->label(ventana_estadisticapool_node[0]+1,ventana_estadisticapool_node[1]+1,"Epoch: ");
        ventana->label(ventana_estadisticapool_node[0]+1,ventana_estadisticapool_node[1]+2,"Slot: ");
        ventana->label(ventana_estadisticapool_node[0]+1,ventana_estadisticapool_node[1]+3,"Block number: ");
        ventana->label(ventana_estadisticapool_node[0]+1,ventana_estadisticapool_node[1]+8,"Total Slot: ");
        if(consulta->kes_actual()!=0){
            ventana->label(ventana_estadisticapool_node[0]+1,ventana_estadisticapool_node[1]+9,"Actual KES period: ");
            ventana->label((ventana_estadisticapool_node[0]+ventana_estadisticapool_node[2]*0.5-13),ventana_estadisticapool_node[1]+11,"Operational Certificate", A_UNDERLINE | A_BOLD);
            ventana->label(ventana_estadisticapool_node[0]+1,ventana_estadisticapool_node[1]+13,"Remaining days for expire: ");
            
            ventana->label(ventana_estadisticapool_node[0]+1,ventana_estadisticapool_node[1]+14,"Created in KES period: ");
            ventana->label(ventana_estadisticapool_node[0]+1,ventana_estadisticapool_node[1]+15,"Expire in KES period: ");
        }else{
            ventana->label((ventana_estadisticapool_node[0]+ventana_estadisticapool_node[2]*0.5-13),ventana_estadisticapool_node[1]+11,"-No information about KES-", A_UNDERLINE | A_BOLD);
        }
        
        ventana->label(ventana_memoria[0]+1,ventana_memoria[1]+1,"Memory Used: ");
        
        ventana->label(ventana_trx[0]+1,ventana_trx[1]+1,"In mempool: ");
        ventana->label((ventana_trx[0]+ventana_trx[2]-11),ventana_trx[1]+1,"Max: ");
        ventana->label(ventana_trx[0]+1,ventana_trx[1]+4,"Total Processed: ");
        
        ventana->label(ventana_peer[0]+1,ventana_peer[1]+1,"Connected: ");
        
        ventana->label(ventana_blockchain[0]+1,ventana_blockchain[1]+1,"Density: ");
        ventana->label(ventana_blockchain[0]+1,ventana_blockchain[1]+2,"Fork: ");
        
        ventana->label(ventana_mined[0]+1,ventana_mined[1]+1,"Slots lead: ");
        ventana->label(ventana_mined[0]+1,ventana_mined[1]+2,"Blocks minted: ");
        ventana->label(ventana_mined[0]+1,ventana_mined[1]+3,"Slots missed: ");
        
        
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
        ventana->label(ventana_tagversion[2]*0.3,ventana_tagversion[1]+1,"version");
        ventana->label(ventana_tagversion[2]*0.6,ventana_tagversion[1]+1,"status");
        ventana->label(ventana_tagversion[0]+1,ventana_tagversion[1]+2,"Github");
        ventana->label(ventana_tagversion[0]+1,ventana_tagversion[1]+3,"Local");
    }else{
        ventana->label(ventana_tagversion[2]*0.3,ventana_tagversion[1]+1,"version");
        ventana->label(ventana_tagversion[2]*0.6,ventana_tagversion[1]+1,"status");	
        ventana->label(ventana_tagversion[0]+1,ventana_tagversion[1]+3,"Local");
        if(!consulta->github_habilitado()){
            ventana->label(ventana_tagversion[0]+1,ventana_tagversion[1]+2,"Queries to Github are disable");
        }
        else{
            ventana->label(ventana_tagversion[2]*0.3,ventana_tagversion[1]+2,"Can't be accessed to Github");
        }	
    }
    if(acceso_adapools){
        ventana->label(ventana_stake[0]+1,ventana_stake[1]+1,"Pledge: ");
        ventana->label(ventana_stake[0]+1,ventana_stake[1]+2,"Total Stake: ");
        ventana->label(ventana_stake[0]+1,ventana_stake[1]+3,"Live Stake: ");
        ventana->label(ventana_stake[0]+1,ventana_stake[1]+4,"Delegator: ");
        ventana->label((ventana_stake[0]+ventana_stake[2]*0.5-6),ventana_stake[1]+6,"Saturation:");   
    }else{
        if(!consulta->adapools_habilitado()){
            ventana->label(ventana_stake[0]+1,ventana_stake[1]+4,"Queries to Adapools are disable");
        }
        else{
            ventana->label(ventana_stake[0]+1,ventana_stake[1]+4,"Can't be accessed to Adapools");
        }
    }
    
    ventana->refrescar();
    
    while(!salir){
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
            ventana_estadisticapool_node[3]=16;
            
            ventana_tagversion[0]=1;//Dimensiones para la ventana de tags
            ventana_tagversion[1]=18;
            ventana_tagversion[2]=pos_ancho;
            ventana_tagversion[3]=5;
            
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
            ventana_mined[3]=4;
            
            pos_ancho3=(pos_x-(5+pos_ancho));
            if(pos_ancho3<27){ ////calculo el ancho minimo para ventana_memoria
                pos_ancho3=27;
            }  
            ventana_memoria[0]=(3+pos_ancho);//Dimensiones para la ventana de memoria
            ventana_memoria[1]=1;
            ventana_memoria[2]=pos_ancho3;
            ventana_memoria[3]=3;
            
            ventana_trx[0]=(3+pos_ancho);//Dimensiones para la ventana de transacciones
            ventana_trx[1]=5;
            ventana_trx[2]=pos_ancho3;
            ventana_trx[3]=5;
            
            
            ventana_peer[0]=(3+pos_ancho);//Dimensiones para la ventana de peer
            ventana_peer[1]=11;
            ventana_peer[2]=pos_ancho3;
            ventana_peer[3]=2;
            
            ventana_blockchain[0]=(3+pos_ancho);//Dimensiones para la ventana de blockchain
            ventana_blockchain[1]=14;
            ventana_blockchain[2]=pos_ancho3;
            ventana_blockchain[3]=3;
            
            ventana->crear_ventantaprincipal(consulta->poolnamew(),A_BOLD,VERSION_WAIRA,A_BOLD);
            ventana->label(1,LINES-2,"Press \"q\" for Quit, \"ESC\" for Main ,\"p\" for Peer,\"a\" for About");
            if(consulta->kes_actual()==0){
                ventana->crear_subventana(ventana_estadisticapool_node[0],ventana_estadisticapool_node[1],ventana_estadisticapool_node[2],ventana_estadisticapool_node[3],"Relay");
            }else{
                ventana->crear_subventana(ventana_estadisticapool_node[0],ventana_estadisticapool_node[1],ventana_estadisticapool_node[2],ventana_estadisticapool_node[3],"Producer");
            }
            ventana->crear_subventana(ventana_tagversion[0],ventana_tagversion[1],ventana_tagversion[2],ventana_tagversion[3],"Tag version","Consulted to github");
            ventana->crear_subventana(ventana_memoria[0],ventana_memoria[1],ventana_memoria[2],ventana_memoria[3],"Memory");
            ventana->crear_subventana(ventana_trx[0],ventana_trx[1],ventana_trx[2],ventana_trx[3],"Transactions");
            ventana->crear_subventana(ventana_stake[0],ventana_stake[1],ventana_stake[2],ventana_stake[3],"Stake", "Consulted to adapools");
            ventana->crear_subventana(ventana_mined[0],ventana_mined[1],ventana_mined[2],ventana_mined[3],"Forge");
            ventana->crear_subventana(ventana_peer[0],ventana_peer[1],ventana_peer[2],ventana_peer[3],"Peers");
            ventana->crear_subventana(ventana_blockchain[0],ventana_blockchain[1],ventana_blockchain[2],ventana_blockchain[3],"Blockchain");
            
            //Se completa las ventanas con sus nombres
            if(acceso_prometheus){
                ventana->label(ventana_estadisticapool_node[0]+1,ventana_estadisticapool_node[1]+1,"Epoch: ");
                ventana->label(ventana_estadisticapool_node[0]+1,ventana_estadisticapool_node[1]+2,"Slot: ");
                ventana->label(ventana_estadisticapool_node[0]+1,ventana_estadisticapool_node[1]+3,"Block number: ");
                ventana->label(ventana_estadisticapool_node[0]+1,ventana_estadisticapool_node[1]+8,"Total Slot: ");
                if(consulta->kes_actual()!=0){
                    ventana->label(ventana_estadisticapool_node[0]+1,ventana_estadisticapool_node[1]+9,"Actual KES period: ");
                    ventana->label((ventana_estadisticapool_node[0]+ventana_estadisticapool_node[2]*0.5-13),ventana_estadisticapool_node[1]+11,"Operational Certificate", A_UNDERLINE | A_BOLD);
                    ventana->label(ventana_estadisticapool_node[0]+1,ventana_estadisticapool_node[1]+13,"Remaining days for expire: ");
                    
                    ventana->label(ventana_estadisticapool_node[0]+1,ventana_estadisticapool_node[1]+14,"Created in KES period: ");
                    ventana->label(ventana_estadisticapool_node[0]+1,ventana_estadisticapool_node[1]+15,"Expire in KES period: ");
                }else{
                    ventana->label((ventana_estadisticapool_node[0]+ventana_estadisticapool_node[2]*0.5-13),ventana_estadisticapool_node[1]+11,"-No information about KES-", A_UNDERLINE | A_BOLD);
                }
                
                ventana->label(ventana_memoria[0]+1,ventana_memoria[1]+1,"Memory Used: ");
                
                ventana->label(ventana_trx[0]+1,ventana_trx[1]+1,"In mempool: ");
                ventana->label((ventana_trx[0]+ventana_trx[2]-11),ventana_trx[1]+1,"Max: ");
                ventana->label(ventana_trx[0]+1,ventana_trx[1]+4,"Total Processed: ");
                
                ventana->label(ventana_peer[0]+1,ventana_peer[1]+1,"Connected: ");
                
                ventana->label(ventana_blockchain[0]+1,ventana_blockchain[1]+1,"Density: ");
                ventana->label(ventana_blockchain[0]+1,ventana_blockchain[1]+2,"Fork: ");
                
                ventana->label(ventana_mined[0]+1,ventana_mined[1]+1,"Slots lead: ");
                ventana->label(ventana_mined[0]+1,ventana_mined[1]+2,"Blocks minted: ");
                ventana->label(ventana_mined[0]+1,ventana_mined[1]+3,"Slots missed: ");
                
                
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
                ventana->label(ventana_tagversion[2]*0.3,ventana_tagversion[1]+1,"version");
                ventana->label(ventana_tagversion[2]*0.6,ventana_tagversion[1]+1,"status");
                ventana->label(ventana_tagversion[0]+1,ventana_tagversion[1]+2,"Github");
                ventana->label(ventana_tagversion[0]+1,ventana_tagversion[1]+3,"Local");
            }else{
                ventana->label(ventana_tagversion[2]*0.3,ventana_tagversion[1]+1,"version");
                ventana->label(ventana_tagversion[2]*0.6,ventana_tagversion[1]+1,"status");	
                ventana->label(ventana_tagversion[0]+1,ventana_tagversion[1]+3,"Local");
                if(!consulta->github_habilitado()){
                    ventana->label(ventana_tagversion[0]+1,ventana_tagversion[1]+2,"Queries to Github are disable");
                }
                else{
                    ventana->label(ventana_tagversion[2]*0.3,ventana_tagversion[1]+2,"Can't be accessed to Github");
                }	
            }
            if(acceso_adapools){
                ventana->label(ventana_stake[0]+1,ventana_stake[1]+1,"Pledge: ");
                ventana->label(ventana_stake[0]+1,ventana_stake[1]+2,"Total Stake: ");
                ventana->label(ventana_stake[0]+1,ventana_stake[1]+3,"Live Stake: ");
                ventana->label(ventana_stake[0]+1,ventana_stake[1]+4,"Delegator: ");
                ventana->label((ventana_stake[0]+ventana_stake[2]*0.5-6),ventana_stake[1]+6,"Saturation:");   
            }else{
                if(!consulta->adapools_habilitado()){
                    ventana->label(ventana_stake[0]+1,ventana_stake[1]+4,"Queries to Adapools are disable");
                }
                else{
                    ventana->label(ventana_stake[0]+1,ventana_stake[1]+4,"Can't be accessed to Adapools");
                }
            }
            
            ktecla=0;
            switchmain=true;
            switchpeer=false;
            switchabout=false;
            ventana->refrescar();
        };break;
        case 27:{  //tecla esc
            if(switchmain==false &&(switchpeer=true || switchabout==true)){
                ventana->borrar_subventana(0,0,COLS,LINES);
                ventana->refrescar();
                ventana->crear_ventantaprincipal(consulta->poolnamew(),A_BOLD,VERSION_WAIRA,A_BOLD);
                ventana->label(1,LINES-2,"Press \"q\" for Quit, \"ESC\" for Main ,\"p\" for Peer,\"a\" for About");
                if(consulta->kes_actual()==0){
                    ventana->crear_subventana(ventana_estadisticapool_node[0],ventana_estadisticapool_node[1],ventana_estadisticapool_node[2],ventana_estadisticapool_node[3],"Relay");
                }else{
                    ventana->crear_subventana(ventana_estadisticapool_node[0],ventana_estadisticapool_node[1],ventana_estadisticapool_node[2],ventana_estadisticapool_node[3],"Producer");
                }
                ventana->crear_subventana(ventana_tagversion[0],ventana_tagversion[1],ventana_tagversion[2],ventana_tagversion[3],"Tag version","Consulted to github");
                ventana->crear_subventana(ventana_memoria[0],ventana_memoria[1],ventana_memoria[2],ventana_memoria[3],"Memory");
                ventana->crear_subventana(ventana_trx[0],ventana_trx[1],ventana_trx[2],ventana_trx[3],"Transactions");
                ventana->crear_subventana(ventana_stake[0],ventana_stake[1],ventana_stake[2],ventana_stake[3],"Stake", "Consulted to adapools");
                ventana->crear_subventana(ventana_mined[0],ventana_mined[1],ventana_mined[2],ventana_mined[3],"Forge");
                ventana->crear_subventana(ventana_peer[0],ventana_peer[1],ventana_peer[2],ventana_peer[3],"Peers");
                ventana->crear_subventana(ventana_blockchain[0],ventana_blockchain[1],ventana_blockchain[2],ventana_blockchain[3],"Blockchain");
                
                //Se completa las ventanas con sus nombres
                if(acceso_prometheus){
                    ventana->label(ventana_estadisticapool_node[0]+1,ventana_estadisticapool_node[1]+1,"Epoch: ");
                    ventana->label(ventana_estadisticapool_node[0]+1,ventana_estadisticapool_node[1]+2,"Slot: ");
                    ventana->label(ventana_estadisticapool_node[0]+1,ventana_estadisticapool_node[1]+3,"Block number: ");
                    ventana->label(ventana_estadisticapool_node[0]+1,ventana_estadisticapool_node[1]+8,"Total Slot: ");
                    if(consulta->kes_actual()!=0){
                        ventana->label(ventana_estadisticapool_node[0]+1,ventana_estadisticapool_node[1]+9,"Actual KES period: ");
                        ventana->label((ventana_estadisticapool_node[0]+ventana_estadisticapool_node[2]*0.5-13),ventana_estadisticapool_node[1]+11,"Operational Certificate", A_UNDERLINE | A_BOLD);
                        ventana->label(ventana_estadisticapool_node[0]+1,ventana_estadisticapool_node[1]+13,"Remaining days for expire: ");
                        
                        ventana->label(ventana_estadisticapool_node[0]+1,ventana_estadisticapool_node[1]+14,"Created in KES period: ");
                        ventana->label(ventana_estadisticapool_node[0]+1,ventana_estadisticapool_node[1]+15,"Expire in KES period: ");
                    }else{
                        ventana->label((ventana_estadisticapool_node[0]+ventana_estadisticapool_node[2]*0.5-13),ventana_estadisticapool_node[1]+11,"-No information about KES-", A_UNDERLINE | A_BOLD);
                    }
                    
                    ventana->label(ventana_memoria[0]+1,ventana_memoria[1]+1,"Memory Used: ");
                    
                    ventana->label(ventana_trx[0]+1,ventana_trx[1]+1,"In mempool: ");
                    ventana->label((ventana_trx[0]+ventana_trx[2]-11),ventana_trx[1]+1,"Max: ");
                    ventana->label(ventana_trx[0]+1,ventana_trx[1]+4,"Total Processed: ");
                    
                    ventana->label(ventana_peer[0]+1,ventana_peer[1]+1,"Connected: ");
                    
                    ventana->label(ventana_blockchain[0]+1,ventana_blockchain[1]+1,"Density: ");
                    ventana->label(ventana_blockchain[0]+1,ventana_blockchain[1]+2,"Fork: ");
                    
                    ventana->label(ventana_mined[0]+1,ventana_mined[1]+1,"Slots lead: ");
                    ventana->label(ventana_mined[0]+1,ventana_mined[1]+2,"Blocks minted: ");
                    ventana->label(ventana_mined[0]+1,ventana_mined[1]+3,"Slots missed: ");
                    
                    
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
                    ventana->label(ventana_tagversion[2]*0.3,ventana_tagversion[1]+1,"version");
                    ventana->label(ventana_tagversion[2]*0.6,ventana_tagversion[1]+1,"status");
                    ventana->label(ventana_tagversion[0]+1,ventana_tagversion[1]+2,"Github");
                    ventana->label(ventana_tagversion[0]+1,ventana_tagversion[1]+3,"Local");
                }else{
                    ventana->label(ventana_tagversion[2]*0.3,ventana_tagversion[1]+1,"version");
                    ventana->label(ventana_tagversion[2]*0.6,ventana_tagversion[1]+1,"status");	
                    ventana->label(ventana_tagversion[0]+1,ventana_tagversion[1]+3,"Local");
                    if(!consulta->github_habilitado()){
                        ventana->label(ventana_tagversion[0]+1,ventana_tagversion[1]+2,"Queries to Github are disable");
                    }
                    else{
                        ventana->label(ventana_tagversion[2]*0.3,ventana_tagversion[1]+2,"Can't be accessed to Github");
                    }	
                }
                if(acceso_adapools){
                    ventana->crear_subventana(ventana_stake[0],ventana_stake[1],ventana_stake[2],ventana_stake[3],"Stake", "Consulted to adapools");
                    ventana->label(ventana_stake[0]+1,ventana_stake[1]+1,"Pledge: ");
                    ventana->label(ventana_stake[0]+1,ventana_stake[1]+2,"Total Stake: ");
                    ventana->label(ventana_stake[0]+1,ventana_stake[1]+3,"Live Stake: ");
                    ventana->label(ventana_stake[0]+1,ventana_stake[1]+4,"Delegator: ");
                    ventana->label((ventana_stake[0]+ventana_stake[2]*0.5-6),ventana_stake[1]+6,"Saturation:");   
                }
                else{
                    if(!consulta->adapools_habilitado()){
                        ventana->label(ventana_stake[0]+1,ventana_stake[1]+4,"Queries to Adapools are disable");
                    }
                    else{
                        ventana->label(ventana_stake[0]+1,ventana_stake[1]+4,"Can't be accessed to Adapools");
                    }
                }
                ktecla=0;
                switchmain=true;
                switchpeer=false;
                switchabout=false;
                ventana->refrescar();            
            }else{
                ktecla=0;
            }
        };break;
        case 97:{ //tecla a
            if(switchabout==false && (switchmain==true || switchpeer==true)){
                ventana->borrar_subventana(0,0,COLS,LINES);
                ventana->refrescar();
                ventana->crear_ventantaprincipal(consulta->poolnamew(),A_BOLD,VERSION_WAIRA,A_BOLD);
                ventana->label(1,LINES-2,"Press \"q\" for Quit, \"ESC\" for Main ,\"p\" for Peer,\"a\" for About");
                ventana->label((COLS/2)-20,(LINES/2)-3,"Created for Eztero");
                ventana->label((COLS/2)-20,(LINES/2)-2,"Operator of TricahuePool");
                ventana->label((COLS/2)-20,(LINES/2)-1,"Twitter: @TCHPool");
                ventana->label((COLS/2)-20,(LINES/2)-0,"Web: https://eztero.github.io/TricahuePool/index_en.html");
                switchmain=false;
                switchpeer=false;
                switchabout=true;
                ventana->refrescar();
            }
        };break;
        case 112:{ //tecla p      ///RECORDAR USAR DEFAULT CON if(switchpeer){} PARA LOS DATOS
            if(switchpeer==false && (switchabout==true || switchmain==true)){
                ventana->borrar_subventana(0,0,COLS,LINES);
                ventana->refrescar();
                ventana->crear_ventantaprincipal(consulta->poolnamew(),A_BOLD,VERSION_WAIRA,A_BOLD);
                ventana->label(1,LINES-2,"Press \"q\" for Quit, \"ESC\" for Main ,\"p\" for Peer,\"a\" for About");
                ventana->label((COLS/2)-9,(LINES/2)-3,"In a future release ");
                ventana->label((COLS/2)-9,(LINES/2)-2,"this page will show your ");
                ventana->label((COLS/2)-9,(LINES/2)-1,"node's peer connection");
                switchpeer=true;
                switchmain=false;
                switchabout=false;
                ventana->refrescar();
            }
        };break;
        default:{
            if(switchmain){
                if(acceso_prometheus){
                    //ventana->crear_subventana(ventana_estadisticapool_node[0],ventana_estadisticapool_node[1],ventana_estadisticapool_node[2],ventana_estadisticapool_node[3],"Producor");
                    tiempo_restantes_slot(consulta->slot_epoca(),&dia,&hora,&min,&seg);
                    //ventana->label(ventana_estadisticapool_node[0]+1,ventana_estadisticapool_node[1]+1,"Epoch: ");
                    
                    ventana->crear_linea_horizontal(ventana_estadisticapool_node[0]+10,ventana_estadisticapool_node[1]+1,espacios[epoca],' ');
                    ventana->label(ventana_estadisticapool_node[0]+10,ventana_estadisticapool_node[1]+1,std::to_string(consulta->epoca()).c_str());
                    espacios[epoca]=std::to_string(consulta->epoca()).length();
                    //ventana->label(ventana_estadisticapool_node[0]+1,ventana_estadisticapool_node[1]+2,"Slot: ");
                    ventana->crear_linea_horizontal(ventana_estadisticapool_node[0]+8,ventana_estadisticapool_node[1]+2,espacios[slot_epoca],' ');
                    ventana->label(ventana_estadisticapool_node[0]+8,ventana_estadisticapool_node[1]+2,std::to_string(consulta->slot_epoca()).c_str());
                    espacios[slot_epoca]=std::to_string(consulta->slot_epoca()).length();
                    //ventana->label(ventana_estadisticapool_node[0]+1,ventana_estadisticapool_node[1]+3,"Block number: ");
                    ventana->label(ventana_estadisticapool_node[0]+15,ventana_estadisticapool_node[1]+3,std::to_string(consulta->numero_bloque()).c_str());
                    
                    mvprintw(ventana_estadisticapool_node[1]+5,ventana_estadisticapool_node[0]+ventana_estadisticapool_node[2]*0.5-14,"                              ");
                    mvprintw(ventana_estadisticapool_node[1]+5,ventana_estadisticapool_node[0]+ventana_estadisticapool_node[2]*0.5-14,"Epoch ends in: %id:%ih:%im:%is",dia,hora,min,seg);
                    ventana->slider_horizontal(ventana_estadisticapool_node[0]+2,ventana_estadisticapool_node[1]+6,ventana_estadisticapool_node[2]-3,EPOCHLENGTH,consulta->slot_epoca());
                    //ventana->label(ventana_estadisticapool_node[0]+1,ventana_estadisticapool_node[1]+8,"Total Slot: ");
                    ventana->label(ventana_estadisticapool_node[0]+14,ventana_estadisticapool_node[1]+8,std::to_string(consulta->totalslot()).c_str());
                    
                    if(consulta->kes_certificado()!=0){
                        //ventana->label(ventana_estadisticapool_node[0]+1,ventana_estadisticapool_node[1]+9,"Actual KES period: ");
                        ventana->label(ventana_estadisticapool_node[0]+21,ventana_estadisticapool_node[1]+9,std::to_string(consulta->kes_actual()).c_str());
                        //ventana->label((ventana_estadisticapool_node[0]+ventana_estadisticapool_node[2]*0.5-13),ventana_estadisticapool_node[1]+11,"---Pool Certificate ---", A_UNDERLINE | A_BOLD);
                        //ventana->label(ventana_estadisticapool_node[0]+1,ventana_estadisticapool_node[1]+13,"Remaining days for expire: ");
                        tbuf=consulta->dias_restante_kes();
                        if(tbuf>20){
                            ventana->crear_linea_horizontal(ventana_estadisticapool_node[0]+28,ventana_estadisticapool_node[1]+13,espacios[dias_restante_kes],' ');
                            ventana->label(ventana_estadisticapool_node[0]+28,ventana_estadisticapool_node[1]+13,std::to_string(tbuf).c_str());
                            espacios[dias_restante_kes]=std::to_string(tbuf).length();
                        }
                        else if(tbuf>5 && tbuf<=20 ){
                            ventana->crear_linea_horizontal(ventana_estadisticapool_node[0]+28,ventana_estadisticapool_node[1]+13,espacios[dias_restante_kes],' ');
                            ventana->label(ventana_estadisticapool_node[0]+28,ventana_estadisticapool_node[1]+13,std::to_string(tbuf).c_str(),COLOR_PAIR(3) | A_BOLD);
                            espacios[dias_restante_kes]=std::to_string(tbuf).length();
                        }
                        else{
                            ventana->crear_linea_horizontal(ventana_estadisticapool_node[0]+28,ventana_estadisticapool_node[1]+13,espacios[dias_restante_kes],' ');
                            ventana->label(ventana_estadisticapool_node[0]+28,ventana_estadisticapool_node[1]+13,std::to_string(tbuf).c_str(), COLOR_PAIR(1) | A_BOLD | A_BLINK);
                            espacios[dias_restante_kes]=std::to_string(tbuf).length();
                        }
                        //ventana->label(ventana_estadisticapool_node[0]+1,ventana_estadisticapool_node[1]+14,"Created in KES period: ");
                        ventana->label(ventana_estadisticapool_node[0]+25,ventana_estadisticapool_node[1]+14,std::to_string(consulta->kes_certificado()).c_str());
                        //ventana->label(ventana_estadisticapool_node[0]+1,ventana_estadisticapool_node[1]+15,"Expire in KES period: ");
                        ventana->label(ventana_estadisticapool_node[0]+24,ventana_estadisticapool_node[1]+15,std::to_string(consulta->kes_termino_certificado()).c_str());
                        
                    }else{
                        ventana->label((ventana_estadisticapool_node[0]+ventana_estadisticapool_node[2]*0.5-13),ventana_estadisticapool_node[1]+11,"-No information about KES-", A_UNDERLINE | A_BOLD);
                        
                        
                    }
                    
                    
                    //ventana->crear_subventana(ventana_memoria[0],ventana_memoria[1],ventana_memoria[2],ventana_memoria[3],"Memory");
                    //ventana->label(ventana_memoria[0]+1,ventana_memoria[1]+1,"Memory Used: ");
                    ventana->crear_linea_horizontal(ventana_memoria[0]+14,ventana_memoria[1]+1,espacios[memoria],' ');
                    ventana->label(ventana_memoria[0]+14,ventana_memoria[1]+1,std::to_string(consulta->memoria()).c_str());
                    espacios[memoria]=std::to_string(consulta->memoria()).length();
                    ventana->label(ventana_memoria[0]+15+espacios[memoria],ventana_memoria[1]+1,"MB");
                    
                    
                    txmin=consulta->transacciones_mempool();
                    if(txmin>txmax){
                        txmax=txmin;
                    }
                    //ventana->crear_subventana(ventana_trx[0],ventana_trx[1],ventana_trx[2],ventana_trx[3],"Transactions");
                    //ventana->label(ventana_trx[0]+1,ventana_trx[1]+1,"In mempool: ");
                    ventana->crear_linea_horizontal(ventana_trx[0]+13,ventana_trx[1]+1,espacios[transacciones_mempool],' ');
                    ventana->label(ventana_trx[0]+13,ventana_trx[1]+1,std::to_string(txmin).c_str());
                    espacios[transacciones_mempool]=std::to_string(txmin).length();
                    //ventana->label((ventana_trx[0]+ventana_trx[2]-11),ventana_trx[1]+1,"Max: ");
                    ventana->label((ventana_trx[0]+ventana_trx[2]-6),ventana_trx[1]+1,std::to_string(txmax).c_str());
                    ventana->slider_horizontal(ventana_trx[0]+2,ventana_trx[1]+2,ventana_trx[2]-3,txmax,txmin);
                    //ventana->label(ventana_trx[0]+1,ventana_trx[1]+4,"Total Processed: ");
                    ventana->label(ventana_trx[0]+18,ventana_trx[1]+4,std::to_string(consulta->transacciones_procesadas()).c_str());
                    
                    
                    
                    //ventana->crear_subventana(ventana_peer[0],ventana_peer[1],ventana_peer[2],ventana_peer[3],"Peers");
                    //ventana->label(ventana_peer[0]+1,ventana_peer[1]+1,"Connected: ");
                    ventana->crear_linea_horizontal(ventana_peer[0]+12,ventana_peer[1]+1,espacios[peers],' ');
                    ventana->label(ventana_peer[0]+12,ventana_peer[1]+1,std::to_string(consulta->peers()).c_str());
                    espacios[peers]=std::to_string(consulta->peers()).length();
                    
                    
                    //ventana->crear_subventana(ventana_blockchain[0],ventana_blockchain[1],ventana_blockchain[2],ventana_blockchain[3],"Blockchain");
                    //ventana->label(ventana_blockchain[0]+1,ventana_blockchain[1]+1,"Density: ");
                    ventana->label(ventana_blockchain[0]+9,ventana_blockchain[1]+1,consulta->densidad_str().c_str());
                    ventana->label(ventana_blockchain[0]+10+std::to_string(consulta->densidad()).length(),ventana_blockchain[1]+1,"\uFF05");
                    //ventana->label(ventana_blockchain[0]+1,ventana_blockchain[1]+2,"Fork: ");
                    ventana->crear_linea_horizontal(ventana_blockchain[0]+7,ventana_blockchain[1]+2,espacios[forks],' ');
                    ventana->label(ventana_blockchain[0]+7,ventana_blockchain[1]+2,std::to_string(consulta->forks()).c_str());  
                    espacios[forks]=std::to_string(consulta->forks()).length();                
                    
                    
                    //ventana->crear_subventana(ventana_mined[0],ventana_mined[1],ventana_mined[2],ventana_mined[3],"Forge");
                    //ventana->label(ventana_mined[0]+1,ventana_mined[1]+1,"Slots lead: ");
                    ventana->crear_linea_horizontal(ventana_mined[0]+13,ventana_mined[1]+1,espacios[bloques_asignados],' ');
                    ventana->label(ventana_mined[0]+13,ventana_mined[1]+1,std::to_string(consulta->bloques_asignados()).c_str());
                    espacios[bloques_asignados]=std::to_string(consulta->bloques_asignados()).length();
                    //ventana->label(ventana_mined[0]+1,ventana_mined[1]+2,"Blocks minted: ");
                    ventana->crear_linea_horizontal(ventana_mined[0]+16,ventana_mined[1]+2,espacios[bloques_creados],' ');
                    ventana->label(ventana_mined[0]+16,ventana_mined[1]+2,std::to_string(consulta->bloques_creados()).c_str());
                    espacios[bloques_creados]=std::to_string(consulta->bloques_creados()).length();
                    //ventana->label(ventana_mined[0]+1,ventana_mined[1]+3,"Slots missed: ");
                    ventana->crear_linea_horizontal(ventana_mined[0]+15,ventana_mined[1]+3,espacios[bloques_perdidos],' ');
                    ventana->label(ventana_mined[0]+15,ventana_mined[1]+3,std::to_string(consulta->bloques_perdidos()).c_str());
                    espacios[bloques_perdidos]=std::to_string(consulta->bloques_perdidos()).length();
                    
                    
                    //Up time
                    tiempo_restantes_ns(consulta->uptimens(),&dia,&hora,&min,&seg);
                    mvprintw(LINES-1,(COLS/2)-14,"                              ");
                    attron(COLOR_PAIR(2) | A_BOLD);
                    mvprintw(LINES-1,(COLS/2)-14,"Node up time: %id:%ih:%im:%is",dia,hora,min,seg);
                    attroff(COLOR_PAIR(2) | A_BOLD);
                    
                }//else{
                //ventana->label((ventana_estadisticapool_node[0]+2),ventana_estadisticapool_node[1]+7,"Can't be accessed");
                //ventana->label((ventana_estadisticapool_node[0]+2),ventana_estadisticapool_node[1]+8,"to port %s",cbuff);
                //ventana->label(ventana_memoria[0]+1,ventana_memoria[1]+1,"Can't be accessed");
                //ventana->label(ventana_trx[0]+1,ventana_trx[1]+1,"Can't be accessed");
                //ventana->label(ventana_peer[0]+1,ventana_peer[1]+1,"Can't be accessed");
                //ventana->label(ventana_blockchain[0]+1,ventana_blockchain[1]+1,"Can't be accessed");
                //ventana->label(ventana_mined[0]+1,ventana_mined[1]+2,"Can't be accessed");
                //}
                
                
                
                //ventana->crear_subventana(ventana_tagversion[0],ventana_tagversion[1],ventana_tagversion[2],ventana_tagversion[3],"Tag version","Consulted to github");
                //ventana->label(ventana_tagversion[2]*0.3,ventana_tagversion[1]+1,"version");
                //ventana->label(ventana_tagversion[2]*0.6,ventana_tagversion[1]+1,"status");
                if(acceso_github){                    
                    //ventana->label(ventana_tagversion[0]+1,ventana_tagversion[1]+2,"Github");
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
                    else{
                        ventana->label(ventana_tagversion[2]*0.6,ventana_tagversion[1]+2,"           ");
                        ventana->label(ventana_tagversion[2]*0.6,ventana_tagversion[1]+2,"Error",COLOR_PAIR(1) | A_BOLD);
                    }
                }//else{
                 //   if(!consulta->github_habilitado()){
                 //       ventana->label(ventana_tagversion[0]+1,ventana_tagversion[1]+2,"Queries to Github are disable");
                 //   }
                 //   else{
                 //       ventana->label(ventana_tagversion[2]*0.3,ventana_tagversion[1]+2,"Can't be accessed to Github");
                 //   }
                //}
                
                
                
                if(acceso_versionnodo){
                    //ventana->label(ventana_tagversion[0]+1,ventana_tagversion[1]+3,"Local");
                    ventana->crear_linea_horizontal(ventana_tagversion[2]*0.3,ventana_tagversion[1]+3,espacios[esp_nversion],' ');
                    ventana->label(ventana_tagversion[2]*0.3,ventana_tagversion[1]+3,nversion.c_str());
                    espacios[esp_nversion]=nversion.length();
                    if(version==nversion || (version!=nversion && estado!="release")){
                        ventana->label(ventana_tagversion[2]*0.6,ventana_tagversion[1]+3,"         ");	
                        ventana->label(ventana_tagversion[2]*0.6,ventana_tagversion[1]+3,"updated",COLOR_PAIR(4) | A_BOLD);	
                    }else{
                        ventana->label(ventana_tagversion[2]*0.6,ventana_tagversion[1]+3,"         ");	
                        ventana->label(ventana_tagversion[2]*0.6,ventana_tagversion[1]+3,"outdated", COLOR_PAIR(6) | A_BOLD | A_BLINK);
                    }	
                    
                }else{ventana->label(ventana_tagversion[2]*0.3,ventana_tagversion[1]+3,"Query error");}
                
                
                
                if(acceso_adapools){
                    //ventana->crear_subventana(ventana_stake[0],ventana_stake[1],ventana_stake[2],ventana_stake[3],"Stake", "Consulted to adapools");
                    //ventana->label(ventana_stake[0]+1,ventana_stake[1]+1,"Pledge: ");
                    ventana->crear_linea_horizontal(ventana_stake[0]+9,ventana_stake[1]+1,2+espacios[pledge],' ');
                    ventana->label(ventana_stake[0]+9,ventana_stake[1]+1,std::to_string(consulta->pledge()).c_str());
                    espacios[pledge]=std::to_string(consulta->pledge()).length();
                    ventana->label(ventana_stake[0]+10+espacios[pledge],ventana_stake[1]+1,"\u20B3");
                    //ventana->label(ventana_stake[0]+1,ventana_stake[1]+2,"Total Stake: ");
                    ventana->crear_linea_horizontal(ventana_stake[0]+14,ventana_stake[1]+2,2+espacios[stake_total],' ');
                    ventana->label(ventana_stake[0]+14,ventana_stake[1]+2,std::to_string(consulta->stake_total()).c_str());
                    espacios[stake_total]=std::to_string(consulta->stake_total()).length();
                    ventana->label(ventana_stake[0]+15+espacios[stake_total],ventana_stake[1]+2,"\u20B3");
                    //ventana->label(ventana_stake[0]+1,ventana_stake[1]+3,"Live Stake: ");
                    ventana->crear_linea_horizontal(ventana_stake[0]+13,ventana_stake[1]+3,2+espacios[stake_activo],' ');
                    ventana->label(ventana_stake[0]+13,ventana_stake[1]+3,std::to_string(consulta->stake_activo()).c_str());
                    espacios[stake_activo]=std::to_string(consulta->stake_activo()).length();
                    ventana->label(ventana_stake[0]+14+espacios[stake_activo],ventana_stake[1]+3,"\u20B3");
                    //ventana->label(ventana_stake[0]+1,ventana_stake[1]+4,"Delegator: ");
                    ventana->crear_linea_horizontal(ventana_stake[0]+12,ventana_stake[1]+4,2+espacios[delegadores],' ');
                    ventana->label(ventana_stake[0]+12,ventana_stake[1]+4,std::to_string(consulta->delegadores()).c_str());
                    espacios[delegadores]=std::to_string(consulta->delegadores()).length();
                    ventana->label(ventana_stake[0]+13+espacios[delegadores],ventana_stake[1]+4,"\uC6C3");
                    //ventana->label((ventana_stake[0]+ventana_stake[2]*0.5-6),ventana_stake[1]+6,"Saturation:");
                    ventana->crear_linea_horizontal((ventana_stake[0]+ventana_stake[2]*0.5+6),ventana_stake[1]+6,2+espacios[saturacion],' ');
                    ventana->label((ventana_stake[0]+ventana_stake[2]*0.5+6),ventana_stake[1]+6,std::to_string(consulta->saturacion()).c_str());
                    espacios[saturacion]=std::to_string(consulta->saturacion()).length();
                    ventana->label((ventana_stake[0]+ventana_stake[2]*0.5+7+espacios[saturacion]),ventana_stake[1]+6,"\uFF05");
                    ventana->slider_horizontal(ventana_stake[0]+2,ventana_stake[1]+7,ventana_stake[2]-3,100,(consulta->saturacion()));
                    
                }//else{
                //	if(!consulta->adapools_habilitado()){
                //		ventana->label(ventana_stake[0]+1,ventana_stake[1]+4,"Queries to Adapools are disable");
                //		}
                //		else{
                //			ventana->label(ventana_stake[0]+1,ventana_stake[1]+4,"Can't be accessed to Adapools");
                //		}
                //	}
                
                ventana->refrescar();
                
            }
        };break;
        }
        conteo_nodo++;
        conteo_github++;
        conteo_adapools++;
        if(conteo_nodo>1){
            consulta->actualizar_datos(&puerto_n);
            conteo_nodo=0;
        }
        if(conteo_github>3600){
            if(consulta->github_habilitado()){
                consulta->github(&version,&estado);
                //if(consulta->github(&version,&estado)){acceso_github=true;}else{acceso_github=false;}
            }
            conteo_github=0;
        }
        if(conteo_adapools>600){
            if(consulta->adapools_habilitado()){
                consulta->actualizar_adapools();
                //if(consulta->actualizar_adapools()){acceso_adapools=true;}else{acceso_adapools=false;}
            }
            conteo_adapools=0;
        }
        sleep(1);
    }
    
}


void tiempo_restantes_slot(const uint32_t slot_actual, uint16_t *dia,uint16_t *hora,uint16_t *min,uint16_t *seg){
    uint32_t slot_restantes=EPOCHLENGTH-slot_actual;
    *dia=slot_restantes/86400;  //dia "86400=1dia"
    *hora=(slot_restantes%86400)/3600;  //horas "3600=1hora"
    *min=(slot_restantes%3600)/60; //minutos "60=1min"
    *seg=(slot_restantes%3600)%60; //seg
}

void tiempo_restantes_ns(const uint64_t nanosegundos, uint16_t *dia,uint16_t *hora,uint16_t *min,uint16_t *seg){
    uint64_t nseg=nanosegundos/1000000000; //seg
    *dia=nseg/86400;
    *hora=(nseg%86400)/3600;
    *min=(nseg%3600)/60;
    *seg=(nseg%3600)%60;
    
}

void salidaforzosa(int a){
    delete consulta;
    delete ventana;
    std::cout<<"The program has closed unexpectedly.\nThis can occur due to misconfiguration in \"wairamonitor.json\" file."<<std::endl;
}
