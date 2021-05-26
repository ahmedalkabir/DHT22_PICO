#include <DHT22_PICO.h>

DHT22 dht_sensor(p12);

uint32_t last_time = 0;

void setup() {
  // put your setup code here, to run once:
  while(!Serial);
  Serial.begin(115200);
  Serial.println("DHT22 Test Library");
  dht_sensor.begin();
  
}

void loop() {
  // put your main code here, to run repeatedly:
  if(millis() - last_time > 1000){
    last_time = millis();  
    // read data
    auto status = dht_sensor.read();
    if(status == DHT22::DHT_STATUS::DHT_OK){
      Serial.print("Temperature: ");
      Serial.print(dht_sensor.read_temperature());
      Serial.print("    ");
      Serial.print("Humidity: ");
      Serial.println(dht_sensor.read_humidity());
    }
  }
}