#include <zephyr/kernel.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/shell/shell.h>
#include <zephyr/device.h>
#include <stdlib.h>
#include "our_driver.h"

#define SENSOR_DEV DEVICE_DT_GET(DT_NODELABEL(our_sensor))

static int cmd_fetch(const struct shell *sh, size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    const struct device *dev = SENSOR_DEV;

    if (!device_is_ready(dev)) {
        shell_error(sh, "Sensor device %s is not ready", dev->name);
        return -ENODEV;
    }

    int ret = sensor_sample_fetch(dev);
    if (ret < 0) {
        shell_error(sh, "Failed to fetch sample (err %d)", ret);
        return ret;
    }

    shell_info(sh, "Sample fetch successful.");
    return 0;
}

static int cmd_read(const struct shell *sh, size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    const struct device *dev = SENSOR_DEV;
    struct sensor_value val;

    if (!device_is_ready(dev)) {
        shell_error(sh, "Sensor device %s is not ready", dev->name);
        return -ENODEV;
    }

    int ret = sensor_channel_get(dev, SENSOR_CHAN_ALL, &val);
    if (ret < 0) {
        shell_error(sh, "Failed to read sensor channel (err %d)", ret);
        return ret;
    }

    shell_info(sh, "Sensor Value (custom_param): %d", val.val1);
    return 0;
}

static int cmd_info(const struct shell *sh, size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    const struct device *dev = SENSOR_DEV;
    bool is_ready = device_is_ready(dev);

    shell_info(sh, "Device Name: %s", dev->name);
    if (is_ready) {
        shell_info(sh, "Ready State: READY");
    } else {
        shell_warn(sh, "Ready State: NOT READY");
    }

    return 0;
}

static int cmd_set(const struct shell *sh, size_t argc, char **argv)
{
    const struct device *dev = SENSOR_DEV;

    if (!device_is_ready(dev)) {
        shell_error(sh, "Sensor device %s is not ready", dev->name);
        return -ENODEV;
    }

    char *endptr;
    long val = strtol(argv[1], &endptr, 10);


    if (*endptr != '\0') {
        shell_error(sh, "Invalid argument: '%s' is not a valid integer.", argv[1]);
        return -EINVAL;
    }


    if (val < 0 || val > 10000) {
        shell_error(sh, "Value out of range: %ld (allowed range: 0 - 10000)", val);
        return -ERANGE;
    }

    int ret = our_driver_set_custom_param(dev, (uint32_t)val);
    if (ret < 0) {
        shell_error(sh, "Failed to set custom parameter (err %d)", ret);
        return ret;
    }

    shell_info(sh, "Successfully set custom parameter to %ld", val);
    return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sub_sensor,
    SHELL_CMD(fetch, NULL, "Fetch sensor sample data", cmd_fetch),
    SHELL_CMD(read,  NULL, "Read and display channel value", cmd_read),
    SHELL_CMD(info,  NULL, "Print device name and ready status", cmd_info),
    SHELL_CMD_ARG(set, NULL, "Set custom driver parameter <value>", cmd_set, 2, 0),
    SHELL_SUBCMD_SET_END
);

/* Register root command 'sensorroot' */
SHELL_CMD_REGISTER(sensorroot, &sub_sensor, "LED-based sensor shell commands", NULL);