#include "Kanoop/pi/mockbackend.h"

MockBackend::MockBackend(QObject* parent) :
    IGpioBackend(parent)
{
}

bool MockBackend::openChipByLabel(const QString& label)
{
    _open = true;
    _openedWith = label;
    return true;
}

bool MockBackend::openChipByPath(const QString& path)
{
    _open = true;
    _openedWith = path;
    return true;
}

void MockBackend::closeChip()
{
    _open = false;
    _owners.clear();
    _values.clear();
}

Gpio::RequestHandle MockBackend::requestOutputs(const Gpio::OutputRequest& request)
{
    if(_open == false) {
        setErrorText(QString("chip is not open"));
        return Gpio::InvalidRequest;
    }

    if(_failNextRequest == true) {
        _failNextRequest = false;
        setErrorText(QString("injected failure"));
        return Gpio::InvalidRequest;
    }

    for(quint32 offset : request.offsets) {
        if(_owners.contains(offset) == true) {
            setErrorText(QString("line %1 is already requested").arg(offset));
            return Gpio::InvalidRequest;
        }
    }

    Gpio::RequestHandle handle = _nextHandle++;
    for(quint32 offset : request.offsets) {
        _owners.insert(offset, handle);
        _values.insert(offset, request.initialValue);
    }
    return handle;
}

Gpio::RequestHandle MockBackend::requestInputs(const Gpio::InputRequest& request)
{
    if(_open == false) {
        setErrorText(QString("chip is not open"));
        return Gpio::InvalidRequest;
    }

    if(_failNextRequest == true) {
        _failNextRequest = false;
        setErrorText(QString("injected failure"));
        return Gpio::InvalidRequest;
    }

    Gpio::RequestHandle handle = _nextHandle++;
    for(quint32 offset : request.offsets) {
        _owners.insert(offset, handle);
    }
    _lastInputRequest = request;
    return handle;
}

void MockBackend::release(Gpio::RequestHandle handle)
{
    QList<quint32> offsets = _owners.keys(handle);
    for(quint32 offset : offsets) {
        _owners.remove(offset);
        _values.remove(offset);
    }
}

bool MockBackend::setValues(Gpio::RequestHandle handle,
                            const QList<quint32>& offsets,
                            const QList<Gpio::Value>& values)
{
    _setValuesCallCount++;
    _lastSetOffsets = offsets;

    if(offsets.count() != values.count()) {
        setErrorText(QString("offset and value counts differ"));
        return false;
    }

    for(int i = 0;i < offsets.count();i++) {
        if(_owners.value(offsets.at(i), Gpio::InvalidRequest) != handle) {
            setErrorText(QString("line %1 is not held by this request").arg(offsets.at(i)));
            return false;
        }
    }

    for(int i = 0;i < offsets.count();i++) {
        _values.insert(offsets.at(i), values.at(i));
    }
    return true;
}

void MockBackend::simulateEdge(quint32 offset, Gpio::Edge edge)
{
    emit edgeEvent(offset, edge);
}

#include "Kanoop/pi/moc_mockbackend.cpp"
