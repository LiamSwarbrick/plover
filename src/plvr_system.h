// NOTE: Platform non-specific
#ifndef PLVR_PLATFORM_NONSPECIFIC
#define PLVR_PLATFORM_NONSPECIFIC

#include "plvr_basic.h"

// NOTE: Each platform layer should implement these functions in their respective headers


/* System */

u64 get_timems();

/* Resource manager */



/* Input */

typedef struct Gamepad_State
{
    // NOTE: Like XInput, 'buttons' is a bitfield (see Gamepad_Buttons_Bits)
    u16 buttons;
    u8 left_trigger;
    u8 right_trigger;
    s16 lstick_x;
    s16 lstick_y;
    s16 rstick_x;
    s16 rstick_y;
}
Gamepad_State;

enum Gamepad_Buttons_Bits
{
    // NOTE: Values are the same as XInput
    GAMEPAD_DPAD_UP    = 0x0001,
    GAMEPAD_DPAD_DOWN  = 0x0002,
    GAMEPAD_DPAD_LEFT  = 0x0004,
    GAMEPAD_DPAD_RIGHT = 0x0008,
    GAMEPAD_START      = 0x0010,
    GAMEPAD_BACK       = 0x0020,
    GAMEPAD_L3         = 0x0040,
    GAMEPAD_R3         = 0x0080,
    GAMEPAD_LBUMPER    = 0x0100,
    GAMEPAD_RBUMPER    = 0x0200,
    GAMEPAD_BUTTON_A   = 0x1000,
    GAMEPAD_BUTTON_B   = 0x2000,
    GAMEPAD_BUTTON_X   = 0x4000,
    GAMEPAD_BUTTON_Y   = 0x8000
};

int key_is_down(u32 key);
int key_is_up(u32 key);
int key_just_pressed(u32 key);
int key_just_released(u32 key);

void mouse_reletive(int* x, int* y);
void mouse_set_pos(int x, int y);

int gamepad_is_supported(void);
Gamepad_State* gamepad_get_state(void);

#endif  // PLVR_PLATFORM_NONSPECIFIC