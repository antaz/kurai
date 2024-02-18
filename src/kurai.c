#include <stdio.h>

#include "state.h"

int main() {
  wlr_log_init(WLR_DEBUG, NULL);

  struct k_state state = {0};
  if (!init_state(&state)) {
    wlr_log(WLR_ERROR, "Failed to initialize state");
  }

  if (!start_backend(&state)) {
    wlr_log(WLR_ERROR, "Failed to start the backend");
  }

  wl_display_run(state.display);
  return 0;
}