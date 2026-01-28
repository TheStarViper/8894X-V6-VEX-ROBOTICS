#pragma once
#include "liblvgl/lvgl.h"
#include <vector>

typedef enum {
    LV_MENU_ITEM_BUILDER_VARIANT_1,
    LV_MENU_ITEM_BUILDER_VARIANT_2
} lv_menu_builder_variant_t;

inline bool color_selected;
void run_selected_auton();

std::vector<int> get_auto_ids(short int section);
static void switch_event_handler(lv_event_t * e);
static void button_event_handler(lv_event_t * e);

static lv_obj_t * create_text(lv_obj_t * parent, 
                              const char * icon, 
                              const char * txt,
                              lv_menu_builder_variant_t builder_variant);


static lv_obj_t * create_slider(lv_obj_t * parent,
                                const char * icon,
                                const char * txt,
                                int32_t min, 
                                int32_t max, 
                                int32_t val);

static lv_obj_t * create_switch(lv_obj_t * parent,
                                const char * icon,
                                const char * txt,
                                bool * external_state_ptr);
lv_obj_t * create_button(lv_obj_t * parent, const char * icon, const char * txt, lv_event_cb_t event_cb, uint32_t button_id);
void lvgl_task(void* param);
void brain_menu(void);