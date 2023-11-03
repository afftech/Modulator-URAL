
#include "handheldModel.h"
void lv_example_arc_1(lv_obj_t* parent);

void Manual_mode_control(lv_obj_t* parent) {
  lv_example_arc_1(parent);
}

void lv_example_arc_1(lv_obj_t* parent) {
  
  /*Create an Arc*/
  lv_obj_t* arc = lv_arc_create(parent);
  lv_obj_t* label = lv_label_create(arc);
  lv_obj_set_size(arc, 150, 150);
  lv_arc_set_rotation(arc, 135);
  lv_arc_set_bg_angles(arc, 0, 270);
  lv_arc_set_value(arc, 10);
  lv_obj_center(arc);
  lv_obj_add_event_cb(arc, value_changed_event_cb, LV_EVENT_VALUE_CHANGED, label);

  /*Manually update the label for the first time*/
  lv_event_send(arc, LV_EVENT_VALUE_CHANGED, NULL);
}