#include <QTest>

#include <Kanoop/pi/mockbackend.h>
#include <Kanoop/pi/outputbank.h>

class TstOutputBank : public QObject
{
    Q_OBJECT

private:
    static QList<quint32> zoneOffsets()
    {
        return QList<quint32>() << 5 << 6 << 13 << 16;
    }

private slots:
    void requestOpensAllLinesInactive()
    {
        MockBackend backend;
        backend.openChipByLabel(QString("test"));
        OutputBank bank(&backend, QString("test"), zoneOffsets(), true);

        QVERIFY(bank.request());
        QVERIFY(bank.isRequested());
        for(quint32 offset : zoneOffsets()) {
            QCOMPARE(bank.isActive(offset), false);
        }
    }

    void requestFailurePropagates()
    {
        MockBackend backend;
        backend.openChipByLabel(QString("test"));
        backend.setFailNextRequest(true);
        OutputBank bank(&backend, QString("test"), zoneOffsets(), true);

        QCOMPARE(bank.request(), false);
        QCOMPARE(bank.isRequested(), false);
        QVERIFY(bank.errorText().isEmpty() == false);
    }

    void setValueDrivesOneLine()
    {
        MockBackend backend;
        backend.openChipByLabel(QString("test"));
        OutputBank bank(&backend, QString("test"), zoneOffsets(), true);
        bank.request();

        QVERIFY(bank.setValue(13, true));
        QCOMPARE(bank.isActive(13), true);
        QCOMPARE(bank.isActive(5), false);
        QCOMPARE(backend.lineValue(13), Gpio::Value::Active);
    }

    void setValuesIsOneBackendCall()
    {
        MockBackend backend;
        backend.openChipByLabel(QString("test"));
        OutputBank bank(&backend, QString("test"), zoneOffsets(), true);
        bank.request();

        int before = backend.setValuesCallCount();

        QMap<quint32, bool> values;
        values.insert(6, false);
        values.insert(13, true);
        QVERIFY(bank.setValues(values));

        QCOMPARE(backend.setValuesCallCount(), before + 1);
        QCOMPARE(bank.isActive(13), true);
        QCOMPARE(bank.isActive(6), false);
    }

    void setValueRejectsAnUnownedOffset()
    {
        MockBackend backend;
        backend.openChipByLabel(QString("test"));
        OutputBank bank(&backend, QString("test"), zoneOffsets(), true);
        bank.request();

        int before = backend.setValuesCallCount();
        QCOMPARE(bank.setValue(99, true), false);
        QCOMPARE(backend.setValuesCallCount(), before);
        QVERIFY(bank.errorText().contains(QString("99")));
    }

    void setValueFailsBeforeRequest()
    {
        MockBackend backend;
        backend.openChipByLabel(QString("test"));
        OutputBank bank(&backend, QString("test"), zoneOffsets(), true);

        QCOMPARE(bank.setValue(5, true), false);
    }

    void allInactiveClearsEveryLineInOneCall()
    {
        MockBackend backend;
        backend.openChipByLabel(QString("test"));
        OutputBank bank(&backend, QString("test"), zoneOffsets(), true);
        bank.request();
        bank.setValue(5, true);
        bank.setValue(16, true);

        int before = backend.setValuesCallCount();
        QVERIFY(bank.allInactive());

        QCOMPARE(backend.setValuesCallCount(), before + 1);
        QCOMPARE(backend.lastSetOffsets().count(), zoneOffsets().count());
        for(quint32 offset : zoneOffsets()) {
            QCOMPARE(bank.isActive(offset), false);
        }
    }

    void activeLowIsPassedToTheBackendRatherThanInvertedHere()
    {
        MockBackend backend;
        backend.openChipByLabel(QString("test"));
        OutputBank bank(&backend, QString("test"), zoneOffsets(), true);
        bank.request();
        bank.setValue(5, true);

        QCOMPARE(backend.lastOutputRequest().activeLow, true);
        QCOMPARE(backend.lineValue(5), Gpio::Value::Active);
    }

    void destructorReleasesTheLines()
    {
        MockBackend backend;
        backend.openChipByLabel(QString("test"));
        {
            OutputBank bank(&backend, QString("test"), zoneOffsets(), true);
            bank.request();
            QCOMPARE(backend.isRequested(5), true);
        }
        QCOMPARE(backend.isRequested(5), false);
    }

    void requestTwiceIsIdempotent()
    {
        MockBackend backend;
        backend.openChipByLabel(QString("test"));
        OutputBank bank(&backend, QString("test"), zoneOffsets(), true);

        QVERIFY(bank.request());
        QVERIFY(bank.request());
        QVERIFY(bank.isRequested());
        QVERIFY(bank.setValue(5, true));
        QCOMPARE(bank.isActive(5), true);
    }

    void releaseBeforeRequestIsHarmless()
    {
        MockBackend backend;
        backend.openChipByLabel(QString("test"));
        OutputBank bank(&backend, QString("test"), zoneOffsets(), true);

        bank.release();
        QCOMPARE(bank.isRequested(), false);
    }

    void readValuesReflectsTheKernelNotTheCache()
    {
        MockBackend backend;
        backend.openChipByLabel(QString("test"));
        OutputBank bank(&backend, QString("test"), QList<quint32>() << 5 << 6, false);

        QVERIFY(bank.request());
        QVERIFY(bank.setValue(5, true));

        backend.setLineValue(5, Gpio::Value::Inactive);

        QMap<quint32, bool> values;
        QVERIFY(bank.readValues(values));
        QCOMPARE(values.count(), 2);
        QCOMPARE(values.value(5), false);
        QCOMPARE(values.value(6), false);
        QCOMPARE(bank.isActive(5), true);
    }

    void readValuesFailsWhenNotRequested()
    {
        MockBackend backend;
        backend.openChipByLabel(QString("test"));
        OutputBank bank(&backend, QString("test"), QList<quint32>() << 5, false);

        QMap<quint32, bool> values;
        QVERIFY(bank.readValues(values) == false);
    }

    void releaseThenDestructorDoesNotDoubleRelease()
    {
        MockBackend backend;
        backend.openChipByLabel(QString("test"));
        {
            OutputBank bank(&backend, QString("test"), zoneOffsets(), true);
            bank.request();
            QCOMPARE(backend.isRequested(5), true);
            bank.release();
            QCOMPARE(backend.isRequested(5), false);
        }
        QCOMPARE(backend.isRequested(5), false);
    }
};

QTEST_MAIN(TstOutputBank)
#include "tst_outputbank.moc"
