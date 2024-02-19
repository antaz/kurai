#include "xdg_shell.h"

void focus_toplevel(struct k_toplevel *toplevel, struct wlr_surface *surface) {
  if (toplevel == NULL ||
      toplevel->xdg_toplevel->base->role != WLR_XDG_SURFACE_ROLE_TOPLEVEL) {
    return;
  }

  struct wlr_xdg_surface *xdg_surface =
      wlr_xdg_surface_try_from_wlr_surface(surface);

  struct k_state *state = toplevel->state;
  struct wlr_seat *seat = state->seat;
  struct wlr_surface *prev_surface = seat->keyboard_state.focused_surface;

  if (prev_surface) {
    struct wlr_xdg_toplevel *prev_toplevel =
        wlr_xdg_toplevel_try_from_wlr_surface(prev_surface);
    if (prev_toplevel != NULL) {
      wlr_xdg_toplevel_set_activated(prev_toplevel, false);
    }
  }
  struct wlr_keyboard *keyboard = wlr_seat_get_keyboard(seat);
  wlr_scene_node_raise_to_top(&toplevel->scene_tree->node);
  wl_list_remove(&toplevel->link);
  wl_list_insert(&state->toplevels, &toplevel->link);
  wlr_xdg_toplevel_set_activated(toplevel->xdg_toplevel, true);
  if (keyboard != NULL) {
    wlr_seat_keyboard_notify_enter(seat, toplevel->xdg_toplevel->base->surface,
                                   keyboard->keycodes, keyboard->num_keycodes,
                                   &keyboard->modifiers);
  }
}

static void toplevel_map(struct wl_listener *listener, void *data) {}

static void toplevel_unmap(struct wl_listener *listener, void *data) {}

static void toplevel_commit(struct wl_listener *listener, void *data) {}

static void toplevel_destroy(struct wl_listener *listener, void *data) {
  struct k_toplevel *toplevel = wl_container_of(listener, toplevel, destroy);

  wl_list_remove(&toplevel->map.link);
  wl_list_remove(&toplevel->unmap.link);
  wl_list_remove(&toplevel->commit.link);
  wl_list_remove(&toplevel->destroy.link);

  wl_list_remove(&toplevel->request_fullscreen.link);
  wl_list_remove(&toplevel->request_minimize.link);
  wl_list_remove(&toplevel->request_maximize.link);
  wl_list_remove(&toplevel->request_move.link);
  wl_list_remove(&toplevel->request_resize.link);

  wl_list_remove(&toplevel->link);
  free(toplevel);
}

static void toplevel_request_fullscreen(struct wl_listener *listener,
                                        void *data) {}

static void toplevel_request_maximize(struct wl_listener *listener,
                                      void *data) {}

static void toplevel_request_minimize(struct wl_listener *listener,
                                      void *data) {}

static void toplevel_request_move(struct wl_listener *listener, void *data) {}

static void toplevel_request_resize(struct wl_listener *listener, void *data) {}

void new_xdg_toplevel(struct wl_listener *listener, void *data) {
  struct k_state *state = wl_container_of(listener, state, new_xdg_surface);
  struct wlr_xdg_toplevel *xdg_toplevel = data;

  struct k_toplevel *toplevel = calloc(1, sizeof(struct k_toplevel));
  toplevel->state = state;
  toplevel->xdg_toplevel = xdg_toplevel;

  // listen to toplevel events
  toplevel->map.notify = toplevel_map;
  wl_signal_add(&xdg_toplevel->base->surface->events.map, &toplevel->map);
  toplevel->unmap.notify = toplevel_unmap;
  wl_signal_add(&xdg_toplevel->base->surface->events.unmap, &toplevel->unmap);
  toplevel->commit.notify = toplevel_commit;
  wl_signal_add(&xdg_toplevel->base->surface->events.commit, &toplevel->commit);
  toplevel->destroy.notify = toplevel_destroy;
  wl_signal_add(&xdg_toplevel->base->events.destroy, &toplevel->destroy);

  toplevel->scene_tree = wlr_scene_xdg_surface_create(
      &toplevel->state->scene->tree, xdg_toplevel->base);
  toplevel->scene_tree->node.data = toplevel;
  xdg_toplevel->base->data = toplevel->scene_tree;

  toplevel->request_fullscreen.notify = toplevel_request_fullscreen;
  wl_signal_add(&xdg_toplevel->events.request_fullscreen,
                &toplevel->request_fullscreen);
  toplevel->request_maximize.notify = toplevel_request_maximize;
  wl_signal_add(&xdg_toplevel->events.request_maximize,
                &toplevel->request_maximize);
  toplevel->request_minimize.notify = toplevel_request_minimize;
  wl_signal_add(&xdg_toplevel->events.request_minimize,
                &toplevel->request_minimize);
  toplevel->request_move.notify = toplevel_request_move;
  wl_signal_add(&xdg_toplevel->events.request_move, &toplevel->request_move);
  toplevel->request_resize.notify = toplevel_request_resize;
  wl_signal_add(&xdg_toplevel->events.request_resize,
                &toplevel->request_resize);

  wl_list_insert(&toplevel->state->toplevels, &toplevel->link);
}

void new_xdg_surface(struct wl_listener *listener, void *data) {
  struct wlr_xdg_surface *xdg_surface = data;

  if (xdg_surface->role == WLR_XDG_SURFACE_ROLE_TOPLEVEL) {
    new_xdg_toplevel(listener, xdg_surface->toplevel);
  }
}

void init_xdg_shell(struct k_state *state) {
  // create XDG SHELL version 3
  state->xdg_shell = wlr_xdg_shell_create(state->display, 3);

  // TODO: support different WLR versions
  state->new_xdg_surface.notify = new_xdg_surface;
  wl_signal_add(&state->xdg_shell->events.new_surface, &state->new_xdg_surface);
}