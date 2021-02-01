#include "consultadatos.h"
consultadatos::consultadatos(){
    clean_array(&datoscli[0]);
    saturated=0;
    pledged="0";
    total_stake="0";
    active_stake="0";
    delegators="0";
    density=0;
    density_str="0";
    url_cardanonode="";
    reward_balance=0;
    pool_id="";
    enable_github=false;
    enable_adapools=false;
    ticks_a=0;
    ticks_b=0;
}

size_t consultadatos::WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) //funcion para curl
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
};

void consultadatos::clean_array(uint64_t datoscli[]){ //borra los arrays
    for(int a=0;a<TAMANO_ARRAY_DATOS;a++){
        datoscli[a]=0;
    }
}

//EN CONSTRUCCION
//void actualizar_ips(uint32_t puerto,std::string url_topology){
//	buff="netstat -na | grep \":";
//	buff.append(to_string(puerto);
//	buff.append("\\ \" | grep ESTABLISHED | awk '{print $5}'");
//	buffer=istringstream(exec(buff));
//  buff.clear();
//    while (getline(buffer,buff)){  //se separan los puertos de las ips obtenidos de netstat
//            size_t t=buff.find(":");
//            ip_in[cantidad_ip]=buff.substr(0,t); 
//            puerto_in[cantidad_ip]=buff.substr(t+1,string::npos);
//            cantidad_ip_in++;
//    }
//    std::ifstream ifs(url_topology, std::ifstream::in); //se leen los datos
//    buffer=std::istringstream(std::string((std::istreambuf_iterator<char>(ifs)),(std::istreambuf_iterator<char>())));
//    ifs.close();
//    
//}

std::string consultadatos::poolnamew(){
    return poolname;
}

//---------------------

bool consultadatos::github_habilitado(){
	return enable_github;
	}

bool consultadatos::adapools_habilitado(){
	return enable_adapools;
	}

bool consultadatos::actualizar_adapools(){
    if(pool_id==""){return false;}
    respuesta_url.clear();
    buff="https://js.adapools.org/pools/";
    buff.append(pool_id);
    buff.append("/summary.json");
    
    curl_global_init(CURL_GLOBAL_DEFAULT);  //parametros de configuracion curl
    curl = curl_easy_init();  //se inicia el curl para realizar consulta
    
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "curl/7.42.0");
        curl_easy_setopt(curl, CURLOPT_URL, buff.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_0); //se agrega soporte http/2 
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &respuesta_url);
        /* res recibira un codigo de retorno*/ 
        res = curl_easy_perform(curl);
        /* chequea si existen errores */ 
        if(res == CURLE_OK){
            if(respuesta_url[0]=='{'){
                json j = json::parse(respuesta_url);
                auto q_estado = j.find("data");
                if((q_estado != j.end())){
                    j=json::parse(to_string(*q_estado));	
                    
                    auto q_estado = j.find("saturated");
                    if((q_estado != j.end())){
                        saturated=*q_estado;
                    }
                    q_estado = j.find("pledged");
                    if((q_estado != j.end())){
                        pledged=*q_estado;
                    }
                    q_estado = j.find("total_stake");
                    if((q_estado != j.end())){
                        total_stake=*q_estado;
                    }
                    q_estado = j.find("active_stake");
                    if((q_estado != j.end())){
                        active_stake=*q_estado;
                    }
                    q_estado = j.find("delegators");
                    if((q_estado != j.end())){
                        delegators=*q_estado;
                    }                
                }
            }else{
                curl_easy_cleanup(curl);
                curl_global_cleanup(); // se borra todo y se cierra el curl 
                return false;
                
            }
        }
        else{
            curl_easy_cleanup(curl);
            curl_global_cleanup(); // se borra todo y se cierra el curl 
            return false;
            
        }
    }
    curl_easy_cleanup(curl);
    curl_global_cleanup(); // se borra todo y se cierra el curl 
    return true;	
    
}

