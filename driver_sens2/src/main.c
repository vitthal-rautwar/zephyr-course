#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/sys/printk.h>
#include "our_driver.h"

int main(void)
{
    const struct device *dev = DEVICE_DT_GET(DT_NODELABEL(our_sensor));
    struct sensor_value val;
    uint32_t new_param_value = 500;

    if (!device_is_ready(dev)) {
        printk("Sensor device not ready\n");
        return 0;
    }

    printk("Testing custom extension API function...\n");

    /* Call custom extension API to alter driver dynamic data */
    our_driver_set_custom_param(dev, new_param_value);

    while (1) {
        printk("Fetching sample (LED ON)...\n");
        sensor_sample_fetch(dev);
        k_msleep(500);

        printk("Getting channel (LED OFF)...\n");
        sensor_channel_get(dev, SENSOR_CHAN_ALL, &val);
        printk("Retrieved custom_param via sensor_channel_get: %d\n", val.val1);
        k_msleep(500);
    }

    return 0;
}