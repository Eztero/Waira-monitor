#ifndef CONSULTADATOS_H
#define CONSULTADATOS_H
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <ctime>
#include <curl/curl.h>
#include "json.hpp"
#include "ngui.h"
#include <iomanip> 
#include <unistd.h>
#define TAMANO_ARRAY_DATOS 18
#define SLOTSPERKESPERIOD 129600
#define EPOCHLENGTH 432000    //cada slot demora 1 segundo
#define TIEMPO_ENTRECONSULTAS 1
#define HERZ 100
using json = nlohmann::json;

class consultadatos{
	public:
	consultadatos();
	bool actualizar_datos(const uint32_t *puerto);
	bool actualizar_adapools();  //consulta en adapools.org por estadisticas de id pool
	uint32_t epoca();
	uint32_t slot_epoca();
	uint64_t totalslot();
	uint32_t kes_actual();
	uint32_t kes_restante();
	uint32_t kes_certificado();
	uint32_t kes_termino_certificado();
	uint32_t puerto_nodo();
	uint16_t saturacion();
	uint32_t delegadores();
	uint64_t pledge();
	uint64_t stake_total();
	uint64_t stake_activo();
	uint32_t memoria();
	uint64_t transacciones_procesadas();
	uint32_t transacciones_mempool();
	uint16_t peers();
	uint32_t dias_restante_kes();
	uint64_t numero_bloque();
	uint64_t uptimens();
	uint32_t forks();
	uint32_t bloques_creados();
	uint32_t bloques_asignados();
	uint32_t bloques_perdidos();
	uint16_t ticks_porciento();
	bool cargar_datosreward(); //consulta la cantidad de reward del pool
	float densidad();
	std::string densidad_str();
	std::string reward();
	std::string version();  //devuelve la version
	std::string poolnamew(); 
	
	bool cargar_configuracion(ngui *ventana, const double ventanapos_xyanchoalto[]); //carga los datos dentro de la ventana creada
	bool github(std::string *nversion, std::string *estado); //consulta el repo de github cardano-node por la version y el estado 
	bool version_nodo(std::string *nversion);
	bool github_habilitado();
	bool adapools_habilitado();
	
	private:   
	float saturated, density;
	std::istringstream isbuffer;
	std::string exec(std::string command);
	void clean_array(uint64_t datoscli[]);
	static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);
	    enum estadistica{  //actualmente tiene 18 No olvidar editar TAMANO_ARRAY_DATOS
        cardano_node_ChainDB_metrics_epoch_int,  //EPOCA
        cardano_node_ChainDB_metrics_slotInEpoch_int, //SLOT
        cardano_node_ChainDB_metrics_slotNum_int, //TOTAL SLOTS
        cardano_node_Forge_metrics_currentKESPeriod_int, //PERIODO KES ACTUAL
        cardano_node_Forge_metrics_remainingKESPeriods_int, //PERIODOS KES RESTANTES PARA CADUCAR CERTIFICADO
        cardano_node_Forge_metrics_operationalCertificateStartKESPeriod_int, //PERIODO KES CREACION CERTIFICADO
        cardano_node_Forge_metrics_operationalCertificateExpiryKESPeriod_int, //PERIODO KES EXPIRACION CERTIFICADO
        cardano_node_metrics_txsProcessedNum_int,  //Transacciones totales Procesadas
        cardano_node_ChainDB_metrics_blockNum_int, // indica el numero del bloque
        cardano_node_metrics_Mem_resident_int, //consumo memoria = (cardano_node_metrics_Stat_rss_int*4096)/(1024*1024)
        cardano_node_metrics_txsInMempool_int, //transacciones que se estan procesando en la memoria
        cardano_node_BlockFetchDecision_peers_connectedPeers_int, //muestra los peers conectados
        cardano_node_metrics_nodeStartTime_int,  //tiempo de actividad del nodo en time_t
        cardano_node_metrics_Forge_forged_int, //son los bloques creados
		cardano_node_metrics_Forge_node_is_leader_int, //son los bloques asignados para crear
		cardano_node_ChainDB_metrics_forksCreatedNum_int, //Forks creados 
		cardano_node_metrics_Forge_didnt_adopt_int, //bloques no adoptados por otros en la red
		cardano_node_metrics_Stat_cputicks_int //los ticks de cpu
        //Falta uso de memoria y cpu y bloques minados, incluir bloques por minutos
        
    };
    uint64_t datoscli[TAMANO_ARRAY_DATOS], uint64buff,reward_balance;
    uint32_t uint32buff,ticks_a,ticks_b;
	uint16_t posicion,port_node,cantidad_ip_in=0;
	bool haypuertorelay, haypuertonode, enable_github,enable_adapools;
    std::string linea,buff,respuesta_url,ip_in[100],puerto_in[100],ips_out[100],puertos_out[100],poolname,url_socket,url_cardanonode,stakeaddress,pool_id,active_stake,total_stake,pledged,delegators,buff_error, density_str;
    CURL *curl;                        //parametros de configiracion curl
    CURLcode res; 
	
	};

#endif
