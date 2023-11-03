#include <lvgl.h>
#include "DisplayView.h"
/*******************************************************************************
 ******************************************************************************/
#include <Arduino_GFX_Library.h>
#define TFT_BL 2
#define GFX_BL DF_GFX_BL

Arduino_ESP32RGBPanel *bus = new Arduino_ESP32RGBPanel(
  GFX_NOT_DEFINED /* CS */, GFX_NOT_DEFINED /* SCK */, GFX_NOT_DEFINED /* SDA */,
  41 /* DE */, 40 /* VSYNC */, 39 /* HSYNC */, 42 /* PCLK */,
  14 /* R0 */, 21 /* R1 */, 47 /* R2 */, 48 /* R3 */, 45 /* R4 */,
  9 /* G0 */, 46 /* G1 */, 3 /* G2 */, 8 /* G3 */, 16 /* G4 */, 1 /* G5 */,
  15 /* B0 */, 7 /* B1 */, 6 /* B2 */, 5 /* B3 */, 4 /* B4 */
);
// option 1:
// 7寸 50PIN 800*480
Arduino_RPi_DPI_RGBPanel *gfx = new Arduino_RPi_DPI_RGBPanel(
  bus,
  //  800 /* width */, 0 /* hsync_polarity */, 8/* hsync_front_porch */, 2 /* hsync_pulse_width */, 43/* hsync_back_porch */,
  //  480 /* height */, 0 /* vsync_polarity */, 8 /* vsync_front_porch */, 2/* vsync_pulse_width */, 12 /* vsync_back_porch */,
  //  1 /* pclk_active_neg */, 16000000 /* prefer_speed */, true /* auto_flush */);

  800 /* width */, 0 /* hsync_polarity */, 210 /* hsync_front_porch */, 30 /* hsync_pulse_width */, 16 /* hsync_back_porch */,
  480 /* height */, 0 /* vsync_polarity */, 22 /* vsync_front_porch */, 13 /* vsync_pulse_width */, 10 /* vsync_back_porch */,
  1 /* pclk_active_neg */, 12000000 /* prefer_speed */, true /* auto_flush */);

/*******************************************************************************
 * Please config the touch panel in touch.h
 ******************************************************************************/
#include "touch.h"

/* Change to your screen resolution */
static uint32_t screenWidth;
static uint32_t screenHeight;
static lv_disp_draw_buf_t draw_buf;
static lv_color_t *disp_draw_buf1;
static lv_color_t *disp_draw_buf2;
static lv_disp_drv_t disp_drv;

/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

#if (LV_COLOR_16_SWAP != 0)
  gfx->draw16bitBeRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#else
  gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#endif

  lv_disp_flush_ready(disp);
}

void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data) {
  if (touch_has_signal()) {
    if (touch_touched()) {
      data->state = LV_INDEV_STATE_PR;

      /*Set the coordinates*/
      data->point.x = touch_last_x;
      data->point.y = touch_last_y;
    } else if (touch_released()) {
      data->state = LV_INDEV_STATE_REL;
    }
  } else {
    data->state = LV_INDEV_STATE_REL;
  }
}
void setup() {
  Serial.begin(115200);
  Serial.println("LVGL Widgets Demo");
  // Init touch device
  // Init Display
  gfx->begin();
#ifdef TFT_BL
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);
#endif
  gfx->fillScreen(BLACK);
  delay(50);
  lv_init();
  delay(10);
  touch_init();
  delay(50);
  screenWidth = gfx->width();
  screenHeight = gfx->height();
#ifdef ESP32
  //disp_draw_buf = (lv_color_t *)heap_caps_malloc(sizeof(lv_color_t) * screenWidth * screenHeight / 4, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
  disp_draw_buf1 = (lv_color_t *)heap_caps_malloc(sizeof(lv_color_t) * screenWidth * screenHeight / 8, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
  disp_draw_buf2 = (lv_color_t *)heap_caps_malloc(sizeof(lv_color_t) * screenWidth * screenHeight / 8, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
#else
  disp_draw_buf = (lv_color_t *)malloc(sizeof(lv_color_t) * screenWidth * screenHeight / 4);
#endif
  if (!disp_draw_buf1 && !disp_draw_buf2) {
    Serial.println("LVGL disp_draw_buf allocate failed!");
  } else {
    lv_disp_draw_buf_init(&draw_buf, disp_draw_buf1, disp_draw_buf2, screenWidth * screenHeight / 8);
    /* Initialize the display */
    lv_disp_drv_init(&disp_drv);
    /* Change the following line to your display resolution */
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);
    /* Initialize the (dummy) input device driver */
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touchpad_read;
    lv_indev_drv_register(&indev_drv);
#if 0
     // lv_example_btn_1();
#else
//запуск виджетов
    lv_example_tabview_2();
    Serial.println("Setup done");
#endif
  }
}
void loop() {
  lv_timer_handler(); /* let the GUI do its work */
}
