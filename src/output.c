#include "output.h"

void output_frame(struct wl_listener *listener, void *data) {
  struct k_output *output = wl_container_of(listener, output, frame);
  struct wlr_scene *scene = output->state->scene;
  struct wlr_scene_output *scene_output =
      wlr_scene_get_scene_output(scene, output->wlr_output);

  wlr_scene_output_commit(scene_output, NULL);

  struct timespec now;
  clock_gettime(CLOCK_MONOTONIC, &now);
  wlr_scene_output_send_frame_done(scene_output, &now);
}

void output_request_state(struct wl_listener *listener, void *data) {
  struct k_output *output = wl_container_of(listener, output, request_state);
  const struct wlr_output_event_request_state *event = data;

  wlr_output_commit_state(output->wlr_output, event->state);
}

void output_destroy(struct wl_listener *listener, void *data) {
  struct k_output *output = wl_container_of(listener, output, destroy);

  wl_list_remove(&output->frame.link);
  wl_list_remove(&output->request_state.link);
  wl_list_remove(&output->destroy.link);

  wl_list_remove(&output->link);
  free(output);
}

void new_output(struct wl_listener *listener, void *data) {
  struct k_state *state = wl_container_of(listener, state, new_output);
  struct wlr_output *wlr_output = data;

  wlr_log(WLR_DEBUG, "New output %s", wlr_output->name);

  wlr_output_init_render(wlr_output, state->allocator, state->renderer);

  // set output mode
  struct wlr_output_mode *mode = wlr_output_preferred_mode(wlr_output);
  struct wlr_output_state output_state;
  wlr_output_state_init(&output_state);
  wlr_output_state_set_enabled(&output_state, true);

  if (mode != NULL) {
    wlr_output_state_set_mode(&output_state, mode);
  }

  wlr_output_commit_state(wlr_output, &output_state);
  wlr_output_state_finish(&output_state);

  // initialize our output state
  struct k_output *output = calloc(1, sizeof(struct k_output));
  output->state = state;
  output->wlr_output = wlr_output;
  wlr_output->data = output;

  /* Initializes the layers */
  size_t num_layers = sizeof(output->layers) / sizeof(struct wlr_scene_node *);
  for (size_t i = 0; i < num_layers; i++) {
    ((struct wlr_scene_node **)&output->layers)[i] =
        &wlr_scene_tree_create(&state->scene->tree)->node;
  }

  // insert output into list
  wl_list_insert(&state->outputs, &output->link);

  // listen to output events
  output->frame.notify = output_frame;
  wl_signal_add(&wlr_output->events.frame, &output->frame);

  output->request_state.notify = output_request_state;
  wl_signal_add(&wlr_output->events.request_state, &output->request_state);

  output->destroy.notify = output_destroy;
  wl_signal_add(&wlr_output->events.destroy, &output->destroy);

  // output configuration
  struct wlr_output_configuration_v1 *configuration =
      wlr_output_configuration_v1_create();
  wlr_output_configuration_head_v1_create(configuration, wlr_output);
  wlr_output_manager_v1_set_configuration(state->wlr_output_manager,
                                          configuration);

  // add output layout
  struct wlr_output_layout_output *l_output =
      wlr_output_layout_add_auto(state->output_layout, wlr_output);

  if (!l_output) {
    return;
  }

  output->scene_output = wlr_scene_output_create(state->scene, wlr_output);
  wlr_scene_output_layout_add_output(state->scene_layout, l_output,
                                     output->scene_output);
}

void init_output(struct k_state *state) {
  // initialize list of outputs
  wl_list_init(&state->outputs);

  state->wlr_output_manager = wlr_output_manager_v1_create(state->display);

  // listen to new_output event
  state->new_output.notify = new_output;
  wl_signal_add(&state->backend->events.new_output, &state->new_output);
}
