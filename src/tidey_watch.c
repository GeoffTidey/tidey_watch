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
  char weather_buffer[BUFFER_SIZE];
} s_data;

enum {
  KEY_TEMPERATURE = 0,
  KEY_LOCATION,
  KEY_WEATHER_DESCRIPTION,
  KEY_UNIX_TIMESTAMP,
};

void process_tuple(Tuple *t)
{
  //Get key
  int key = t->key;

  //Get integer value, if present
  int value = t->value->int32;

  //Get string value, if present
  char string_value[BUFFER_SIZE];
  strcpy(string_value, t->value->cstring);

  // struct tm *weather_time;
  // int len;

  //Decide what to do
  switch(key) {
    // case KEY_TEMPERATURE:
    //   //Temperature received
    //   snprintf(s_data.weather_buffer, sizeof("Temperature: XX \u00B0C"), "Temperature: %d \u00B0C", value);
    //   text_layer_set_text(s_data.weather_label, (char*) &s_data.weather_buffer);
    //   break;
    case KEY_WEATHER_DESCRIPTION:
      //Temperature received
      memset(s_data.weather_buffer, 0, BUFFER_SIZE);
      strcpy(s_data.weather_buffer, string_value);
      text_layer_set_text(s_data.weather_label, (char*) &s_data.weather_buffer);
      break;
    // case KEY_UNIX_TIMESTAMP:
    //   memset(s_data.weather_buffer, 0, BUFFER_SIZE);
    //   APP_LOG(APP_LOG_LEVEL_DEBUG, "...recvd tuple key: %i", value);
      //sprintf(s_data.weather_buffer, "%d", value);
      // const time_t * new_time = (const time_t *) value;
      // localtime(new_time);
      // len = strlen(s_data.weather_buffer);
      // strftime(&s_data.weather_buffer[len], BUFFER_SIZE - len, " %R", weather_time);
      //strftime(s_data.weather_buffer, BUFFER_SIZE, " %R", weather_time);

      // break;
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
  //Every five minutes
  // if (tick_time->tm_min % 5 == 0) {
    //Send an arbitrary message, the response will be handled by in_received_handler()
    send_int(5, 5);
  // }
}

static void do_init(void) {
  s_data.window = window_create();
  const bool animated = true;
  window_stack_push(s_data.window, animated);

  window_set_background_color(s_data.window, GColorBlack);

  Layer *root_layer = window_get_root_layer(s_data.window);
  GRect frame = layer_get_frame(root_layer);

  int top_y = 20;
  int bottom_y = 21;

  s_data.weather_label = init_text_layer(GRect(0, 0, frame.size.w, top_y - 1), GColorWhite, GColorBlack, "RESOURCE_ID_GOTHIC_18_BOLD", GTextAlignmentCenter);
  layer_add_child(root_layer, text_layer_get_layer(s_data.weather_label));

  s_data.time_label = init_text_layer(GRect(0, top_y, frame.size.w, frame.size.h - bottom_y - top_y), GColorWhite, GColorBlack, "RESOURCE_ID_BITHAM_30_BLACK", GTextAlignmentLeft);
  layer_add_child(root_layer, text_layer_get_layer(s_data.time_label));

  s_data.date_label = init_text_layer(GRect(0, frame.size.h - bottom_y, frame.size.w, bottom_y), GColorWhite, GColorBlack, "RESOURCE_ID_GOTHIC_18_BOLD", GTextAlignmentCenter);
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
