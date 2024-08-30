#include <Wire.h>
#include <MAX30105.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <WiFi.h>
#include <HTTPClient.h>

// Configuración de la pantalla OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_I2C_ADDRESS 0x3C  // Dirección I2C del OLED
#define MODE_BUTTON_PIN 14  // Botón para activar/desactivar WiFi

const char* ssid = "NEWTON";
const char* password = "M@ntenimiento1";

Adafruit_SH1106G display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

MAX30105 particleSensor;

// Tamaño de la gráfica
#define GRAPH_WIDTH (SCREEN_WIDTH / 2 - 2)  // La gráfica ocupa la mitad izquierda
#define GRAPH_HEIGHT (SCREEN_HEIGHT - 2)
#define DATA_POINTS 64  // Número de puntos en la gráfica

int irData[DATA_POINTS];
int dataIndex = 0;
bool useWiFi = false;  // Variable para controlar el uso de WiFi
int modeButtonState = 0;
int lastModeButtonState = 0;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;
unsigned long buttonPressTime = 0;
unsigned long longPressDuration = 1000;
unsigned long anteriorMillis = 0;

void setupWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.println();
  Serial.println("-------------");
  Serial.print("Connecting");

  int connecting_process_timed_out = 400;
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    if (connecting_process_timed_out > 0) connecting_process_timed_out--;
    if (connecting_process_timed_out == 0) {
      delay(1000);
      ESP.restart();
    }
    delay(100);
  }

  Serial.println();
  Serial.print("Successfully connected to : ");
  Serial.println(ssid);
  Serial.println("-------------");
  delay(2000);
}

float calculateSpO2(int irValue, int redValue) {
  if (irValue < 2000) return 0;  // Si el valor de IR es menor que 1000, SpO2 = 0
  float ratio = (float)redValue / (float)irValue;
  float SpO2 = 110.0 - 12.0 * ratio;
  if (SpO2 > 100) SpO2 = 100;
  if (SpO2 < 0) SpO2 = 0;
  return SpO2;
}

void setup() {
  Serial.begin(115200);
  pinMode(MODE_BUTTON_PIN, INPUT_PULLDOWN);
  delay(100);

  if (!display.begin(OLED_I2C_ADDRESS, OLED_RESET)) {
    Serial.println(F("Fallo al inicializar la pantalla SH1106"));
    while (1);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);
  display.println(F("Inicializando..."));
  display.display();
  delay(2000);

  if (!particleSensor.begin()) {
    Serial.println("No se encontró el sensor MAX30102. Asegúrate de que esté conectado.");
    while (1);
  }

  particleSensor.setup();  // Configuración básica del sensor
  particleSensor.setPulseAmplitudeRed(0x0A);  // Ajusta la amplitud de la señal roja
  particleSensor.setPulseAmplitudeIR(0x0A);   // Ajusta la amplitud de la señal IR

  for (int i = 0; i < DATA_POINTS; i++) {
    irData[i] = 0;
  }

  // Verifica el estado inicial del botón
  int initialModeButtonState = digitalRead(MODE_BUTTON_PIN);
  if (initialModeButtonState == HIGH) {
    useWiFi = true;
    display.clearDisplay();
    display.setCursor(0, 30);
    display.print("Ejecutando modo");
    display.setCursor(0, 40);
    display.print("conexion...");
    display.display();
    delay(3000);
    setupWiFi();
  } else {
    useWiFi = false;
    display.clearDisplay();
    display.setCursor(0, 30);
    display.print("Ejecutando modo");
    display.setCursor(0, 40);
    display.print("sin conexion...");
    display.display();
    delay(3000);
  }
}

void loop() {
  int modeReading = digitalRead(MODE_BUTTON_PIN);

  if (modeReading != lastModeButtonState) {
    lastDebounceTime = millis();
    if (modeReading == HIGH) {
      buttonPressTime = millis();
    }
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (modeReading != modeButtonState) {
      modeButtonState = modeReading;
      if (modeButtonState == HIGH && (millis() - buttonPressTime) > longPressDuration) {
        useWiFi = !useWiFi;
        display.clearDisplay();
        display.setCursor(0, 30);
        display.print("WiFi ");
        if (useWiFi) {
          display.print("Activado");
          setupWiFi();
        } else {
          display.print("Desactivado");
          WiFi.disconnect();
        }
        display.display();
        delay(2000);
      }
    }
  }

  lastModeButtonState = modeReading;

  long irValue = particleSensor.getIR();  // Lee el valor IR
  long redValue = particleSensor.getRed();  // Lee el valor de la luz roja

  // Calcula el SpO2
  float SpO2 = calculateSpO2(irValue, redValue);

  // Mueve los datos hacia la izquierda
  for (int i = 1; i < DATA_POINTS; i++) {
    irData[i - 1] = irData[i];
  }

  // Añade el nuevo dato al final
  irData[DATA_POINTS - 1] = map(irValue, 0, 100000, 0, GRAPH_HEIGHT);  // Ajusta el rango de valores según sea necesario

  // Envia los datos al Serial Plotter
  Serial.print("IR:");
  Serial.println(irValue);
  Serial.print(",");
  Serial.print("Red:");
  Serial.println(redValue);
  Serial.print(",");
  Serial.print("SpO2:");
  Serial.print(SpO2);

  // Muestra los datos en la pantalla OLED
  display.clearDisplay();  // Limpia la pantalla

  // Dibuja la gráfica IR en la mitad izquierda de la pantalla
  for (int i = 1; i < DATA_POINTS; i++) {
    display.drawLine(i - 1, SCREEN_HEIGHT - irData[i - 1] - 1, i, SCREEN_HEIGHT - irData[i] - 1, SH110X_WHITE);
  }

  // Imprime los valores IR, Red y SpO2 en la mitad derecha de la pantalla
  display.setCursor(GRAPH_WIDTH + 2, 0);
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.print(F("IR: "));
  display.println(irValue);
  display.print(F("Red: "));
  display.println(redValue);
  display.print(F("SpO2: "));
  display.println(SpO2);

  display.display();  // Actualiza la pantalla

  if (useWiFi && WiFi.status() == WL_CONNECTED) {
    if (irValue >= 2000) {  // Sólo envía datos si el valor IR es mayor o igual a 1000
      HTTPClient http;
      String postData = "id=esp32_01&ir=" + String(irValue) + "&red=" + String(redValue) + "&spo2=" + String(SpO2);

      Serial.print("postData : ");
      Serial.println(postData);

      http.begin("http://192.168.1.153/prueba/test/post2.php");  // Cambia la URL según tu servidor
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");

      int httpCode = http.POST(postData);
      String payload = http.getString();

      Serial.print("httpCode : ");
      Serial.println(httpCode);
      Serial.print("Response Payload: ");
      Serial.println(payload);

      http.end();
    }
  }

  unsigned long tiempo = millis() - anteriorMillis;
  display.setCursor(25, 57);
  display.print(tiempo);
  anteriorMillis = millis();
  display.print(F(" ms"));

  delay(20);  // Ajusta la frecuencia de actualización según sea necesario
}
