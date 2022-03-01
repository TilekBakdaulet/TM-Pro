
#include <SPI.h>              // include libraries
#include <LoRa.h>
#include <SD.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include "EspMQTTClient.h"

#include <ThreeWire.h>   
#include <RtcDS1302.h>


//#define WIFI_SSID "Nargela bukety"
//#define WIFI_PASSWORD "7478051404"
#define WIFI_SSID "TP-Link_B4D1" // TP-Link_B4D1
#define WIFI_PASSWORD "10922002" // 10922002

//#define WIFI_SSID "VICHUNAI" //    0FF9 || 1451825048
//#define WIFI_PASSWORD "178vici2018"

// Telegram BOT Token (Get from Botfather)
#define BOT_TOKEN "5005398589:AAGXVUr1oZmdeAjj9Z6e5QL47oRXRNuKPD8" // Gelly
// "5228020768:AAHKW_o08kP1C_K3f7p2xIwk-ZY_ldCERQE" // - Вичи

EspMQTTClient client(
  //"Nargela bukety",
  //"7478051404",1451825048
 "TP-Link_B4D1",
  "10922002",
  "dev.rightech.io",
  "mqtt-j228008-99g8mi"
  //"mqtt-vvichi-lrnetx"
);

ThreeWire myWire(2,5,0); // IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);

const unsigned long BOT_MTBS = 5000;
const unsigned long TEMP_MTBS= 600000;//10 min
unsigned long bot_lasttime;
unsigned long temp_lasttime;
X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);

const int csPin = 15;          // LoRa radiochip select
const int resetPin = 10;       // LoRa radio reset
const int irqPin = 4;         // change for your board; must be a hardware interrupt pin

String outgoing;              // outgoing message
String chat_id;
byte msgCount = 0;            // count of outgoing messages
byte localAddress = 0x99;     // address of this device
File FileS1,FileS2,FileS3,FileS4,FileS5,FileS6,FileS7;
File FileS8;

String s[10];
bool f1,f2,f3,f4,f5,f6,f7,f8;
char datestring[30];
String answer,j,k,g;
//==============================
int maxTemp= 25;
int minTemp= 15;
//==============================
String str,v,r;
bool flagStart=false;
String postStr;
String D;
void setup() {
  Serial.begin(9600);                  
  initSdSerial();
  bot_setup();
  Rtc.Begin();
  initLora();
  LoRa.onReceive(onReceive);
  LoRa.receive();    
  RtcDateTime now = Rtc.GetDateTime();
  printDateTime(now);
}

void loop() {
  client.loop();
  checkNewMassage();
  criticalTemp();
}

void onConnectionEstablished(){
   /*client.subscribe("ProPlast/D1", [](const String & topic, const String & payload) {
    Serial.println("topic: " + topic + ", payload: " + payload);
});
   client.subscribe("ProPlast/D2", [](const String & topic, const String & payload) {
    Serial.println("topic: " + topic + ", payload: " + payload);
  });
   client.subscribe("ProPlast/D3", [](const String & topic, const String & payload) {
    Serial.println("topic: " + topic + ", payload: " + payload);
  });  
  client.subscribe("ProPlast/D4", [](const String & topic, const String & payload) {
    Serial.println("topic: " + topic + ", payload: " + payload);
  });  
  client.subscribe("ProPlast/D5", [](const String & topic, const String & payload) {
    Serial.println("topic: " + topic + ", payload: " + payload);
  });  
  client.subscribe("ProPlast/D6", [](const String & topic, const String & payload) {
    Serial.println("topic: " + topic + ", payload: " + payload);
  });
  client.subscribe("ProPlast/D7", [](const String & topic, const String & payload) {
    Serial.println("topic: " + topic + ", payload: " + payload);
  });
  client.subscribe("ProPlast/D8", [](const String & topic, const String & payload) {
    Serial.println("topic: " + topic + ", payload: " + payload);
  });*/
}

