#include "shared.h"
#include "app_message.h"
#include "frozen_pebble.h"

void init_app_message_listener() {
    app_message_register_inbox_received(in_received_handler);
    app_message_register_inbox_dropped(in_dropped_handler);
    app_message_register_outbox_sent(out_sent_handler);
    app_message_register_outbox_failed(out_fail_handler);

    const int inbound_size = 124;
    const int outbound_size = 124;
    app_message_open(inbound_size, outbound_size);
}

void out_sent_handler(DictionaryIterator *sent, void *context) {
    // outgoing message was delivered
}


void out_fail_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
    // outgoing message failed
}


void in_received_handler(DictionaryIterator *received, void *context) {
    APP_LOG(APP_LOG_LEVEL_INFO, "In received handler");
    // incoming message received
    on_new_message(received, context);
}


void in_dropped_handler(AppMessageResult reason, void *context) {
    // incoming message dropped
}

/**
 * Send a given message to the phone. This could be more generalized,
 * but is easy now for convenience.
 *
 * This is generally going to be (0, COMMAND_ID), where COMMAND_ID
 * is a command specified in the Javascript file.
 */
void send_message_to_phone(Tuplet value) {
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);
    dict_write_tuplet(iter, &value);
    app_message_outbox_send();
}