bool consultadatos::github(std::string *nversion, std::string *estado){
    respuesta_url.clear();
    bool draft, prerelease;
    curl_global_init(CURL_GLOBAL_DEFAULT);  //parametros de configuracion curl
    curl = curl_easy_init();  //se inicia el curl para realizar consulta
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "curl/7.42.0");
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.github.com/repos/input-output-hk/cardano-node/releases");
        curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_0); //se agrega soporte http/2 
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &respuesta_url);
        /* res recibira un codigo de retorno*/ 
        res = curl_easy_perform(curl);
        /* chequea si existen errores */ 
        if(res == CURLE_OK){
            if(respuesta_url[0]=='['){
                json j = json::parse(respuesta_url);
                j= j.at(0);  //se pasa solo el primer array del array json
                draft=j["draft"].get<bool>();
                prerelease=j["prerelease"].get<bool>();
                
                auto q_estado = j.find("tag_name");
                if((q_estado != j.end())){
                    *nversion=*q_estado;
                    if(draft){
                        *estado="draft";
                    }
                    else if(prerelease){
                        *estado="prerelease";
                    }else{
                        *estado="release";	
                    }
                }else{
                    *nversion="Not found tag";
                    curl_easy_cleanup(curl);
                    curl_global_cleanup(); // se borra todo y se cierra el curl 
                    return false;
                }
            }else{
				*nversion="\"No json\"";
				}
        }
        else{
            buff_error= std::string(curl_easy_strerror(res));
            curl_easy_cleanup(curl);
            curl_global_cleanup(); // se borra todo y se cierra el curl 
            return false;
        }
    }
    curl_easy_cleanup(curl);
    curl_global_cleanup(); // se borra todo y se cierra el curl 
    return true;
    
}

