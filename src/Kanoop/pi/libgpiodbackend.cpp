#include "Kanoop/pi/libgpiodbackend.h"

#include <cerrno>
#include <cstring>

#include <QDir>
#include <QSocketNotifier>

#include <gpiod.h>

namespace
{

constexpr size_t EdgeEventBufferCapacity = 16;

class PendingEdgeEvent
{
public:
    quint32 offset = 0;
    Gpio::Edge edge = Gpio::Edge::None;
};

enum gpiod_line_value toLineValue(Gpio::Value value)
{
    return value == Gpio::Value::Active
           ? GPIOD_LINE_VALUE_ACTIVE
           : GPIOD_LINE_VALUE_INACTIVE;
}

enum gpiod_line_bias toBias(Gpio::Bias bias)
{
    switch(bias) {
    case Gpio::Bias::Disabled:  return GPIOD_LINE_BIAS_DISABLED;
    case Gpio::Bias::PullUp:    return GPIOD_LINE_BIAS_PULL_UP;
    case Gpio::Bias::PullDown:  return GPIOD_LINE_BIAS_PULL_DOWN;
    case Gpio::Bias::AsIs:      break;
    }
    return GPIOD_LINE_BIAS_AS_IS;
}

enum gpiod_line_edge toEdge(Gpio::Edge edge)
{
    switch(edge) {
    case Gpio::Edge::Rising:    return GPIOD_LINE_EDGE_RISING;
    case Gpio::Edge::Falling:   return GPIOD_LINE_EDGE_FALLING;
    case Gpio::Edge::Both:      return GPIOD_LINE_EDGE_BOTH;
    case Gpio::Edge::None:      break;
    }
    return GPIOD_LINE_EDGE_NONE;
}

}

LibGpiodBackend::LibGpiodBackend(QObject* parent) :
    IGpioBackend(parent)
{
}

LibGpiodBackend::~LibGpiodBackend()
{
    closeChip();
}

QStringList LibGpiodBackend::chipPaths()
{
    QStringList result;
    QDir dev(QString("/dev"));
    QStringList names = dev.entryList(QStringList() << QString("gpiochip*"), QDir::System);
    for(const QString& name : names) {
        result.append(dev.absoluteFilePath(name));
    }
    result.sort();
    return result;
}

QString LibGpiodBackend::labelOf(const QString& path)
{
    QString result;
    gpiod_chip* chip = gpiod_chip_open(qPrintable(path));
    if(chip == nullptr) {
        return result;
    }

    gpiod_chip_info* info = gpiod_chip_get_info(chip);
    if(info != nullptr) {
        result = QString(gpiod_chip_info_get_label(info));
        gpiod_chip_info_free(info);
    }
    gpiod_chip_close(chip);
    return result;
}

bool LibGpiodBackend::openChipByPath(const QString& path)
{
    closeChip();

    _chip = gpiod_chip_open(qPrintable(path));
    if(_chip == nullptr) {
        setErrorText(QString("cannot open %1: %2").arg(path).arg(strerror(errno)));
        return false;
    }
    return true;
}

bool LibGpiodBackend::openChipByLabel(const QString& label)
{
    closeChip();

    QStringList paths = chipPaths();
    for(const QString& path : paths) {
        if(labelOf(path) == label) {
            return openChipByPath(path);
        }
    }

    setErrorText(QString("no GPIO chip with label '%1'").arg(label));
    return false;
}

void LibGpiodBackend::closeChip()
{
    releaseAll();

    if(_chip != nullptr) {
        gpiod_chip_close(_chip);
        _chip = nullptr;
    }
}

void LibGpiodBackend::releaseAll()
{
    QList<Gpio::RequestHandle> handles = _requests.keys();
    for(Gpio::RequestHandle handle : handles) {
        release(handle);
    }
}

