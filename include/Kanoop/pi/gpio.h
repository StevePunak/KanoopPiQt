#ifndef KANOOP_PI_GPIO_H
#define KANOOP_PI_GPIO_H

#include <QList>
#include <QString>

#include <Kanoop/pi/kanooppi.h>

/** @brief Scoping holder for GPIO enumerations and request descriptors. */
class KANOOPPI_EXPORT Gpio
{
public:
    /** @brief Logical line level, after any active-low inversion. */
    enum class Value
    {
        Inactive = 0,
        Active = 1
    };

    /** @brief Internal bias applied to an input line. */
    enum class Bias
    {
        AsIs,
        Disabled,
        PullUp,
        PullDown
    };

    /** @brief Edge detection selection for an input line. */
    enum class Edge
    {
        None,
        Rising,
        Falling,
        Both
    };

    /** @brief Opaque identifier for a set of requested lines. */
    typedef quint64 RequestHandle;

    /** @brief Value returned by a failed request. */
    static constexpr RequestHandle InvalidRequest = 0;

    /** @brief Parameters for requesting a set of output lines. */
    class OutputRequest
    {
    public:
        QString consumer;
        QList<quint32> offsets;
        bool activeLow = false;
        Value initialValue = Value::Inactive;
    };

    /** @brief Parameters for requesting a set of input lines. */
    class InputRequest
    {
    public:
        QString consumer;
        QList<quint32> offsets;
        bool activeLow = false;
        Bias bias = Bias::AsIs;
        Edge edge = Edge::None;
        unsigned long debounceMicroseconds = 0;
    };
};

Q_DECLARE_METATYPE(Gpio::Value)
Q_DECLARE_METATYPE(Gpio::Bias)
Q_DECLARE_METATYPE(Gpio::Edge)

#endif // KANOOP_PI_GPIO_H
