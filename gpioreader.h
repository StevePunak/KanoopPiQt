#ifndef GPIOREADER_H
#define GPIOREADER_H
#include <QObject>
#include <QSocketNotifier>

class GPIOReader : public QObject
{
    Q_OBJECT

public:
    GPIOReader();

public slots:
    void handleSigHup(int fd);

private:
    static void hupSignalHandler(void);

    static int sighupFd[2];
    QSocketNotifier *snHup;
};

#endif // GPIOREADER_H
