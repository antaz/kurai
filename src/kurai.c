#include <signal.h>
#include <stdio.h>
#include <unistd.h>

#include "state.h"

struct k_state state = {0};

void termination_handler(int signum) {
  switch (signum) {
  case SIGINT:
  case SIGTERM:
    wl_display_terminate(state.display);
    break;
  }
}

int main(int argc, char **argv) {
  wlr_log_init(WLR_DEBUG, NULL);
  char *startup_cmd = NULL;

  int c;
  while ((c = getopt(argc, argv, "s:h")) != -1) {
    switch (c) {
    case 's':
      startup_cmd = optarg;
      break;
    default:
      printf("Usage: %s [-s startup command]\n", argv[0]);
      return 0;
    }
  }
  if (optind < argc) {
    printf("Usage: %s [-s startup command]\n", argv[0]);
    return 0;
  }

  if (!init_state(&state)) {
    wlr_log(WLR_ERROR, "Failed to initialize state");
  }

  if (!start_backend(&state)) {
    wlr_log(WLR_ERROR, "Failed to start the backend");
  }

  if (startup_cmd) {
    if (fork() == 0) {
      execl("/bin/sh", "/bin/sh", "-c", startup_cmd, (char *)NULL);
    }
  }

  if (signal(SIGINT, termination_handler) == SIG_IGN)
    signal(SIGINT, SIG_IGN);
  if (signal(SIGHUP, termination_handler) == SIG_IGN)
    signal(SIGHUP, SIG_IGN);
  if (signal(SIGTERM, termination_handler) == SIG_IGN)
    signal(SIGTERM, SIG_IGN);

  wl_display_run(state.display);

  destroy_state(&state);

  return 0;
}