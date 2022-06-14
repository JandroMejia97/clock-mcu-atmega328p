<div align="center">
  <h1>
    Circuitos Digitales y Microcontroladores
  </h1>
  <p style="text-align:center">
    <strong>
      Universidad Nacional de La Plata
    </strong>
  </p>
  <p style="text-align:center">
      Facultad de Ingeniería. Departamento de Electrónica
  </p>
  <p style="text-align:center">
    23 de Abril de 2022
  </p>
  <p>
    <a>
      1323/7 Fernández Francisco
    </a>
    &nbsp;
    <a>
      2402/0 Hernández Mejía Carlos Alejandro
    </a>
  </p>
</div>
</hr>

## Problema

Implementar con el MCU un reloj con fecha y hora como el mostrado en la Fig.1. Para esto se dispone de un display LCD de 2 líneas, un teclado matricial 4x4 y el Atmega328p. La implementación deberá hacerse con máquinas de estados finitos temporizadas con Timer. A continuación se enumeran los requerimientos que debe satisfacer el sistema: 

a) En el estado por defecto se deberá mostrar en la primera línea del LCD un reloj funcionando con el formato HH:MM:SS (horas, minutos y segundos) y en la segunda línea la fecha en formato DD/MM/AA (día, mes y año). El valor inicial del reloj se establece en tiempo de compilación por ejemplo 23:59:59 de 31/12/21. 

b) Para modificar los datos (fecha y hora) se deberá presionar ‘A’. Para cancelar y no aceptar la modificación se deberá presionar ‘D’ en cuyo caso se volverá al estado por defecto. 

c) El proceso de modificación es secuencial, empezando por el año y siguiendo con el mes, el día, luego la hora, minutos y por último segundos. Para pasar de un campo al siguiente se deberá presionar ‘A’. Si es el campo segundos al presionar ‘A’ se establecerán los nuevos datos al sistema volviendo al estado por defecto. 

d) Cada campo a modificar tendrá su rango de validez, por ejemplo la hora tiene un rango de 0 a 23, los minutos y segundos de 0 a 59, el año de 0 a 99, los meses de 1 a 12. Los días dependen de cada mes. Para incrementar el valor actual de un campo se deberá presionar ‘B’ y para decrementarlo se deberá presionar ‘C’. 

## Interpretación

Debemos diseñar e implementar una interfaz de usuario de un reloj que muestra fecha y hora a través de un teclado matricial de 4x4 para la entrada y un display LCD de 2 líneas para la salida. La interfaz permite tanto mostrar como modificar la fecha y hora actual.

La hora debe ser actualizada cada 1 segundo de manera exacta de modo que quede sincronizada con la hora real, y además, dicha tarea debe ser concurrente respecto a la actualización de la máquina de estados finitos usada para modelar el sistema.

Lo anterior, nos hace pensar que no podemos utilizar retardos bloqueantes que nos perjudiquen en la ejecución de tareas concurrentes. Por lo tanto, sincronizamos nuestro sistema por medio de interrupciones del timer, que, además de no realizar retardos bloqueantes, no dependen del tiempo que tarden en ser ejecutadas las tareas, siempre interrumpirán de manera periódica.

Para realizar lo anterior, se deben tener en cuenta manejadores de interrupciones no bloqueantes y tareas que tarden menos que lo que se configura como período de interrupción.

Al conectar el sistema a la alimentación, la hora se inicializa con el valor 23:59 y la fecha 31/12/2021, utilizando un formato de 24hs para mostrar la hora y dd/mm/aaaa para la fecha.

Cuando se aprieta la ‘A’ se procede a modificar de forma secuencial el año, luego el mes, el día, seguido la hora, los minutos y por último, los segundos.

Si se presiona la tecla ‘D’ en cualquier momento, se vuelve al estado por defecto, es decir, mostrando la hora en la primera línea y la fecha en la segunda.



## Resolución

La solución se dividirá en 4 partes, siendo estas: Las máquinas de estados finitos, La temporización, la interfaz de usuario y la organización del código

### Parte 1.1: Máquinas de estados finitos

Modelamos el funcionamiento del sistema como una máquina de estados de Moore, es decir, la salida depende únicamente del estado actual y no de la entrada. A continuación se muestra la tabla de transición de estados.

