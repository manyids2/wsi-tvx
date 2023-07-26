# wsi-tvx

Whole slide image - terminal viewer.

Refactor of `wsi-tv`, with focus on clarity, caching.

![home](./screenshots/home.png)

![thumbnail](./screenshots/thumbnail.png)

![debug](./screenshots/debug.png)

## Installation

```bash
make
```

## Usage

```bash
./bin/wsi-tvx <slidepath>
```

## Keys

- `q` - quit
- `r` - reload
- `d` - toggle debug info
- `t` - toggle thumbnail

- `j` / `↓` - down
- `k` / `↑` - up
- `l` / `←` - right
- `h` / `→` - left
- `i` - zoom in ( keeps top left constant )
- `o` - zoom out ( keeps top left constant )

## Limitations

- Needs terminal support for [kitty image protocol](https://sw.kovidgoyal.net/kitty/graphics-protocol/).
- Terminal needs to respond to `\x1b[14t` request for pixel dimensions.
- Only tested on Linux.
- Not optimal base64.
  - Use [race64](https://github.com/skeeto/race64) for base64 encoding ( with SIMD optimizations ).
  - However, we convert RGBA to RGB to send less info ( 25% lesser than original for each tile ).

## Issues

- Crashes sometimes, use `r` to reset.
- Need to somehow make the crashes reproducible.

## Tests

```bash
make test
```

### `test-tiles`, `test-base64`

Load from openslide and print with kitty as fast as possible,
including using multiple threads.

```bash
 NUM_PIXELS : 256 * 256 * 32
 NUM_TILES  : 4096
 NUM_THREADS: 4096

❯ test_serial
Executed in   22.78 secs    fish           external
  usr time   15.23 secs    0.00 millis   15.23 secs
  sys time    1.61 secs    1.03 millis    1.61 secs*

❯ test_parallel
Executed in   17.31 secs    fish           external
 usr time    7.40 secs   20.87 millis    7.38 secs
 sys time    2.68 secs    8.27 millis    2.67 secs
```

## Further analysis

Major work is done by:

```c
void tiles_load_view(
    tiles_t *tiles,
    slide_t *slide,
    view_t *view,
    world_t *world,
    int force
)`
```

Currently:

1. Clears all tiles
2. Handles full reload ( `clear_screen`, `slide_provision_thumbnail` )
3. Handles display and loading of tiles
   a. display loaded visible tiles
   b. load and display remaining visible tiles
   c. load left and right margin ( around visible )
   d. load top and bottom margin ( around visible )
   e. diplay visible tiles again

Source of issues is no control over which tiles are overwritten.
Most probably, tiles that are visible are being overwritten for loading margins, etc.

Possible solutions:

- For each tile, compute 'moves' required to become visible
  - Keep track of requested, loading, loaded, old :
    - When loading requested tiles, load into `kitty_id` from old.
    - Works well with threads
  - Assign priorities :
    - Maintain arrays for each distance, and load into those far away.
    - Try to maintain distribution over distances.

Testing:

- Using `draw_tiles_debug`
  - print tiles in each queue
  - `sleep(1)` to simulate slow loading
