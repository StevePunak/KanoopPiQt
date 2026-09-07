#include "Kanoop/pi/outputbank.h"

OutputBank::OutputBank(IGpioBackend* backend,
                       const QString& consumer,
                       const QList<quint32>& offsets,
                       bool activeLow,
                       QObject* parent) :
    QObject(parent),
    _backend(backend), _consumer(consumer),
    _offsets(offsets), _activeLow(activeLow)
{
}

OutputBank::~OutputBank()
{
    release();
}

bool OutputBank::request()
{
    if(isRequested() == true) {
        return true;
    }

    Gpio::OutputRequest request;
    request.consumer = _consumer;
    request.offsets = _offsets;
    request.activeLow = _activeLow;
    request.initialValue = Gpio::Value::Inactive;

    _handle = _backend->requestOutputs(request);
    if(_handle == Gpio::InvalidRequest) {
        _errorText = _backend->errorText();
        return false;
    }

    for(quint32 offset : _offsets) {
        _active.insert(offset, false);
    }
    return true;
}

void OutputBank::release()
{
    if(isRequested() == false) {
        return;
    }

    _backend->release(_handle);
    _handle = Gpio::InvalidRequest;
    _active.clear();
}

bool OutputBank::setValue(quint32 offset, bool active)
{
    QMap<quint32, bool> values;
    values.insert(offset, active);
    return setValues(values);
}

bool OutputBank::setValues(const QMap<quint32, bool>& values)
{
    if(isRequested() == false) {
        _errorText = QString("bank is not requested");
        return false;
    }

    QList<quint32> offsets = values.keys();
    if(ownsAll(offsets) == false) {
        return false;
    }

    QList<Gpio::Value> lineValues;
    for(quint32 offset : offsets) {
        lineValues.append(values.value(offset) == true
                          ? Gpio::Value::Active
                          : Gpio::Value::Inactive);
    }

    if(_backend->setValues(_handle, offsets, lineValues) == false) {
        _errorText = _backend->errorText();
        return false;
    }

    for(quint32 offset : offsets) {
        _active.insert(offset, values.value(offset));
    }
    return true;
}

bool OutputBank::allInactive()
{
    QMap<quint32, bool> values;
    for(quint32 offset : _offsets) {
        values.insert(offset, false);
    }
    return setValues(values);
}

bool OutputBank::ownsAll(const QList<quint32>& offsets)
{
    for(quint32 offset : offsets) {
        if(_offsets.contains(offset) == false) {
            _errorText = QString("line %1 is not in this bank").arg(offset);
            return false;
        }
    }
    return true;
}

#include "Kanoop/pi/moc_outputbank.cpp"