<table>
  <tr>
   <td><strong>Entrada</strong>
   </td>
   <td><strong>Estado actual</strong>
   </td>
   <td><strong>Estado siguiente</strong>
   </td>
  </tr>
  <tr>
   <td>Se aprieta la ‘A’
   </td>
   <td>Default
   </td>
   <td>Editar año
   </td>
  </tr>
  <tr>
   <td>Se aprieta la ‘A’
   </td>
   <td>Editar año
   </td>
   <td>Editar mes
   </td>
  </tr>
  <tr>
   <td>Se aprieta la ‘A’
   </td>
   <td>Editar mes
   </td>
   <td>Editar día
   </td>
  </tr>
  <tr>
   <td>Se aprieta la ‘A’
   </td>
   <td>Editar día
   </td>
   <td>Editar hora
   </td>
  </tr>
  <tr>
   <td>Se aprieta la ‘A’
   </td>
   <td>Editar hora
   </td>
   <td>Editar minuto
   </td>
  </tr>
  <tr>
   <td>Se aprieta la ‘A’
   </td>
   <td>Editar minuto
   </td>
   <td>Editar segundo
   </td>
  </tr>
  <tr>
   <td>Se aprieta la ‘A’
   </td>
   <td>Editar segundo
   </td>
   <td>Finalizar Edición
   </td>
  </tr>
  <tr>
   <td>-
   </td>
   <td>Finalizar edición
   </td>
   <td>Default
   </td>
  </tr>
  <tr>
   <td>Se aprieta la ‘D’
   </td>
   <td>!Default
   </td>
   <td>Edición cancelada
   </td>
  </tr>
  <tr>
   <td>-
   </td>
   <td>Edición cancelada
   </td>
   <td>Default
   </td>
  </tr>
</table>

### Parte 1.2: Implementación en pseudocódigo

La máquina de estados finitos la implementamos haciendo uso de una estructura de control switch-case que, de acuerdo al estado actual, ejecuta una serie de tareas, entre las que lee las entradas (solo las que podrían provocar un cambio del estado), actualiza el estado y al transicionar de estado, actualiza las salidas. 

    - Default:
      - Si no se presionan teclas
        - Actualizar fecha y hora en el display
      - Si se presiona la tecla A
        - Default:
          - Próximo estado: Editar año
        - Editar año:
          - Próximo estado: Editar mes
        - Editar mes:
          - seteo los respectivos límites
          - Próximo estado: Editar día
        - Editar día:
          - seteo los respectivos límites
          - Próximo estado: Editar hora
        - Editar hora:
          - seteo los respectivos límites
          - Próximo estado: Editar minuto
        - Editar minuto:
          - seteo los respectivos límites
          - Próximo estado: Editar segundo
        - Editar segundo:
          - seteo los respectivos límites
          - Próximo estado: Finalizar edición
        - Finalizar edición: Seteo los datos y actualizar fecha y hora en el display

Adicionalmente:

    - En cualquier otro estado distinto de  default:
      - Si se presiona la tecla B
        - Incremento el campo
      -	Si se presiona la tecla C
        - Decremento el campo
      -	Si se presiona la tecla D
        - Cancelar edición
      -	Actualizar fecha y hora en el display

### Parte 1.3: Implementación en código C

**<p align="center" >Vea los archivos adjuntos en el repositorio</p>**

En caso de presionar la ‘B’ o la ‘C’ en un estado distinto al default vamos a aumentar o decrementar el campo que estemos editando, para ello, debemos tener los límites asociados a cada campo. Para ello, usamos un método genérico que se va adaptando al campo a modificar y según la tecla presiona, incrementa o decrementa dicho valor.
Si presionamos A se hace la transición al siguiente estado, y si presionamos la D en un estado distinto al default cancelamos la edición.


![alt_text](images/image1.png "image_tooltip")


### Parte 2.1: Tareas

Debemos planificar y ejecutar dos tareas en simultáneo. Una es la de mantener actualizado el LCD con la fecha y hora real del sistema, y por otro lado, proveer la interfaz de usuario del sistema para que sea posible interactuar con el mismo y poder modificar los datos.

Para esto usamos la arquitectura de software time-triggered, que consiste en una única interrupción periódica que marca el “tick” del sistema, y en base a esta un planificador de tareas y un despachador de las mismas permite su ejecución. Como restricciones, el tiempo de ejecución de las tareas debe ser menor que el tiempo de “tick” elegido. Por ello, las tareas deben ser no-bloqueantes: hacer uso de máquinas de estado finito para controlar tareas que reaccionan ante eventos es una buena opción para esto. Además, el tiempo de “tick” debe ser elegido en base a la necesidad de las tareas a ejecutarse cada determinada cantidad de tiempo.

