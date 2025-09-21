# hyprdrag

`hyprdrag` is a Hyprland plugin to enable windowrules and IPC events for window move and resize events. This opens up many possibilities using standard `windowrule` configurations. Here is an example demonstrating transparency while moving and resizing windows:

https://github.com/user-attachments/assets/ab72bcab-2743-4419-9402-9c62e67f6aac

## Install with `hyprpm`

```bash
hyprpm add https://github.com/yz778/hyprdrag
hyprpm enable hyprdrag
```
## Manual install

1. Clone this repository
2. Build plugin: `make -C src all`
3. Add plugin to your `hyprland.conf`:
 ```
 plugin = /full_path_to/hyprdrag.so
 ```

## Configuration

Add window rules to your `hyprland.conf`, for example:

```ini
windowrule = noblur, tag:hyprdrag-move
windowrule = noblur, tag:hyprdrag-resize
windowrule = opacity 0.50 override, tag:hyprdrag-move
windowrule = opacity 0.80 override, tag:hyprdrag-resize
windowrule = bordersize 5, tag:hyprdrag-move
windowrule = bordersize 5, tag:hyprdrag-resize
windowrule = bordercolor rgba(ffa726ee) rgba(ffeb3bee) 120deg, tag:hyprdrag-move
windowrule = bordercolor rgba(ffcc80ee) rgba(fff9c4dd) 120deg, tag:hyprdrag-resize

```

## IPC Events

`hyprdrag` emits four new IPC events:

```
hyprdrag-movestart>>[window address]
```

```
hyprdrag-movestop>>[window address]
```

```
hyprdrag-resizestart>>[window address]
```

```
hyprdrag-resizestop>>[window address]
```