Gpio::RequestHandle LibGpiodBackend::requestOutputs(const Gpio::OutputRequest& request)
{
    if(isOpen() == false) {
        setErrorText(QString("chip is not open"));
        return Gpio::InvalidRequest;
    }

    gpiod_line_settings* settings = gpiod_line_settings_new();
    gpiod_line_config* lineConfig = gpiod_line_config_new();
    gpiod_request_config* reqConfig = gpiod_request_config_new();
    gpiod_line_request* lineRequest = nullptr;

    if(settings != nullptr && lineConfig != nullptr && reqConfig != nullptr) {
        gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_OUTPUT);
        gpiod_line_settings_set_output_value(settings, toLineValue(request.initialValue));
        gpiod_line_settings_set_active_low(settings, request.activeLow);

        QList<unsigned int> offsets;
        for(quint32 offset : request.offsets) {
            offsets.append(offset);
        }

        if(gpiod_line_config_add_line_settings(lineConfig, offsets.constData(),
                                               static_cast<size_t>(offsets.count()), settings) == 0) {
            gpiod_request_config_set_consumer(reqConfig, qPrintable(request.consumer));
            lineRequest = gpiod_chip_request_lines(_chip, reqConfig, lineConfig);
        }
    }

    if(reqConfig != nullptr) {
        gpiod_request_config_free(reqConfig);
    }
    if(lineConfig != nullptr) {
        gpiod_line_config_free(lineConfig);
    }
    if(settings != nullptr) {
        gpiod_line_settings_free(settings);
    }

    if(lineRequest == nullptr) {
        setErrorText(QString("request failed: %1").arg(strerror(errno)));
        return Gpio::InvalidRequest;
    }

    Request entry;
    entry.request = lineRequest;

    Gpio::RequestHandle handle = _nextHandle++;
    _requests.insert(handle, entry);
    return handle;
}

Gpio::RequestHandle LibGpiodBackend::requestInputs(const Gpio::InputRequest& request)
{
    if(isOpen() == false) {
        setErrorText(QString("chip is not open"));
        return Gpio::InvalidRequest;
    }

    gpiod_line_settings* settings = gpiod_line_settings_new();
    gpiod_line_config* lineConfig = gpiod_line_config_new();
    gpiod_request_config* reqConfig = gpiod_request_config_new();
    gpiod_line_request* lineRequest = nullptr;

    if(settings != nullptr && lineConfig != nullptr && reqConfig != nullptr) {
        gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_INPUT);
        gpiod_line_settings_set_bias(settings, toBias(request.bias));
        gpiod_line_settings_set_edge_detection(settings, toEdge(request.edge));
        gpiod_line_settings_set_active_low(settings, request.activeLow);
        if(request.debounceMicroseconds > 0) {
            gpiod_line_settings_set_debounce_period_us(settings, request.debounceMicroseconds);
        }

        QList<unsigned int> offsets;
        for(quint32 offset : request.offsets) {
            offsets.append(offset);
        }

        if(gpiod_line_config_add_line_settings(lineConfig, offsets.constData(),
                                               static_cast<size_t>(offsets.count()), settings) == 0) {
            gpiod_request_config_set_consumer(reqConfig, qPrintable(request.consumer));
            lineRequest = gpiod_chip_request_lines(_chip, reqConfig, lineConfig);
        }
    }

    if(reqConfig != nullptr) {
        gpiod_request_config_free(reqConfig);
    }
    if(lineConfig != nullptr) {
        gpiod_line_config_free(lineConfig);
    }
    if(settings != nullptr) {
        gpiod_line_settings_free(settings);
    }

    if(lineRequest == nullptr) {
        setErrorText(QString("request failed: %1").arg(strerror(errno)));
        return Gpio::InvalidRequest;
    }

    Request entry;
    entry.request = lineRequest;

    if(request.edge != Gpio::Edge::None) {
        entry.eventBuffer = gpiod_edge_event_buffer_new(EdgeEventBufferCapacity);
        if(entry.eventBuffer == nullptr) {
            gpiod_line_request_release(lineRequest);
            setErrorText(QString("event buffer allocation failed: %1").arg(strerror(errno)));
            return Gpio::InvalidRequest;
        }
    }

    Gpio::RequestHandle handle = _nextHandle++;

    if(entry.eventBuffer != nullptr) {
        int fd = gpiod_line_request_get_fd(lineRequest);
        entry.notifier = new QSocketNotifier(fd, QSocketNotifier::Read, this);
        connect(entry.notifier, &QSocketNotifier::activated,
                this, &LibGpiodBackend::onEventFdActivated);
        _handleByFd.insert(fd, handle);
    }

    _requests.insert(handle, entry);
    return handle;
}

