# SimTower resource extraction (dump layout)

When OpenSkyscraper dumps resources from `SIMTOWER.EXE`, it writes files under the dump path as follows.

## Bitmaps

- **Named bitmaps** (e.g. `simtower/condo.png`, `simtower/office.png`)  
  Composed images used by the game: combined and paletted from raw resources.

- **`bitmaps/raw/`**  
  Every bitmap resource from the executable, one file per resource ID, as `0x????.bmp`.  
  These are standard Windows BMP (resource types 0x8002 and 0xFF02), with headers added by the loader where needed. Use this folder for a complete, by-ID extraction.

- **`bitmaps/unhandled/`**  
  Only bitmap IDs that are **not** referenced by the current loader (no `loadBitmap` / `loadAnimatedBitmap` / map/cloud usage).  
  Same format as `raw/` (`0x????.bmp`). Use this to find candidates for new mappings or to see what the game still doesn’t use.

## Sounds

- **Named sounds** (e.g. `simtower/construction/normal.wav`)  
  Sound resources that are loaded and used by the game (resource type 0xFF0A).

- **`sounds/unhandled/`**  
  Sound resources that are not in the loader’s named list, written as `0x????.wav`.  
  SimTower stores these with standard WAV headers in the executable, so these files should be playable as-is.

## References

- **Resource types:** [Used Resource Types.md](Used%20Resource%20Types.md)
- **NE executable layout:** [Windows NE Format.txt](Windows%20NE%20Format.txt) and [Windows NE Resource Types.txt](Windows%20NE%20Resource%20Types.txt)
- **External:** [tower-docs](https://github.com/dfloer/tower-docs), [VG Resource – SimTower assets](https://www.vg-resource.com/thread-36194-nextoldest.html)
