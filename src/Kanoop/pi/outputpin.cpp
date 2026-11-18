#include "Kanoop/pi/outputpin.h"

OutputPin::OutputPin(IGpioBackend* backend,
                     const QString& consumer,
                     quint32 offset,
                     bool activeLow,
                     QObject* parent) :
    QObject(parent),
    _offset(offset),
    _bank(backend, consumer, QList<quint32>() << offset, activeLow)
{
}

#include "Kanoop/pi/moc_outputpin.cpp"
