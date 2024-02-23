#ifndef XDG_LAYER_H
#define XDG_LAYER_H

#include "state.h"
#include <wlr/types/wlr_layer_shell_v1.h>

struct k_layer {
  struct k_state *state;
  struct k_output *output;

  struct wlr_scene_layer_surface_v1 *scene;

  struct wl_listener map;
  struct wl_listener unmap;
  struct wl_listener new_surface;
  struct wl_listener new_popup;
  struct wl_listener destroy;

  bool mapped;
};

void init_layer(struct k_state *state);

#endif /* XDG_LAYER_H */