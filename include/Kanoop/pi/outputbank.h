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
 *
 * @warning The kernel releases a requested line when the owning process exits,
 *          and a released line reverts to input. Hardware that must stay
 *          de-energised while no process holds the line needs an external
 *          pull resistor; there is no way to latch an output state across
 *          process exit.
 */
class KANOOPPI_EXPORT OutputBank : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief Constructs an OutputBank for a set of lines.
     * @param backend The GPIO backend to use for requests.
     * @param consumer The string the kernel records as the owner of these lines.
     * @param offsets The GPIO line offsets to request together.
     * @param activeLow Whether these lines are active-low.
     * @param parent Qt parent object.
     */
    OutputBank(IGpioBackend* backend,
               const QString& consumer,
               const QList<quint32>& offsets,
               bool activeLow,
               QObject* parent = nullptr);

    /** @brief Releases the lines if held. */
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
