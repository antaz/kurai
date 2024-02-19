#include "state.h"
#include "output.h"
#include "xdg_shell.h"

bool init_state(struct k_state *state) {
  // create display
  state->display = wl_display_create();
  if (state->display == NULL) {
    return false;
  }

  // store the Event Loop
  state->event_loop = wl_display_get_event_loop(state->display);
  if (state->event_loop == NULL) {
    return false;
  }

  // create backend
  state->backend = wlr_backend_autocreate(state->display, &state->session);
  if (state->backend == NULL) {
    return false;
  }

  // create renderer
  state->renderer = wlr_renderer_autocreate(state->backend);
  if (state->renderer == NULL) {
    return false;
  }

  // initialize display
  wlr_renderer_init_wl_display(state->renderer, state->display);

  // create allocator
  state->allocator = wlr_allocator_autocreate(state->backend, state->renderer);
  if (state->allocator == NULL) {
    return false;
  }

  // create compositor
  state->compositor = wlr_compositor_create(state->display, 5, state->renderer);
  state->subcompositor = wlr_subcompositor_create(state->display);

  // output layout
  state->output_layout = wlr_output_layout_create();

  // output manager
  state->xdg_output_manager =
      wlr_xdg_output_manager_v1_create(state->display, state->output_layout);

  return true;
}

bool start_backend(struct k_state *state) {
  init_output(state);

  // create the scene
  state->scene = wlr_scene_create();
  state->scene_layout =
      wlr_scene_attach_output_layout(state->scene, state->output_layout);

  // create socket
  const char *socket = wl_display_add_socket_auto(state->display);
  if (!socket) {
    wlr_log(WLR_ERROR, "FAILED TO CREATE SOCKET\n");
    return false;
  }

  // start backend
  if (!wlr_backend_start(state->backend)) {
    wlr_backend_destroy(state->backend);
    wl_display_destroy(state->display);
    wlr_log(WLR_ERROR, "FAILED TO START BACKEND\n");
    return false;
  }

  setenv("WAYLAND_DISPLAY", socket, true);

  // create device manager
  wlr_data_device_manager_create(state->display);

  init_xdg_shell(state);

  return true;
}