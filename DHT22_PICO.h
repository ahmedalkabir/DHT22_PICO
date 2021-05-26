#ifndef _DHT22_PICO_H_
#define _DHT22_PICO_H_

#include <Arduino.h>

class DHT22
{
private:
  enum class DHT_STATUS
  {
    DHT_OK,
    DHT_TIMEOUT,
    DHT_ERROR,
    DHT_ERR_NAN,
    DHT_ERR_CHECKSUM
  };
  uint32_t _last_read_time;
  bool _last_result = false;
  PinName _dht_pin;
  float temperature;
  float humidity;
  inline float word(uint8_t first, uint8_t second);
  uint32_t wait_for(uint8_t expect);
  DHT_STATUS _read_dht();

public:
  DHT22(PinName pin) : _dht_pin(pin)
  {
  }
  void begin();
  DHT_STATUS read();
  float read_temperature();
  float read_humidity();
  typedef DHT_STATUS DHT_STATUS;
};

#endif