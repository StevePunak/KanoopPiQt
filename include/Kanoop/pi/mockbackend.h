#ifndef KANOOP_PI_MOCKBACKEND_H
#define KANOOP_PI_MOCKBACKEND_H

#include <QMap>

#include <Kanoop/pi/gpiobackend.h>

/**
 * @brief In-memory IGpioBackend for tests and for running a consumer on a host
 *        with no GPIO hardware.
 */
class KANOOPPI_EXPORT MockBackend : public IGpioBackend
{
    Q_OBJECT
public:
    explicit MockBackend(QObject* parent = nullptr);

    virtual bool openChipByLabel(const QString& label) override;
    virtual bool openChipByPath(const QString& path) override;
    virtual void closeChip() override;
    virtual bool isOpen() const override { return _open; }

    virtual Gpio::RequestHandle requestOutputs(const Gpio::OutputRequest& request) override;
    virtual Gpio::RequestHandle requestInputs(const Gpio::InputRequest& request) override;
    virtual void release(Gpio::RequestHandle handle) override;

    virtual bool setValues(Gpio::RequestHandle handle,
                           const QList<quint32>& offsets,
                           const QList<Gpio::Value>& values) override;

    /** @brief Emits edgeEvent() for @p offset as though hardware had detected @p edge. */
    void simulateEdge(quint32 offset, Gpio::Edge edge);

    /** @brief Returns the current logical value of @p offset. */
    Gpio::Value lineValue(quint32 offset) const { return _values.value(offset, Gpio::Value::Inactive); }

    /** @brief Returns true when @p offset belongs to a live request. */
    bool isRequested(quint32 offset) const { return _owners.contains(offset); }

    /** @brief Returns the number of setValues() calls since construction. */
    int setValuesCallCount() const { return _setValuesCallCount; }

    /** @brief Returns the offsets passed to the most recent setValues() call. */
    QList<quint32> lastSetOffsets() const { return _lastSetOffsets; }

    /** @brief Returns the label or path the chip was opened with. */
    QString openedWith() const { return _openedWith; }

    /** @brief Returns the request descriptor recorded for the most recent requestInputs(). */
    Gpio::InputRequest lastInputRequest() const { return _lastInputRequest; }

    /** @brief Returns the request descriptor recorded for the most recent requestOutputs(). */
    Gpio::OutputRequest lastOutputRequest() const { return _lastOutputRequest; }

    /** @brief When set, the next request call fails and clears the flag. */
    void setFailNextRequest(bool value) { _failNextRequest = value; }

private:
    bool _open = false;
    bool _failNextRequest = false;
    QString _openedWith;
    Gpio::RequestHandle _nextHandle = 1;
    QMap<quint32, Gpio::Value> _values;
    QMap<quint32, Gpio::RequestHandle> _owners;
    Gpio::InputRequest _lastInputRequest;
    Gpio::OutputRequest _lastOutputRequest;
    int _setValuesCallCount = 0;
    QList<quint32> _lastSetOffsets;
};

#endif // KANOOP_PI_MOCKBACKEND_H
