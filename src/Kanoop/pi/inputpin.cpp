#include "Kanoop/pi/inputpin.h"

InputPin::InputPin(IGpioBackend* backend,
                   const QString& consumer,
                   quint32 offset,
                   QObject* parent) :
    QObject(parent),
    _backend(backend), _consumer(consumer), _offset(offset)
{
    connect(_backend, &IGpioBackend::edgeEvent, this, &InputPin::onBackendEdgeEvent);
}

InputPin::~InputPin()
{
    release();
}

bool InputPin::request()
{
    if(isRequested() == true) {
        return true;
    }

    Gpio::InputRequest request;
    request.consumer = _consumer;
    request.offsets = QList<quint32>() << _offset;
    request.activeLow = _activeLow;
    request.bias = _bias;
    request.edge = _edge;
    request.debounceMicroseconds = static_cast<unsigned long>(qMax(0.0, _debounce.totalMicroseconds()));

    _handle = _backend->requestInputs(request);
    if(_handle == Gpio::InvalidRequest) {
        _errorText = _backend->errorText();
        return false;
    }
    return true;
}

void InputPin::release()
{
    if(isRequested() == false) {
        return;
    }

    _backend->release(_handle);
    _handle = Gpio::InvalidRequest;
}

bool InputPin::isAsserted(bool* ok)
{
    if(ok != nullptr) {
        *ok = false;
    }

    if(isRequested() == false) {
        _errorText = QString("Line is not requested");
        return false;
    }

    QList<Gpio::Value> values;
    if(_backend->getValues(_handle, QList<quint32>() << _offset, values) == false) {
        _errorText = _backend->errorText();
        return false;
    }

    if(ok != nullptr) {
        *ok = true;
    }
    return values.first() == Gpio::Value::Active;
}

void InputPin::onBackendEdgeEvent(quint32 offset, Gpio::Edge edgeType)
{
    if(offset != _offset || isRequested() == false) {
        return;
    }

    emit edge(edgeType);

    if(edgeType == Gpio::Edge::Rising) {
        emit asserted();
    }
    else if(edgeType == Gpio::Edge::Falling) {
        emit deasserted();
    }
}

#include "Kanoop/pi/moc_inputpin.cpp"
