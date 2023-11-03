static void set_value(void *indic, int32_t v) {
  lv_meter_set_indicator_value(meter, (lv_meter_indicator_t *)indic, v);
}