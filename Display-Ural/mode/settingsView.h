static lv_obj_t* spinbox;
#include "settingsModel.h"
void lv_example_spinbox_1(lv_obj_t* parent);
void Settings_mode_control(lv_obj_t* parent){
 lv_example_spinbox_1(parent);
}
void lv_example_spinbox_1(lv_obj_t* parent) {
  spinbox = lv_spinbox_create(parent);
  lv_spinbox_set_range(spinbox, -1000, 25000);
  lv_spinbox_set_digit_format(spinbox, 5, 2);
  lv_spinbox_step_prev(spinbox);
  lv_obj_set_width(spinbox, 100);
  lv_obj_center(spinbox);

  lv_coord_t h = lv_obj_get_height(spinbox);

  lv_obj_t* btn = lv_btn_create(parent);
  lv_obj_set_size(btn, h, h);
  lv_obj_align_to(btn, spinbox, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
  lv_obj_set_style_bg_img_src(btn, LV_SYMBOL_PLUS, 0);
  lv_obj_add_event_cb(btn, lv_spinbox_increment_event_cb, LV_EVENT_ALL, NULL);

  btn = lv_btn_create(parent);
  lv_obj_set_size(btn, h, h);
  lv_obj_align_to(btn, spinbox, LV_ALIGN_OUT_LEFT_MID, -5, 0);
  lv_obj_set_style_bg_img_src(btn, LV_SYMBOL_MINUS, 0);
  lv_obj_add_event_cb(btn, lv_spinbox_decrement_event_cb, LV_EVENT_ALL, NULL);
}
