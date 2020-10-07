#include "lectura_red.h"

lectura_red::lectura_red(){
    index=0;
}
void lectura_red::puerto_tcp(const uint16_t *puerto, std::string tcp_dec[][5]){
    std::string tcp_hex[540][5];
    std::string linea="";
    size_t pos_l;
    index=0;
    std::ifstream tcpdump("/proc/net/tcp",std::ifstream::in);
    //std::ifstream tcpdump("dumptcp.txt",std::ifstream::in);
    if(*puerto==0){
        if(tcpdump.is_open()){
            while(std::getline(tcpdump,linea)){
                pos_l=linea.find(":");
                if(pos_l!=std::string::npos){
                    pos_l=linea.find(":",(pos_l+1));
                    tcp_hex[index][0]=linea.substr(pos_l-8,8); //guarda ip_local
                    tcp_hex[index][1]=linea.substr(pos_l+1,4); //guarda el puerto_local
                    tcp_hex[index][2]=linea.substr(pos_l+6,8); //guarda ip_remota
                    tcp_hex[index][3]=linea.substr(pos_l+15,4); //guarda puerto_remoto
                    tcp_hex[index][4]=linea.substr(pos_l+20,2); //guarda el estado
                    index++;
                }
                
            }
            tcpdump.close();
            
        }
    }
    else{
		char c[5];
		std::string puerto_hex="";
        sprintf(c, "%X", *puerto);
        size_t c_len=strlen(c);
        switch(c_len){
			case 1:{puerto_hex.append("000");puerto_hex.append(c);}break;
			case 2:{puerto_hex.append("00");puerto_hex.append(c);}break;
			case 3:{puerto_hex.append("0");puerto_hex.append(c);}break;
			case 4:{puerto_hex.append(c);}break;
			}
        if(tcpdump.is_open()){
            while(std::getline(tcpdump,linea)){
                pos_l=linea.find(":");
                if(pos_l!=std::string::npos){
                    pos_l=linea.find(":",(pos_l+1));
                    tcp_hex[index][1]=linea.substr(pos_l+1,4); //guarda el puerto_local
                    if(puerto_hex==tcp_hex[index][1]){ //se compara el puerto encontrado
                        tcp_hex[index][0]=linea.substr(pos_l-8,8); //guarda ip_local
                        tcp_hex[index][2]=linea.substr(pos_l+6,8); //guarda ip_remota
                        tcp_hex[index][3]=linea.substr(pos_l+15,4); //guarda puerto_remoto
                        tcp_hex[index][4]=linea.substr(pos_l+20,2); //guarda el estado
                        index++;
                    }
                }
                
            }
            tcpdump.close();
            
        }	
        
    }
    //Convertir a formato leible
    if(index>540){
		index=540;
	}
    for(int b=0;b<index;b++){
        //ip local to dec
        linea=tcp_hex[b][0].substr(6,2);
        tcp_dec[b][0]=std::to_string(std::stoi(linea,nullptr,16));
        tcp_dec[b][0].append(".");
        linea=tcp_hex[b][0].substr(4,2);
        tcp_dec[b][0].append(std::to_string(std::stoi(linea,nullptr,16)));
        tcp_dec[b][0].append(".");
        linea=tcp_hex[b][0].substr(2,2);
        tcp_dec[b][0].append(std::to_string(std::stoi(linea,nullptr,16)));
        tcp_dec[b][0].append(".");
        linea=tcp_hex[b][0].substr(0,2);
        tcp_dec[b][0].append(std::to_string(std::stoi(linea,nullptr,16)));
        
        //puerto local
        linea=tcp_hex[b][1];
        tcp_dec[b][1]=std::to_string(std::stoi(linea,nullptr,16));
        
        //ip remota to dec
        linea=tcp_hex[b][2].substr(6,2);
        tcp_dec[b][2]=std::to_string(std::stoi(linea,nullptr,16));
        tcp_dec[b][2].append(".");
        linea=tcp_hex[b][2].substr(4,2);
        tcp_dec[b][2].append(std::to_string(std::stoi(linea,nullptr,16)));
        tcp_dec[b][2].append(".");
        linea=tcp_hex[b][2].substr(2,2);
        tcp_dec[b][2].append(std::to_string(std::stoi(linea,nullptr,16)));
        tcp_dec[b][2].append(".");
        linea=tcp_hex[b][2].substr(0,2);
        tcp_dec[b][2].append(std::to_string(std::stoi(linea,nullptr,16)));
        
        //puerto
        linea=tcp_hex[b][3];
        tcp_dec[b][3]=std::to_string(std::stoi(linea,nullptr,16));
        
        //Estado
        linea=tcp_hex[b][4];
        tcp_dec[b][4]=std::to_string(std::stoi(linea,nullptr,16));
    }
    
}

uint16_t lectura_red::cantidad_ips(){
    return index;
}
