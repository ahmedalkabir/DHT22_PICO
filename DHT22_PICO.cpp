#include "DHT22_PICO.h"

// public methods
void DHT22::begin()
{
    pinMode(_dht_pin, OUTPUT);
    digitalWrite(_dht_pin, HIGH);
}

DHT22::DHT_STATUS DHT22::read()
{
    DHT_STATUS status = DHT22::DHT_STATUS::DHT_ERROR;
    if (_last_result && (millis() - _last_read_time > 500))
    {
        _last_read_time = millis();
        _last_result = false;
    }
    else if (!_last_result)
    {
        // read new data from the sensor
        _last_result = true;
        status = _read_dht();
    }
    return status;
}

float DHT22::read_temperature()
{
    return temperature;
}

float DHT22::read_humidity()
{
    return humidity;
}

// private methods
inline float DHT22::word(uint8_t first, uint8_t second)
{
    return (float)((first << 8) + second);
}

uint32_t DHT22::wait_for(uint8_t expect)
{
    uint32_t then = time_us_32();
    while (expect != gpio_get(static_cast<uint8_t>(_dht_pin)))
    {
        sleep_us(10);
        if (time_us_32() - then > 5000)
            return 0;
    }
    return time_us_32() - then;
}

DHT22::DHT_STATUS DHT22::_read_dht()
{
    uint8_t data[5] = {0, 0, 0, 0, 0};

    gpio_set_dir(static_cast<uint8_t>(_dht_pin), GPIO_OUT);
    gpio_put(static_cast<uint8_t>(_dht_pin), 0);
    sleep_ms(10); // at least 1ms but we set 10ms
    gpio_put(static_cast<uint8_t>(_dht_pin), 1);
    sleep_us(40); // pull time I guess ??!

    // wait for acknowledgement
    gpio_set_dir(static_cast<uint8_t>(_dht_pin), GPIO_IN);
    wait_for(0);
    wait_for(1);
    wait_for(0);

    // read sample (40 bits = 5 bytes)
    for (uint8_t bit = 0; bit < 40; ++bit)
    {
        wait_for(1);
        uint8_t count = wait_for(0);
        data[bit / 8] <<= 1;
        if (count > 50)
        {
            data[bit / 8] |= 1;
        }
    }

    // pull back up to mark end of read
    gpio_set_dir(static_cast<uint8_t>(_dht_pin), GPIO_OUT);
    gpio_put(static_cast<uint8_t>(_dht_pin), 1);

    // checksum
    if (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF))
    {
        float _h = word(data[0], data[1]) / 10;
        float _t = word(data[2] & 0x7F, data[3]) / 10;

        // if the highest bit is 1, temperature is negative
        if (data[2] & 0x80)
        {
            _t = -_t;
        }

        // check if checksum was OK but something else went wrong
        if (isnan(_t) || isnan(_h) || _t == 0)
        {
            temperature = 0;
            humidity = 0;
            return DHT_STATUS::DHT_ERR_NAN;
        }
        else
        {
            humidity = _h;
            temperature = _t;
            return DHT_STATUS::DHT_OK;
        }
    }
    else
    {
        temperature = 0;
        humidity = 0;
        return DHT_STATUS::DHT_ERR_CHECKSUM;
    }
}