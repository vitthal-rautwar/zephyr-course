#include <zephyr/kernel.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/shell/shell.h>
#include <zephyr/device.h>

/* Retrieve the device instance configured for your driver */
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

    int ret = sensor_channel_get(dev, SENSOR_CHAN_LIGHT, &val);
    if (ret < 0) {
        shell_error(sh, "Failed to read sensor channel (err %d)", ret);
        return ret;
    }

    /* Informational output using shell_info */
    shell_info(sh, "Sensor Value: %d.%06d", val.val1, val.val2);
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

/* Define subcommand structure under sub_sensor */
SHELL_STATIC_SUBCMD_SET_CREATE(sub_sensor,
    SHELL_CMD(fetch, NULL, "Fetch sensor sample data", cmd_fetch),
    SHELL_CMD(read,  NULL, "Read and display channel value", cmd_read),
    SHELL_CMD(info,  NULL, "Print device name and ready status", cmd_info),
    SHELL_SUBCMD_SET_END
);

/* Register root command 'sensorroot' */
SHELL_CMD_REGISTER(sensorroot, &sub_sensor, "LED-based sensor shell commands", NULL);