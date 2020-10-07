/*
Estados
    TCP_ESTABLISHED = 1,
    TCP_SYN_SENT = 2
    TCP_SYN_RECV = 3
    TCP_FIN_WAIT1 = 4
    TCP_FIN_WAIT2 = 5
    TCP_TIME_WAIT = 6
    TCP_CLOSE = 7
    TCP_CLOSE_WAIT = 8
    TCP_LAST_ACK = 9
    TCP_LISTEN = 10
    TCP_CLOSING = 11 
    TCP_NEW_SYN_RECV = 12
    TCP_MAX_STATES = 13
*/


#ifndef LECTURARED_H
#define LECTURARED_H
#include <sstream>
#include <fstream> 
#include <string>
#include <cstring>

class lectura_red{ //libreria para escanear los puertos de tu computadora
public:
lectura_red();
void puerto_tcp(const uint16_t *puerto, std::string tcp_dec[][5]); //escanea el puerto local especifico tcp (0=escanea todos)  y devuelve los datos encontrados tcp_dec[540][5] (tcp_dec[index][ip_local,puerto_local,ip_remota,puerto_remoto,estado])
uint16_t cantidad_ips(); //indica la cantidad de ips tcp encontradas

private:	
uint16_t index;
};

#endif
