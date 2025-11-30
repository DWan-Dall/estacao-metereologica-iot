#include <ESP8266WiFi.h> 
#include <DHT.h> 
#include <SFE_BMP180.h>
#include <Wire.h>

// ----------- WIFI -----------
#define STASSID "The Internet"
#define STAPSK "C0hUodP9Gw=="
const char* server = "api.thingspeak.com";
String apiKey = "PUS08GJEPMRB3MGB";

const char* ssid = STASSID;
const char* pass = STAPSK;

// ----------- DHT11 -----------
#define DHTPIN D4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
float u = 0.0, t = 0.0;

// ----------- MQ135 -----------
int sensorAr = A0;
float R0 = 10.55;  
float ppm = 0;
float sensor_volt, Rs, razao, ppm_log;

String classificarAr(float ppm) {
  if (ppm <= 50) return "Bom";
  if (ppm <= 100) return "Moderado";
  if (ppm <= 150) return "Insalubre (grupos sensíveis)";
  if (ppm <= 200) return "Insalubre";
  if (ppm <= 300) return "Muito insalubre";
  return "Perigoso";
}

// ----------- BMP180 -----------
SFE_BMP180 sensorP;
#define ALTITUDE 920
double tempBMP, pressao_abs, pressao_relativa;

// ----------- SENSOR DE CHUVA -----------
int sensorChuva = D5;
String chuva;

// ----------------------------------------------------

void lerDHT() {
  u = dht.readHumidity();
  t = dht.readTemperature();
}

void lerMQ135() {
  sensor_volt = analogRead(sensorAr) * (3.3 / 1023.0);

  if (sensor_volt < 0.1) {
    ppm = 0;
    return;
  }

  Rs = (3.3 - sensor_volt) / sensor_volt * 10.0;
  razao = Rs / R0;

  // Curva aproximada MQ135
  float m2 = -0.42;
  float b2 = 1.92;

  ppm_log = (log10(razao) - b2) / m2;
  ppm = pow(10, ppm_log);

  if (ppm < 0) ppm = 0;
}

void lerPressao() {
  if (sensorP.startTemperature() != 0) {
    sensorP.getTemperature(tempBMP);
    if (sensorP.startPressure(3) != 0) {
      sensorP.getPressure(pressao_abs, tempBMP);
      pressao_relativa = sensorP.sealevel(pressao_abs, ALTITUDE);
    }
  }
}

void lerChuva() {
  chuva = digitalRead(sensorChuva) ? "Não está chovendo" : "Está chovendo";
}

// ----------------------------------------------------

WiFiClient client;

void setup() {
  Serial.begin(115200);

  dht.begin();
  sensorP.begin();

  pinMode(sensorAr, INPUT);
  pinMode(sensorChuva, INPUT);

  Serial.println("Conectando ao WiFi...");
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  
  Serial.println("\nWiFi conectado.");
  Serial.println(WiFi.localIP());
}

// ----------------------------------------------------

void loop() {
  Serial.println("===== Estação Meteorológica IoT =====");

  lerDHT();
  lerMQ135();
  lerPressao();
  lerChuva();

  String qualidadeTexto = classificarAr(ppm);

  // ENVIO AO THINGSPEAK
  if (client.connect(server, 80)) {
    String postStr = apiKey;
    postStr += "&field1=" + String(u);
    postStr += "&field2=" + String(t);
    postStr += "&field3=" + String(pressao_relativa);
    postStr += "&field4=" + String(pressao_abs);
    postStr += "&field5=" + String(ppm);
    postStr += "&field6=" + String(sensorChuva);
    postStr += "\r\n\r\n";

    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: " + String(postStr.length()) + "\n\n");
    client.print(postStr);
  }

  // SERIAL MONITOR
  Serial.print("Temperatura: "); Serial.println(t);
  Serial.print("Umidade: "); Serial.println(u);
  Serial.print("Pressão abs: "); Serial.println(pressao_abs);
  Serial.print("Pressão rel: "); Serial.println(pressao_relativa);
  Serial.print("Qualidade do ar (ppm): "); Serial.println(ppm);
  Serial.print("Classificação: "); Serial.println(qualidadeTexto);
  Serial.print("Chuva: "); Serial.println(chuva);
  Serial.println("--------------------------------------");

  client.stop();
  delay(2000);
}
