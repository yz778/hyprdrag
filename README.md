# hyprdrag

`hyprdrag` is a Hyprland plugin to enable windowrules and IPC events for window move and resize events.

It assigns temporary tags whenever a window is moved or resized. This opens up new possibilities with windowrule configs. Here's the config used in the demo:

## Install with `hyprpm`

Then add this repository:
```bash
hyprpm add https://github.com/yz778/hyprdrag
```
Enable the plugin:
```bash
hyprpm enable hyprdrag
```
## Manual install

1. Clone this repository
2. Build plugin: `make -C src all`
3. Add manuall to your `hyprland.conf`:
 ```
 plugin = /full_path_to/hyprdrag.so`
 ```

## Configuration

Add window rules to your `hyprland.conf`, for example:

```ini
windowrule = noblur, tag:dragging-move
windowrule = noblur, tag:dragging-resize
windowrule = opacity 0.50 override, tag:dragging-move
windowrule = opacity 0.80 override, tag:dragging-resize
windowrule = bordersize 5, tag:dragging-move
windowrule = bordersize 5, tag:dragging-resize
windowrule = bordercolor rgba(ffa726ee) rgba(ffeb3bee) 120deg, tag:dragging-move
windowrule = bordercolor rgba(ffcc80ee) rgba(fff9c4dd) 120deg, tag:dragging-resize

```

## IPC Events

Hyprdrag emits four new IPC events:

```
dragstart-move>>[window address]
```

```
dragend-move>>[window address]
```

```
dragstart-resize>>[window address]
```

```
dragend-resize>>[window address]
```