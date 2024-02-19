#ifndef XDG_SHELL_H
#define XDG_SHELL_H

#include "state.h"

struct k_toplevel {
	struct k_state *state;
	struct wlr_xdg_toplevel *xdg_toplevel;
	struct wlr_scene_tree *scene_tree;

	// listeners for toplevel events
	struct wl_listener map;
	struct wl_listener unmap;
	struct wl_listener commit;
	struct wl_listener destroy;
	struct wl_listener new_popup;
	struct wl_listener request_fullscreen;
	struct wl_listener request_maximize;
	struct wl_listener request_minimize;
	struct wl_listener request_move;
	struct wl_listener request_resize;

	// geometry of the toplevel
	struct wlr_box geometry;

	// link to next toplevel node
	struct wl_list link;
};

void init_xdg_shell(struct k_state *state);

#endif /* XDG_SHELL_H */