bool consultadatos::actualizar_datos(const uint32_t *puerto){
    linea.clear();
    respuesta_url.clear();
    buff="http://127.0.0.1:";
    buff.append(std::to_string(*puerto));
    buff.append("/metrics");
    curl_global_init(CURL_GLOBAL_DEFAULT);  // se inicia parametros de configuracion curl
    curl = curl_easy_init();  //se inicia el curl para realizar consulta
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, buff.c_str());
        //curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "Waira_system"); //user agent
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &respuesta_url);
        /* res recibira un codigo de retorno*/ 
        res = curl_easy_perform(curl);
        /* chequea si existen errores */ 
        if(res != CURLE_OK){
            buff_error=std::string(curl_easy_strerror(res));
            curl_easy_cleanup(curl); // se borra todo y se cierra el curl
            curl_global_cleanup(); // se borra todo y se cierra el curl
            return false;
        }
        else{
            std::istringstream ifbuff(respuesta_url);
            
            //std::ifstream ifs("datos.txt", std::ifstream::in); //se leen los datos
            //std::istringstream ifbuff=std::istringstream(std::string((std::istreambuf_iterator<char>(ifs)),(std::istreambuf_iterator<char>())));
            //ifs.close();
            
            while(std::getline(ifbuff,linea)){ //Se almacena en el array datoscli
                posicion= linea.find(" ");
                buff=linea.substr(0,posicion);
                if(buff=="cardano_node_metrics_epoch_int"){
                    datoscli[cardano_node_metrics_epoch_int]=std::stoul(linea.substr(posicion,std::string::npos));
                }else if(buff=="cardano_node_metrics_slotInEpoch_int"){
                    datoscli[cardano_node_metrics_slotInEpoch_int]=std::stoi(linea.substr(posicion,std::string::npos));
                    
                }else if(buff=="cardano_node_metrics_slotNum_int"){
                    datoscli[cardano_node_metrics_slotNum_int]=std::stoul(linea.substr(posicion,std::string::npos));
                    
                }else if(buff=="cardano_node_metrics_currentKESPeriod_int"){
                    datoscli[cardano_node_metrics_currentKESPeriod_int]=std::stoul(linea.substr(posicion,std::string::npos));
                    
                }
                else if(buff=="cardano_node_metrics_remainingKESPeriods_int"){
                    datoscli[cardano_node_metrics_remainingKESPeriods_int]=std::stoi(linea.substr(posicion,std::string::npos));
                }
                else if(buff=="cardano_node_metrics_operationalCertificateStartKESPeriod_int"){
                    datoscli[cardano_node_metrics_operationalCertificateStartKESPeriod_int]=std::stoul(linea.substr(posicion,std::string::npos));
                }
                else if(buff=="cardano_node_metrics_operationalCertificateExpiryKESPeriod_int"){
                    datoscli[cardano_node_metrics_operationalCertificateExpiryKESPeriod_int]=std::stoul(linea.substr(posicion,std::string::npos));
                }
                else if(buff=="cardano_node_metrics_txsProcessedNum_int"){
                    datoscli[cardano_node_metrics_txsProcessedNum_int]=std::stoul(linea.substr(posicion,std::string::npos));
                }
                else if(buff=="cardano_node_metrics_blockNum_int"){
                    datoscli[cardano_node_metrics_blockNum_int]=std::stoul(linea.substr(posicion,std::string::npos));
                }
                else if(buff=="cardano_node_metrics_Mem_resident_int"){
                    datoscli[cardano_node_metrics_Mem_resident_int]=std::stoul(linea.substr(posicion,std::string::npos));
                }
                else if(buff=="cardano_node_metrics_txsInMempool_int"){
                    datoscli[cardano_node_metrics_txsInMempool_int]=std::stoi(linea.substr(posicion,std::string::npos));
                }
                else if(buff=="cardano_node_metrics_density_real"){
                    density=std::stof(linea.substr(posicion,8));
                    density_str=linea.substr(posicion,8);
                }
                else if(buff=="cardano_node_metrics_connectedPeers_int"){
                    datoscli[cardano_node_metrics_connectedPeers_int]=std::stoi(linea.substr(posicion,std::string::npos));
                }
                else if(buff=="cardano_node_metrics_nodeStartTime_int"){
                    datoscli[cardano_node_metrics_nodeStartTime_int]=std::stoull(linea.substr(posicion,std::string::npos));
                }
                else if(buff=="cardano_node_metrics_Stat_cputicks_int"){
                    datoscli[cardano_node_metrics_Stat_cputicks_int]=std::stoul(linea.substr(posicion,std::string::npos));
                    ticks_a=ticks_b;
                    ticks_b=datoscli[cardano_node_metrics_Stat_cputicks_int];
                }
                else if(buff=="cardano_node_metrics_Forge_forged_int"){
                    datoscli[cardano_node_metrics_Forge_forged_int]=std::stoi(linea.substr(posicion,std::string::npos));
                }
                else if(buff=="cardano_node_metrics_Forge_node_is_leader_int"){
                    datoscli[cardano_node_metrics_Forge_node_is_leader_int]=std::stoi(linea.substr(posicion,std::string::npos));
                }
                else if(buff=="cardano_node_metrics_Forge_didnt_adopt_int"){
                    datoscli[cardano_node_metrics_Forge_didnt_adopt_int]=std::stoi(linea.substr(posicion,std::string::npos));
                }
                
            }	
        }
    }
    curl_easy_cleanup(curl); // se borra todo y se cierra el curl
    curl_global_cleanup(); // se borra todo y se cierra el curl
    return true;
}

bool consultadatos::version_nodo(std::string *nversion){
    buff=url_cardanonode;
    if(buff.length()==0){
		return false;
	}else if(buff[0]==' '){
        return false;
    }
    uint32buff=buff.length();
    if(buff[uint32buff-1]=='/'){
        buff.append("cardano-node version");
    }
    else{
        buff.append("/cardano-node version");
    }
    
    isbuffer=std::istringstream(exec(buff));
    buff.clear();
    getline(isbuffer,buff); 
    size_t t=buff.find(" ");
    linea=buff.substr(t+1,std::string::npos); 
    t=linea.find(" ");
    linea=linea.substr(0,t); 
    *nversion=linea;
    return true;
}

uint32_t consultadatos::epoca(){
    return datoscli[cardano_node_metrics_epoch_int];
}

uint32_t consultadatos::slot_epoca(){
    return datoscli[cardano_node_metrics_slotInEpoch_int];
}

