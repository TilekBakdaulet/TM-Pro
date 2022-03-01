#include <SPI.h>             
#include <LoRa.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <GyverPower.h>
#define ONE_WIRE_BUS 2
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS(&oneWire);
const int csPin = 10;         // LoRa radiochip select
const int resetPin = 9;       // LoRa radio reset
const int irqPin = 3;         // change for your board; must be a hardware interrupt pin
String outgoing;              // outgoing message
byte destination = 0x99;      // destination to send to
byte msgCount = 1;            // count of outgoing messages
float tempC;

byte localAddress = 0x11;     // address of this device
//*byte localAddress = 0x22;     // address of this device
//*byte localAddress = 0x33;     // address of this device
//*byte localAddress = 0x44;     // address of this device
//byte localAddress = 0xA1;     // address of this device
//*byte localAddress = 0x66;     // address of this device
//*byte localAddress = 0x77;     // address of this device
//byte localAddress = 0x88;     // address of this device



void setup() {
  DS.begin();              
  delay(1000);
  initLora();
  delay(1000);
  initSleepmode();
  delay(1000);
}

void loop() {
  delay(300);
  DS.requestTemperatures();
  delay(300);
  tempC = DS.getTempCByIndex(0);
  delay(300);
  String message = String(tempC);  
  sendMessage(message); // send a message
  delay(300);
  LoRa.sleep();
  power.sleepDelay(300000);
}

void sendMessage(String outgoing) {
  LoRa.beginPacket();                   // start packet
  LoRa.write(destination);              // add destination address
  LoRa.write(localAddress);             // add sender address
  LoRa.write(msgCount);                 // add message ID
  LoRa.write(outgoing.length());        // add payload length
  LoRa.print(outgoing);                 // add payload
  LoRa.endPacket();                     // finish packet and send it                     // increment message ID
}

void initLora(){
  LoRa.setPins(csPin, resetPin);// set CS, reset, IRQ pin
  if (!LoRa.begin(433E6)) {             // initialize ratio at 915 MHz                      // if failed, do nothing
  }
}

void initSleepmode(){
   power.autoCalibrate(); // автоматическая калибровка ~ 2 секунды , средняя но достаточная точность
  // отключение ненужной периферии
  power.hardwareDisable(PWR_ADC | PWR_TIMER1); // см раздел константы в GyverPower.h, разделяющий знак " | "
  // управление системной частотой
  power.setSystemPrescaler(PRESCALER_2); // см константы в GyverPower.h
  
  // настройка параметров сна
  power.setSleepMode(POWERDOWN_SLEEP); // если нужен другой режим сна, см константы в GyverPower.h (по умолчанию POWERDOWN_SLEEP)
  power.bodInSleep(false); // рекомендуется выключить bod во сне для сохранения энергии (по умолчанию false - выключен!!)
  }
