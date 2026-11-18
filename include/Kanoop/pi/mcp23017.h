#ifndef KANOOP_PI_MCP23017_H
#define KANOOP_PI_MCP23017_H

#include <Kanoop/pi/i2cbus.h>
#include <Kanoop/pi/kanooppi.h>

/**
 * @brief MCP23017 as 16 inputs, interrupt on any change, INTA/INTB mirrored, open-drain.
 *
 * ⚠ Register addresses assume IOCON.BANK = 0, which initialize() writes.
 */
class KANOOPPI_EXPORT Mcp23017
{
public:
    Mcp23017(I2cBus* bus, uint8_t address) : _bus(bus), _address(address) {}

    bool initialize();

    /** @brief Bit n is DI n: GPA0–7 are DI0–7, GPB0–7 are DI8–15. Reading clears the interrupt. */
    bool readInputs(uint16_t* bits);

private:
    bool writeRegister(uint8_t reg, uint8_t value);

    static constexpr uint8_t IODIRA = 0x00;
    static constexpr uint8_t IODIRB = 0x01;
    static constexpr uint8_t GPINTENA = 0x04;
    static constexpr uint8_t GPINTENB = 0x05;
    static constexpr uint8_t INTCONA = 0x08;
    static constexpr uint8_t INTCONB = 0x09;
    static constexpr uint8_t IOCON = 0x0A;
    static constexpr uint8_t GPPUA = 0x0C;
    static constexpr uint8_t GPPUB = 0x0D;
    static constexpr uint8_t GPIOA = 0x12;

    static constexpr uint8_t IOCON_MIRROR = 0x40;
    static constexpr uint8_t IOCON_ODR = 0x04;

    I2cBus* _bus = nullptr;
    uint8_t _address = 0;
};

#endif // KANOOP_PI_MCP23017_H
