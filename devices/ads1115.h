#ifndef ADS1115_H
#define ADS1115_H
#include <QObject>
#include <QThread>
#include <QTimer>

class ADS1115 : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int interval MEMBER _interval READ interval WRITE setInterval);

public:
    enum InputPin { A0, A1, A2, A3 };
    enum Gain { GAIN_6_144, GAIN_4_096, GAIN_2_048, GAIN_1_024, GAIN_0_512, GAIN_0_256 };

    explicit ADS1115(quint8 address);

    bool start();
    bool stop();

    int interval() const { return _interval; }
    void setInterval(int interval) { _interval = interval; }

    Gain gain() const { return _gain; }
    void setGain(Gain gain);

private:
    quint8 _address;
    Gain _gain;
    QThread _thread;
    QTimer _timer;
    int _interval;
    InputPin _nextInputPin;

    static qreal _gainDivisors[6];

signals:
    void voltageA0(qreal value);
    void voltageA1(qreal value);
    void voltageA2(qreal value);
    void voltageA3(qreal value);

private slots:
    void handleTimer();
    void handleThreadStarted();
};


#endif // ADS1115_H