La tarea de actualizar el reloj del sistema debe ser ejecutada cada 1 segundo, ya que la resolución de nuestro reloj es de 1 segundo, y consta de una serie de operaciones básicas de comparación y modificación de variables para calcular la hora, los minutos, los segundos (y por consecuencia, el día, mes y el año) al incrementar 1 segundo. Es decir, es una tarea no bloqueante que tiene un corto tiempo de ejecución, del orden de los microsegundos dado que trabajamos con un microcontrolador con un oscilador externo de 16MHz.

Por otro lado, la tarea de llevar a cabo la interfaz de usuario es implementada a través de una máquina de estados finitos que es actualizada periódicamente, haciendo las tareas de lectura de entradas y actualización de salidas. 

El período de tiempo que toma la actualización de la MEF está relacionado con el tiempo de respuesta que se desea obtener en la interfaz con el usuario. Estamos hablando de un teclado matricial, por lo tanto, un tiempo de 50ms es suficiente.

En síntesis, debemos ejecutar una de las tareas cada 50ms y otra cada 1 segundo. Por lo tanto, debemos obtener un tick que sea común divisor de ambos tiempos. Utilizamos como tiempo de tick el máximo común divisor entre 50ms y 1000ms, que es 50ms.

Por último, para lograr que la ejecución de cada una de las tareas sea periódica y no se vea afectada por el tiempo de ejecución de la otra, podemos dividir el tick en 2 “slots” de tiempo, obteniendo un tick de 25ms y ejecutar la tarea de actualización de la hora cada 1000ms, empezando en el “tiempo 0ms” y la de actualización de la MEF cada 100ms empezando en el “tiempo 25ms”.

### Parte 2.2: Configuración del microcontrolador

Utilizamos el TIMER 1 para ejecutar la interrupción periódica cada 50ms. Éste trabaja con un preescalador de 8 bits y un contador de 16 bits. Lo programamos en modo CTC (cuenta hasta un número e interrumpe). Además, habilitamos la interrupción del timer 1 y las interrupciones globales.

Teniendo una frecuencia de CPU de 16 Mhz, si usamos el preescalador que divide la frecuencia por 256, obtenemos una nueva frecuencia de 65.5KHz. Es decir, 65500 oscilaciones por segundo. Si queremos que interrumpa cada 0.05 segundos, hacemos 65600 * 0.05 y nos da como resultado 3125, qué es valor al cual el contador debe llegar para interrumpir. 

En el módulo SEOS, se observa el siguiente código.
    	
A continuación, se muestran el planificador de tareas, que prende un flag si la cantidad de ticks correspondiente para cada tarea se ha cumplido, y el despachador de tareas, que es ejecutado en el super-loop que chequea la tarea a ser ejecutada. Cada 1 segundo se actualiza la hora, cada 500ms o medio segundo se actualiza el blink (solo activo en modo edición), y cada 50ms se actualiza la MEF.

![alt_text](images/image2.png "image_tooltip")

Y por último la interrupción periódica del timer que se encarga de ejecutar el planificador de tareas.	

### Parte 3.1: La pantalla LCD

El LCD utilizado es una pantalla de cristal líquido que nos permite mostrar información en formato texto. Posee una interfaz que se controla con 8 (o 4) pines de datos y 3 de control que debemos manipular para mostrar lo buscado en la pantalla. Para esto, hacemos uso de una librería “lcd” provista por la cátedra.

Internamente se encuentra compuesto por una DDRAM (Data Display RAM), que no es más que una memoria RAM de 128 bytes que contiene los datos a mostrar en el LCD. A su vez también contiene una CGRAM (Character Generator RAM), esta de 64 bytes, contiene los ocho caracteres generados por el usuario. El dispositivo cuenta con un cursor que apunta a las direcciones de las memorias anteriormente mencionadas. 

Por último, cuenta con un registro de datos de 8 bits, para escribir datos y uno de comandos para controlar la pantalla mediante comandos.

Al escribir un byte de datos en el registro de datos, los mismos se escribirán donde apunta actualmente el cursor, y este se incrementará de forma predeterminada. A su vez, si seguimos enviando códigos de comandos al registro de comando, podemos realizar diferentes funciones preestablecidas como por ejemplo limpiar la pantalla.

