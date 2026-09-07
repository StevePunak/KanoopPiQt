#ifndef KANOOP_PI_LIBGPIODBACKEND_H
#define KANOOP_PI_LIBGPIODBACKEND_H

#include <QMap>
#include <QStringList>

#include <Kanoop/pi/gpiobackend.h>

class QSocketNotifier;

struct gpiod_chip;
struct gpiod_line_request;
struct gpiod_edge_event_buffer;

/** @brief IGpioBackend implemented over libgpiod v2. */
class KANOOPPI_EXPORT LibGpiodBackend : public IGpioBackend
{
    Q_OBJECT
public:
    /** @brief Constructs a backend with no chip open. */
    explicit LibGpiodBackend(QObject* parent = nullptr);

    /** @brief Closes the chip and releases every outstanding request. */
    virtual ~LibGpiodBackend();

    /** @brief Opens the chip whose kernel label matches @p label. @return True on success. */
    virtual bool openChipByLabel(const QString& label) override;

    /** @brief Opens the chip at character device @p path. @return True on success. */
    virtual bool openChipByPath(const QString& path) override;

    /** @brief Releases every request and closes the chip. */
    virtual void closeChip() override;

    /** @brief Returns true when a chip is open. */
    virtual bool isOpen() const override { return _chip != nullptr; }

    /** @brief Requests @p request as output lines. @return A handle, or Gpio::InvalidRequest on failure. */
    virtual Gpio::RequestHandle requestOutputs(const Gpio::OutputRequest& request) override;

    /** @brief Requests @p request as input lines. @return A handle, or Gpio::InvalidRequest on failure. */
    virtual Gpio::RequestHandle requestInputs(const Gpio::InputRequest& request) override;

    /** @brief Releases the lines held by @p handle. */
    virtual void release(Gpio::RequestHandle handle) override;

    /**
     * @brief Writes @p values to @p offsets in a single operation.
     * @return True on success.
     */
    virtual bool setValues(Gpio::RequestHandle handle,
                           const QList<quint32>& offsets,
                           const QList<Gpio::Value>& values) override;

    /** @brief Returns every /dev/gpiochipN path present on the system, sorted. */
    static QStringList chipPaths();

    /** @brief Returns the kernel label of the chip at @p path, or an empty string. */
    static QString labelOf(const QString& path);

private slots:
    /** @brief Reads and dispatches edge events pending on @p fd. */
    void onEventFdActivated(int fd);

private:
    class Request
    {
    public:
        gpiod_line_request* request = nullptr;
        gpiod_edge_event_buffer* eventBuffer = nullptr;
        QSocketNotifier* notifier = nullptr;
    };

    void releaseAll();

    gpiod_chip* _chip = nullptr;
    Gpio::RequestHandle _nextHandle = 1;
    QMap<Gpio::RequestHandle, Request> _requests;
    QMap<int, Gpio::RequestHandle> _handleByFd;
};

#endif // KANOOP_PI_LIBGPIODBACKEND_H
