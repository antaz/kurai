#include "xdg_layer.h"

void new_layer_surface(struct wl_listener *listener, void *data) {
  struct wlr_layer_surface_v1 *layer_surface = data;
}

void init_layer(struct k_state *state) {
  state->layer_shell = wlr_layer_shell_v1_create(state->display, 4);
  state->new_layer_surface.notify = new_layer_surface;
  wl_signal_add(&state->layer_shell->events.new_surface,
                &state->new_layer_surface);
}