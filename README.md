# Ejercicio 1: Ahorcado
## Taller de Programación I

**Matias Charrut** - 101137

[Link al repositorio](https://github.com/mati1297/tp1_taller1)

## Descripción

El ejercicio consistió en el armado de un juego del ahorcado distribuido, es decir, que tiene dos partes. Una es el servidor, el cual lee palabras desde un archivo de texto y se pone a disposición de que los clientes se conecten a el, uno a la vez. La otra parte es el cliente, el cual se conecta al servidor y recibe de el la informacion sobre el juego y su estado. Además, es quien interactúa con el usuario, leyendo las letras que ingresa éste y enviandoselas al servidor.

Para la resolucion del problema se planteó primeramente la construcción del juego, constituido por el TDA ```Hanged```, el cual representa al juego en su totalidad. Este TDA se abstrae totalmente de la implementación del juego en la manera distribuida, por lo que podría ser ejecutado sin utilizar un server y un cliente. Sus funciones se componen de inicializadores y desinicializadores, una función que permiten agregar una palabra al juego eliminando la anterior(```hangedAddWord()```), una función que permite probar una letra y validar si la partida terminó y cual fue el resultado (```hangedTryLetter()```) y una serie de getters utilizados por otras partes del programa. Además cuenta con algunas funciones de tipo ```static``` que son usadas de manera privada por el TDA.

Luego, el programa tiene sus dos TDAs principales: ```Server``` y ```Client```. Estos serán directamente manejados a través de sus métodos por el main correspondiente a cada uno. Ambos tienen una estructura similar, teniendo funciones para inicializar, ejecutar, y desinicializar. Ambos poseen una instancia del TDA ```Socket```, para la comunicación, y otra del TDA ```FileReader```, para la lectura de de datos que le corresponde a cada uno. Además, el TDA ```Server``` posee una instancia de ```Hanged```, en donde se ejecuta el juego.

```Client``` no tiene ninguna información sobre el estado del juego más que la que recibe de parte del server. Por lo que unicamente se encarga de recibir datos, desempaquetarlos y mostrarlos por pantalla. Luego recibe los datos de entrada y los envía al server. Toda la validación y lógica del juego es realizada en el server. Lo único que verifica es que el caracter sea valido, para poder mostrar un mensaje al usuario notificandolo. 

Para la comuncición se utiliza el TDA ```Socket```. El cual permite y encapsula el uso de los sockets. Este posee funciones tanto útiles para el server como para el cliente: funciones de conexión (como ```socketConnect()```, ```socketBindAndListen()```, ```socketAccept()```) y funciones que permiten enviar y recibir datos (```socketSend()``` y ```socketReceive()```).

Para la lectura de archivos, se utiliza el TDA ```FileReader()```. Este permite el manejo de archivos (o el flujo stdin). Su función principal utilizada en el programa es ```fileReaderReadLine()```, la cual permite leer una linea desde la entrada.

DIAGRAMA

ACLARACION SOBRE HEAP.
