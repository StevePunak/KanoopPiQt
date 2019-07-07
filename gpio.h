#ifndef GPIO_H
#define GPIO_H


class GPIO
{
public:
    enum Pin
    {
        Unset = 0,

        Pin01 = 1,
        Pin02 = 2,
        Pin03 = 3,
        Pin04 = 4,
        Pin05 = 5,
        Pin06 = 6,
        Pin07 = 7,
        Pin08 = 8,
        Pin09 = 9,
        Pin10 = 10,
        Pin11 = 11,
        Pin12 = 12,
        Pin13 = 13,
        Pin14 = 14,
        Pin15 = 15,
        Pin16 = 16,
        Pin17 = 17,
        Pin18 = 18,
        Pin19 = 19,
        Pin20 = 20,
        Pin21 = 21,
        Pin22 = 22,
        Pin23 = 23,
        Pin24 = 24,
        Pin25 = 25,
        Pin26 = 26,
        Pin27 = 27,
        Pin28 = 28,
        Pin29 = 29,
    };

    enum PinState
    {
        Low = 0,
        High = 1
    };

    enum PinMode
    {
        Input = 'r',
        Output = 'w'
    };
};

#endif // GPIO_H
