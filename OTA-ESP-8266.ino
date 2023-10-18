# include "espwifieepromotafirebase.h"

BearSSL::CertStore certStore;
#include <time.h>
 
//const String FirmwareVer={"1.7"}; 
//#define URL_fw_Version "https://raw.githubusercontent.com/nguyenvanthang2002/esp-12f-update-4in-from-nvt/master/bin_version.txt"
#define URL_fw_Bin "https://raw.githubusercontent.com/nguyenvanthang2002/esp-12f-update-4in-from-nvt/master/onlineota.bin"
const char* host = "raw.githubusercontent.com";
const int httpsPort = 443;

// DigiCert High Assurance EV Root CA
const char trustRoot[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDrzCCApegAwIBAgIQCDvgVpBCRrGhdWrJWZHHSjANBgkqhkiG9w0BAQUFADBh
MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3
d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD
QTAeFw0wNjExMTAwMDAwMDBaFw0zMTExMTAwMDAwMDBaMGExCzAJBgNVBAYTAlVT
MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j
b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IENBMIIBIjANBgkqhkiG
9w0BAQEFAAOCAQ8AMIIBCgKCAQEA4jvhEXLeqKTTo1eqUKKPC3eQyaKl7hLOllsB
CSDMAZOnTjC3U/dDxGkAV53ijSLdhwZAAIEJzs4bg7/fzTtxRuLWZscFs3YnFo97
nh6Vfe63SKMI2tavegw5BmV/Sl0fvBf4q77uKNd0f3p4mVmFaG5cIzJLv07A6Fpt
43C/dxC//AH2hdmoRBBYMql1GNXRor5H4idq9Joz+EkIYIvUX7Q6hL+hqkpMfT7P
T19sdl6gSzeRntwi5m3OFBqOasv+zbMUZBfHWymeMr/y7vrTC0LUq7dBMtoM1O/4
gdW7jVg/tRvoSSiicNoxBN33shbyTApOB6jtSj1etX+jkMOvJwIDAQABo2MwYTAO
BgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUA95QNVbR
TLtm8KPiGxvDl7I90VUwHwYDVR0jBBgwFoAUA95QNVbRTLtm8KPiGxvDl7I90VUw
DQYJKoZIhvcNAQEFBQADggEBAMucN6pIExIK+t1EnE9SsPTfrgT1eXkIoyQY/Esr
hMAtudXH/vTBH1jLuG2cenTnmCmrEbXjcKChzUyImZOMkXDiqw8cvpOp/2PV5Adg
06O/nVsJ8dWO41P0jmP6P6fbtGbfYmbW0W5BjfIttep3Sp+dWOIrWcBAI+0tKIJF
PnlUkiaY4IBIqDfv8NZ5YBberOgOzW6sRBc4L0na4UU+Krk2U886UAb3LujEV0ls
YSEY1QSteDwsOoBrp+uvFRTp2InBuThs4pFsiv9kuXclVzDAGySj4dzp30d8tbQk
CAUw7C29C79Fv1C5qfPrmAESrciIxpg0X40KPMbp1ZWVbd4=
-----END CERTIFICATE-----
)EOF";
X509List cert(trustRoot);


extern const unsigned char caCert[] PROGMEM;
extern const unsigned int caCertLen;

const char* ssid = "Live4";
const char* password = "thang1234";

///
int deBug = 1;
bool Status = false;
bool isWifiConnected = false;
int loopCount = 0;
String timeCurent;

//

myClass myObject(1);
FirebaseData firebaseData;
FirebaseData stream;
WiFiClient client;
FirebaseData updateStream;
FirebaseData ledStateStream;

//

String macAddress =  WiFi.macAddress();
String apSuffix = macAddress.substring(12, 17); 
String apSSID = "ESPlocal:"+ apSuffix;
const char* apSSIDChar = apSSID.c_str();
const char* apPassword = "12345678"; // Mật khẩu của mạng Wi-Fi điểm truy cập

const char* defaultSSID = "Live4"; // Tên mạng Wi-Fi mặc định
const char* defaultPassword = "thang1234"; // Mật khẩu của mạng Wiy-Fi mặc định 0903878959


#define FIREBASE_HOST "https://esp12f-on-off-4in-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define FIREBASE_AUTH "xHuRm1Bf3Fu2ldEi9fwBjeHPUEnc9EtMNdRyKVDD"

//

ESP8266WebServer server(80);

const int maxWiFiCredentials = 6; // Số lượng tối đa của mật khẩu Wi-Fi
WiFiCredentials wifiCredentials[maxWiFiCredentials]; // Danh sách mật khẩu Wi-Fi
int currentWiFiCredential ; // Chỉ số của mật khẩu Wi-Fi hiện tại

