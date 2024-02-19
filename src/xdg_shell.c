#include "xdg_shell.h"

void new_xdg_toplevel(struct wl_listener *listener, void *data) {
	struct k_state *state = wl_container_of(listener, state, new_xdg_surface);
	struct wlr_xdg_toplevel *xdg_toplevel = data;

	struct k_toplevel *toplevel = calloc(1, sizeof(struct k_toplevel));
	toplevel->state = state;
	toplevel->xdg_toplevel = xdg_toplevel;

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