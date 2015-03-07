#include "pebble.h"
#include "num2words.h"
#include "secret.h"

#define BUFFER_SIZE 86

static struct CommonWordsData {
  Window *window;
  TextLayer *time_label;
  TextLayer *date_label;
  TextLayer *weather_label;
  char time_buffer[BUFFER_SIZE];
  char date_buffer[BUFFER_SIZE];
  char weather_description[BUFFER_SIZE];
  char weather_temperature[BUFFER_SIZE];
  char weather_timestamp[BUFFER_SIZE];
  char weather_buffer[BUFFER_SIZE];
} s_data;

static bool force_update = false;

enum {
  KEY_TEMPERATURE = 0,
  KEY_HOUR_FROM,
  KEY_HOUR_SUMMARY
};

void build_weather_label(void) {
  memset(s_data.weather_buffer, 0, BUFFER_SIZE);
  snprintf(s_data.weather_buffer, BUFFER_SIZE, "%s %s %s",
            s_data.weather_temperature,
            s_data.weather_timestamp,
            s_data.weather_description
          );
  text_layer_set_text(s_data.weather_label, (char*) &s_data.weather_buffer);
}

size_t printed_length ( int x )
{
  return snprintf(NULL, 0, "%d", x);
}

void process_tuple(Tuple *t)
{
  // Get key
  int key = t->key;

  // Get integer value, if present
  int value = t->value->int32;

  // Get string value, if present
  char string_value[BUFFER_SIZE];
  strcpy(string_value, t->value->cstring);

  // Decide what to do
  switch(key) {
    case KEY_TEMPERATURE:
      memset(s_data.weather_temperature, 0, BUFFER_SIZE);

      snprintf(s_data.weather_temperature, printed_length(value) + sizeof(" \u00B0C"), "%d \u00B0C", value);
      break;
    case KEY_HOUR_FROM:
      memset(s_data.weather_timestamp, 0, BUFFER_SIZE);
      time_t sec = (time_t) value;
      struct tm *tm;
      tm = localtime (&sec);
      strftime(s_data.weather_timestamp, BUFFER_SIZE, "%H:%M", tm);
      break;
    case KEY_HOUR_SUMMARY:
      memset(s_data.weather_description, 0, BUFFER_SIZE);
      strcpy(s_data.weather_description, string_value);
      break;
  }
}

static void in_received_handler(DictionaryIterator *iter, void *context)
{
  // Get data
  Tuple *t = dict_read_first(iter);
  if (t) {
    process_tuple(t);
  }
  // Get next
  while(t != NULL) {
    t = dict_read_next(iter);
    if (t) {
      process_tuple(t);
    }
  }
  build_weather_label();
}

static TextLayer* init_text_layer(GRect location, GColor colour, GColor background, const char *res_id, GTextAlignment alignment)
{
  TextLayer *layer = text_layer_create(location);
  text_layer_set_text_color(layer, colour);
  text_layer_set_background_color(layer, background);
  text_layer_set_font(layer, fonts_get_system_font(res_id));
  text_layer_set_text_alignment(layer, alignment);

  return layer;
}

static void update_time(struct tm* t) {
  fuzzy_time_to_words(t->tm_hour, t->tm_min, s_data.time_buffer, BUFFER_SIZE);
  // strcpy(s_data.time_buffer, "just gone quarter to midnight");
  text_layer_set_text(s_data.time_label, s_data.time_buffer);
}

static void update_date(struct tm* t) {
  clock_copy_time_string(s_data.date_buffer, BUFFER_SIZE);
  int len = strlen(s_data.date_buffer);
  strftime(&s_data.date_buffer[len], BUFFER_SIZE - len, " %a %e %b", t);
  text_layer_set_text(s_data.date_label, s_data.date_buffer);
}

void update_weather_on_phone(void)
{
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);

  // change this to hosted solution if making .pbw public.
  dict_write_cstring(iter, 1, (char *) API_KEY);
  dict_write_end(iter);

  app_message_outbox_send();
}

static bool every_ten_minutes(struct tm* t) {
  return t->tm_min % 10 == 0;
}

static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
  update_time(tick_time);
  update_date(tick_time);
  if (force_update || every_ten_minutes(tick_time)) {
    update_weather_on_phone();
  }
}

static void do_init(void) {
  s_data.window = window_create();
  const bool animated = true;
  window_stack_push(s_data.window, animated);

  window_set_background_color(s_data.window, GColorBlack);

  Layer *root_layer = window_get_root_layer(s_data.window);
  GRect frame = layer_get_frame(root_layer);

  int top_y = 36;
  int bottom_y = 20;

  s_data.weather_label = init_text_layer(GRect(0, -5, frame.size.w, top_y), GColorWhite, GColorBlack, "RESOURCE_ID_GOTHIC_18_BOLD", GTextAlignmentCenter);
  layer_add_child(root_layer, text_layer_get_layer(s_data.weather_label));

  s_data.time_label = init_text_layer(GRect(0, top_y - 5, frame.size.w, frame.size.h - bottom_y - top_y + 9), GColorWhite, GColorBlack, "RESOURCE_ID_BITHAM_30_BLACK", GTextAlignmentLeft);
  layer_add_child(root_layer, text_layer_get_layer(s_data.time_label));

  s_data.date_label = init_text_layer(GRect(0, frame.size.h - bottom_y + 2, frame.size.w, bottom_y + 1), GColorWhite, GColorBlack, "RESOURCE_ID_GOTHIC_18_BOLD", GTextAlignmentCenter);
  layer_add_child(root_layer, text_layer_get_layer(s_data.date_label));

  //Register AppMessage events
  app_message_register_inbox_received(in_received_handler);
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());

  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  if (!every_ten_minutes(t)) {
    force_update = true;
  }
  handle_minute_tick(t, 0);
  force_update = false;

  tick_timer_service_subscribe(MINUTE_UNIT, &handle_minute_tick);
}

static void do_deinit(void) {
  window_destroy(s_data.window);
  text_layer_destroy(s_data.date_label);
  text_layer_destroy(s_data.time_label);
  text_layer_destroy(s_data.weather_label);
}

int main(void) {
  do_init();
  app_event_loop();
  do_deinit();
}
