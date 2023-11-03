static void value_changed_event_cb(lv_event_t* e) {

  lv_obj_t* arc = (lv_obj_t*)lv_event_get_target(e);
  lv_obj_t* label = (lv_obj_t*)lv_event_get_user_data(e);

  lv_label_set_text_fmt(label, "%d%%", lv_arc_get_value(arc));

  /*Rotate the label to the current position of the arc*/
  //lv_arc_rotate_obj_to_angle(arc, label, 25);
}
