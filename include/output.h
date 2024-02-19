#ifndef OUTPUT_H
#define OUTPUT_H

#include "state.h"
#include <wlr/types/wlr_output_management_v1.h>

struct k_output {
  struct k_state *state;
  struct wlr_output *wlr_output;
  struct wlr_scene_output *scene_output;

  // layers
  struct {
    struct wlr_scene_tree *shell_background;
    struct wlr_scene_tree *shell_bottom;
    struct wlr_scene_tree *shell_fullscreen;
    struct wlr_scene_tree *shell_overlay;
    struct wlr_scene_tree *shell_top;
  } layers;

  struct wlr_box geometry;

  // output events
  struct wl_listener frame;
  struct wl_listener request_state;
  struct wl_listener destroy;

  // link to next output
  struct wl_list link;
};

void init_output(struct k_state *state);

#endif /* OUTPUT_H */