#ifndef KANOOP_PI_INPUTPIN_H
#define KANOOP_PI_INPUTPIN_H

#include <Kanoop/timespan.h>

#include <Kanoop/pi/gpiobackend.h>

/**
 * @brief A single edge-triggered input line.
 *
 * Debounce is applied by the kernel from the period given to setDebounce().
 */
class KANOOPPI_EXPORT InputPin : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief Constructs an input pin for a single GPIO line.
     *
     * @param backend The GPIO backend.
     * @param consumer The string the kernel records as the line's owner.
     * @param offset The line offset.
     * @param parent The Qt parent object.
     */
    InputPin(IGpioBackend* backend,
             const QString& consumer,
             quint32 offset,
             QObject* parent = nullptr);

    /** @brief Releases the line if held. */
    virtual ~InputPin();

    /** @brief Sets the internal bias applied when the line is requested. */
    void setBias(Gpio::Bias value) { _bias = value; }

    /** @brief Sets which edges are detected. */
    void setEdge(Gpio::Edge value) { _edge = value; }

    /** @brief Sets the kernel debounce period. */
    void setDebounce(const TimeSpan& value) { _debounce = value; }

    /** @brief Sets whether a low level is the asserted state. */
    void setActiveLow(bool value) { _activeLow = value; }

    /** @brief Requests the line. @return True on success. */
    bool request();

    /** @brief Releases the line if held. */
    void release();

    /** @brief Returns true while the line is held. */
    bool isRequested() const { return _handle != Gpio::InvalidRequest; }

    /** @brief Returns the line offset. */
    quint32 offset() const { return _offset; }

    /** @brief Returns the text of the most recent failure. */
    QString errorText() const { return _errorText; }

signals:
    /** @brief Emitted for every detected edge on this line. */
    void edge(Gpio::Edge edge);

    /** @brief Emitted when the line enters its asserted state. */
    void asserted();

    /** @brief Emitted when the line leaves its asserted state. */
    void deasserted();

private slots:
    /** @brief Forwards backend edge events on @ref offset() to edge(), asserted() and deasserted(). */
    void onBackendEdgeEvent(quint32 offset, Gpio::Edge edge);

private:
    IGpioBackend* _backend;
    QString _consumer;
    quint32 _offset;
    Gpio::Bias _bias = Gpio::Bias::AsIs;
    Gpio::Edge _edge = Gpio::Edge::Both;
    TimeSpan _debounce;
    bool _activeLow = false;
    Gpio::RequestHandle _handle = Gpio::InvalidRequest;
    QString _errorText;
};

#endif // KANOOP_PI_INPUTPIN_H
