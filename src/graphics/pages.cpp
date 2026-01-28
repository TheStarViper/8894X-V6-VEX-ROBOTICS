#include "pages.hpp"
#include "liblvgl/lvgl.h"
#include "api.h"
#include <vector>
#include "autons.hpp"





short int selected_auto = 1;      // which specific auton inside a color menu (to be used later)
short int selected_section = 0;   // 0 = red, 1 = blue, 2 = skills, 3 = driving skills
std::string auton_names = "";
bool show_selected_auto = false;


using FunctionPointer = std::function<void()>;
struct auton_data{
    short int catagory;
    std::string func_name;
    FunctionPointer func;
};

const auton_data autolist[10] = {
//  cat |    name                |   function pointer
    {0  ,"Right 10 long"          ,auton_routes::red_1},
    {0  ,"Right 2 controls"       ,auton_routes::red_2},
    {0  ,"Awp Right"              ,auton_routes::red_3},
    {0  ,"Left Long"              ,auton_routes::red_4},
    {0  ,"Right 2 Controls"       ,auton_routes::red_5},
    {1  ,"blue left"              ,auton_routes::blue_1},
    {1  ,"blue 3"                 ,auton_routes::blue_3},
    {2  ,"skills auton routine"   ,auton_routes::skills_auton_routine},
    {3  ,"skills driving routine" ,auton_routes::skills_driving_routine},
}; 



const int AUTOLIST_SIZE = sizeof(autolist) / sizeof(auton_data);

std::vector<int> get_auto_ids(short int section){
    std::vector<int> list_of_ids;
    for (int i = 0; i < 8; i++){
        if (autolist[i].catagory == section){
            list_of_ids.push_back(i+1);}}
    return list_of_ids;
}

const char *get_auton_name(int group_param, int routine_param) {
    // Iterate through every element in the autolist array
    for (int i = 0; i < AUTOLIST_SIZE; i++) {
        if (autolist[i].catagory == group_param && i == routine_param-1) {
            return autolist[i].func_name.c_str();
        }
    }
    return "UNKNOWN_AUTON_ROUTINE"; //panik
}

void lvgl_task(void* param) {
    while (true) {
        lv_tick_inc(15);
        lv_timer_handler();
        pros::delay(15);}
}

void create_sub_section(lv_obj_t * sub_page, short int section_param,lv_obj_t * menu,lv_obj_t * &section) {
    lv_obj_set_style_pad_hor(sub_page, lv_obj_get_style_pad_left(lv_menu_get_main_header(menu), 0), 0);
    lv_menu_separator_create(sub_page);
    section = lv_menu_section_create(sub_page);
    std::vector<int> receivedVector = get_auto_ids(section_param);
    short int iterations = 0;
    for (int val : receivedVector) {
        create_button(section, LV_SYMBOL_PLAY, get_auton_name(section_param,val), button_event_handler, val);
        iterations++;
    }

}
#if LV_USE_MENU && LV_USE_MSGBOX && LV_BUILD_EXAMPLES
lv_obj_t * root_page;

