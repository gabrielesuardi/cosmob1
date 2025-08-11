#include <WiFi.h>
#include <WebServer.h>

//–– CONFIGURAZIONE AP Wi-Fi
const char* ap_ssid = "ESP32_Toolkit";
const char* ap_pass = "password123";

//–– SERVER HTTP
WebServer server(80);

//–– STATO
String currentTask = "";
bool isRunning = false;

//–– HTML + CSS + JS dentro il codice
const char index_html[] PROGMEM = R"rawliteral(

)rawliteral";

//–– STOStub / placeholder
void startFlood(int count, int interval) {
  Serial.printf("[START FLOOD] count=%d interval=%dms\n", count, interval);
}
void startTcpSniff(int duration) {
  Serial.printf("[START TCP] duration=%ds\n", duration);
}
void startFakeAP(const String& ssid, int channel) {
  Serial.printf("[START FAKEAP] SSID=%s channel=%d\n", ssid.c_str(), channel);
}
void startHID(const String& seq) {
  Serial.printf("[START HID] sequence=%s\n", seq.c_str());
}
void stopAll() {
  if (isRunning) {
    Serial.printf("[STOP] Stopping %s\n", currentTask.c_str());
    isRunning = false;
    currentTask = "";
  }
}

void setup() {
  Serial.begin(115200);
  // avvia AP
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ap_ssid, ap_pass);
  Serial.printf("AP creato: %s / %s\n", ap_ssid, ap_pass);

  // route principale
  server.on("/", HTTP_GET, [](){
    server.send_P(200, "text/html", index_html);
  });

  // gestione /cmd?run=...
  server.on("/cmd", HTTP_GET, [](){
    if (!server.hasArg("run")) {
      server.send(400, "text/plain", "Parametro 'run' mancante");
      return;
    }
    String cmd = server.arg("run");
    stopAll();
    if (cmd == "flood") {
      int cnt = server.arg("count").toInt();
      int itt = server.arg("interval").toInt();
      startFlood(cnt, itt);
    }
    else if (cmd == "tcp") {
      int dur = server.arg("duration").toInt();
      startTcpSniff(dur);
    }
    else if (cmd == "ap") {
      String ss = server.arg("ssid");
      int ch = server.arg("channel").toInt();
      startFakeAP(ss, ch);
    }
    else if (cmd == "hid") {
      String sq = server.arg("seq");
      startHID(sq);
    }
    // stop non chiama nulla di extra
    currentTask = cmd;
    isRunning = (cmd != "stop");

    server.send(200, "text/plain", String("Eseguito: ") + cmd);
  });

  server.begin();
  Serial.println("Server HTTP avviato su 192.168.4.1");
}

void loop() {
  server.handleClient();
}
