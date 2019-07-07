#ifndef PIGS_H
#define PIGS_H
#include <QObject>
#include <QTcpSocket>
#include <QString>
#include <QWaitCondition>
#include <QThread>
#include "pigcommand.h"

class Pigs : public QObject
{
    Q_OBJECT

public:
    static void setAddress(const QString& address, quint16 port);

    static void SetMode(GPIO::Pin gpioPin, GPIO::PinMode mode);
    static void SetHardwarePWM(GPIO::Pin gpioPin, quint32 frequency, quint32 dutyCyclePercent);
    static void SetPWM(GPIO::Pin gpioPin, quint32 dutyCycle);
    static void SetServoPosition(GPIO::Pin gpioPin, int position);
    static void SetServoPosition(GPIO::Pin gpioPin, quint32 position);
    static void SetOutputPin(GPIO::Pin gpioPin, GPIO::PinState value);
    static void SetOutputPin(GPIO::Pin gpioPin, bool value);

    static void stop();

private slots:
    void handleSocketStateChange(QTcpSocket::SocketState state);
    void handleSocketError(QTcpSocket::SocketError error);
    void handleSocketConnected();
    void handleSocketDisconnected();

private:
    Pigs();
    static Pigs* instance();

    void sendCommand(PigCommand& command);

    static Pigs* _instance;
    bool _connected;
    QString _hostAddress;
    quint16 _hostPort;
    QTcpSocket _socket;
    QWaitCondition _connectEvent;
    QThread _thread;
};

#endif // PIGS_H