void onReceive(int packetSize){
  if (packetSize == 0) return;          // if there's no packet, return

  // read packet header bytes:
  int recipient = LoRa.read();          // recipient address
  byte sender = LoRa.read();            // sender address
  byte incomingMsgId = LoRa.read();     // incoming msg ID
  byte incomingLength = LoRa.read();    // incoming msg length

  String incoming = "";

  while (LoRa.available()) {
    incoming += (char)LoRa.read();
  }

  // if the recipient isn't this device or broadcast,
  if (recipient != localAddress && recipient != 0xFF) {
    Serial.println("This message is not for me.");
    return;                             // skip rest of function
  }

  Serial.println("Received from: 0x" + String(sender, HEX));
  Serial.println("Message: " + incoming);
  Serial.println();

  
// Обьявление обьекта 

  if(sender==0x11){
    s[1]=incoming;
    f1=true;
    WriteSD();
    }
  else if(sender==0x22){
    s[2]=incoming;
    f2=true;
    WriteSD();
    }
  else if(sender==0x33){
    s[3]=incoming;
    f3=true;
    WriteSD();
    }
  else if(sender==0x44){
    s[4]=incoming;
    f4=true;
    WriteSD();
    }
  else if(sender==0x55){
    s[5]=incoming;
    f5=true;
    WriteSD();
    }
  else if(sender==0x66){
    s[6]=incoming;
    f6=true;
    WriteSD();
    }
  else if(sender==0x77){
    s[7]=incoming;
    f7=true;
    WriteSD();
    }
  else if(sender==0x88){
    s[8]=incoming;
    f8=true;
    WriteSD();
    }
    sender=0x00;
}

void initLora(){
    while (!Serial);

  LoRa.setPins(csPin, resetPin, irqPin);// set CS, reset, IRQ pin
  Serial.println("LoRa init ...");
  if (!LoRa.begin(433E6)) {             // initialize ratio at 915 MHz
  Serial.println("LoRa init failed. Check your connections.");
    while (true);                       // if failed, do nothing
  }
  Serial.println("LoRa init success");
  LoRa.onReceive(onReceive);
  LoRa.receive();
}

void initSdSerial(void){
  Serial.println("Initializing SD card...");
  if (SD.begin(16)) {
     Serial.println("initialization done.");
  }else{
    Serial.println("initialization failed!");
    }
}

