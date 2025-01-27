#include <WiFi.h>
#include <WiFiUdp.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define SCREEN_ADDRESS 0x3c
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// WiFi Configuration
const char* ssid = "Skynet";
const char* password = "L19S19d85p87";
IPAddress serverIP(192, 168, 1, 51);  // Dirección IP de tu servidor SimHub
WiFiUDP udp;
unsigned int localUdpPort = 8888;  // Puerto para escuchar mensajes de SimHub
char incomingPacket[255];
bool simhubConnected = false;

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);

  // Inicializar pantalla OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("SSD1306 initialization failed!");
    for (;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.display();

  // Configuración WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  display.print("Connecting to WiFi...");
  display.display();

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("Connected to WiFi");
  display.clearDisplay();
  display.print("WiFi Connected");
  display.display();

  // Iniciar UDP
  udp.begin(localUdpPort);
  Serial.printf("Now listening on UDP port %d\n", localUdpPort);
  display.setCursor(0, 20);
  display.print("Listening UDP:");
  display.setCursor(0, 40);
  display.print(localUdpPort);
  display.display();
}

void loop() {
  receiveSimHubData();
}

void receiveSimHubData() {
  int packetSize = udp.parsePacket();
  if (packetSize) {
    // Recibir paquete UDP
    int len = udp.read(incomingPacket, 255);
    if (len > 0) {
      incomingPacket[len] = 0;  // Asegúrate de terminar la cadena con null
    }
    String simhubInput = String(incomingPacket);
    Serial.printf("Received: %s\n", simhubInput.c_str());

    // Manejar datos de SimHub
    if (simhubInput == "hi") {
      simhubConnected = true;
      display.clearDisplay();
      display.setCursor(0, 30);
      display.print("Connected to SimHub");
      display.display();
    } else if (simhubInput == "bye") {
      simhubConnected = false;
      display.clearDisplay();
      display.setCursor(0, 50);
      display.print("Disconnected.");
      display.display();
    } else if (simhubConnected) {
      // Buscar datos de GameData.gear
      if (simhubInput.startsWith("GameData.gear:")) {
        String gearValue = simhubInput.substring(simhubInput.indexOf(':') + 1);
        display.clearDisplay();
        display.setCursor(0, 50);
        display.print("Gear: ");
        display.print(gearValue);
        display.display();
      }
    }
  }
}
