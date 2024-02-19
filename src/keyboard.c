#include "keyboard.h"
#include "xdg_shell.h"

static bool handle_keybinding(struct k_state *state, xkb_keysym_t sym) {
  switch (sym) {
  case XKB_KEY_Escape:
    wl_display_terminate(state->display);
    break;
  case XKB_KEY_F1:
    /* Cycle to the next toplevel */
    if (wl_list_length(&state->toplevels) < 2) {
      break;
    }
    struct k_toplevel *next_toplevel =
        wl_container_of(state->toplevels.prev, next_toplevel, link);
    focus_toplevel(next_toplevel, next_toplevel->xdg_toplevel->base->surface);
    break;
  default:
    return false;
  }
  return true;
}
static void keyboard_key(struct wl_listener *listener, void *data) {
  struct k_keyboard *keyboard = wl_container_of(listener, keyboard, key);
  struct k_state *state = keyboard->state;
  struct wlr_keyboard_key_event *event = data;
  struct wlr_seat *seat = state->seat;

  /* Translate libinput keycode -> xkbcommon */
  uint32_t keycode = event->keycode + 8;
  /* Get a list of keysyms based on the keymap for this keyboard */
  const xkb_keysym_t *syms;
  int nsyms =
      xkb_state_key_get_syms(keyboard->wlr_keyboard->xkb_state, keycode, &syms);

  bool handled = false;
  uint32_t modifiers = wlr_keyboard_get_modifiers(keyboard->wlr_keyboard);
  if ((modifiers & WLR_MODIFIER_ALT) &&
      event->state == WL_KEYBOARD_KEY_STATE_PRESSED) {
    for (int i = 0; i < nsyms; i++) {
      handled = handle_keybinding(state, syms[i]);
    }
  }

  if (!handled) {
    /* Otherwise, we pass it along to the client. */
    wlr_seat_set_keyboard(seat, keyboard->wlr_keyboard);
    wlr_seat_keyboard_notify_key(seat, event->time_msec, event->keycode,
                                 event->state);
  }
}

static void keyboard_modifiers(struct wl_listener *listener, void *data) {}

static void keyboard_destroy(struct wl_listener *listener, void *data) {}

void init_keyboard(struct k_state *state, struct wlr_input_device *device) {
  struct k_keyboard *keyboard = calloc(1, sizeof(struct k_keyboard));
  keyboard->state = state;
  keyboard->wlr_keyboard = wlr_keyboard_from_input_device(device);

  struct xkb_context *context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
  struct xkb_keymap *keymap =
      xkb_keymap_new_from_names(context, NULL, XKB_KEYMAP_COMPILE_NO_FLAGS);

  wlr_keyboard_set_keymap(keyboard->wlr_keyboard, keymap);
  xkb_keymap_unref(keymap);
  xkb_context_unref(context);
  wlr_keyboard_set_repeat_info(keyboard->wlr_keyboard, 25, 600);

  // listen to keyboard events
  keyboard->key.notify = keyboard_key;
  wl_signal_add(&keyboard->wlr_keyboard->events.key, &keyboard->key);
  keyboard->modifiers.notify = keyboard_modifiers;
  wl_signal_add(&keyboard->wlr_keyboard->events.modifiers,
                &keyboard->modifiers);
  keyboard->destroy.notify = keyboard_destroy;
  wl_signal_add(&device->events.destroy, &keyboard->destroy);

  wlr_seat_set_keyboard(state->seat, keyboard->wlr_keyboard);
  wl_list_insert(&state->keyboards, &keyboard->link);
}
