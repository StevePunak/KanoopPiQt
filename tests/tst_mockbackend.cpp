#include <QSignalSpy>
#include <QTest>

#include <Kanoop/pi/mockbackend.h>

class TstMockBackend : public QObject
{
    Q_OBJECT

private slots:
    void requestFailsWhenChipIsClosed()
    {
        MockBackend backend;
        Gpio::OutputRequest request;
        request.offsets = QList<quint32>() << 5;

        QCOMPARE(backend.requestOutputs(request), Gpio::InvalidRequest);
        QVERIFY(backend.errorText().isEmpty() == false);
    }

    void requestRecordsInitialValue()
    {
        MockBackend backend;
        QVERIFY(backend.openChipByLabel(QString("test")));

        Gpio::OutputRequest request;
        request.offsets = QList<quint32>() << 5 << 6;
        request.initialValue = Gpio::Value::Active;

        Gpio::RequestHandle handle = backend.requestOutputs(request);
        QVERIFY(handle != Gpio::InvalidRequest);
        QCOMPARE(backend.lineValue(5), Gpio::Value::Active);
        QCOMPARE(backend.lineValue(6), Gpio::Value::Active);
        QVERIFY(backend.isRequested(5));
    }

    void secondRequestOfTheSameLineFails()
    {
        MockBackend backend;
        backend.openChipByLabel(QString("test"));

        Gpio::OutputRequest first;
        first.offsets = QList<quint32>() << 5;
        QVERIFY(backend.requestOutputs(first) != Gpio::InvalidRequest);

        Gpio::OutputRequest second;
        second.offsets = QList<quint32>() << 5;
        QCOMPARE(backend.requestOutputs(second), Gpio::InvalidRequest);
    }

    void secondRequestOfTheSameLineAsInputFails()
    {
        MockBackend backend;
        backend.openChipByLabel(QString("test"));

        Gpio::InputRequest first;
        first.offsets = QList<quint32>() << 5;
        QVERIFY(backend.requestInputs(first) != Gpio::InvalidRequest);

        Gpio::InputRequest second;
        second.offsets = QList<quint32>() << 5;
        QCOMPARE(backend.requestInputs(second), Gpio::InvalidRequest);
    }

    void requestInputsRejectsOutputOwnedLine()
    {
        MockBackend backend;
        backend.openChipByLabel(QString("test"));

        Gpio::OutputRequest output;
        output.offsets = QList<quint32>() << 5;
        QVERIFY(backend.requestOutputs(output) != Gpio::InvalidRequest);

        Gpio::InputRequest input;
        input.offsets = QList<quint32>() << 5;
        QCOMPARE(backend.requestInputs(input), Gpio::InvalidRequest);
    }

    void setValuesRejectsForeignLines()
    {
        MockBackend backend;
        backend.openChipByLabel(QString("test"));

        Gpio::OutputRequest firstReq;
        firstReq.offsets = QList<quint32>() << 5;
        Gpio::RequestHandle firstHandle = backend.requestOutputs(firstReq);

        Gpio::OutputRequest secondReq;
        secondReq.offsets = QList<quint32>() << 9;
        Gpio::RequestHandle secondHandle = backend.requestOutputs(secondReq);

        QList<quint32> offsets = QList<quint32>() << 9;
        QList<Gpio::Value> values = QList<Gpio::Value>() << Gpio::Value::Active;
        QCOMPARE(backend.setValues(firstHandle, offsets, values), false);
    }

    void releaseFreesTheLines()
    {
        MockBackend backend;
        backend.openChipByLabel(QString("test"));

        Gpio::OutputRequest request;
        request.offsets = QList<quint32>() << 5;
        Gpio::RequestHandle handle = backend.requestOutputs(request);

        backend.release(handle);
        QCOMPARE(backend.isRequested(5), false);
    }

    void simulateEdgeEmitsTheSignal()
    {
        MockBackend backend;
        QSignalSpy spy(&backend, &IGpioBackend::edgeEvent);

        backend.simulateEdge(25, Gpio::Edge::Falling);

        QCOMPARE(spy.count(), 1);
        QCOMPARE(spy.at(0).at(0).toUInt(), 25u);
        QCOMPARE(spy.at(0).at(1).value<Gpio::Edge>(), Gpio::Edge::Falling);
    }

    void failNextRequestIsOneShot()
    {
        MockBackend backend;
        backend.openChipByLabel(QString("test"));
        backend.setFailNextRequest(true);

        Gpio::OutputRequest request;
        request.offsets = QList<quint32>() << 5;

        QCOMPARE(backend.requestOutputs(request), Gpio::InvalidRequest);
        QVERIFY(backend.requestOutputs(request) != Gpio::InvalidRequest);
    }
};

QTEST_MAIN(TstMockBackend)
#include "tst_mockbackend.moc"
