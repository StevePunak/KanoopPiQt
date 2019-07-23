#ifndef BMP280_H
#define BMP280_H
#include <QObject>
#include <QThread>
#include <QTimer>

class BMP280 : public QObject
{
    Q_OBJECT

public:
    BMP280(quint8 address) :
        QObject(),
        _address(address),
        _fd(0) {}

    bool start();
    bool stop();

private:
    quint8 _address;
    int _fd;
    QThread _thread;
    QTimer _timer;

signals:
    void temperature(qreal value);
    void pressure(qreal value);

private slots:
    void handleTimer();
};

#endif // BMP280_H