///
/////////////////////////////////////
void handleRoot() {
  String requestData = server.arg("plain"); // Lấy dữ liệu gửi từ ứng dụng Android
  String part1;
  String part2;
  String wifiSSID;
  String wifiPassword;
  
  int delimiterIndex = requestData.indexOf("&");
  if (delimiterIndex != -1) {
    part1 = requestData.substring(0, delimiterIndex); // Lấy phần đầu tiên là SSID của Wi-Fi
    part2 = requestData.substring(delimiterIndex + 1); // Lấy phần thứ hai là mật khẩu của Wi-Fi
    if (part1.equals("time")) {
        timeCurent = part2 ; // Gán part2 bằng giá trị currentTime
        myObject.PusleSound(3, 1000);
        myObject.PusleSound(2, 2000);
        myObject.PusleSound(1, 3000);
        if (deBug == 1)Serial.print("thời gian hiện tại: ");
         Serial.println(timeCurent);
         //////////////////////////
        int day, month, year, hour, minute, second;
        sscanf(timeCurent.c_str(), "%d:%d:%d:%d:%d:%d", &day, &month, &year, &hour, &minute, &second);
        int b10 = year % 100;
     
    ////////////////////////////////
    } else {
        // Gán part1 cho wifiSSID và part2 cho wifiPassword
        wifiSSID = part1;
        wifiPassword = part2;
        myObject.PusleSound(3, 1000);
        myObject.PusleSound(2, 2000);
        myObject.PusleSound(1, 3000);
    

   // Lưu thông tin SSID và mật khẩu vào EEPROM
    WiFiCredentials credentials;
    strncpy(credentials.ssid, wifiSSID.c_str(), sizeof(credentials.ssid));
    strncpy(credentials.password, wifiPassword.c_str(), sizeof(credentials.password));
    myObject.saveWiFiCredentials(credentials, currentWiFiCredential);

    // Chuyển sang mật khẩu Wi-Fi mới trong danh sách
    if (deBug == 1)Serial.print("Saved Wi-Fi credentials - SSID: ");
    if (deBug == 1)Serial.println(credentials.ssid);
    if (deBug == 1)Serial.print("Saved Wi-Fi credentials - Password: ");
    if (deBug == 1)Serial.println(credentials.password);

    currentWiFiCredential++;
    if (currentWiFiCredential >= maxWiFiCredentials) {
      currentWiFiCredential = 0; // Quay lại mật khẩu đầu tiên nếu đã hết danh sách
    }

    WiFi.begin(wifiSSID.c_str(), wifiPassword.c_str());
    unsigned long startTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startTime < 7000) {
      delay(500);
      static bool ledState = LOW;
      digitalWrite(13, ledState);
      ledState = (ledState == LOW) ? HIGH : LOW;
      if (deBug == 1)Serial.println("Connecting to Wi-Fi...");
    }
    
    if (WiFi.status() == WL_CONNECTED) {
      loopCount = 1;
      if (deBug == 1)Serial.println("Connected to Wi-Fi");
      isWifiConnected = true;
      myObject.PusleSound(10, 2000);
    
      // WiFi.softAPdisconnect();
     if (deBug == 1) Serial.println("Disabled AP mode");
    } else {
      WiFi.mode(WIFI_AP);
      WiFi.softAP(apSSID, apPassword);
      isWifiConnected = false;
     if (deBug == 1) Serial.print("Địa chỉ IP truy cập của ESP8266: ");
     if (deBug == 1) Serial.println(WiFi.softAPIP());
        myObject.PusleSound(1, 3000);
        myObject.PusleSound(2, 2000);
        myObject.PusleSound(3, 1000);
    }
   if (deBug == 1) Serial.print("Received data: ");
 if (deBug == 1) Serial.println(requestData);

  }}
  
}
void handleMacAddress() {
  String macAddress = WiFi.macAddress();
  server.send(200, "text/plain", macAddress);
}
void connectToWiFi() {
  for (int i = 0; i < maxWiFiCredentials && loopCount < 1; i++) {
  WiFiCredentials credentials = myObject.readWiFiCredentials(i);
  if (deBug == 1)Serial.println(currentWiFiCredential);
  if (deBug == 1)Serial.print("WiFi ");
  if (deBug == 1)Serial.print(i + 1);
  if (deBug == 1)Serial.print(" - SSID: ");
  if (deBug == 1)Serial.print(credentials.ssid);
if (deBug == 1)Serial.print(", Password: ");
  if (deBug == 1)Serial.println(credentials.password);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(apSSID, apPassword);
  WiFi.begin(credentials.ssid, credentials.password);
  unsigned long connectionStartTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - connectionStartTime < 8000) {
    for (int i =0 ; i<222220;i++){
      server.handleClient();
    }
    if (deBug == 1)Serial.println("Connecting to Wi-Fi...");
    server.handleClient();
     static bool ledState = LOW;
digitalWrite(13, ledState);
ledState = (ledState == LOW) ? HIGH : LOW;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
  
    myObject.PusleSound(10, 2000);
   if (deBug == 1) Serial.print("Switched to Wi-Fi: ");
   if (deBug == 1) Serial.println(credentials.ssid);
   if (deBug == 1) Serial.println("Connected to Wi-Fi");
    loopCount++;  // Tăng giá trị của loopCount để thoát khỏi vòng lặp
    break;
  } else {
    if (deBug == 1)Serial.println("Failed to connect to Wi-Fi");
  }
}
}
//////////////////////////////////////////////////////////
void setClock() {
   // Set time via NTP, as required for x.509 validation
  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");
  Serial.print("Waiting for NTP time sync: ");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(500);
    Serial.println(".");
    now = time(nullptr);
  }
  /*
  Serial.println("");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: ");
  Serial.print(asctime(&timeinfo));
  */
}
  
