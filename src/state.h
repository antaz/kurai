#ifndef STATE_H
#define STATE_H

#include <wlr/backend.h>
#include <wlr/render/allocator.h>
#include <wlr/render/wlr_renderer.h>
#include <wlr/types/wlr_compositor.h>
#include <wlr/types/wlr_data_device.h>
#include <wlr/types/wlr_gamma_control_v1.h>
#include <wlr/types/wlr_idle_notify_v1.h>
#include <wlr/types/wlr_output_layout.h>
#include <wlr/types/wlr_scene.h>
#include <wlr/types/wlr_screencopy_v1.h>
#include <wlr/types/wlr_subcompositor.h>
#include <wlr/types/wlr_xdg_output_v1.h>
#include <wlr/util/log.h>

#include <stdlib.h>

struct k_state {
  struct wl_display *display;
  struct wl_event_loop *event_loop;

  struct wlr_allocator *allocator;
  struct wlr_backend *backend;
  struct wlr_compositor *compositor;
  struct wlr_subcompositor *subcompositor;
  struct wlr_renderer *renderer;
  struct wlr_session *session;
  struct wlr_seat *seat;
  struct wlr_output_layout *output_layout;
  struct wlr_scene *scene;

  // Listeners for state events
  struct wl_listener new_input;
  struct wl_listener new_output;

  // Lists
  struct wl_list outputs;
};

bool init_state(struct k_state *state);

bool start_backend(struct k_state *state);

#endif /* STATE_H */