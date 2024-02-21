#ifndef CURSOR_H
#define CURSOR_H

#include <wlr/types/wlr_cursor.h>
#include <wlr/types/wlr_xcursor_manager.h>

#include "state.h"

enum k_cursor_mode {
  K_CURSOR_PASSTHROUGH,
  K_CURSOR_MOVE,
  K_CURSOR_RESIZE,
};

struct k_cursor {
  struct k_state *state;
  struct wlr_cursor *wlr_cursor;
  struct wlr_xcursor_manager *xcursor_manager;

  enum k_cursor_mode cursor_mode;

  // listen to cursor events
  struct wl_listener motion;
  struct wl_listener motion_absolute;
  struct wl_listener button;
  struct wl_listener axis;
  struct wl_listener frame;
  struct wl_listener request_cursor;
};

void create_cursor(struct k_state *state);
void init_cursor(struct k_state *state, struct wlr_input_device *device);
void destroy_cursor(struct k_cursor *cursor);

#endif /* CURSOR_H */