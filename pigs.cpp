#include <QDebug>
#include <QMutex>
#include "pigs.h"
#include "klog.h"

Pigs* Pigs::_instance = nullptr;

Pigs::Pigs()
{
    connect(&_socket, QOverload<QAbstractSocket::SocketState>::of(&QAbstractSocket::stateChanged), this, &Pigs::handleSocketStateChange);
    connect(&_socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, &Pigs::handleSocketError);
    connect(&_socket, &QTcpSocket::connected, this, &Pigs::handleSocketConnected);
    connect(this, &Pigs::writeCommand, this, &Pigs::handleWriteCommand);
}

Pigs *Pigs::instance()
{
    if(_instance == nullptr)
    {
        _instance = new Pigs();
    }
    return _instance;
}

void Pigs::setAddress(const QString &address, quint16 port)
{
    if(instance()->_socket.state() == QTcpSocket::ConnectedState && (instance()->_hostAddress != address || instance()->_hostPort != port))
    {
        _instance->_socket.disconnect();
    }

    _instance->_hostAddress = address;
    _instance->_hostPort = port;
    if(_instance->_socket.state() == QTcpSocket::UnconnectedState)
    {
        _instance->_socket.connectToHost(address, port);
        if(!_instance->_socket.waitForConnected())
        {
            qDebug() << "Could not connect pigs socket " << _instance->_socket.state();
        }

    }
}

void Pigs::handleSocketStateChange(QAbstractSocket::SocketState state)
{
    Q_UNUSED(state);
}

void Pigs::handleSocketError(QAbstractSocket::SocketError error)
{
    Q_UNUSED(error);
}

void Pigs::handleSocketConnected()
{
}

void Pigs::handleSocketDisconnected()
{
}

void Pigs::handleWriteCommand(QByteArray commandData)
{
    if(KLog::systemVerbosity() > 1)
        KLog::sysLogHex(commandData);
    _socket.write(commandData);
    _socket.waitForBytesWritten(5000);
}

void Pigs::SetMode(GPIO::Pin gpioPin, GPIO::PinMode mode)
{
#ifdef DEBUG_PIGS
    qDebug() << QString("PIGS set mode %1 %2)").arg(gpioPin).arg(mode);
#endif

    PigCommand command(PigCommand::CommandType::MODES, gpioPin, (char)mode);
    instance()->sendCommand(command);
}

void Pigs::SetHardwarePWM(GPIO::Pin gpioPin, quint32 frequency, quint32 dutyCyclePercent)
{
#ifdef DEBUG_PIGS
    qDebug() << QString("PIGS set Hardware PWM %1 %2 %3").arg(gpioPin).arg(frequency).arg(dutyCyclePercent);
#endif
    if(dutyCyclePercent <= 100)
    {
        PigCommand command(PigCommand::CommandType::HP, gpioPin, frequency, dutyCyclePercent * 10000);
        instance()->sendCommand(command);
    }
    else
    {
        qDebug() << "Illegal PWM value";
    }
}

void Pigs::SetPWM(GPIO::Pin gpioPin, quint32 dutyCycle)
{
#ifdef DEBUG_PIGS
    qDebug() << QString("PIGS set PWM duty cycle %1 %2").arg(gpioPin).arg(dutyCycle);
#endif

    PigCommand command(PigCommand::CommandType::PWM, gpioPin, dutyCycle);
    instance()->sendCommand(command);
}

void Pigs::SetServoPosition(GPIO::Pin gpioPin, int position)
{
    SetServoPosition(gpioPin, (quint32)position);
}

void Pigs::SetServoPosition(GPIO::Pin gpioPin, quint32 position)
{
#ifdef DEBUG_PIGS
    qDebug() << QString("Set servo %1 to %2").arg(gpioPin).arg(position);
#endif

    PigCommand command(PigCommand::CommandType::SERVO, gpioPin, position);
    instance()->sendCommand(command);
}

void Pigs::SetOutputPin(GPIO::Pin gpioPin, GPIO::PinState value)
{
#ifdef DEBUG_PIGS
    qDebug() << QString("*********** PIGS output pin %1 %2").arg(gpioPin).arg(value);
#endif

    PigCommand command(PigCommand::CommandType::WRITE, gpioPin, (quint32)value);
    instance()->sendCommand(command);
}

void Pigs::SetOutputPin(GPIO::Pin gpioPin, bool value)
{
#ifdef DEBUG_PIGS
    qDebug() << QString(">>>>>>>>>>>>>>>> PIGS output pin %1 %2").arg(gpioPin).arg(value);
#endif

    PigCommand command(PigCommand::CommandType::WRITE, gpioPin, value ? (quint32)1 : (quint32)0);
    instance()->sendCommand(command);
}

void Pigs::sendCommand(PigCommand& command)
{
    if(_socket.state() == QTcpSocket::ConnectedState)
    {
        QByteArray serialized = command.serialize();
        emit writeCommand(serialized);
    }
    else
    {
        qDebug() << "Pigs socket not connected " << _socket.state();
    }
}

void Pigs::stop()
{
    if(instance()->_socket.state() == QTcpSocket::ConnectedState)
    {
        instance()->_socket.disconnect();
    }
}


