#pragma once
#include "shared.h"

void init_app_message_listener();

void out_sent_handler(DictionaryIterator *sent, void *context);
void out_fail_handler(DictionaryIterator *failed, AppMessageResult reason, void *context);
void in_received_handler(DictionaryIterator *received, void *context);
void in_dropped_handler(AppMessageResult reason, void *context);

void send_message_to_phone(Tuplet value);