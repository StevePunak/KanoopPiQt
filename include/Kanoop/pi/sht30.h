#ifndef KANOOP_PI_SHT30_H
#define KANOOP_PI_SHT30_H

#include <Kanoop/pi/i2cbus.h>
#include <Kanoop/pi/kanooppi.h>

/** @brief SHT30 single-shot, high repeatability, no clock stretching: start, wait ≥ 16 ms, read. */
class KANOOPPI_EXPORT Sht30
{
public:
    Sht30(I2cBus* bus, uint8_t address) : _bus(bus), _address(address) {}

    bool startMeasurement();

    /** @return false on a bus error or a CRC mismatch. */
    bool readMeasurement(double* celsius, double* percent);

    static constexpr int MeasurementMs = 20;

private:
    static uint8_t crc8(const uint8_t* data, int length);

    I2cBus* _bus = nullptr;
    uint8_t _address = 0;
};

#endif // KANOOP_PI_SHT30_H
