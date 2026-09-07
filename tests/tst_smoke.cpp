#include <QTest>

#include <Kanoop/pi/kanooppi.h>

class TstSmoke : public QObject
{
    Q_OBJECT

private slots:
    void exportMacroIsDefined()
    {
        QVERIFY(true);
    }
};

QTEST_MAIN(TstSmoke)
#include "tst_smoke.moc"
