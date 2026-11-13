#ifndef KANOOP_PI_I2CBUS_H
#define KANOOP_PI_I2CBUS_H

#include <QByteArray>
#include <QString>

#include <Kanoop/pi/kanooppi.h>

/**
 * @brief A Linux I2C adapter (/dev/i2c-N). Each of write(), read() and writeRead() is one I2C_RDWR transaction.
 */
class KANOOPPI_EXPORT I2cBus
{
public:
    explicit I2cBus(const QString& path) : _path(path) {}
    ~I2cBus();

    bool open();
    bool isOpen() const { return _fd >= 0; }
    bool write(uint8_t address, const QByteArray& bytes);

    /** @brief Writes then reads as one combined transaction (repeated start). */
    bool writeRead(uint8_t address, const QByteArray& bytes, int readLength, QByteArray* result);

    bool read(uint8_t address, int readLength, QByteArray* result);

    QString path() const { return _path; }

private:
    Q_DISABLE_COPY(I2cBus)

    QString _path;
    int _fd = -1;
};

#endif // KANOOP_PI_I2CBUS_H
