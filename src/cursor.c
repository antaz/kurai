#include "cursor.h"
#include "xdg_shell.h"

struct k_toplevel *toplevel_at(struct k_state *state, double lx, double ly,
                               struct wlr_surface **surface, double *sx,
                               double *sy) {
  struct wlr_scene_node *node =
      wlr_scene_node_at(&state->scene->tree.node, lx, ly, sx, sy);
  if (node == NULL || node->type != WLR_SCENE_NODE_BUFFER) {
    return NULL;
  }
  struct wlr_scene_buffer *scene_buffer = wlr_scene_buffer_from_node(node);
  struct wlr_scene_surface *scene_surface =
      wlr_scene_surface_try_from_buffer(scene_buffer);
  if (!scene_surface) {
    return NULL;
  }

  *surface = scene_surface->surface;
  struct wlr_scene_tree *tree = node->parent;
  while (tree != NULL && tree->node.data == NULL) {
    tree = tree->node.parent;
  }
  return tree->node.data;
}

static void process_cursor_motion(struct k_state *state, uint32_t time) {
  if (state->cursor->cursor_mode == K_CURSOR_MOVE) {
    return;
  } else if (state->cursor->cursor_mode == K_CURSOR_RESIZE) {
    return;
  }

  double sx, sy;
  struct wlr_seat *seat = state->seat;
  struct wlr_surface *surface = NULL;
  struct k_toplevel *toplevel =
      toplevel_at(state, state->cursor->wlr_cursor->x,
                  state->cursor->wlr_cursor->y, &surface, &sx, &sy);

  if (!toplevel) {
    wlr_cursor_set_xcursor(state->cursor->wlr_cursor,
                           state->cursor->xcursor_manager, "default");
  }

  if (surface) {
    wlr_seat_pointer_notify_enter(seat, surface, sx, sy);
    wlr_seat_pointer_notify_motion(seat, time, sx, sy);
  } else {
    wlr_seat_pointer_clear_focus(seat);
  }
}

static void cursor_motion_absolute(struct wl_listener *listener, void *data) {
  struct k_cursor *cursor = wl_container_of(listener, cursor, motion_absolute);
  struct wlr_pointer_motion_absolute_event *event = data;
  wlr_cursor_warp_absolute(cursor->wlr_cursor, &event->pointer->base, event->x,
                           event->y);

  process_cursor_motion(cursor->state, event->time_msec);
}

static void cursor_motion(struct wl_listener *listener, void *data) {
  struct k_cursor *cursor = wl_container_of(listener, cursor, motion);
  struct wlr_pointer_motion_event *event = data;
  wlr_cursor_move(cursor->wlr_cursor, &event->pointer->base, event->delta_x,
                  event->delta_y);

  process_cursor_motion(cursor->state, event->time_msec);
}

static void cursor_axis(struct wl_listener *listener, void *data) {
  struct k_cursor *cursor = wl_container_of(listener, cursor, axis);
  struct wlr_pointer_axis_event *event = data;
  wlr_seat_pointer_notify_axis(cursor->state->seat, event->time_msec,
                               event->orientation, event->delta,
                               event->delta_discrete, event->source);
}

static void cursor_frame(struct wl_listener *listener, void *data) {
  struct k_cursor *cursor = wl_container_of(listener, cursor, frame);
  wlr_seat_pointer_notify_frame(cursor->state->seat);
}

static void cursor_button(struct wl_listener *listener, void *data) {
  struct k_cursor *cursor = wl_container_of(listener, cursor, button);
  struct wlr_pointer_button_event *event = data;

  wlr_seat_pointer_notify_button(cursor->state->seat,
			event->time_msec, event->button, event->state);

  double sx, sy;
	struct wlr_surface *surface = NULL;
	struct k_toplevel *toplevel = toplevel_at(cursor->state,
			cursor->state->cursor->wlr_cursor->x, cursor->state->cursor->wlr_cursor->y, &surface, &sx, &sy);
	if (event->state == WLR_BUTTON_RELEASED) {
		cursor->cursor_mode = K_CURSOR_PASSTHROUGH;
		cursor->state->grabbed_toplevel = NULL;
	} else {
		focus_toplevel(toplevel, surface);
	}
}

static void request_cursor(struct wl_listener *listener, void *data) {
  struct k_cursor *cursor = wl_container_of(listener, cursor, request_cursor);
  struct wlr_seat_pointer_request_set_cursor_event *event = data;

  struct wlr_seat_client *focused_client =
      cursor->state->seat->pointer_state.focused_client;

  if (focused_client == event->seat_client) {
    wlr_cursor_set_surface(cursor->wlr_cursor, event->surface, event->hotspot_x,
                           event->hotspot_y);
  }
}

void init_cursor(struct k_state *state, struct wlr_input_device *device) {
  struct k_cursor *cursor = calloc(1, sizeof(struct k_cursor));
  cursor->wlr_cursor = wlr_cursor_create();
  cursor->cursor_mode = K_CURSOR_PASSTHROUGH;
  cursor->state = state;
  state->cursor = cursor;

  const char *xcursor_size = getenv("XCURSOR_SIZE");
  cursor->xcursor_manager = wlr_xcursor_manager_create(
      getenv("XCURSOR_THEME"),
      xcursor_size ? strtoul(xcursor_size, (char **)NULL, 10) : 24);

  cursor->axis.notify = cursor_axis;
  wl_signal_add(&cursor->wlr_cursor->events.axis, &cursor->axis);

  cursor->frame.notify = cursor_frame;
  wl_signal_add(&cursor->wlr_cursor->events.frame, &cursor->frame);

  cursor->motion.notify = cursor_motion;
  wl_signal_add(&cursor->wlr_cursor->events.motion, &cursor->motion);

  cursor->motion_absolute.notify = cursor_motion_absolute;
  wl_signal_add(&cursor->wlr_cursor->events.motion_absolute,
                &cursor->motion_absolute);

  cursor->button.notify = cursor_button;
  wl_signal_add(&cursor->wlr_cursor->events.button,
                &cursor->button);

  cursor->request_cursor.notify = request_cursor;
  wl_signal_add(&state->seat->events.request_set_cursor,
                &cursor->request_cursor);

  wlr_cursor_attach_output_layout(cursor->wlr_cursor, state->output_layout);
  wlr_cursor_attach_input_device(cursor->wlr_cursor, device);
}

void destroy_cursor(struct k_cursor *cursor) {
  wlr_xcursor_manager_destroy(cursor->xcursor_manager);
  wlr_cursor_destroy(cursor->wlr_cursor);
  free(cursor);
}