#ifndef KANOOP_PI_OUTPUTBANK_H
#define KANOOP_PI_OUTPUTBANK_H

#include <QMap>
#include <QObject>

#include <Kanoop/pi/gpiobackend.h>

/**
 * @brief A set of output lines requested and driven together.
 *
 * Values are logical. setValue(offset, true) energises the load; the kernel
 * performs any electrical inversion because activeLow was set on the request.
 */
class KANOOPPI_EXPORT OutputBank : public QObject
{
    Q_OBJECT
public:
    OutputBank(IGpioBackend* backend,
               const QString& consumer,
               const QList<quint32>& offsets,
               bool activeLow,
               QObject* parent = nullptr);

    virtual ~OutputBank();

    /** @brief Requests the lines, all inactive. @return True on success. */
    bool request();

    /** @brief Releases the lines if held. */
    void release();

    /** @brief Returns true while the lines are held. */
    bool isRequested() const { return _handle != Gpio::InvalidRequest; }

    /** @brief Drives @p offset to @p active. @return True on success. */
    bool setValue(quint32 offset, bool active);

    /** @brief Drives every entry of @p values in a single operation. @return True on success. */
    bool setValues(const QMap<quint32, bool>& values);

    /** @brief Drives every line in the bank inactive in a single operation. @return True on success. */
    bool allInactive();

    /** @brief Returns the last value written to @p offset. */
    bool isActive(quint32 offset) const { return _active.value(offset, false); }

    /** @brief Returns the line offsets in this bank. */
    QList<quint32> offsets() const { return _offsets; }

    /** @brief Returns the text of the most recent failure. */
    QString errorText() const { return _errorText; }

private:
    bool ownsAll(const QList<quint32>& offsets);

    IGpioBackend* _backend;
    QString _consumer;
    QList<quint32> _offsets;
    bool _activeLow;
    Gpio::RequestHandle _handle = Gpio::InvalidRequest;
    QMap<quint32, bool> _active;
    QString _errorText;
};

#endif // KANOOP_PI_OUTPUTBANK_H
