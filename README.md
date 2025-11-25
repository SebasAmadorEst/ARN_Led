Avances del Proyecto
1. Construcción de la estructura base del árbol Rojo-Negro
El primer paso consistió en generar, con apoyo de herramientas de inteligencia artificial, la estructura inicial del árbol Rojo-Negro.  
Se implementó la clase principal del árbol con los componentes esenciales:

* Definición de nodos con valor, color y punteros a sus hijos y padre.
* Implementación de las rotaciones izquierda y derecha.
* Función de inserción básica.
* Mecanismo de corrección posterior a la inserción (arreglarInsercion).
* Opciones de menú:
  * Insertar valor.
  * Eliminar el árbol completo.
  * Salir del programa.

Esta primera versión trabajaba únicamente con la lógica estándar del árbol Rojo-Negro: los nodos se insertaban siempre como ROJOS, tal como lo establece la estructura de este tipo de árbol.

2. Análisis manual del comportamiento al modificar el color de inserción
Como siguiente actividad, se realizó un análisis de escritorio mediante la representación manual de diferentes escenarios de inserción.  
El objetivo era determinar qué sucedía si un nodo recién insertado se establecía inicialmente como NEGRO, en lugar de respetar la regla del árbol Rojo-Negro.  
El análisis permitió concluir que asignar un nodo nuevo como NEGRO altera la estructura y rompe las propiedades fundamentales del árbol, específicamente:

* Se producen inconsistencias que afectan el proceso de corrección posterior a la inserción.

Debido a estos resultados, se determinó que el comportamiento estándar debía conservarse.

3. Implementación del parámetro de color en la función de inserción
A partir de las conclusiones anteriores y siguiendo con los requerimientos del entregable final, se decidió introducir un nuevo parámetro en la función insertar, permitiendo que el usuario indicará un color deseado para el nuevo nodo.  
No obstante, para preservar las propiedades del árbol Rojo-Negro:

* Si el usuario solicita insertar un nodo ROJO, el proceso continúa normalmente, ya que coincide con el comportamiento estándar.
* Si el usuario solicita insertar un nodo NEGRO, el programa:
  * Muestra un mensaje advirtiendo que el color seleccionado no es válido para una inserción inicial.
  * Inserta el nodo como ROJO de todos modos.

Esta decisión mantiene la integridad del árbol y evita violaciones a las reglas establecidas.

4. Integración del árbol Rojo-Negro con hardware y representación visual mediante un LED RGB
Tras obtener un funcionamiento estable del árbol en entorno de consola, el proyecto avanzó hacia una implementación más interactiva: representar en tiempo real las operaciones del árbol mediante un LED RGB conectado a un microcontrolador.

4.1. Elección del microcontrolador: transición de ESP32 a Arduino Uno  
Inicialmente, el sistema estaba planeado para ejecutarse en una ESP32.  
No obstante, durante las pruebas en Wokwi se presentó un inconveniente:

* El monitor serial de la ESP32 no se mostraba correctamente en el simulador.

Luego de varias pruebas, se concluyó que:

* Era indispensable contar con salida por monitor serial para visualizar mensajes,

Por lo que se optó por migrar a un Arduino Uno, que sí permitió visualizar el monitor serial correctamente en Wokwi.  
Esta decisión no afectó la lógica ni las funcionalidades del proyecto.

4.2. Implementación del árbol en Arduino Uno  
Se desarrolló un nuevo código en Arduino (sketch.ino), manteniendo la estructura del árbol Rojo-Negro, pero añadiendo:

* Interfaz serial completa:
  * Menú interactivo.
  * Solicitud de valor y color.
  * Mensajes detallados de cada operación del árbol.
* Representación visual mediante LED RGB:
  * ROJO → inserciones / cambios de color.
  * VERDE → advertencia por inserción de nodo negro / cambios de color.
  * AMARILLO → rotación derecha.
  * AZUL → rotación izquierda.

Cada operación del árbol es acompañada por:

1. Un mensaje descriptivo en el monitor serial.
2. Un destello del LED RGB correspondiente al tipo de operación ejecutada.

Esto permitió convertir el algoritmo abstracto del árbol Rojo-Negro en un proceso observable y didáctico.

4.3. Integración del circuito en Wokwi  
Se elaboró un archivo (diagram.json) con la representación del circuito:

* Arduino Uno.
* LED RGB de cátodo común.
* Tres resistencias de 220 Ω.
* Conexiones debidamente configuradas en pines:
  * LED Rojo → Pin 8
  * LED Verde → Pin 9
  * LED Azul → Pin 10

