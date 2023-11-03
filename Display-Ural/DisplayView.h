//основные цвета
lv_color_t colorTab = lv_color_hex(0xb09989);

#include "mode/autoView.h"
#include "handheldView.h"
#include "mode/settingsView.h"
#include "DisplayModel.h"
//область видимости

void lv_example_tabview_2(void) {
  /*Create a Tab view object*/
  lv_obj_t* tabview;
  tabview = lv_tabview_create(lv_scr_act(), LV_DIR_LEFT, 80);

  lv_obj_set_style_bg_color(tabview, colorTab, 0);

  lv_obj_t* tab_btns = lv_tabview_get_tab_btns(tabview);
  lv_obj_set_style_bg_color(tab_btns, lv_palette_darken(LV_PALETTE_GREY, 3), 0);
  lv_obj_set_style_text_color(tab_btns, lv_palette_lighten(LV_PALETTE_GREY, 5), 0);
  lv_obj_set_style_border_side(tab_btns, LV_BORDER_SIDE_RIGHT, LV_PART_ITEMS | LV_STATE_CHECKED);

  /*Add 3 tabs (the tabs are page (lv_page) and can be scrolled*/
  lv_obj_t* tab1 = lv_tabview_add_tab(tabview, "Auto mode");
  lv_obj_t* tab2 = lv_tabview_add_tab(tabview, "Settings");
  lv_obj_t* tab3 = lv_tabview_add_tab(tabview, "Handheld mode");
  lv_obj_t* tab4 = lv_tabview_add_tab(tabview, "Other");

  lv_obj_set_style_bg_color(tab2, colorTab, 0);
  lv_obj_set_style_bg_opa(tab2, LV_OPA_COVER, 0);

  /*Add content to the tabs*/
  lv_obj_t*
    label = lv_label_create(tab1);
  lv_label_set_text(label, "First tab");
  Auto_mode_control(tab1);
  label = lv_label_create(tab2);
  lv_label_set_text(label, "Second tab");
  Settings_mode_control(tab2);
  label = lv_label_create(tab3);
  lv_label_set_text(label, "Third tab");
  Manual_mode_control(tab3);
  label = lv_label_create(tab4);
  lv_label_set_text(label, "Forth tab");

  lv_obj_clear_flag(lv_tabview_get_content(tabview), LV_OBJ_FLAG_SCROLLABLE);
}
