#pragma once

// Caps (CL_MEH) — Tap: Escape | Hold: Meh (Ctrl+Shift+Alt)
// Permissive hold: if another key is pressed AND released while Caps is held, it's Meh + that key,
// even inside the tapping term. Rolling (Caps released first) stays Esc.
#define PERMISSIVE_HOLD_PER_KEY  // only applies to CL_MEH, see get_permissive_hold()

// Caps Word: double-tap either Shift to toggle capitalization until space/punctuation
// No extra config needed in modern QMK — it's on by default