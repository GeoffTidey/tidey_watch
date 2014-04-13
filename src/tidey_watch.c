#include "pebble.h"
#include "num2words.h"

#define BUFFER_SIZE 86

static struct CommonWordsData {
  Window *window;
  TextLayer *time_label;
  TextLayer *date_label;
  int current_day;
  char buffer[BUFFER_SIZE];
} s_data;

static void update_time(struct tm* t) {
  fuzzy_time_to_words(t->tm_hour, t->tm_min, s_data.buffer, BUFFER_SIZE);
  text_layer_set_text(s_data.time_label, s_data.buffer);
}

// static void update_date(struct tm* t) {
//   strftime(s_data.buffer, BUFFER_SIZE, "%a %e %b", t);
//   text_layer_set_text(s_data.date_label, s_data.buffer);
// }

static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
  update_time(tick_time);
  // if (s_data.current_day != tick_time->tm_day) {
  //   s_data.current_day = tick_time->tm_day;
  //   update_date(tick_time);
  // }
}

static void do_init(void) {
  s_data.window = window_create();
  const bool animated = true;
  window_stack_push(s_data.window, animated);

  window_set_background_color(s_data.window, GColorBlack);
  GFont time_font = fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK);
  GFont date_font = fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD);

  Layer *root_layer = window_get_root_layer(s_data.window);
  GRect frame = layer_get_frame(root_layer);

  s_data.time_label = text_layer_create(GRect(0, 20, frame.size.w, frame.size.h - 40));
  text_layer_set_background_color(s_data.time_label, GColorBlack);
  text_layer_set_text_color(s_data.time_label, GColorWhite);
  text_layer_set_font(s_data.time_label, time_font);
  layer_add_child(root_layer, text_layer_get_layer(s_data.time_label));

  // s_data.date_label = text_layer_create(GRect(0, frame.size.h - 40, frame.size.w, 40));
  // text_layer_set_background_color(s_data.date_label, GColorBlack);
  // text_layer_set_text_color(s_data.date_label, GColorWhite);
  // text_layer_set_font(s_data.date_label, date_font);
  // layer_add_child(root_layer, text_layer_get_layer(s_data.date_label));

  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  s_data.current_day = t->tm_day;
  update_time(t);
  // update_date(t);

  tick_timer_service_subscribe(MINUTE_UNIT, &handle_minute_tick);
}

static void do_deinit(void) {
  window_destroy(s_data.window);
  // text_layer_destroy(s_data.date_label);
  text_layer_destroy(s_data.time_label);
}

int main(void) {
  do_init();
  app_event_loop();
  do_deinit();
}
