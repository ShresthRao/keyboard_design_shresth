#include QMK_KEYBOARD_H

static uint8_t tail_count = 0;
static uint16_t anim_timer = 0;
static bool is_folding = false;

/* 3x4 Layout: 
   Row 0: Encoder/Mute | Swipe Up | Speed- | Speed+
   Row 1: Swipe Left   | Spotlight| Swipe Right | Power
   Row 2: Bright-      | Bright+  | Copy   | Paste
*/
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT_ortho_3x4(
        KC_MUTE,        LCTL(KC_UP),    S(KC_COMMA),    S(KC_DOT), 
        LCTL(KC_LEFT),  LGUI(KC_SPC),   LCTL(KC_RIGHT), KC_PWR,    
        KC_SCLN,        KC_SCRL,        KC_COPY,        KC_PASTE   
    )
};

// Grow tails on keypress
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        if (!is_folding) {
            if (tail_count < 9) {
                tail_count++;
            } else {
                is_folding = true;
                anim_timer = timer_read();
            }
        }
    }
    return true;
}

#ifdef OLED_ENABLE
// Graphical flame icon (8x8 pixels)
static const char PROGMEM flame_icon[] = {
    0x00, 0x10, 0x38, 0x7c, 0xfe, 0x7c, 0x38, 0x00
};

void render_kitsune_flames(void) {
    const uint8_t root_x = 7; // Bottom Center
    const uint8_t root_y = 3;

    // Relative coordinates for the 9-tail fan
    static const int8_t tail_map[9][2] = {
        {-6, 0}, {-5, -1}, {-3, -2}, {-1, -3}, {0, -3}, {1, -3}, {3, -2}, {5, -1}, {6, 0}
    };

    // Folding logic (10th press triggers sequence)
    if (is_folding && timer_elapsed(anim_timer) > 60) {
        if (tail_count > 0) {
            tail_count--;
            anim_timer = timer_read();
        } else {
            is_folding = false;
        }
    }

    // Draw only active flames
    for (uint8_t i = 0; i < tail_count; i++) {
        oled_set_cursor(root_x + tail_map[i][0], root_y + tail_map[i][1]);
        oled_write_raw_P(flame_icon, 8); 
    }
}

bool oled_task_user(void) {
    render_kitsune_flames();
    return false;
}
#endif

// Volume Control
bool encoder_update_user(uint8_t index, bool clockwise) {
    if (index == 0) {
        if (clockwise) tap_code(KC_VOLU);
        else tap_code(KC_VOLD);
    }
    return true;
}