#include "Kanoop/pi/mcp23017.h"

bool Mcp23017::writeRegister(uint8_t reg, uint8_t value)
{
    QByteArray bytes;
    bytes.append((char)reg);
    bytes.append((char)value);
    return _bus->write(_address, bytes);
}

bool Mcp23017::initialize()
{
    return writeRegister(IOCON, IOCON_MIRROR | IOCON_ODR) &&
           writeRegister(IODIRA, 0xFF) &&
           writeRegister(IODIRB, 0xFF) &&
           writeRegister(GPPUA, 0xFF) &&
           writeRegister(GPPUB, 0xFF) &&
           writeRegister(INTCONA, 0x00) &&
           writeRegister(INTCONB, 0x00) &&
           writeRegister(GPINTENA, 0xFF) &&
           writeRegister(GPINTENB, 0xFF);
}

bool Mcp23017::readInputs(uint16_t* bits)
{
    QByteArray result;
    QByteArray reg;
    reg.append((char)GPIOA);
    if(_bus->writeRead(_address, reg, 2, &result) == false) {
        return false;
    }
    *bits = (uint16_t)((uint8_t)result.at(0) | ((uint8_t)result.at(1) << 8));
    return true;
}