void bot_setup(){
    configTime(0, 0, "pool.ntp.org");      // get UTC time via NTP
  secured_client.setTrustAnchors(&cert); // Add root certificate for api.telegram.org
  Serial.print("Connecting to Wifi SSID ");
  Serial.print(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.print("\nWiFi connected. IP address: ");
  Serial.println(WiFi.localIP());


  const String commands = F("["
                            "{\"command\":\"start\",  \"description\":\"Zapusk\"},"
                            "{\"command\":\"stop\", \"description\":\"Stop\"},"
                            "{\"command\":\"temperature\",\"description\":\"Zapros dannyh\"}" // no comma on last command
                            "]");
  bot.setMyCommands(commands);
}

void handleNewMessages(int numNewMessages){
  //Serial.print("handleNewMessages ");
  //Serial.println(numNewMessages);
  for (int i = 0; i < numNewMessages; i++)
  {
    chat_id = String(bot.messages[i].chat_id);
    telegramMessage &msg = bot.messages[i];
    Serial.println(chat_id);
    Serial.println("Received " + msg.text);
    if(msg.text == "/start"){
      flagStart=true;
      answer="Старт!";
      bot.sendMessage(chat_id, answer, "");
      }
    if(msg.text == "/stop"){
      flagStart=false;
      answer="Стоп!";
      bot.sendMessage(chat_id, answer, "");
      }
    if (msg.text == "/temperature"){
      if(flagStart){
        for(int i=1;i<9;i++){ // Измененеие кол-во датчиков
           j='D'+String(i);
           k=j+'=';
           g=k+s[i]; 
           answer=g+"C°"; 
           Serial.println(answer);
           bot.sendMessage(chat_id, answer, "");
           delay(100);
           answer="";
           j="";
           k="";
          }
        }
      }
    else
     { answer = "error";}
  }
}

void checkNewMassage(){
if (millis() - bot_lasttime > BOT_MTBS)
  { 
    //Serial.println("Check response");
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages)
    {
      //Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    bot_lasttime = millis();
  }
}

void WriteSD(){
  Serial.println("Sellect");
  RtcDateTime now = Rtc.GetDateTime();
  printDateTime(now);
  D=datestring;
  if(f1){
    f1=false;
    Serial.println("Writing to d1");
    FileS1 = SD.open("Data1.txt", FILE_WRITE);
    FileS1.print("Температура d1: ");
    FileS1.print(s[1]);
    FileS1.print(" °C  ");
    FileS1.println(D);
    FileS1.close();
    client.publish("D1", s[1]);
    }
  else if(f2){
    f2=false;
    Serial.println("Writing to d2");
    FileS2 = SD.open("Data2.txt", FILE_WRITE);
    FileS2.print("Температура d2: ");
    FileS2.print(s[2]);
    FileS2.print(" °C  ");
    FileS2.println(D);
    FileS2.close();
    client.publish("D2", s[2]);
    }
  else if(f3){
    f3=false;
    Serial.println("Writing to d3");
    FileS3 = SD.open("Data3.txt", FILE_WRITE);
    FileS3.print("Температура d3: ");
    FileS3.print(s[3]);
    FileS3.print(" °C  ");
    FileS3.println(D);
    FileS3.close();
    client.publish("D3", s[3]);
    }
  else if(f4){
    Serial.println("Writing to d4");
    FileS4 = SD.open("Data4.txt", FILE_WRITE);
    FileS4.print("Температура d4: ");
    FileS4.print(s[4]);
    FileS4.print(" °C  ");
    FileS4.println(D);
    FileS4.close();
    client.publish("D4", s[4]);
    f4=false;
    }
  else if(f5){ 
    f5=false;
    Serial.print("Writing to d5");
    FileS5 = SD.open("Data5.txt", FILE_WRITE);
    FileS5.print("Температура d5: ");
    FileS5.print(s[5]);
    FileS5.print(" °C  ");
    FileS5.println(D);
    FileS5.close();
    client.publish("D5", s[5]);
    }
  else if(f6){
    f6=false;
    Serial.println("Writing to d6");
    FileS6 = SD.open("Data6.txt", FILE_WRITE);
    FileS6.print("Температура d6: ");
    FileS6.print(s[6]);
    FileS6.print(" °C  ");
    FileS6.println(D);
    FileS6.close();
    client.publish("D6", s[6]);
    }
  else if(f7){
    f7=false;
    Serial.println("Writing to d7");
    FileS7 = SD.open("Data7.txt", FILE_WRITE);
    FileS7.print("Температура d7: ");
    FileS7.print(s[7]);
    FileS7.print(" °C  ");
    FileS7.println(D);
    FileS7.close();
    client.publish("D7", s[7]);
    }
  else if(f8){
    f8=false;
    Serial.println("Writing to d8");
    FileS8 = SD.open("Data8.txt", FILE_WRITE);
    FileS8.print("Температура d8: ");
    FileS8.print(s[8]);
    FileS8.print(" °C  ");
    FileS8.println(D);
    FileS8.close();
    client.publish("D8", s[8]);
    }
  }

#define countof(a) (sizeof(a) / sizeof(a[0]))
void printDateTime(const RtcDateTime& dt){
    snprintf_P(datestring, 
            countof(datestring),
            PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
            dt.Month(),
            dt.Day(),
            dt.Year(),
            dt.Hour(),
            dt.Minute(),
            dt.Second() );
    Serial.print(datestring);
}
void criticalTemp(){
    if(millis() - temp_lasttime > TEMP_MTBS){
      if(flagStart){
        for(int i=1;i<9;i++){
          int q=s[i].toInt();
          if(q >= maxTemp&&q!=0){
            r="Температура  D"+String(i);
            str = r+" повышена от указанной нормы: " + String(s[i]);
            bot.sendMessage(chat_id, str, "");
            //Serial.println("повышена от указанной нормы");
          }
          if(q <= minTemp&&q!=0){
            v="Температура  D"+String(i);
            str = v+" снижена от указанной нормы: " + String(s[i]);
            bot.sendMessage(chat_id, str,"");
            //Serial.println("снижена от указанной нормы");
            }
          }
        }
        /*for(int i=0;i<7;i++){
          s[i]="";
        }*/
        temp_lasttime = millis();
    }
}
