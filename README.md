# Kurai
Simple `wlroots` based wayland compositor

## Dependencies
- Wayland
- wlroots 0.17
- libinput
- xkbcommon

## build
```
make -j4
```

## Run
foot is a wayland terminal
```
./build/release/kurai -s /bin/foot
```

## Roadmap

- [x] Backend creation and initialization
- [x] Output create and initalize
- [x] Keyboard implementation
- [x] Pointer (and cursor) implementation
- [x] XDG-SHELL implementation
- [ ] XDG-LAYER-SHELL implementation
- [ ] `wlr_idle_inhibit_v1`
- [ ] Toplevels indicators
- [ ] Configuration