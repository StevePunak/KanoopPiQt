#include <QDir>
#include <QTest>

#include <Kanoop/pi/libgpiodbackend.h>

class TstLibGpiodBackend : public QObject
{
    Q_OBJECT

private:
    /** @brief Returns the path of the first chip that actually opens, or an empty string. */
    static QString openableChipPath()
    {
        QStringList paths = LibGpiodBackend::chipPaths();
        for(const QString& path : paths) {
            LibGpiodBackend backend;
            if(backend.openChipByPath(path) == true) {
                backend.closeChip();
                return path;
            }
        }
        return QString();
    }

private slots:
    void chipPathsEnumeratesDevNodes()
    {
        QStringList paths = LibGpiodBackend::chipPaths();
        for(const QString& path : paths) {
            QVERIFY(path.startsWith(QString("/dev/gpiochip")));
        }
    }

    void openByPathFailsForAMissingDevice()
    {
        LibGpiodBackend backend;
        QCOMPARE(backend.openChipByPath(QString("/dev/gpiochip-does-not-exist")), false);
        QVERIFY(backend.errorText().isEmpty() == false);
        QCOMPARE(backend.isOpen(), false);
    }

    void openByLabelFailsForAnUnknownLabel()
    {
        LibGpiodBackend backend;
        QCOMPARE(backend.openChipByLabel(QString("no-such-label")), false);
        QCOMPARE(backend.isOpen(), false);
    }

    void requestFailsBeforeOpen()
    {
        LibGpiodBackend backend;
        Gpio::OutputRequest request;
        request.consumer = QString("tst");
        request.offsets = QList<quint32>() << 5;

        QCOMPARE(backend.requestOutputs(request), Gpio::InvalidRequest);
    }

    void openByLabelSucceedsOnHardware()
    {
        QString path = openableChipPath();
        if(path.isEmpty() == true) {
            QSKIP("no openable GPIO character device found on this host");
        }

        QString label = LibGpiodBackend::labelOf(path);
        QVERIFY(label.isEmpty() == false);

        LibGpiodBackend backend;
        QVERIFY(backend.openChipByLabel(label));
        QCOMPARE(backend.isOpen(), true);
        backend.closeChip();
        QCOMPARE(backend.isOpen(), false);
    }
};

QTEST_MAIN(TstLibGpiodBackend)
#include "tst_libgpiodbackend.moc"
