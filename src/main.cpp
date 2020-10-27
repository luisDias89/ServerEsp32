#include <Arduino.h>      //Biblioteca do arduino

#include <WiFi.h>         // Biblioteca Wi-Fi

// Conecção à Rede de Casa, Inserir o username e Password Correto
const char* ssid = "MEO-580965";
const char* password = "986B27FF9F";

// O servidor Web está à escuta na porta 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String saida16estado = "off";
String saida14estado = "off";

// Configuração das portas
const int output16 = 16;
const int output14 = 14;

// Tempo agora
unsigned long currentTime = millis();
// Tempo antes
unsigned long previousTime = 0;
// Define o timeout (exemplo: 2000ms = 2s)
const long timeoutTime = 2000;

void setup() {
  Serial.begin(115200);
  // Initialize the output variables as outputs
  pinMode(output16, OUTPUT);
  pinMode(output14, OUTPUT);
  // Set outputs to LOW
  digitalWrite(output16, LOW);
  digitalWrite(output14, LOW);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Logo após a conexão
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            // Colocar as GPIO ON ou OFF
            if (header.indexOf("GET /16/on") >= 0) {
              Serial.println("Saida 16 ligada");
              saida16estado = "on";
              digitalWrite(output16, HIGH);

            } else if (header.indexOf("GET /16/off") >= 0) {
              Serial.println("Saida 16 desligada");
              saida16estado = "off";
              digitalWrite(output16, LOW);

            } else if (header.indexOf("GET /14/on") >= 0) {
              Serial.println("Saida 14 ligada");
              saida14estado = "on";
              digitalWrite(output14, HIGH);
            } else if (header.indexOf("GET /14/off") >= 0) {
              Serial.println("Saida 14 desligada");
              saida14estado = "off";
              digitalWrite(output14, LOW);
            }

            // Ficheiro HTML
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS  on/off buttons 
            // Background
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");
            
           
            client.println("<body><h1>Servidor ESP32, Luis Dias</h1>");
            
            
            client.println("<p>GPIO 16 - Estado " + saida16estado + "</p>");
            
            
            if (saida16estado=="off") {
              client.println("<p><a href=\"/16/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/16/off\"><button class=\"button button2\">OFF</button></a></p>");
            }



            client.println("<p>GPIO 14 - Estado " + saida14estado + "</p>");
            // Se a saida 14 esta off, vais colocar o botão a off
            if (saida14estado=="off") {
              client.println("<p><a href=\"/14/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/14/off\"><button class=\"button button2\">OFF</button></a></p>");
            }
            client.println("</body></html>");

            // o Fim do HTML é quando imprimo uma linha em branco
            client.println();
            // saida do loop
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {  
          currentLine += c;      
        }
      }
    }
   
   
    header = "";
    // Termina a conexão
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}