static lv_obj_t * create_text(lv_obj_t * parent,
                              const char * icon,
                              const char * txt,
                              lv_menu_builder_variant_t builder_variant){

    lv_obj_t * obj = lv_menu_cont_create(parent);
    lv_obj_t * img = NULL;
    lv_obj_t * label = NULL;
    if(icon) {
        img = lv_image_create(obj);
        lv_image_set_src(img, icon);
    }

    if(txt) {

        label = lv_label_create(obj);
        lv_label_set_text(label, txt);
        lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
        lv_obj_set_flex_grow(label, 1);
    }

    if(builder_variant == LV_MENU_ITEM_BUILDER_VARIANT_2 && icon && txt) {
        lv_obj_add_flag(img, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
        lv_obj_swap(img, label);
    }
    return obj;
}

static lv_obj_t * create_slider(lv_obj_t * parent,
                                const char * icon,
                                const char * txt,
                                int32_t min,
                                int32_t max,
                                int32_t val){
        
    lv_obj_t * obj = create_text(parent, icon, txt, LV_MENU_ITEM_BUILDER_VARIANT_1);

    lv_obj_t * slider = lv_slider_create(obj);
    lv_obj_set_flex_grow(slider, 1);
    lv_slider_set_range(slider, min, max);
    lv_slider_set_value(slider, val, LV_ANIM_OFF);

    if(icon == NULL) {
        lv_obj_add_flag(slider, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
    }

    return obj;
}

static void button_event_handler(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        lv_obj_t * btn = (lv_obj_t *)lv_event_get_target(e);
        lv_obj_t * section = lv_obj_get_parent(lv_obj_get_parent(btn)); 
        uint32_t child_count = lv_obj_get_child_count(section);
        
        for (uint32_t i = 0; i < child_count; i++) {
            lv_obj_t * child_cont = lv_obj_get_child(section, i); 
            
            // Safety check: ensure the container is valid
            if (child_cont != NULL) {
                lv_obj_t * sibling_btn = lv_obj_get_child(child_cont, 2); 
                
                if (sibling_btn != NULL) {
                    lv_obj_remove_state(sibling_btn, LV_STATE_CHECKED); 
                    // lv_label_set_text(sibling_btn, "Select");
                }
            }
        }
        lv_obj_add_state(btn, LV_STATE_CHECKED);
        void * user_data_ptr = lv_obj_get_user_data(btn);
        uint32_t retrieved_id = (uint32_t)(intptr_t)user_data_ptr;
        selected_auto = retrieved_id;
        lv_obj_t * label = lv_obj_get_child(btn, 0);
        lv_label_set_text(label, "Selected");

        LV_LOG_USER("Button clicked! Starting operation.");
    }
}

lv_obj_t * create_button(lv_obj_t * parent, const char * icon, const char * txt, lv_event_cb_t event_cb, uint32_t button_id) {
    lv_obj_t * cont = lv_menu_cont_create(parent);

    lv_obj_t * icon_label = lv_label_create(cont);
    lv_label_set_text(icon_label, icon);
    
    lv_obj_t * label = lv_label_create(cont);
    lv_label_set_text(label, txt);

    lv_obj_t * btn = lv_button_create(cont);
    lv_obj_set_size(btn, LV_SIZE_CONTENT, LV_SIZE_CONTENT); 
    
    lv_obj_t * btn_label = lv_label_create(btn);
    lv_label_set_text(btn_label, "Select");
    lv_obj_center(btn_label);

    lv_obj_set_user_data(btn, (void *)(intptr_t)button_id);
    if (event_cb != NULL) {
        lv_obj_add_event_cb(btn, event_cb, LV_EVENT_CLICKED, NULL);
    }
    return cont;
}

static void switch_event_handler(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e); 
    
    if (code == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t * sw = (lv_obj_t *)lv_event_get_target(e); 
        bool * state_ptr = (bool *)lv_obj_get_user_data(sw); 
        if (state_ptr != NULL) {
            bool new_state = lv_obj_has_state(sw, LV_STATE_CHECKED);
            *state_ptr = new_state;
            LV_LOG_USER("Switch toggled. External state is now: %s", new_state ? "true" : "false");
        } else {
            LV_LOG_WARN("Switch handler called, but state pointer is NULL!");
        }
    }
}

static lv_obj_t * create_switch(lv_obj_t * parent, const char * icon, const char * txt, bool * external_state_ptr)
{
    lv_obj_t * obj = create_text(parent, icon, txt,LV_MENU_ITEM_BUILDER_VARIANT_1);
    lv_obj_t * sw = lv_switch_create(obj);
    if (external_state_ptr != NULL && *external_state_ptr) {
        lv_obj_add_state(sw, LV_STATE_CHECKED);}

    lv_obj_set_user_data(sw, (void *)external_state_ptr);
    lv_obj_add_event_cb(sw, switch_event_handler, LV_EVENT_VALUE_CHANGED, NULL);

    return obj;
}



