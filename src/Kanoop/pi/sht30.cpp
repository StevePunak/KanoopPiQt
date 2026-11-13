#include "Kanoop/pi/sht30.h"

bool Sht30::startMeasurement()
{
    QByteArray command;
    command.append((char)0x24);
    command.append((char)0x00);
    return _bus->write(_address, command);
}

bool Sht30::readMeasurement(double* celsius, double* percent)
{
    QByteArray result;
    if(_bus->read(_address, 6, &result) == false) {
        return false;
    }
    const uint8_t* bytes = (const uint8_t*)result.constData();
    if(crc8(bytes, 2) != bytes[2] || crc8(bytes + 3, 2) != bytes[5]) {
        return false;
    }
    const uint16_t rawTemperature = (uint16_t)((bytes[0] << 8) | bytes[1]);
    const uint16_t rawHumidity = (uint16_t)((bytes[3] << 8) | bytes[4]);
    *celsius = -45.0 + 175.0 * rawTemperature / 65535.0;
    *percent = 100.0 * rawHumidity / 65535.0;
    return true;
}

uint8_t Sht30::crc8(const uint8_t* data, int length)
{
    uint8_t crc = 0xFF;
    for(int i = 0; i < length; i++) {
        crc ^= data[i];
        for(int bit = 0; bit < 8; bit++) {
            crc = (crc & 0x80) != 0 ? (uint8_t)((crc << 1) ^ 0x31) : (uint8_t)(crc << 1);
        }
    }
    return crc;
}
