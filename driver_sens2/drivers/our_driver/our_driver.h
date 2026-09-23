#ifndef OUR_DRIVER_H_
#define OUR_DRIVER_H_

#include <zephyr/device.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Custom extension API function to update a driver parameter.
 *
 * @param dev Pointer to the device structure for the driver instance.
 * @param new_param New parameter value to store in the dynamic data struct.
 * @return 0 on success, negative error code on failure.
 */
int our_driver_set_custom_param(const struct device *dev, uint32_t new_param);

#ifdef __cplusplus
}
#endif

#endif /* OUR_DRIVER_H_ */