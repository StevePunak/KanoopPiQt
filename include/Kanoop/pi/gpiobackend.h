#ifndef KANOOP_PI_GPIOBACKEND_H
#define KANOOP_PI_GPIOBACKEND_H

#include <QObject>

#include <Kanoop/pi/gpio.h>

/**
 * @brief Abstract GPIO transport.
 *
 * Consumers of this library are written against this interface so they can be
 * exercised with MockBackend on a host with no GPIO hardware.
 */
class KANOOPPI_EXPORT IGpioBackend : public QObject
{
    Q_OBJECT
public:
    explicit IGpioBackend(QObject* parent = nullptr) :
        QObject(parent) {}

    virtual ~IGpioBackend() {}

    /** @brief Opens the chip whose kernel label matches @p label. @return True on success. */
    virtual bool openChipByLabel(const QString& label) = 0;

    /** @brief Opens the chip at character device @p path. @return True on success. */
    virtual bool openChipByPath(const QString& path) = 0;

    /** @brief Releases every request and closes the chip. */
    virtual void closeChip() = 0;

    /** @brief Returns true when a chip is open. */
    virtual bool isOpen() const = 0;

    /** @brief Requests @p request as output lines. @return A handle, or Gpio::InvalidRequest on failure. */
    virtual Gpio::RequestHandle requestOutputs(const Gpio::OutputRequest& request) = 0;

    /** @brief Requests @p request as input lines. @return A handle, or Gpio::InvalidRequest on failure. */
    virtual Gpio::RequestHandle requestInputs(const Gpio::InputRequest& request) = 0;

    /** @brief Releases the lines held by @p handle. */
    virtual void release(Gpio::RequestHandle handle) = 0;

    /**
     * @brief Writes @p values to @p offsets in a single operation.
     * @return True on success.
     */
    virtual bool setValues(Gpio::RequestHandle handle,
                           const QList<quint32>& offsets,
                           const QList<Gpio::Value>& values) = 0;

    /** @brief Returns the text of the most recent failure. */
    QString errorText() const { return _errorText; }

signals:
    /** @brief Emitted when an edge is detected on a requested input line. */
    void edgeEvent(quint32 offset, Gpio::Edge edge);

protected:
    /** @brief Records @p value as the most recent failure text. */
    void setErrorText(const QString& value) { _errorText = value; }

private:
    QString _errorText;
};

#endif // KANOOP_PI_GPIOBACKEND_H