El funcionamiento fue probado en Wokwi, confirmando:

* Correcto manejo del LED RGB.
* Funcionamiento estable del monitor serial.
* Lógica completa del árbol y sus mensajes.

5. Estado del proyecto  
Para este momento, el sistema cuenta con:

* Árbol Rojo-Negro completamente funcional.
* Control total de inserción, recoloración y rotaciones.
* Representación visual de eventos mediante iluminación RGB.
* Interfaz serial robusta para interacción con el usuario.
* Integración exitosa con circuito simulado en Wokwi.
* Migración de ESP32 a Arduino Uno por razones de compatibilidad de entorno.

El proyecto pasó de ser un algoritmo aislado en consola a un sistema interactivo físico-lógico, útil tanto para demostración como para aprendizaje.

6. Función para inserción con valores aleatorios  
Se agregó la opción: Iniciar inserción por secuencia aleatoria al menú interactivo.  
Permite iniciar una inserción automática de nodos con valores aleatorios entre 36 y 70 (inclusive), evitando duplicados.

* Rango: 1 a 70 (solo se insertan valores > 35)
* Sin duplicados gracias a función de búsqueda interna (Si detecta un valor duplicado, el led se encendera de color magenta)
* Inserciones cada ~900 ms para permitir observar los efectos visuales y mensajes
* Detención inmediata presionando la tecla s o S
* Regresa al menú principal al detenerse
* Compatible con todos los efectos visuales LED y mensajes paso a paso

7. Avances del punto 6.1 — Integración de sensores y nuevo sistema de inserción  
En esta etapa se amplió el sistema de inserción del árbol para permitir operaciones provenientes de sensores simulados (temperatura e iluminación) y se integró completamente con el LED RGB para distinguir cada tipo de dato.

Los principales avances fueron:

7.1. Inserción desde sensores  
Se creó la función insertarDesdeSensor(), que recibe un valor entero y un indicador del tipo de sensor:

* Temperatura → ROJO
* Iluminación → NEGRO (intento), pero se inserta como ROJO
  * Se muestra advertencia y se ilumina LED VERDE.

Durante la inserción:

* Se generan mensajes descriptivos en consola:
  * Registro del dato.
  * Tipo de sensor.
  * Color solicitado y color permitido.
* Se ejecuta una inserción estándar en el árbol Rojo-Negro.
* Se mantienen todos los efectos visuales del LED:
  * Inserción (rojo), recoloraciones, rotaciones.

7.2. Validación y control del usuario

* Validación de valores de sensor incluyendo números reales (con redondeo automático).
* Validación del tipo de sensor (0 = temperatura, 1 = iluminación).
* Prevención de duplicados con advertencia LED magenta.

7.3. Menú ampliado  
Se añadió una nueva opción en el menú serial:

* Opción 2: Insertar valor desde sensores.

Incluye:

* Lectura del valor real.
* Redondeo claro y anunciado.
* Selección del tipo de sensor.
* Mensajes informativos del proceso completo.

7.4. Integración total con el LED RGB  
Durante la inserción desde sensores:

* Temperatura → comportamiento normal (ROJO).
* Iluminación → advertencia con destello VERDE.
* El resto del proceso mantiene rotaciones y recoloraciones iluminadas como en puntos previos.

Luego se le añadieron 2 pequeñas mejoras:

1. Restricción para que no permita ingresar un valor para un nodo mayor a 3 dígitos.
2. Iluminación blanca (calibrada con PWM) en el LED si rompe la restricción anterior.

8. Implementación final: Simulación IoT autónoma con actuadores inteligentes (Opción 5 del menú)
Como culminación del proyecto, se desarrolló una simulación completa de un sistema IoT tipo invernadero inteligente, con lógica de control basada en umbrales críticos.

Esta funcionalidad se integró como la opción 5 del menú principal:  
**"Iniciar simulación IoT (sensores + actuadores)"**

8.1. Sensores simulados
* Temperatura: valores aleatorios entre 0 y 40
* Iluminación: valores aleatorios entre 0 y 50

8.2. Actuadores inteligentes con umbrales críticos
* Bomba de enfriamiento → se activa si Temperatura > 30
  * Inserta el valor como nodo ROJO
* Lámpara de crecimiento → se activa si Iluminación < 15
  * Inserta el valor como nodo NEGRO

8.3. Lógica realista y profesional
* Si ambos umbrales se superan → ambos actuadores se activan
* Si solo uno → solo ese actuador
* Si ninguno → NO se inserta nada → se muestra: [OK] Todo en rango normal. Sin acción.
