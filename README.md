 # Proyecto: Monitor de Oxigenación y Ritmo Cardíaco con ESP32

Este proyecto consiste en la implementación de un sistema para medir la oxigenación en la sangre (SpO2) y el ritmo cardíaco utilizando una **ESP32**, un sensor **MAX30102** y una pantalla **OLED** de 1.3 pulgadas. El código que se carga en la ESP32 se encuentra en el archivo `esp32.ino`.

## Descripción General

El sistema desarrollado permite la medición de valores de IR (Infrarrojo), Red y SpO2 (Saturación de Oxígeno) a través del sensor MAX30102. Los valores medidos se muestran en una pantalla OLED y, si el dispositivo está conectado a una red WiFi, se envían a una base de datos para su almacenamiento y posterior análisis. Además, se tiene la opción de cambiar entre el sensor MAX30102 y un sensor analógico utilizando un botón físico.

El sistema también cuenta con un botón para activar o desactivar la conexión WiFi, permitiendo su funcionamiento tanto en modo conectado como desconectado.

## Requisitos de Software

Para desarrollar y ejecutar este proyecto, se necesitan los siguientes programas:

- **XAMPP**: para configurar un servidor local y alojar el archivo `post2.php`, que maneja la conexión y el envío de datos a la base de datos.  
  [Descargar XAMPP](https://sourceforge.net/projects/xampp/files/XAMPP%20Windows/8.2.12/xampp-windows-x64-8.2.12-0-VS16-installer.exe/download)

- **Composer**: para gestionar dependencias PHP necesarias en el entorno de XAMPP.  
  [Descargar Composer](https://getcomposer.org/Composer-Setup.exe)

- **Arduino IDE**: para escribir y cargar el código en la ESP32.  
  [Descargar Arduino IDE](https://www.arduino.cc/en/software)

- **Grafana**: para visualizar los datos en tiempo real o almacenados en la base de datos.  
  [Descargar Grafana](https://grafana.com/grafana/download?pg=get&plcmt=selfmanaged-box1-cta1)

## Conexión del Hardware

- **ESP32**: Microcontrolador que ejecuta el código y se comunica con los sensores y la pantalla OLED.
- **MAX30102**: Sensor utilizado para medir los niveles de SpO2 y el ritmo cardíaco.
- **OLED de 1.3 pulgadas**: Pantalla que muestra los valores medidos por los sensores en tiempo real.

## Conexiones de Hardware

1. **Conexión MAX30105 con ESP32**:
   - **SCL** del MAX30105 a **GPIO22** del ESP32
   - **SDA** del MAX30105 a **GPIO21** del ESP32
   - **VCC** del MAX30105 a **3.3V/5V** del ESP32
   - **GND** del MAX30105 a **GND** del ESP32

2. **Conexión Pantalla OLED con ESP32**:
   - **SCL** de la pantalla OLED a **GPIO22** del ESP32
   - **SDA** de la pantalla OLED a **GPIO21** del ESP32
   - **VCC** de la pantalla OLED a **3.3V/5V** del ESP32
   - **GND** de la pantalla OLED a **GND** del ESP32

## Archivos del Proyecto

- `esp32.ino`: Archivo que contiene el código principal a cargar en la ESP32. Incluye la lógica para leer los datos del sensor, calcular el valor de SpO2 utilizando la fórmula proporcionada, mostrar los valores en la pantalla OLED, y enviar los datos (IR, Red y SpO2) al servidor a través de WiFi.

- `post2.php`: Archivo que se encuentra en la carpeta de XAMPP y es responsable de manejar la conexión con la base de datos y el envío de datos desde la ESP32 a la tabla `datos_oxi` en la base de datos `tesis`.

- `base_de_datos.sql`: Contiene el código SQL necesario para crear la base de datos y la tabla en la que se almacenarán los datos medidos por la ESP32.

## Configuración del Proyecto

1. **Preparar el entorno de desarrollo**:
   - Instalar XAMPP y Composer.
   - Instalar Arduino IDE y Grafana.

2. **Configurar la base de datos**:
   - Ejecutar el archivo `base_de_datos.sql` en el servidor MySQL proporcionado por XAMPP para crear la base de datos `tesis` y la tabla `datos_oxi`.

3. **Cargar el código en la ESP32**:
   - Abrir el archivo `esp32.ino` en el Arduino IDE.
   - Configurar los valores de SSID y contraseña de tu red WiFi.
   - Cargar el código en la ESP32.

4. **Ejecutar el servidor**:
   - Colocar el archivo `post2.php` en la carpeta `htdocs` de XAMPP.
   - Iniciar el servidor Apache y MySQL desde el panel de control de XAMPP.

5. **Visualización de los datos**:
   - Usar Grafana para conectarse a la base de datos y visualizar los datos de oxigenación y ritmo cardíaco en tiempo real.

---

**Ariel Moreta - Adrian Vera**
