#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "state.h"

struct k_keyboard {
  struct k_state *state;
  struct wlr_keyboard *wlr_keyboard;

  struct wl_listener key;
  struct wl_listener modifiers;
  struct wl_listener destroy;

  struct wl_list link;
};

void init_keyboard(struct k_state *state, struct wlr_input_device *device);

#endif /* KEYBOARD_H */
