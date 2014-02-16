#include "shared.h"
#include "frozen_pebble.h"
#include "app_message.h"

void on_new_message(DictionaryIterator *received, void *context) {
  Tuple *cmd_tuple = dict_find(received, 0);
  switch(cmd_tuple->value->int16) {
    case 1: // Track update
      update_track(received, context);
      break;
    case 2: // Playpause update:
      update_playpause(received, context);
      break;
  }
}

void update_track(DictionaryIterator *received, void *context) {
  Tuple *title_tuple = dict_find(received, 1);
  text_layer_set_text(title_layer, title_tuple->value->cstring);

  Tuple *artist_tuple = dict_find(received, 2);
  text_layer_set_text(artist_layer, artist_tuple->value->cstring);
}

void update_playpause(DictionaryIterator *received, void *context) {

}

void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  send_message_to_phone(TupletInteger(0, 1)); // Playpause
}

void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  send_message_to_phone(TupletInteger(0, 3)); // Prev Track
}

void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  send_message_to_phone(TupletInteger(0, 2)); // Next track
}

void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  title_layer = text_layer_create((GRect) { .origin = { 0, 0 }, .size = { bounds.size.w, 28 } });
  text_layer_set_text_alignment(title_layer, GTextAlignmentCenter);
  text_layer_set_font(title_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text(title_layer, "Loading...");
  layer_add_child(window_layer, text_layer_get_layer(title_layer));

  artist_layer = text_layer_create((GRect) { .origin = { 0, 28 }, .size = { bounds.size.w, 28 } });
  text_layer_set_text_alignment(artist_layer, GTextAlignmentCenter);
  text_layer_set_font(artist_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  layer_add_child(window_layer, text_layer_get_layer(artist_layer));
}

void window_unload(Window *window) {
  text_layer_destroy(title_layer);
  text_layer_destroy(artist_layer);
}

void accel_tap_handler(AccelAxisType axis, int32_t direction) {
  // Uncomment the below line for FROZEN ROCK OUT MODE :D :D :D
  send_message_to_phone(TupletInteger(0, 4));
}

void init(void) {
  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);

  init_app_message_listener();

  accel_tap_service_subscribe(&accel_tap_handler);
}

void deinit(void) {
  window_destroy(window);
}
