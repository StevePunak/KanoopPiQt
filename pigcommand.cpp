#include "pigcommand.h"
#include <QDataStream>

QByteArray PigCommand::serialize()
{
    const int BASE_SIZE = sizeof(quint32) * 4;

    int size = BASE_SIZE;
    if(hasExtended())
    {
        foreach(ExtendedParameter item, _extendedItems)
        {
            size += item.length();
        }
    }

    QByteArray buffer;
    buffer.reserve(size);
    QDataStream outstream(&buffer, QIODevice::WriteOnly);
    outstream.setByteOrder(QDataStream::ByteOrder::LittleEndian);

    outstream << (quint32)_type;
    outstream << (quint32)_pin;
    outstream << _p1;
    outstream << _p2;
    if(hasExtended())
    {
        foreach(ExtendedParameter item, _extendedItems)
        {
            switch(item.length())
            {
            case 2:
                outstream << (quint16)item.value();
                break;
            case 4:
                outstream << (quint32)item.value();
                break;
            case 8:
                outstream << (quint64)item.value();
                break;
            }
        }
    }
    return buffer;
}

QString PigCommand::toString()
{
    return QString("%1 Pin %2 %3 %4").arg(_type).arg(_pin).arg(_p1).arg(_p2);
}
