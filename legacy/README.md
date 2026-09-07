# Unported code

These files predate the libgpiod v2 conversion and are excluded from the build.

- `i2c.{h,cpp}` — a stub over the I2C ioctls, never completed.
- `devices/ads1115.{h,cpp}` — ADS1115 ADC driver.
- `devices/bmp280.{h,cpp}` — BMP280 pressure/temperature driver.

Port them when a consuming project needs them. They were written against the
pigpio daemon's transport, which no longer exists in this library.
