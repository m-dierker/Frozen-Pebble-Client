#pragma once
#include "shared.h"
#include "app_message.h"

static Window *window;
static TextLayer *title_layer;
static TextLayer *artist_layer;

void on_new_message(DictionaryIterator *received, void *context);
void update_track(DictionaryIterator *received, void *context);
void update_playpause(DictionaryIterator *received, void *context);

void select_click_handler(ClickRecognizerRef recognizer, void *context);
void up_click_handler(ClickRecognizerRef recognizer, void *context);
void down_click_handler(ClickRecognizerRef recognizer, void *context);
void click_config_provider(void *context);

void window_load(Window *window);
void window_unload(Window *window);

void accel_tap_handler(AccelAxisType axis, int32_t direction);

void init(void);

void deinit(void);