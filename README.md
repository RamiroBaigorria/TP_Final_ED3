AUTO ESQUIVA OBJETOS

Descripción:
    El proyecto presentado se trata de un prototipo de auto el cual tendrá la
autonomía de desplazarse por sí solo en línea recta y, en caso de detectar un
obstáculo en el camino, poder cambiar su sentido y redirigirse hacia otra dirección
donde pueda seguir avanzando.

Funcionamiento:
    Al inicializarlo, el LPC1769 recibirá por consola a través del UART un valor
correspondiente a la velocidad a la que se quiere que se mueva el auto. Una vez
hecho esto, el micro transformará dicho valor en su equivalente en tensión y lo
enviará a los motores (DC o paso a paso [en decisión]) a través del DAC.
Una vez en movimiento, mediante un sensor infrarrojo y el ADC, se tomará
un muestreo cada cierto tiempo (definido por un TIMER) de la distancia que existe
entre el auto y el obstáculo que tenga en frente, y una vez que esta medición se
encuentre dentro de un rango limite (el auto se encuentra muy próximo al objeto en
cuestión), el programa hará girar los motores conectados a las ruedas para cambiar
la dirección del auto (el sentido y el ángulo de giro serán determinados en la
programación). Una vez se detecte un camino libre de obstáculos, el auto iniciará
nuevamente su recorrido hasta encontrarse con otro obstáculo.
Para la implementación del DMA, una vez tomada la muestra de la distancia
con el ADC, el GPDMA transferirá estos datos a otra parte de la memoria para
analizar si la medición supera o no el límite mínimo requerido y luego tomar las
decisiones necesarias.

Ideas de implementación: (a consultar con el profe)
• A través de la consola, poder cambiar la dirección del auto, detenerlo,
reanudar su funcionamiento o modificar la velocidad a la q avance.

Componentes:
• Sensor infrarrojo SHARP (2Y0A21-F-8X) [x1]
• Motor paso a paso [x2] (opción 1)
• Motor DC [x2] (opción 2)
• Componentes electrónicos generales (cables, protoboard, etc.)
• Carcasa + ruedas

Integrantes:
• Cravero Lorenzo Agustin
• Gonzalez Tobias Simon
• Baigorria Ramiro Javier