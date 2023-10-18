//***************************************************************************
// 
// 파일: key_event.h
// 
// 설명: 키보드/마우스 이벤트
// 
// 작성자: bumpsgoodman
// 
// 작성일: 2023/10/18
// 
//***************************************************************************

#include <stdint.h>

typedef enum keyboard_state
{
    KEYBOARD_STATE_UP = 0x00,
    KEYBOARD_STATE_DOWN = 0x01,
    KEYBOARD_STATE_PRESSED = 0x10
} keyboard_state_t;

void on_key_down(const uint8_t key);
void on_key_up(const uint8_t key);
void reset_keyboard(void);
keyboard_state_t get_key_state(const uint8_t key);