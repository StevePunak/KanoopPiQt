#include "Kanoop/pi/i2cbus.h"

#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <sys/ioctl.h>
#include <unistd.h>

I2cBus::~I2cBus()
{
    if(_fd >= 0) {
        ::close(_fd);
    }
}

bool I2cBus::open()
{
    _fd = ::open(_path.toLocal8Bit().constData(), O_RDWR | O_CLOEXEC);
    return _fd >= 0;
}

bool I2cBus::write(uint8_t address, const QByteArray& bytes)
{
    if(_fd < 0) {
        return false;
    }
    i2c_msg message;
    message.addr = address;
    message.flags = 0;
    message.len = (uint16_t)bytes.size();
    message.buf = (uint8_t*)bytes.constData();
    i2c_rdwr_ioctl_data data { &message, 1 };
    return ::ioctl(_fd, I2C_RDWR, &data) == 1;
}

bool I2cBus::writeRead(uint8_t address, const QByteArray& bytes, int readLength, QByteArray* result)
{
    if(_fd < 0) {
        return false;
    }
    QByteArray buffer(readLength, 0);
    i2c_msg messages[2];
    messages[0].addr = address;
    messages[0].flags = 0;
    messages[0].len = (uint16_t)bytes.size();
    messages[0].buf = (uint8_t*)bytes.constData();
    messages[1].addr = address;
    messages[1].flags = I2C_M_RD;
    messages[1].len = (uint16_t)readLength;
    messages[1].buf = (uint8_t*)buffer.data();
    i2c_rdwr_ioctl_data data { messages, 2 };
    if(::ioctl(_fd, I2C_RDWR, &data) != 2) {
        return false;
    }
    *result = buffer;
    return true;
}

bool I2cBus::read(uint8_t address, int readLength, QByteArray* result)
{
    if(_fd < 0) {
        return false;
    }
    QByteArray buffer(readLength, 0);
    i2c_msg message;
    message.addr = address;
    message.flags = I2C_M_RD;
    message.len = (uint16_t)readLength;
    message.buf = (uint8_t*)buffer.data();
    i2c_rdwr_ioctl_data data { &message, 1 };
    if(::ioctl(_fd, I2C_RDWR, &data) != 1) {
        return false;
    }
    *result = buffer;
    return true;
}