void FirmwareUpdate()
{  
  WiFiClientSecure client;
  client.setTrustAnchors(&cert);
  if (!client.connect(host, httpsPort)) {
    Serial.println("Connection failed");
    return;
  }
   Serial.println("New firmware detected");
    ESPhttpUpdate.setLedPin(LED_BUILTIN, LOW); 
    t_httpUpdate_return ret = ESPhttpUpdate.update(client, URL_fw_Bin);
        
    switch (ret) {
      case HTTP_UPDATE_FAILED:
        Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
        break;

      case HTTP_UPDATE_NO_UPDATES:
        Serial.println("HTTP_UPDATE_NO_UPDATES");
        break;

      case HTTP_UPDATE_OK:
        Serial.println("HTTP_UPDATE_OK");
        break;
    } 
  }  
void connect_wifi();
unsigned long previousMillis_2 = 0;
unsigned long previousMillis = 0;        // will store last time LED was updated
const long interval = 60000;
const long mini_interval = 1000;
 void repeatedCall(){
    unsigned long currentMillis = millis();
    if ((currentMillis - previousMillis) >= interval) 
    {
      // save the last time you blinked the LED
      previousMillis = currentMillis;
      setClock();
      FirmwareUpdate();
    }

    if ((currentMillis - previousMillis_2) >= mini_interval) {
      static int idle_counter=0;
      previousMillis_2 = currentMillis;    
      Serial.print(" Active fw version:");
 //     Serial.println(FirmwareVer);
      Serial.print("Idle Loop....");
      Serial.println(idle_counter++);
     if(idle_counter%2==0)
      digitalWrite(LED_BUILTIN, HIGH);
     else 
      digitalWrite(LED_BUILTIN, LOW);
     if(WiFi.status() == !WL_CONNECTED) 
          connect_wifi();
   }
 }

  
void setup()
{
  Serial.begin(115200);
  Serial.println("");
  Serial.println("Start");
  WiFi.mode(WIFI_STA);
  
  connect_wifi();  

  pinMode(LED_BUILTIN, OUTPUT);
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  pinMode(bSound, OUTPUT);
  pinMode(in1, INPUT);
  pinMode(in2, INPUT);
  pinMode(in3, INPUT);
  pinMode(in4, INPUT);
  pinMode(13, OUTPUT);
 // pinMode(RESET_PIN, OUTPUT);
  //digitalWrite(RESET_PIN, HIGH);
  digitalWrite(13, LOW); 
  currentWiFiCredential = myObject.readCurrentWiFiCredential();

   // WiFi.mode(WIFI_AP);
   // WiFi.softAP(apSSID, apPassword);
    WiFi.mode(WIFI_STA);
    //delay(5000);
    setClock();
  //FirmwareUpdate();
   
  server.on("/", handleRoot);
  server.on("/mac", handleMacAddress);
  server.begin();
  
}
void loop()
{
  WiFi.mode(WIFI_AP);
  WiFi.softAP(apSSID, apPassword);
  //WiFi.softAPdisconnect(true);
  //WiFi.mode(WIFI_STA);
  //delay(3000);
  //setClock();
  //FirmwareUpdate();
      //delay(5000);digitalWrite(RESET_PIN, LOW);
      //Serial.print("OKOK");
     // ESP.restart();


    // không thể cập  đc nếu nhật đc nếu mà bật WiFi.mode(WIFI_AP); trc khi update và gọi hàm set clock trc.
}

void connect_wifi()
{
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print("O");
  }                                   
  Serial.println("Connected to WiFi");
}