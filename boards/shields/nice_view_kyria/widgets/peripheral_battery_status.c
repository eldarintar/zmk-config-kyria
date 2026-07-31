#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/battery.h>
#include <zmk/display.h>
#include <zmk/events/battery_state_changed.h>
#include <zmk/event_manager.h>

#include "peripheral_battery_status.h"

struct peripheral_battery_state {
    uint8_t source;
    uint8_t level;
};

static void set_battery_symbol(lv_obj_t *label, struct peripheral_battery_state state) {
    char text[6] = {};
    snprintf(text, sizeof(text), "%d%%", state.level);
    lv_label_set_text(label, text);
}

static void peripheral_battery_status_update_cb(struct peripheral_battery_state state) {
    struct zmk_widget_peripheral_battery_status *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) {
        set_battery_symbol(widget->obj, state);
    }
}

static struct peripheral_battery_state peripheral_battery_status_get_state(const zmk_event_t *eh) {
    const struct zmk_battery_state_changed *ev = as_zmk_battery_state_changed(eh);
    return (struct peripheral_battery_state){
        .source = ev->source,
        .level = ev->state_of_charge,
    };
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_peripheral_battery_status, struct peripheral_battery_state,
                            peripheral_battery_status_update_cb, peripheral_battery_status_get_state)
ZMK_SUBSCRIPTION(widget_peripheral_battery_status, zmk_battery_state_changed);

int zmk_widget_peripheral_battery_status_init(struct zmk_widget_peripheral_battery_status *widget,
                                              lv_obj_t *parent) {
    widget->obj = lv_label_create(parent);
    lv_label_set_text(widget->obj, "--");

    sys_slist_append(&widgets, &widget->node);
    widget_peripheral_battery_status_init();
    return 0;
                                              }

                                              lv_obj_t *zmk_widget_peripheral_battery_status_obj(struct zmk_widget_peripheral_battery_status *widget) {
                                                  return widget->obj;
                                              }
