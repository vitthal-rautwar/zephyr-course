#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/sys/printk.h>

int main(void)
{
    const struct device *dev = DEVICE_DT_GET(DT_NODELABEL(our_sensor));
    struct sensor_value val;

    if (!device_is_ready(dev)) {
        printk("Sensor device not ready\n");
        return 0;
    }

    while (1) {
        printk("Fetching sample (LED ON)...\n");
        sensor_sample_fetch(dev);
        k_msleep(2000);

        printk("Getting channel (LED OFF)...\n");
        sensor_channel_get(dev, SENSOR_CHAN_ALL, &val);
        k_msleep(2000);
    }

    return 0;
}