void brain_menu(void)
{   
    
    lv_obj_t * menu = lv_menu_create(lv_screen_active());

    lv_color_t bg_color = lv_obj_get_style_bg_color(menu, 0);
    if(lv_color_brightness(bg_color) > 127) {
        lv_obj_set_style_bg_color(menu, lv_color_darken(lv_obj_get_style_bg_color(menu, 0), 10), 0);}
    else {lv_obj_set_style_bg_color(menu, lv_color_darken(lv_obj_get_style_bg_color(menu, 0), 50), 0);}
    
    lv_obj_set_size(menu, lv_display_get_horizontal_resolution(NULL), lv_display_get_vertical_resolution(NULL));
    lv_obj_center(menu);

    lv_obj_t * cont;
    lv_obj_t * section;

    //Create sub pages
    lv_obj_t * sub_red_page = lv_menu_page_create(menu, NULL);
    create_sub_section(sub_red_page,0,menu,section);

    lv_obj_t * sub_blue_page = lv_menu_page_create(menu, NULL);
    create_sub_section(sub_blue_page,1,menu,section);

    lv_obj_t * sub_skills_page = lv_menu_page_create(menu, NULL);
    create_sub_section(sub_skills_page,2,menu,section);

    lv_obj_t * sub_selections_page = lv_menu_page_create(menu, NULL);
    lv_obj_set_style_pad_hor(sub_selections_page, lv_obj_get_style_pad_left(lv_menu_get_main_header(menu), 0), 0);
    lv_menu_separator_create(sub_selections_page);
    section = lv_menu_section_create(sub_selections_page);
    create_switch(section, LV_SYMBOL_EYE_OPEN, "Show Selected Auto", &show_selected_auto);


    lv_obj_t * sub_information_page = lv_menu_page_create(menu, NULL);
    lv_obj_set_style_pad_hor(sub_information_page, lv_obj_get_style_pad_left(lv_menu_get_main_header(menu), 0), 0);
    lv_menu_separator_create(sub_information_page);
    section = lv_menu_section_create(sub_information_page);
    create_text(section, NULL, "502N V3\nBy Team 502N", LV_MENU_ITEM_BUILDER_VARIANT_1);
    if (show_selected_auto){
        std::string selected_auto_str = "Selected Auto ID: " + std::to_string(selected_auto);
        create_text(section, NULL, selected_auto_str.c_str(), LV_MENU_ITEM_BUILDER_VARIANT_1);
    }
    
    //Create a root page
    root_page = lv_menu_page_create(menu, "Autos");
    lv_obj_set_style_pad_hor(root_page, lv_obj_get_style_pad_left(lv_menu_get_main_header(menu), 0), 0);
    section = lv_menu_section_create(root_page);
    cont = create_text(section, LV_SYMBOL_PLAY, "Red Auto", LV_MENU_ITEM_BUILDER_VARIANT_1);
    lv_menu_set_load_page_event(menu, cont, sub_red_page);
    cont = create_text(section, LV_SYMBOL_PLAY, "Blue Auto", LV_MENU_ITEM_BUILDER_VARIANT_1);
    lv_menu_set_load_page_event(menu, cont, sub_blue_page);
    cont = create_text(section, LV_SYMBOL_GPS, "Skills", LV_MENU_ITEM_BUILDER_VARIANT_1);
    lv_menu_set_load_page_event(menu, cont, sub_skills_page);
    
    create_text(root_page, NULL, "Debug", LV_MENU_ITEM_BUILDER_VARIANT_1);
    section = lv_menu_section_create(root_page);
    cont = create_text(section, LV_SYMBOL_EYE_OPEN, "Selections", LV_MENU_ITEM_BUILDER_VARIANT_1);
    lv_menu_set_load_page_event(menu, cont, sub_selections_page);
    cont = create_text(section, LV_SYMBOL_DRIVE, "Info", LV_MENU_ITEM_BUILDER_VARIANT_1);
    lv_menu_set_load_page_event(menu, cont, sub_information_page);

    lv_menu_set_sidebar_page(menu, root_page);

    lv_obj_send_event(lv_obj_get_child(lv_obj_get_child(lv_menu_get_cur_sidebar_page(menu), 0), 0), LV_EVENT_CLICKED,NULL);
}

#endif