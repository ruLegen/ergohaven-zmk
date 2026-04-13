/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#define DT_DRV_COMPAT zmk_behavior_battery_status

#include <zephyr/device.h>
#include <drivers/behavior.h>
#include <zephyr/logging/log.h>

#include <zmk/behavior.h>
#include <zmk/battery.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#if DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT)
/*
struct zmk_behavior_binding {
#if IS_ENABLED(CONFIG_ZMK_BEHAVIOR_LOCAL_IDS_IN_BINDINGS)
    zmk_behavior_local_id_t local_id;
#endif // IS_ENABLED(CONFIG_ZMK_BEHAVIOR_LOCAL_IDS_IN_BINDINGS)
    const char *behavior_dev;
    uint32_t param1;
    uint32_t param2;
};

struct zmk_behavior_binding_event {
    int layer;
    uint32_t position;
    int64_t timestamp;
#if IS_ENABLED(CONFIG_ZMK_SPLIT)
    uint8_t source;
#endif
};


 * @brief Invoke a behavior given its binding and invoking event details.
 *
 * @param src_binding Behavior binding to invoke.
 * @param event The binding event struct containing details of the event that invoked it.
 * @param pressed Whether the binding is pressed or released.
 *
 * @retval 0 If successful.
 * @retval Negative errno code if failure.
 
int zmk_behavior_invoke_binding(const struct zmk_behavior_binding *src_binding,
                                struct zmk_behavior_binding_event event, bool pressed);

*/
static int on_keymap_binding_pressed(struct zmk_behavior_binding *binding,
                                     struct zmk_behavior_binding_event event) {

    uint8_t res =  zmk_battery_state_of_charge();
    LOG_DBG("=====BATTER_STATUS======= PRESSED");
    LOG_DBG("=== %d keycode 0x%02X",event.position, binding->param1);
    LOG_DBG("=== BATTERY: %d",res);
    LOG_DBG("=== BEHAVIOUR_DEV: %s",binding->behavior_dev);
    const struct device *kp = zmk_behavior_get_binding("key_press");
    if (!kp) {
        LOG_DBG("No behavior kp");
    }else
    {
        LOG_DBG("===== KP FOUND %s", kp->name);
    }

    struct zmk_behavior_binding kp_binding = {
        .behavior_dev = "key_press",
        .param1 = 0x7001E,
    };    
    struct zmk_behavior_binding_event kp_binding_event = {
        .position = ZMK_VIRTUAL_KEY_POSITION_COMBO(combo_idx),
        .timestamp = event.timestamp,
#if IS_ENABLED(CONFIG_ZMK_SPLIT)
        .source = event.source
#endif
    };
    //0x7001E
    zmk_behavior_invoke_binding(&kp_binding, kp_binding_event, true);
    zmk_behavior_invoke_binding(&kp_binding, kp_binding_event, false);

    return ZMK_BEHAVIOR_OPAQUE;
}

static int on_keymap_binding_released(struct zmk_behavior_binding *binding,
                                      struct zmk_behavior_binding_event event) {
    LOG_DBG("=====BATTER_STATUS======= RELEASED %d keycode 0x%02X", event.position, binding->param1);
    return ZMK_BEHAVIOR_TRANSPARENT;
}

static const struct behavior_driver_api behavior_battery_status_driver_api = {
    .binding_pressed = on_keymap_binding_pressed,
    .binding_released = on_keymap_binding_released,
    .locality = BEHAVIOR_LOCALITY_EVENT_SOURCE,
#if IS_ENABLED(CONFIG_ZMK_BEHAVIOR_METADATA)
    .get_parameter_metadata = zmk_behavior_get_empty_param_metadata,
#endif // IS_ENABLED(CONFIG_ZMK_BEHAVIOR_METADATA)
};

BEHAVIOR_DT_INST_DEFINE(0, NULL, NULL, NULL, NULL, POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT,
                        &behavior_battery_status_driver_api);

#endif /* DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT) */