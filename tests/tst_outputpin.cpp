#include <QTest>

#include <Kanoop/pi/mockbackend.h>
#include <Kanoop/pi/outputpin.h>

class TstOutputPin : public QObject
{
    Q_OBJECT

private slots:
    void requestStartsInactive()
    {
        MockBackend backend;
        backend.openChipByLabel(QString("test"));
        OutputPin pin(&backend, QString("test"), 25, true);

        QVERIFY(pin.request());
        QVERIFY(pin.isRequested());
        QCOMPARE(pin.isActive(), false);
        QCOMPARE(backend.lineValue(25), Gpio::Value::Inactive);
    }

    void setActiveDrivesTheLine()
    {
        MockBackend backend;
        backend.openChipByLabel(QString("test"));
        OutputPin pin(&backend, QString("test"), 25, true);
        pin.request();

        QVERIFY(pin.setActive(true));
        QCOMPARE(pin.isActive(), true);
        QCOMPARE(backend.lineValue(25), Gpio::Value::Active);
    }

    void toggleInvertsTheCurrentValue()
    {
        MockBackend backend;
        backend.openChipByLabel(QString("test"));
        OutputPin pin(&backend, QString("test"), 25, true);
        pin.request();

        QVERIFY(pin.toggle());
        QCOMPARE(pin.isActive(), true);
        QVERIFY(pin.toggle());
        QCOMPARE(pin.isActive(), false);
    }

    void offsetIsReported()
    {
        MockBackend backend;
        OutputPin pin(&backend, QString("test"), 25, true);
        QCOMPARE(pin.offset(), 25u);
    }

    void destructorReleasesTheLine()
    {
        MockBackend backend;
        backend.openChipByLabel(QString("test"));
        {
            OutputPin pin(&backend, QString("test"), 25, true);
            pin.request();
            QCOMPARE(backend.isRequested(25), true);
        }
        QCOMPARE(backend.isRequested(25), false);
    }
};

QTEST_MAIN(TstOutputPin)
#include "tst_outputpin.moc"
