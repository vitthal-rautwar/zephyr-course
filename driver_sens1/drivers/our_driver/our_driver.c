#define DT_DRV_COMPAT our_driver

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(our_driver, CONFIG_SENSOR_LOG_LEVEL);

struct our_driver_config {
    struct gpio_dt_spec led_gpio;
};

struct our_driver_data {
    bool led_state;
};

static int our_driver_sample_fetch(const struct device *dev, enum sensor_channel chan)
{
    const struct our_driver_config *config = dev->config;
    struct our_driver_data *data = dev->data;

    int ret = gpio_pin_set_dt(&config->led_gpio, 1);
    if (ret < 0) {
        return ret;
    }

    data->led_state = true;
    LOG_INF("our_driver: LED turned ON (sample_fetch)");
    return 0;
}

static int our_driver_channel_get(const struct device *dev, enum sensor_channel chan, struct sensor_value *val)
{
    const struct our_driver_config *config = dev->config;
    struct our_driver_data *data = dev->data;

    int ret = gpio_pin_set_dt(&config->led_gpio, 0);
    if (ret < 0) {
        return ret;
    }

    data->led_state = false;
    val->val1 = 0;
    val->val2 = 0;
    LOG_INF("our_driver: LED turned OFF (channel_get)");
    return 0;
}

static const struct sensor_driver_api our_driver_api = {
    .sample_fetch = our_driver_sample_fetch,
    .channel_get = our_driver_channel_get,
};

static int our_driver_init(const struct device *dev)
{
    const struct our_driver_config *config = dev->config;

    if (!gpio_is_ready_dt(&config->led_gpio)) {
        LOG_ERR("GPIO device not ready");
        return -ENODEV;
    }

    return gpio_pin_configure_dt(&config->led_gpio, GPIO_OUTPUT_INACTIVE);
}

#define OUR_DRIVER_INIT(inst)                                              \
    static struct our_driver_data our_driver_data_##inst;                  \
    static const struct our_driver_config our_driver_config_##inst = {    \
        .led_gpio = GPIO_DT_SPEC_INST_GET(inst, led_gpios),               \
    };                                                                     \
    SENSOR_DEVICE_DT_INST_DEFINE(inst, our_driver_init, NULL,              \
                   &our_driver_data_##inst, &our_driver_config_##inst,     \
                   POST_KERNEL, CONFIG_SENSOR_INIT_PRIORITY,               \
                   &our_driver_api);

DT_INST_FOREACH_STATUS_OKAY(OUR_DRIVER_INIT)