Utilizamos la configuración de 4 pines para optimizar el uso de los puertos de entrada y salida. Conectamos los pines del LCD D7, D6, D5 y D4 a los pines del ATmega328P, B1, B2, C2 y C1 respectivamente. Los pines de control los conectamos al puerto D0 Y D1. Luego, indicamos en las constantes de la librería los puertos, pines y modo elegidos.

### Parte 3.2: El teclado matricial

El teclado matricial es un dispositivo que cuenta con varios pulsadores agrupados en filas y columnas formando una matriz. Usaremos uno de 4x4, que contará con 16 teclas de las cuales nos interesa especialmente la última columna, es decir, las letras ‘A’, ‘B’, ‘C, y ‘D’, que son las necesarias para interactuar con nuestro sistema.

![alt_text](images/image3.png "image_tooltip")

Sin embargo, el teclado cuenta con problemáticas que debemos tener en cuenta, que no son ni más ni menos las que ya afrontamos con el uso de pulsadores. Se debe determinar qué hacer ante situaciones como cuando varias teclas son presionadas al mismo tiempo, o que se mantengan presionadas por un tiempo, además de tener en cuenta el efecto rebote que produce cualquier pulsador mecánico.

La lectura de las entradas se hace con un período de 50ms que coincide con la actualización periódica de las máquinas de estado. Se resuelve hacer el antirrebote entre actualización y actualización, es decir, sin hacer uso de retardos bloqueantes. Aún teniendo en cuenta que se leerá como válida una entrada después de 100 ms, conseguimos un buen tiempo de respuesta para el usuario al tratarse de un pulsador.

En un módulo keypad.c se encuentran las funciones asociadas al procedimiento mencionado. Una para detectar si una tecla está siendo presionada y otra que permite solucionar el anti-rebote y la detección múltiple mediante un sistema de verificaciones.

En nuestro caso, las filas del teclado se encuentran conectadas a los pines PB4, PB3, PB0 y PD7 respectivamente. Las columnas corresponden a los pines PD3, PD5, PD4 y PD2, como se observa en la siguiente figura.

![alt_text](images/image4.png "image_tooltip")

### Parte 4: Organización del código

Dividimos el programa en varios módulos para ayudar a la independización de tareas, modularización, mantenimiento y legibilidad de la solución.
* **CLOCK.C y CLOCK.H:** contienen la funcionalidad asociada a la fecha y hora del sistema. Encapsula éstas y expone métodos para actualizar, obtener y modificarlas.
![alt_text](images/image5.png "image_tooltip")
![alt_text](images/image6.png "image_tooltip")
* **KEYPAD.C y KEYPAD.H:** contiene las funciones para leer del teclado matricial y configura los puertos a ser utilizados por el mismo. Se tiene especial cuidado a la hora de configurar los puertos para no interferir con el LCD.
![alt_text](images/image7.png "image_tooltip")
![alt_text](images/image8.png "image_tooltip")
![alt_text](images/image9.png "image_tooltip")
![alt_text](images/image10.png "image_tooltip")
* **LCD.C y LCD.H:** provisto por la cátedra para el uso de la pantalla para la interfaz de usuario.
* **SEOS.C y SEOS.H:** el sistema operativo que se encarga de planificar y ejecutar las tareas, previamente mostradas en la parte 2.2
* **MEF_GENERAL.C y MEF_GENERAL.H:** módulo que contiene la máquina de estados para el funcionamiento del sistema y el 80% de la lógica del programa.

## VALIDACIÓN

Se valida la implementación de la solución con el simulador Proteus. A continuación, se provee un link a un video donde se han probado todos los requerimientos del sistema (en este video se utilizó una frecuencia de parpadeo de pantalla más rápida cuando se edita un campo para hacerlo más visual, en la implementación en el código que se encuentra arriba esto está configurado como se pedía en el TP, es decir, 1 vez por segundo):
[TP2 - Proteus 8 Professional - Schematic Capture 2022-05-22 21-51-58.mp4](https://drive.google.com/file/d/1Tldi3E_NV67bJApWO6i1OCMlFutTaOIC/view?usp=sharing)

Se configura el microcontrolador con un clock externo de 16 MHz, que es la frecuencia definida en el código.
![alt_text](images/image11.png "image_tooltip")

En la siguiente imagen se puede apreciar el sistema en funcionamiento:
![alt_text](images/image12.png "image_tooltip")
