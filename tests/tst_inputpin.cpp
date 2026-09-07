#include <QSignalSpy>
#include <QTest>

#include <Kanoop/timespan.h>
#include <Kanoop/pi/inputpin.h>
#include <Kanoop/pi/mockbackend.h>

class TstInputPin : public QObject
{
    Q_OBJECT

private slots:
    void requestPassesBiasEdgeAndDebounce()
    {
        MockBackend backend;
        backend.openChipByLabel(QString("test"));

        InputPin pin(&backend, QString("test"), 25);
        pin.setBias(Gpio::Bias::PullUp);
        pin.setEdge(Gpio::Edge::Falling);
        pin.setDebounce(TimeSpan::fromMilliseconds(20));
        pin.setActiveLow(true);

        QVERIFY(pin.request());

        Gpio::InputRequest request = backend.lastInputRequest();
        QCOMPARE(request.bias, Gpio::Bias::PullUp);
        QCOMPARE(request.edge, Gpio::Edge::Falling);
        QCOMPARE(request.debounceMicroseconds, 20000ul);
        QCOMPARE(request.activeLow, true);
        QCOMPARE(request.offsets.count(), 1);
        QCOMPARE(request.offsets.at(0), 25u);
    }

    void requestFailurePropagates()
    {
        MockBackend backend;
        backend.openChipByLabel(QString("test"));
        backend.setFailNextRequest(true);

        InputPin pin(&backend, QString("test"), 25);
        QCOMPARE(pin.request(), false);
        QVERIFY(pin.errorText().isEmpty() == false);
    }

    void edgeOnThisOffsetIsForwarded()
    {
        MockBackend backend;
        backend.openChipByLabel(QString("test"));
        InputPin pin(&backend, QString("test"), 25);
        pin.request();

        QSignalSpy spy(&pin, &InputPin::edge);
        backend.simulateEdge(25, Gpio::Edge::Falling);

        QCOMPARE(spy.count(), 1);
        QCOMPARE(spy.at(0).at(0).value<Gpio::Edge>(), Gpio::Edge::Falling);
    }

    void edgeOnAnotherOffsetIsIgnored()
    {
        MockBackend backend;
        backend.openChipByLabel(QString("test"));
        InputPin pin(&backend, QString("test"), 25);
        pin.request();

        QSignalSpy spy(&pin, &InputPin::edge);
        backend.simulateEdge(26, Gpio::Edge::Falling);

        QCOMPARE(spy.count(), 0);
    }

    void activeLowMapsFallingToAsserted()
    {
        MockBackend backend;
        backend.openChipByLabel(QString("test"));
        InputPin pin(&backend, QString("test"), 25);
        pin.setActiveLow(true);
        pin.request();

        QSignalSpy asserted(&pin, &InputPin::asserted);
        QSignalSpy deasserted(&pin, &InputPin::deasserted);

        backend.simulateEdge(25, Gpio::Edge::Falling);
        QCOMPARE(asserted.count(), 1);
        QCOMPARE(deasserted.count(), 0);

        backend.simulateEdge(25, Gpio::Edge::Rising);
        QCOMPARE(deasserted.count(), 1);
    }

    void activeHighMapsRisingToAsserted()
    {
        MockBackend backend;
        backend.openChipByLabel(QString("test"));
        InputPin pin(&backend, QString("test"), 25);
        pin.setActiveLow(false);
        pin.request();

        QSignalSpy asserted(&pin, &InputPin::asserted);
        backend.simulateEdge(25, Gpio::Edge::Rising);

        QCOMPARE(asserted.count(), 1);
    }

    void destructorReleasesTheLine()
    {
        MockBackend backend;
        backend.openChipByLabel(QString("test"));
        {
            InputPin pin(&backend, QString("test"), 25);
            pin.request();
            QCOMPARE(backend.isRequested(25), true);
        }
        QCOMPARE(backend.isRequested(25), false);
    }
};

QTEST_MAIN(TstInputPin)
#include "tst_inputpin.moc"