uint64_t consultadatos::totalslot(){
    return datoscli[cardano_node_metrics_slotNum_int];
}

uint32_t consultadatos::kes_actual(){
    return datoscli[cardano_node_metrics_currentKESPeriod_int];
}

uint32_t consultadatos::kes_restante(){
    return datoscli[cardano_node_metrics_remainingKESPeriods_int];
}

uint32_t consultadatos::kes_certificado(){
    return datoscli[cardano_node_metrics_operationalCertificateStartKESPeriod_int];
}

uint32_t consultadatos::kes_termino_certificado(){
    return datoscli[cardano_node_metrics_operationalCertificateExpiryKESPeriod_int];
}

uint16_t consultadatos::ticks_porciento(){
   uint32_t resultado=(ticks_b-ticks_a)/(TIEMPO_ENTRECONSULTAS);
		if(resultado>100){
			return 100;
		}
    return resultado;
}

uint32_t consultadatos::puerto_nodo(){
    return port_node;
}

uint16_t consultadatos::saturacion(){
    uint32buff=saturated*100;
    return uint32buff;
}

uint32_t consultadatos::delegadores(){
    uint32buff=std::stoi(delegators);
    return uint32buff;
}

uint64_t consultadatos::pledge(){
    uint64buff=std::stof(pledged);
    uint64buff=uint64buff/1000000;
    return uint64buff;
}

uint64_t consultadatos::stake_total(){
    uint64buff=std::stof(total_stake);
    uint64buff=uint64buff/1000000;
    return uint64buff;
}

uint64_t consultadatos::stake_activo(){
    uint64buff=std::stof(active_stake);
    uint64buff=uint64buff/1000000;
    return uint64buff;
}

uint32_t consultadatos::memoria(){
    uint32buff=(datoscli[cardano_node_metrics_Mem_resident_int])/(1048576);
    return uint32buff;	
}

uint64_t consultadatos::transacciones_procesadas(){
    return datoscli[cardano_node_metrics_txsProcessedNum_int];
}

uint32_t consultadatos::transacciones_mempool(){
    return datoscli[cardano_node_metrics_txsInMempool_int];
}

uint64_t consultadatos::numero_bloque(){
	return datoscli[cardano_node_metrics_blockNum_int];
	}
	
uint64_t consultadatos::uptimens(){
	time_t nodeStartTime=datoscli[cardano_node_metrics_nodeStartTime_int];
	time_t tiempo_maquina_local;
	time(&tiempo_maquina_local);
	uint64_t segundos=0;
	if(nodeStartTime>0){
	segundos=difftime(tiempo_maquina_local, nodeStartTime);
	}
	return segundos;
}

float consultadatos::densidad(){
    uint16_t a=(density*100);
    float b=a*0.01; 
    return b;    
}

std::string consultadatos::densidad_str(){
    return density_str;
}

std::string consultadatos::reward(){
	uint64buff=reward_balance/1000000;
	return std::to_string(uint64buff);
	}

uint16_t consultadatos::peers(){
    return datoscli[cardano_node_metrics_connectedPeers_int];
}

uint32_t consultadatos::forks(){
	return datoscli[cardano_node_metrics_forksCreatedNum_int];
	}

uint32_t consultadatos::bloques_creados(){
	return datoscli[cardano_node_metrics_Forge_forged_int];
	}

uint32_t consultadatos::bloques_asignados(){
	return datoscli[cardano_node_metrics_Forge_node_is_leader_int];
	}

uint32_t consultadatos::bloques_perdidos(){
	return datoscli[cardano_node_metrics_Forge_didnt_adopt_int];
	}

std::string consultadatos::exec(std::string command) {
    char *buffer = new char[490];
    std::string result = "";
    // abre la tuberia al archivo
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        return "popen failed!";
    }
    //lee hasta el final del proceso:
    while (!feof(pipe)) {
        // usa el buffer para leer y agregarlo a result
        if (fgets(buffer, 490, pipe) != NULL){
            result.append(buffer);
        }
    }
    pclose(pipe);
    delete[] buffer;
    return result;
}

