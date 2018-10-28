#include <EtherCard.h>
#include <DHT.h>
#include <MQ135.h>

#define REQUEST_RATE 10000 // milliseconds

MQ135 gasSensor = MQ135(9);
int val_air;
int Senpin_air;
int Senpin_temp = 7;
int temp;
int hmd;
float ppm;
DHT dht (Senpin_temp, DHT11);

// ethernet interface mac address
static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };
// remote website name
const char website[] = "170.238.226.93";
static byte hisip[] = { 170,238,226,93 };
char websiteIP[] = "170.238.226.93";
EtherCard eth;

const char headers[] = "?idSensor=1&humedad=%d&temperatura=%d&co2=%d\r\nHost:170.238.226.93\r\n\r\n";

byte Ethernet::buffer[700];
static long timer;

// called when the client request is complete
static void my_result_cb (byte status, word off, word len) {
  Serial.print("<<< reply ");
  Serial.print(millis() - timer);
  Serial.println(" ms");
  Serial.println((const char*) Ethernet::buffer + off);
}

void setupEthernet() {
  if (ether.begin(sizeof Ethernet::buffer, mymac, SS) == 0)
    Serial.println( "Failed to access Ethernet controller");

  if (!ether.dhcpSetup())
    Serial.println("DHCP failed");

  ether.printIp("My IP: ", ether.myip);
  ether.printIp("GW IP: ", ether.gwip);
  ether.printIp("DNS IP: ", ether.dnsip);

  ether.copyIp(ether.hisip, hisip);
  ether.printIp("Server: ", ether.hisip); 
}

void setupSensor() {
  pinMode(Senpin_temp, INPUT);
  dht.begin();
}

void setup () {
  Serial.begin(57600);
  Serial.println("[AirQuality 2.0]");

  setupEthernet();
  setupSensor();
 
  timer = - REQUEST_RATE; // start timing out right away
}


void loop () {

  ether.packetLoop(ether.packetReceive());

  if (millis() > timer + REQUEST_RATE) {
    timer = millis();
    
    Senpin_temp = digitalRead(7);
    temp = dht.readTemperature();
    hmd = dht.readHumidity();
    Senpin_air = digitalRead(9);
    ppm = gasSensor.getPPM();
    Serial.println();
    Serial.print("Temperatura: ");
    Serial.println(temp);
    Serial.print("Humedad: ");
    Serial.println(hmd);
    Serial.print("CO2: ");
    Serial.println(ppm);
    
    Serial.println(">>> REQ");
    sendDataToServer(hmd, temp, ppm);
  }
}

void sendDataToServer(int humedad, int temperatura, int co2) {
  char bufferr[sizeof(headers) + 30];
  sprintf(bufferr, headers, humedad, temperatura, co2);
  Serial.println(bufferr);
  ether.browseUrl(PSTR("/info/save2/"), bufferr, website, my_result_cb);  
}