void LibGpiodBackend::release(Gpio::RequestHandle handle)
{
    if(_requests.contains(handle) == false) {
        return;
    }

    Request entry = _requests.take(handle);

    if(entry.notifier != nullptr) {
        // The line request owns the fd; destroy the notifier before releasing the request.
        _handleByFd.remove(static_cast<int>(entry.notifier->socket()));
        entry.notifier->setEnabled(false);
        delete entry.notifier;
    }
    if(entry.eventBuffer != nullptr) {
        gpiod_edge_event_buffer_free(entry.eventBuffer);
    }
    if(entry.request != nullptr) {
        // Closes the fd from gpiod_line_request_get_fd(); never close() it elsewhere.
        gpiod_line_request_release(entry.request);
    }
}

bool LibGpiodBackend::setValues(Gpio::RequestHandle handle,
                                const QList<quint32>& offsets,
                                const QList<Gpio::Value>& values)
{
    if(_requests.contains(handle) == false) {
        setErrorText(QString("unknown request handle"));
        return false;
    }

    if(offsets.count() != values.count()) {
        setErrorText(QString("offset and value counts differ"));
        return false;
    }

    QList<unsigned int> rawOffsets;
    QList<enum gpiod_line_value> rawValues;
    for(int i = 0;i < offsets.count();i++) {
        rawOffsets.append(offsets.at(i));
        rawValues.append(toLineValue(values.at(i)));
    }

    gpiod_line_request* lineRequest = _requests.value(handle).request;
    if(gpiod_line_request_set_values_subset(lineRequest,
                                            static_cast<size_t>(rawOffsets.count()),
                                            rawOffsets.constData(),
                                            rawValues.constData()) != 0) {
        setErrorText(QString("set values failed: %1").arg(strerror(errno)));
        return false;
    }
    return true;
}

bool LibGpiodBackend::getValues(Gpio::RequestHandle handle,
                                const QList<quint32>& offsets,
                                QList<Gpio::Value>& values)
{
    if(_requests.contains(handle) == false) {
        setErrorText(QString("unknown request handle"));
        return false;
    }

    gpiod_line_request* lineRequest = _requests.value(handle).request;

    QList<unsigned int> rawOffsets;
    for(quint32 offset : offsets) {
        rawOffsets.append(offset);
    }

    QList<enum gpiod_line_value> rawValues;
    rawValues.resize(offsets.count());

    if(gpiod_line_request_get_values_subset(lineRequest,
                                            static_cast<size_t>(rawOffsets.count()),
                                            rawOffsets.constData(),
                                            rawValues.data()) != 0) {
        setErrorText(QString("get values failed: %1").arg(strerror(errno)));
        return false;
    }

    QList<Gpio::Value> result;
    for(enum gpiod_line_value raw : rawValues) {
        if(raw == GPIOD_LINE_VALUE_ERROR) {
            setErrorText(QString("kernel reported GPIOD_LINE_VALUE_ERROR"));
            return false;
        }
        result.append(raw == GPIOD_LINE_VALUE_ACTIVE ? Gpio::Value::Active : Gpio::Value::Inactive);
    }

    values = result;
    return true;
}

void LibGpiodBackend::onEventFdActivated(int fd)
{
    Gpio::RequestHandle handle = _handleByFd.value(fd, Gpio::InvalidRequest);
    if(handle == Gpio::InvalidRequest) {
        return;
    }

    Request entry = _requests.value(handle);
    if(entry.eventBuffer == nullptr) {
        return;
    }

    int count = gpiod_line_request_read_edge_events(entry.request, entry.eventBuffer,
                                                    EdgeEventBufferCapacity);
    if(count < 0) {
        setErrorText(QString("read edge events failed: %1").arg(strerror(errno)));
        return;
    }

    QList<PendingEdgeEvent> pending;
    for(int i = 0;i < count;i++) {
        gpiod_edge_event* event =
            gpiod_edge_event_buffer_get_event(entry.eventBuffer, static_cast<unsigned long>(i));
        if(event == nullptr) {
            continue;
        }

        PendingEdgeEvent item;
        item.offset = gpiod_edge_event_get_line_offset(event);
        item.edge = gpiod_edge_event_get_event_type(event) == GPIOD_EDGE_EVENT_RISING_EDGE
                    ? Gpio::Edge::Rising
                    : Gpio::Edge::Falling;
        pending.append(item);
    }

    for(const PendingEdgeEvent& item : pending) {
        if(_requests.contains(handle) == false) {
            break;
        }
        emit edgeEvent(item.offset, item.edge);
    }
}

#include "Kanoop/pi/moc_libgpiodbackend.cpp"
