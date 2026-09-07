///////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <sys/types.h> // open
#include <sys/stat.h>  // open
#include <fcntl.h>     // open
#include <sys/ioctl.h>
#include <unistd.h>    // read/write usleep
#include <stdlib.h>    // exit
#include <inttypes.h>  // uint8_t, etc
#include <linux/i2c-dev.h> // I2C bus definitions
////////////////////////////////////////////////
#include "ads1115.h"
#include "i2c.h"
#include "klog.h"

#include <wiringPi.h>
#include <ads1115.h>

#define	AD_BASE 120

qreal ADS1115::_gainDivisors[6] =
{
    6.144, 4.096, 2.048, 1.024, 0.512, 0.256
};

ADS1115::ADS1115(quint8 address) :
    QObject(),
    _address(address),
    _gain(Gain::GAIN_6_144),
    _interval(500),
    _nextInputPin(InputPin::A0)
{
}

bool ADS1115::start()
{
    int result = wiringPiSetup();
    result = ads1115Setup(AD_BASE,0x48);

    setGain(_gain);

    connect(&_timer, &QTimer::timeout, this, &ADS1115::handleTimer);
    connect(&_thread, &QThread::started, this, &ADS1115::handleThreadStarted);
    _thread.setObjectName("BMP280");
    _timer.moveToThread(&_thread);
    moveToThread(&_thread);
    _thread.start();

    return result == true;
}

void ADS1115::handleTimer()
{
    int error = 0;

    for(int pin = InputPin::A0;pin <= InputPin::A3;pin++)
    {
        int result = analogRead(AD_BASE+pin);
        float voltage = (float)result*_gainDivisors[_gain] / 32768.0;
        switch(pin)
        {
        case InputPin::A0:
            emit voltageA0(voltage);
            break;
        case InputPin::A1:
            emit voltageA1(voltage);
            break;
        case InputPin::A2:
            emit voltageA2(voltage);
            break;
        case InputPin::A3:
            emit voltageA3(voltage);
            break;
        }
    }
    if(error == 0)
        _timer.start(_interval);

}

void ADS1115::handleThreadStarted()
{
    _timer.start(1000);
}

bool ADS1115::stop()
{
    _timer.stop();
    return true;
}

void ADS1115::setGain(ADS1115::Gain gain)
{
    if(gain < 0 || gain > 5)
        return;
    digitalWrite(AD_BASE, gain);
}

