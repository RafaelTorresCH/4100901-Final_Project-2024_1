Proyecto: Advanced Turn Hazard Lights System
Descripción
Este proyecto es un sistema avanzado de luces direccionales que utiliza un teclado hexadecimal para controlar las señales de giro (izquierda, derecha y luces de parqueo) 
en un vehículo. Además, incluye conectividad a internet para la monitorización y control remoto, integrando diversos periféricos como un módulo OLED 
para mostrar mensajes y un módulo ESP01 para comunicación en red. También cuenta con la capacidad de bloqueo y desbloqueo del sistema a través de teclas específicas en el teclado.
Funcionalidades

1. Control de luces direccionales:
   - Las teclas `1`, `2`, y `3` encienden las luces direccionales (izquierda, parqueo y derecha, respectivamente).
   - La tecla `*` apaga todas las luces.

2. Sistema de bloqueo y desbloqueo:
   - Tecla `A`: Desbloquea el sistema para poder activar las luces.
   - Tecla `B`: Bloquea el sistema, desactivando todas las luces.

3. Validación de contraseña:
   - Se debe ingresar una contraseña de dos dígitos (por defecto "11") a través del teclado hexadecimal para desbloquear el sistema y acceder a las funcionalidades de control de luces.

4. Modo de baja potencia:
   - El sistema entra en modo de bajo consumo si no hay actividad durante 10 segundos, conservando energía hasta que se detecte una nueva acción.


