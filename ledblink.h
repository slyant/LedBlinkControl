#ifndef __LEDBLINK_H__
#define __LEDBLINK_H__

#ifdef LED_BLINK_USER_DEF
#include "led_blink_user_def.h"
#ifndef led_blink_list
#error "Please define 'led_blink_list' in the header file:'led_blink_user_def.h"
#endif
#else
enum led_name
{
    LED_0 = 0, LED_1, LED_2, LED_COUNT
};
#define led_blink_list {        \
    {LED_0, 0, GET_PIN(A, 1)},  \
    {LED_1, 0, GET_PIN(A, 2)},  \
    {LED_2, 0, GET_PIN(A, 3)},  \
}
#endif

struct led_blink_item
{
    enum led_name name;
    rt_uint8_t on_state;
    rt_base_t pin;
};

typedef enum
{
    ON = 0, OFF, BLINK, TOG,
} led_status;

#pragma pack(1)
typedef struct
{
    led_status state;
    uint8_t ison;
    uint32_t on_time;
    uint32_t off_time;
    uint32_t tick;
} led_item;
#pragma pack()

void led_set_level(enum led_name name, uint8_t level);
void led_on(enum led_name name);
void led_off(enum led_name name);
void led_tog(enum led_name name);
void led_blink(enum led_name name, int ontime, int offtime);
#endif
