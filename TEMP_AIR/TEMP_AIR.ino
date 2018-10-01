#include <DHT.h>
#include <MQ135.h>

MQ135 gasSensor = MQ135(9);
int val_air;
int Senpin_air;
int Senpin_temp = 7;
int temp;
int hmd;
DHT dht (Senpin_temp, DHT11);

void setup() {
  pinMode(Senpin_temp, INPUT);
  Serial.begin(9600);
  dht.begin();
}

void loop() {
  Senpin_temp = digitalRead(7);
  temp = dht.readTemperature();
  hmd = dht.readHumidity();

  Senpin_air = digitalRead(9);       // read analog input pin 0
  float ppm = gasSensor.getPPM();
  Serial.print(Senpin_air, DEC);  // prints the value read
  Serial.println(ppm);
  if (Senpin_air > 500) {
    Serial.println(" AIRE CONTAMINADO");
  }
  else {

    Serial.println("AIRE NO CONTAMINADO");
  }
  Serial.print ("Temperatura :");
  Serial.print (temp);
  Serial.print(" Grados \n");
  Serial.print ("Humedad :");
  Serial.print (hmd);
  Serial.print(" % \n");
  Serial.println();
  delay(500);
}

