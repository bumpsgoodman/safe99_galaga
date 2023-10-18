#include "precompiled.h"

static keyboard_state_t s_key_states[256];

void on_key_down(const uint8_t key)
{
    if (s_key_states[key] == KEYBOARD_STATE_DOWN || s_key_states[key] == KEYBOARD_STATE_PRESSED)
    {
        s_key_states[key] = KEYBOARD_STATE_PRESSED;
    }
    else
    {
        s_key_states[key] = KEYBOARD_STATE_DOWN;
    }
}

void on_key_up(const uint8_t key)
{
    s_key_states[key] = KEYBOARD_STATE_UP;
}

void reset_keyboard(void)
{
    memset(s_key_states, KEYBOARD_STATE_UP, sizeof(keyboard_state_t) * 256);
}

keyboard_state_t get_key_state(const uint8_t key)
{
    return s_key_states[key];
}