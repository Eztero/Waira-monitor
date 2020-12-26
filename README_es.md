# Waira-monitor

*Modo Rele*
![ ](./images/monitor.jpg  "Relay mode")

*Modo Nodo*

![ ](./images/monitor2.jpg  "Node mode")

Es una herramienta creada para monitorizar los nuevos nodos Haskell (cardano-node). Esta escrita en c++ y usa ncurses para su interfaz gráfico, por lo que se puede visualizar en cualquier dispositivo que use una terminal.
Este monitor nació de la necesidad de tener un dashboard sin depender de programas externos como Grafana y así se pueda acceder más fácilmente a estos datos estadísticos con un interfaz liviano y simple.

Dentro de las características que ofrece este monitor, están:

* Comprueba las versiones de cardano-node de tu servidor con los repositorios de github para que no te pierdas ninguna actualización.

* Realiza consultas a adapools.org para ver estadísticas de tu pool, como delegaciones, stake y saturación.

* Muestras las estadísticas básicas de tu nodo en funcionamiento, por ejemplo, el KES y así no olvides renovar tus llaves. 


## Compilación

Primero deberás instalar las dependencias requeridas para compilar Waira-monitor.

	sudo apt install g++ make libncurses-dev libcurl4-openssl-dev

en algunos sistemas linux da un error al compilar ncursesw, en ese caso debes instalar esta dependencia.

    sudo apt install libncursesw5-dev

luego descarga la versión más reciente desde la pestaña reléase, y descomprímela en el directorio que desees, en ese mismo directorio ahora veras un archivo llamado "Makefile" y una carpeta llamada "src”.
Ahora en tu terminal deberás ir al directorio donde descomprimiste el Waira-monitor y realizar un make.

	make

Esto generara un archivo llamado "wairamonitor"

puedes ejecutar con:

	./wairamonitor

>Waira-monitor usa un archivo de configuración para ajustar los parámetros personales, si este no existe el mismo programa creara uno el cual podrás editar después.

## Configuración

Waira-monitor usa un archivo de configuración para que puedas ajustar más fácilmente los datos de tu monitor, este está en formato json, para que sea más amigable para los usuarios.

el archivo de configuración se llama "wairamonitor.conf", el programa lo puede crear automáticamente cuando no lo detecta, una vez creado puedes editarlo completando los datos que desees.
el archivo de configuración "wairamonitor.conf" obedece a esta configuración.

	{
    	"Directory_path": "",
    	"Name": "MyPoolName",
    	"Pool_id": "",
    	"Prometheus_port": 12798,
    	"Query_github":false,
        "Query_adapools":false
   	 }

* Directory_path: Corresponde a la ruta donde se encuentra tu cardano-node y cardano-cli, por ejemplo "~/Rele/" y es usado para comparar tu versión de nodo con la de los repositorios de Github, si se deja en blanco, solo mostrara la última versión del cardano-node de Github.
* Name: Es el nombre que tendrá la ventana de tu monitor.
* Pool_id: Es la id de tu pool y se usa para realizar las consultas a adapools.org, si lo dejas en blanco o simplemente borras esta configuración, las consultas no se realizarán.
* Prometheus_port: Conecta a tu puerto prometheus de tu cardano-node, por defecto siempre es el 12798.
* Query_github: Habilita (true) o deshabilita (false) las consultas a Github.
* Query_adapools: Habilita (true) o deshabilita (false) las consultas a adapools.org.

>Como medida de seguridad, se recomienda que las consultas solo estén habilitadas en el nodo Rele, ya que estas IP son de conocimiento público.
Si habilita esto en el nodo productor, su IP podría verse comprometida en los servidores donde se realizan las consultas.

En el archivo de configuración de tu nodo "mainnet-config.json" deberás establecer en "true" el "TraceBlockFetchDecisions" de esta manera podrás visualizar la estadística de los peer.

	"TraceBlockFetchDecisions": true,

En ese mismo archivo de configuración más abajo podrás ver las configuraciones de los puerto Prometheus y EKG.

	"hasEKG": 12788,
	"hasPrometheus": [
	"127.0.0.1",
	12798
	],

Como puedes ver el puerto para Prometheus en este ejemplo, se encuentra corriendo por defecto en el 12798.


## Ejecución
Una vez configurado tu archivo de configuración, puedes dejar tu ejecutable junto con su archivo de configuración donde desees, luego para ejecutarlo debes tener tu terminal en la misma ruta de tu ejecutable y hacer un:

	./wairamonitor

***

Si quieres apoyar mi trabajo :)

[![](https://raw.githubusercontent.com/Eztero/donacion/master/imagen/donate_cardano.png)](https://raw.githubusercontent.com/Eztero/donacion/master/texto/donate_cardano.txt)
