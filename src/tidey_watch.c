#include "pebble.h"
#include "num2words.h"

#define BUFFER_SIZE 86

static struct CommonWordsData {
  Window *window;
  TextLayer *time_label;
  TextLayer *date_label;
  char time_buffer[BUFFER_SIZE];
  char date_buffer[BUFFER_SIZE];
} s_data;

static void update_time(struct tm* t) {
  fuzzy_time_to_words(t->tm_hour, t->tm_min, s_data.time_buffer, BUFFER_SIZE);
  text_layer_set_text(s_data.time_label, s_data.time_buffer);
}

static void update_date(struct tm* t) {
  clock_copy_time_string(s_data.date_buffer, BUFFER_SIZE);
  int len = strlen(s_data.date_buffer);
  strftime(&s_data.date_buffer[len], BUFFER_SIZE - len, " %a %e %b", t);
  text_layer_set_text(s_data.date_label, s_data.date_buffer);
}

static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
  update_time(tick_time);
  update_date(tick_time);
}

static void do_init(void) {
  s_data.window = window_create();
  const bool animated = true;
  window_stack_push(s_data.window, animated);

  window_set_background_color(s_data.window, GColorBlack);
  GFont time_font = fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK);
  GFont date_font = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);

  Layer *root_layer = window_get_root_layer(s_data.window);
  GRect frame = layer_get_frame(root_layer);

  int top_y = 9;
  int bottom_y = 25;

  s_data.time_label = text_layer_create(GRect(0, top_y, frame.size.w, frame.size.h - bottom_y - top_y));
  text_layer_set_background_color(s_data.time_label, GColorBlack);
  text_layer_set_text_color(s_data.time_label, GColorWhite);
  text_layer_set_font(s_data.time_label, time_font);
  layer_add_child(root_layer, text_layer_get_layer(s_data.time_label));

  int date_x = clock_is_24h_style() ? 12 : 0;
  s_data.date_label = text_layer_create(GRect(date_x, frame.size.h - bottom_y, frame.size.w, bottom_y));
  text_layer_set_background_color(s_data.date_label, GColorBlack);
  text_layer_set_text_color(s_data.date_label, GColorWhite);
  text_layer_set_font(s_data.date_label, date_font);
  layer_add_child(root_layer, text_layer_get_layer(s_data.date_label));

  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  handle_minute_tick(t, 0);

  tick_timer_service_subscribe(MINUTE_UNIT, &handle_minute_tick);
}

static void do_deinit(void) {
  window_destroy(s_data.window);
  text_layer_destroy(s_data.date_label);
  text_layer_destroy(s_data.time_label);
}

int main(void) {
  do_init();
  app_event_loop();
  do_deinit();
}
