#include "api-gpio.h"

osMutexId_t gpioInitMutex;

bool gpio_api_init(void) {
    gpioInitMutex = osMutexNew(NULL);
    if(gpioInitMutex == NULL) return false;
    return true;
}

// init GPIO
void gpio_init(GpioPin* gpio, GpioMode mode) {
    if(osMutexAcquire(gpioInitMutex, osWaitForever) == osOK) {
        hal_gpio_init(gpio, mode, GpioPullNo, GpioSpeedLow);
        osMutexRelease(gpioInitMutex);
    }
}

// init GPIO, extended version
void gpio_init_ex(GpioPin* gpio, GpioMode mode, GpioPull pull, GpioSpeed speed) {
    hal_gpio_init(gpio, mode, pull, speed);
}

// put GPIO to Z-state
void gpio_disable(GpioDisableRecord* gpio_record) {
    GpioPin* gpio_pin = acquire_mutex(gpio_record->gpio_mutex, 0);
    if(gpio_pin == NULL) {
        gpio_pin = gpio_record->gpio;
    }
    hal_gpio_init(gpio_pin, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    release_mutex(gpio_record->gpio_mutex, gpio_pin);
}

// get GPIO record
ValueMutex* gpio_open_mutex(const char* name) {
    ValueMutex* gpio_mutex = (ValueMutex*)furi_open(name);

    // TODO disable gpio on app exit
    //if(gpio_mutex != NULL) flapp_on_exit(gpio_disable, gpio_mutex);

    return gpio_mutex;
}

// get GPIO record and acquire mutex
GpioPin* gpio_open(const char* name) {
    ValueMutex* gpio_mutex = gpio_open_mutex(name);
    GpioPin* gpio_pin = acquire_mutex(gpio_mutex, FLIPPER_HELPER_TIMEOUT);
    return gpio_pin;
}