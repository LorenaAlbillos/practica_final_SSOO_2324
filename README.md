# **⚡ PRACTICA FINAL SISTEMAS OPERATIVOS 2023/24 ⚡**


## **🧙‍♂️ Autores**
[Marcos Fernández Martínez](https://github.com/marcosfnmr).

[Lorena Albillos Muñoz](https://github.com/LorenaAlbillos).

[Samuel Gil Lopez ](https://github.com/samuuu16).

[Pedro Blanco García ](https://github.com/pblang03).

##  **📖 Enunciado**
El objetivo de la práctica es realizar un programa que simule el sistema de fila única para el cobro a clientes en
un supermercado. La prácticA tiene una parte básica donde se define el funcionamiento básico del sistema
de fila única y que es obligatorio implementar (supondrá corno mucho el de la nota) y, además, se proponen
una serie de mejoras opcionales para aumentar la nota (como mucho supondrá el 20 % de la nota).

### **Parte Básica (80% de la nota)**
El sistema de fila única para el cobro a clientes del supermercado funciona de la siguiente forma:
+ El supermercado cuenta con 3 para atender a sus clientes. 3 clientes pueden ser atendidos a la vez, el
resto esperará formando una fila única.
+ Cuando clientes terminan sus compras se colocan en la fila única. A cada cliente se le asignará un identificador
único y secuencial (cliente—l, cliente-2, cliente_N). El número máximo de clientes que pueden ponerse a la
cola es de 20.
+ Los clientes pueden cansarse de esperar y abandonar la cola dejando sus compras.
+ El supermercado cuenta con 1 reponedor al que los cajeros pueden llamar para consultar un precio.
Cada cliente será atendido por un cajero que:
+ Tiene un identificador único (cajero_lt cajero_2 y cajero.3).
+ Solamente cuando haya terminado con un cliente pasará a atender al siguiente.
+ Cada vez que un cajero atiende a 10 clientes se toma un descanso (duerme 20 segundos), y no atiende al siguiente hasta que vuelve.
+ De los clientes atendidos el 70 % no tiene problemas. El 25 % tiene algún problema con el precio de alguno de los productos que ha comprado y es necesario que el reponedor vaya a comprobarlo, si está cliente y cajero deberán esperar. El último 5 % no puede realizar la compra por algún motivo (no tiene dinero, no funciona su tarjeta, etc.).

Toda la actividad quedará registrada en un fichero plano de texto llamado registroCaja. log:

+ Cada vez que se atiende a un cliente se registrará en el log las horas de inicio y final de la atención al mismo.
+ Se registrará el precio total de la compra.
+ Se registrará si hay algún problema con algún precio o si el cliente no finalizó la compra.
+ Se registrará la entrada y salida del descanso por parte del cajero.
+ Al finalizar el programa se debe registrar el número de clientes atendidos por cada cajero.
  
Consideraciones prácticas:

+ Simularemos la llegada de clientes a la fila única mediante la señal SIGUSRI. Se enviarán desde fuera del
programa mediante el comando kilt (kill —SIGUSR1 PID).
+ Los clientes pueden irse sin ser atendidos tras 10 segundos de espera. Se simulará esta posibilidad haciendo que el 10% lo hagan.
+ Simularemos el tiempo de atención de cada cliente con un valor aleatorio entre 1 y 5 segundos.
+ El precio total de cada compra será un número aleatorio entre 1 y 100.
+ Todas las entradas que se hagan en el log deben tener el siguiente formato: [YYYY-MM-DDHH:MI:SS] identificador: mensaje . , donde identificador puede ser el identificador del cliente, el identificador del cajero, el identificador
el reponedor o el identificador del departamento de atención al cliente y mensaje es una breve descripción del
evento ocurrido.
+ Las entradas del log deben quedar escritas en orden cronológico.
+ Hay que controlar el acceso a los recursos compartidos (al menos la cola de clientes y al fichero de log) utilizando semáforos (mutex).
+ Para comunicar a los cajeros con el reponedor, utilizaremos una variable de condición. Los cajeros señalizarán
la condición cuando necesiten que el reponedor consulte un precio. El reponedor estará a la espera de que esto ocurra.
+ Simularemos el tiempo de atención del reponedor durmiendo un valor aleatorio entre 1 y 5 segundos.

### **Partes opcionales (20% de la nota)**
+ **Asignación estática de recursos (10%):**
    + Modifica el programa para que el número de clientes que pueda acceder al la fila única sea un parámetro que reciba el programa al ser ejecutado desde la línea de comandos.
    + Modifica el programa para que el número de cajeros que atienden sea un parámetro que reciba el programa al ser ejecutado desde la línea de comandos.
+ **Asignación dinámica de recursos I (5%):**
    + Modifica el programa para que el número de clientes que puedan acceder a la fila única pueda modificarse en tiempo de ejecución.
    + olamente es necesario contemplar un incremento en el número de clientes. No es necesario contemplar la reducción.
    + Cada vez que se cambie el número de clientes tiene que reflejarse en el log.
+ **Asignación dinámica de recursos II (5%):**
    + Modifica el programa para que el número de cajeros que atiende se pueda modificar en tiempo de ejecución.
    + Solamente es necesario cxjntemplar un incremento en el número de cajertF. No es necesario contemplar la reducción.
    + Cada vez que se produce un cambio en este sentido debe quedar reflejado en el log.

> [!CAUTION]
> La fecha de entrega es el 08/01/2024.