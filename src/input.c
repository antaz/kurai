#include "input.h"
#include "cursor.h"
#include "keyboard.h"

void new_input(struct wl_listener *listener, void *data) {
  struct k_state *state = wl_container_of(listener, state, new_input);
  struct wlr_input_device *device = data;
  switch (device->type) {
  case WLR_INPUT_DEVICE_KEYBOARD:
    wlr_log(WLR_INFO, "New keyboard");
    // handle keyboard device
    init_keyboard(state, device);
    break;
  case WLR_INPUT_DEVICE_POINTER:
    wlr_log(WLR_INFO, "New pointer");
    // handle pointer device
    init_cursor(state, device);
    break;
  default:
    wlr_log(WLR_INFO, "New unsupported device");
  }

  uint32_t caps = WL_SEAT_CAPABILITY_POINTER;
  if (!wl_list_empty(&state->keyboards)) {
    caps |= WL_SEAT_CAPABILITY_KEYBOARD;
  }
  wlr_seat_set_capabilities(state->seat, caps);
}

void init_input(struct k_state *state) {
  state->new_input.notify = new_input;
  wl_signal_add(&state->backend->events.new_input, &state->new_input);
}