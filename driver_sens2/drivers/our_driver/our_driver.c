#define DT_DRV_COMPAT our_driver

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>
#include "our_driver.h"

LOG_MODULE_REGISTER(our_driver, CONFIG_SENSOR_LOG_LEVEL);

struct our_driver_config {
    struct gpio_dt_spec led_gpio;
};

struct our_driver_data {
    bool led_state;
    uint32_t custom_param; /* Dynamic parameter modified by extension API */
};

/* Custom Extension API Implementation */
int our_driver_set_custom_param(const struct device *dev, uint32_t new_param)
{
    if (dev == NULL) {
        return -EINVAL;
    }

    struct our_driver_data *data = dev->data;
    data->custom_param = new_param;

    LOG_INF("our_driver: custom_param updated to %u in dynamic data struct", data->custom_param);
    return 0;
}

static int our_driver_sample_fetch(const struct device *dev, enum sensor_channel chan)
{
    const struct our_driver_config *config = dev->config;
    struct our_driver_data *data = dev->data;

    int ret = gpio_pin_set_dt(&config->led_gpio, 1);
    if (ret < 0) {
        return ret;
    }

    data->led_state = true;
    LOG_INF("our_driver: LED turned ON (sample_fetch) | custom_param=%u", data->custom_param);
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
    val->val1 = data->custom_param;
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
    struct our_driver_data *data = dev->data;

    if (!gpio_is_ready_dt(&config->led_gpio)) {
        LOG_ERR("GPIO device not ready");
        return -ENODEV;
    }

    data->custom_param = 100; /* Default parameter initialization */

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