static void event_handler(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);

  if (code == LV_EVENT_CLICKED) {
    LV_LOG_USER("Clicked");
    Serial.println("test");
  } else if (code == LV_EVENT_VALUE_CHANGED) {
    LV_LOG_USER("Toggled");
    Serial.println("test2");
  }
}