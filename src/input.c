#include "input.h"
#include "cursor.h"

void new_input(struct wl_listener *listener, void *data) {
  struct k_state *state = wl_container_of(listener, state, new_input);
  struct wlr_input_device *device = data;
  switch (device->type) {
  case WLR_INPUT_DEVICE_KEYBOARD:
    wlr_log(WLR_INFO, "New keyboard\n");
    // handle keyboard device
    break;
  case WLR_INPUT_DEVICE_POINTER:
    wlr_log(WLR_INFO, "New pointer\n");
    // handle pointer device
    init_cursor(state);
    break;
  default:
    wlr_log(WLR_INFO, "New unsupported device\n");
  }
}

void init_input(struct k_state *state) {
  state->new_input.notify = new_input;
  wl_signal_add(&state->backend->events.new_input, &state->new_input);
}