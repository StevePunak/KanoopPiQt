#ifndef KANOOP_PI_OUTPUTPIN_H
#define KANOOP_PI_OUTPUTPIN_H

#include <Kanoop/pi/outputbank.h>

/**
 * @brief A single output line.
 *
 * @warning The kernel releases a requested line when the owning process exits,
 *          and a released line reverts to input. Hardware that must stay
 *          de-energised while no process holds the line needs an external
 *          pull resistor; there is no way to latch an output state across
 *          process exit.
 */
class KANOOPPI_EXPORT OutputPin : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief Constructs an output pin wrapping a single-line OutputBank.
     *
     * @param backend The GPIO backend.
     * @param consumer The string the kernel records as the line's owner.
     * @param offset The line offset.
     * @param activeLow If true, the line is active-low (inverted).
     * @param parent The Qt parent object.
     */
    OutputPin(IGpioBackend* backend,
              const QString& consumer,
              quint32 offset,
              bool activeLow,
              QObject* parent = nullptr);

    /** @brief Requests the line, inactive. @return True on success. */
    bool request() { return _bank.request(); }

    /** @brief Releases the line if held. */
    void release() { _bank.release(); }

    /** @brief Returns true while the line is held. */
    bool isRequested() const { return _bank.isRequested(); }

    /** @brief Drives the line to @p active. @return True on success. */
    bool setActive(bool active) { return _bank.setValue(_offset, active); }

    /** @brief Drives the line to the inverse of its current value. @return True on success. */
    bool toggle() { return setActive(isActive() == false); }

    /** @brief Returns the last value written. */
    bool isActive() const { return _bank.isActive(_offset); }

    /** @brief Returns the line offset. */
    quint32 offset() const { return _offset; }

    /** @brief Returns the text of the most recent failure. */
    QString errorText() const { return _bank.errorText(); }

private:
    quint32 _offset = 0;
    OutputBank _bank;
};

#endif // KANOOP_PI_OUTPUTPIN_H
