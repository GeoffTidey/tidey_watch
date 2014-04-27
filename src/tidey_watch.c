#include "pebble.h"
#include "num2words.h"

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
  char weather_location[BUFFER_SIZE];
  char weather_timestamp[BUFFER_SIZE];
  char weather_buffer[BUFFER_SIZE];
} s_data;

enum {
  KEY_TEMPERATURE = 0,
  KEY_LOCATION,
  KEY_WEATHER_DESCRIPTION,
  KEY_UNIX_TIMESTAMP,
};

void build_weather_label(void) {
  memset(s_data.weather_buffer, 0, BUFFER_SIZE);
  snprintf(s_data.weather_buffer, BUFFER_SIZE, "%s %s %s %s",
            s_data.weather_timestamp,
            s_data.weather_location,
            s_data.weather_temperature,
            s_data.weather_description
          );
  //strcpy(s_data.weather_buffer, "this is two lines of lots and lots of fun");
  text_layer_set_text(s_data.weather_label, (char*) &s_data.weather_buffer);
}

void process_tuple(Tuple *t)
{
  //Get key
  int key = t->key;

  //Get integer value, if present
  int value = t->value->int32;

  //Get string value, if present
  char string_value[BUFFER_SIZE];
  strcpy(string_value, t->value->cstring);

  //Decide what to do
  switch(key) {
    case KEY_TEMPERATURE:
      //Temperature received
      memset(s_data.weather_temperature, 0, BUFFER_SIZE);
      snprintf(s_data.weather_temperature, sizeof("XX \u00B0C"), "%d \u00B0C", value);
      break;
    case KEY_LOCATION:
      memset(s_data.weather_location, 0, BUFFER_SIZE);
      strcpy(s_data.weather_location, string_value);
      build_weather_label();
      break;

    case KEY_WEATHER_DESCRIPTION:
      //Temperature received
      memset(s_data.weather_description, 0, BUFFER_SIZE);
      strcpy(s_data.weather_description, string_value);
      build_weather_label();
      break;
    case KEY_UNIX_TIMESTAMP:
      memset(s_data.weather_timestamp, 0, BUFFER_SIZE);
      int minutes = value / 60;
      if (minutes < 60) {
        snprintf(s_data.weather_timestamp, BUFFER_SIZE, "%s%d min%s", minutes >= 0 ? "" : "-", minutes, minutes > 1 ? "s" : "");
      } else {
        int hours = minutes / 60;
        snprintf(s_data.weather_timestamp, BUFFER_SIZE, "%s%d hr%s", hours >= 0 ? "" : "-", hours, hours > 1 ? "s" : "");
      }

      build_weather_label();
      break;
  }
}

static void in_received_handler(DictionaryIterator *iter, void *context)
{
  //Get data
  Tuple *t = dict_read_first(iter);
  if (t) {
    process_tuple(t);
  }
  //Get next
  while(t != NULL) {
    t = dict_read_next(iter);
    if (t) {
      process_tuple(t);
    }
  }
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

void send_int(uint8_t key, uint8_t cmd)
{
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);

  Tuplet value = TupletInteger(key, cmd);
  dict_write_tuplet(iter, &value);

  app_message_outbox_send();
}

static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
  update_time(tick_time);
  update_date(tick_time);
  //Every ten minutes
  if (tick_time->tm_min % 10 == 0) {
    //Send an arbitrary message, the response will be handled by in_received_handler()
    send_int(5, 5);
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
  app_message_open(512, 512);   //Large input and output buffer sizes

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