bool consultadatos::cargar_configuracion(ngui *ventana, const double ventanapos_xyanchoalto[]){
    //haypuertorelay=false;
    haypuertonode=false;
    bool haynombre=true;
    //-----------Verifica el archivo de configuracion -----------------------
    std::ifstream conf("wairamonitor.conf",std::ifstream::in);
    if(conf.is_open()){
        ventana->label(ventanapos_xyanchoalto[0]+2,ventanapos_xyanchoalto[1]+1,"* Configuration file \"wairamonitor.conf\" found",COLOR_PAIR(4));
        json rconfig = json::parse(conf);
        conf.close();
        ventana->refrescar();
        usleep(100000);
        ventana->label(ventanapos_xyanchoalto[0]+2,ventanapos_xyanchoalto[1]+2,"* Load configuration of \"wairamonitor.conf\"",COLOR_PAIR(4));
        auto q_estado = rconfig.find("Name"); 
        if(q_estado != rconfig.end()){poolname=*q_estado;}else{ventana->label(ventanapos_xyanchoalto[0]+2,ventanapos_xyanchoalto[1]+3,"* Name not found",COLOR_PAIR(1) | A_BLINK);haynombre=false;}
        ventana->refrescar();
        usleep(100000);
        q_estado = rconfig.find("Prometheus_port");  
        if(q_estado != rconfig.end()){port_node=*q_estado;if(port_node>1){haypuertonode=true;ventana->label(ventanapos_xyanchoalto[0]+2,ventanapos_xyanchoalto[1]+4,"* The prometheus \"node\" port found",COLOR_PAIR(4));}else{ventana->label(ventanapos_xyanchoalto[0]+2,ventanapos_xyanchoalto[1]+4,"* The prometheus \"node\" port is not valid number",COLOR_PAIR(1));haypuertonode=false;}}else{ventana->label(ventanapos_xyanchoalto[0]+2,ventanapos_xyanchoalto[1]+4,"* Prometheus port of \"node\" not found",COLOR_PAIR(1));haypuertonode=false;}
        ventana->refrescar();
        usleep(100000);
        q_estado = rconfig.find("Directory_path"); 
        if(q_estado != rconfig.end()){url_cardanonode=*q_estado;if(url_cardanonode!=""){ventana->label(ventanapos_xyanchoalto[0]+2,ventanapos_xyanchoalto[1]+5,"* Path to cardano-node and cardano-cli found",COLOR_PAIR(4));}else{ventana->label(ventanapos_xyanchoalto[0]+2,ventanapos_xyanchoalto[1]+5,"* Path to cardano-node and cardano-cli is empty",COLOR_PAIR(3));}}else{ventana->label(ventanapos_xyanchoalto[0]+2,ventanapos_xyanchoalto[1]+5,"* Path to cardano-node and cardano-cli not found",COLOR_PAIR(3));}
        ventana->refrescar();
        usleep(100000);
        q_estado = rconfig.find("Pool_id"); 
        if(q_estado != rconfig.end()){pool_id=*q_estado;if(pool_id!=""){ventana->label(ventanapos_xyanchoalto[0]+2,ventanapos_xyanchoalto[1]+6,"* Pool id found",COLOR_PAIR(4));}else{ventana->label(ventanapos_xyanchoalto[0]+2,ventanapos_xyanchoalto[1]+6,"* Pool Id is empty",COLOR_PAIR(3));}}else{ventana->label(ventanapos_xyanchoalto[0]+2,ventanapos_xyanchoalto[1]+6,"* Pool Id not found",COLOR_PAIR(3));}
        ventana->refrescar();
        usleep(100000);
        q_estado = rconfig.find("Query_github"); 
        if(q_estado != rconfig.end()){enable_github=*q_estado;if(enable_github){ventana->label(ventanapos_xyanchoalto[0]+2,ventanapos_xyanchoalto[1]+7,"* Queries to Github are enable",COLOR_PAIR(4));}else{ventana->label(ventanapos_xyanchoalto[0]+2,ventanapos_xyanchoalto[1]+7,"* Queries to Github are disable",COLOR_PAIR(3));}}else{enable_github=false;ventana->label(ventanapos_xyanchoalto[0]+2,ventanapos_xyanchoalto[1]+7,"* Queries to Github are disable",COLOR_PAIR(3));}
        ventana->refrescar();
        usleep(100000);
        q_estado = rconfig.find("Query_adapools"); 
        if(q_estado != rconfig.end()){enable_adapools=*q_estado;if(enable_adapools){ventana->label(ventanapos_xyanchoalto[0]+2,ventanapos_xyanchoalto[1]+8,"* Queries to Adapools are enable",COLOR_PAIR(4));}else{ventana->label(ventanapos_xyanchoalto[0]+2,ventanapos_xyanchoalto[1]+8,"* Queries to Adapools are disable",COLOR_PAIR(3));}}else{enable_adapools=false;ventana->label(ventanapos_xyanchoalto[0]+2,ventanapos_xyanchoalto[1]+8,"* Queries to Adapools are disable",COLOR_PAIR(3));}
        usleep(100000);
        ventana->refrescar();
        ventana->label(ventanapos_xyanchoalto[0]+2,ventanapos_xyanchoalto[1]+10,"* Configuration Loaded",COLOR_PAIR(4) | A_BOLD);
        ventana->refrescar();
        usleep(100000);
        if(!haypuertonode){
            ventana->label(ventanapos_xyanchoalto[0]+2,ventanapos_xyanchoalto[1]+11,"* Have not Prometheus ports",COLOR_PAIR(1) | A_BOLD | A_BLINK);
            ventana->refrescar();
            return false;
        }else if(!haynombre){
            return false;
        }else{
            return true;
        }
        
        
        return true;
    }
    else{
        ventana->label(ventanapos_xyanchoalto[0]+2,ventanapos_xyanchoalto[1]+2,"* Configuration file not found \"wairamonitor.conf\"",A_BOLD | COLOR_PAIR(3));
        ventana->refrescar();
        usleep(100000);
        ventana->label(ventanapos_xyanchoalto[0]+2,ventanapos_xyanchoalto[1]+3,"* Creating configuration file ...",COLOR_PAIR(4));
        json wconfig;
        wconfig = {
			{"Query_github", false},
			{"Query_adapools",false},
            {"Name","MyPoolName"},
            {"Prometheus_port",12798},
            {"Directory_path",""},
            //{"Socket_path", ""},
            {"Pool_id",""},
            //{"Stake_address",""}
        }; 
        std::ofstream escribir("wairamonitor.conf",std::ios::out);
        ventana->refrescar();
        usleep(100000);
        if(escribir.is_open()){
            escribir << std::setw(4) << wconfig << std::endl;
            escribir.close();
            ventana->label(ventanapos_xyanchoalto[0]+2,ventanapos_xyanchoalto[1]+4,"* A configuration file has been created.",A_BOLD | COLOR_PAIR(2));
            usleep(100000);
            ventana->label(ventanapos_xyanchoalto[0]+2,ventanapos_xyanchoalto[1]+6,"Please open the file \"wairamonitor.conf\"", A_BOLD);
            ventana->label(ventanapos_xyanchoalto[0]+2,ventanapos_xyanchoalto[1]+7,"if you want to configure your parameters", A_BOLD);
            ventana->refrescar();
            return false;
        }
        else{
            ventana->label(ventanapos_xyanchoalto[0]+2,ventanapos_xyanchoalto[1]+4,"Could not create a configuration file",A_BLINK | A_BOLD |COLOR_PAIR(1));
            ventana->refrescar();
            return false;
        }
    }
    return true;
}

uint32_t consultadatos::dias_restante_kes(){  //calcula los dias que faltan para renovar el certificado
    uint32_t diasfaltantes;
    diasfaltantes=datoscli[cardano_node_metrics_remainingKESPeriods_int]*SLOTSPERKESPERIOD; //convierte a slots
    diasfaltantes=diasfaltantes/86400; // dias faltantes
    return diasfaltantes;
}
