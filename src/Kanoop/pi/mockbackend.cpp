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
    _liveHandles.clear();
}

namespace
{

bool offsetsAreShapedForRequest(const QList<quint32>& offsets, QString* errorText)
{
    if(offsets.isEmpty() == true) {
        *errorText = QString("request has no offsets");
        return false;
    }

    QSet<quint32> unique(offsets.begin(), offsets.end());
    if(unique.count() != offsets.count()) {
        *errorText = QString("request contains duplicate offsets");
        return false;
    }

    return true;
}

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

    QString shapeError;
    if(offsetsAreShapedForRequest(request.offsets, &shapeError) == false) {
        setErrorText(shapeError);
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
    _liveHandles.insert(handle);
    _lastOutputRequest = request;
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

    QString shapeError;
    if(offsetsAreShapedForRequest(request.offsets, &shapeError) == false) {
        setErrorText(shapeError);
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
    }
    _liveHandles.insert(handle);
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
    _liveHandles.remove(handle);
}

bool MockBackend::setValues(Gpio::RequestHandle handle,
                            const QList<quint32>& offsets,
                            const QList<Gpio::Value>& values)
{
    _setValuesCallCount++;
    _lastSetOffsets = offsets;

    if(_open == false) {
        setErrorText(QString("chip is not open"));
        return false;
    }

    if(_liveHandles.contains(handle) == false) {
        setErrorText(QString("unknown request handle"));
        return false;
    }

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

bool MockBackend::getValues(Gpio::RequestHandle handle,
                            const QList<quint32>& offsets,
                            QList<Gpio::Value>& values)
{
    if(_liveHandles.contains(handle) == false) {
        setErrorText(QString("unknown request handle"));
        return false;
    }

    QList<Gpio::Value> result;
    for(quint32 offset : offsets) {
        if(_owners.value(offset, Gpio::InvalidRequest) != handle) {
            setErrorText(QString("line %1 is not held by this request").arg(offset));
            return false;
        }
        result.append(_values.value(offset, Gpio::Value::Inactive));
    }

    values = result;
    return true;
}

void MockBackend::simulateEdge(quint32 offset, Gpio::Edge edge)
{
    emit edgeEvent(offset, edge);
}

#include "Kanoop/pi/moc_mockbackend.cpp"
