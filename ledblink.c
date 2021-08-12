#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include "ledblink.h"

static struct led_blink_item led_blink_ls[LED_COUNT] = led_blink_list;
static led_item *ls = 0;

void led_set_level(enum led_name name, uint8_t level)
{
    if (name < 0 || name > LED_COUNT)
        return;
    rt_pin_write(led_blink_ls[name].pin, level);
}

void led_on(enum led_name name)
{
    if (name < 0 || name > LED_COUNT)
        return;
    ls[name].state = ON;
    rt_pin_write(led_blink_ls[name].pin, led_blink_ls[name].on_state);
}

void led_off(enum led_name name)
{
    if (name < 0 || name > LED_COUNT)
        return;
    ls[name].state = OFF;
    rt_pin_write(led_blink_ls[name].pin, !led_blink_ls[name].on_state);
}

void led_tog(enum led_name name)
{
    if (name < 0 || name > LED_COUNT)
        return;
    ls[name].state = TOG;
    if (ls[name].ison)
    {
        rt_pin_write(led_blink_ls[name].pin, !led_blink_ls[name].on_state);
    }
    else
    {
        rt_pin_write(led_blink_ls[name].pin, led_blink_ls[name].on_state);
    }
    ls[name].ison = 1 - ls[name].ison;
}

/**
 * @brief  设置led为闪烁状态
 * @note   
 * @param  name: led名称
 * @param  ontime: 一周期内led亮的时间，单位ms
 * @param  offtime: 一周期内led灭的时间，单位ms
 * @retval None
 */
void led_blink(enum led_name name, int ontime_ms, int offtime_ms)
{
    if (name < 0 || name > LED_COUNT)
        return;
    led_set_level(name, led_blink_ls[name].on_state);

    ls[name].ison = 1;
    ls[name].state = BLINK;
    ls[name].tick = rt_tick_from_millisecond(ontime_ms);
    ls[name].on_time = rt_tick_from_millisecond(ontime_ms);
    ls[name].off_time = rt_tick_from_millisecond(offtime_ms);
}

static void led_blink_init(void)
{
    ls = (led_item *) rt_calloc(LED_COUNT, sizeof(led_item));
    if (ls)
    {
        for (int i = 0; i < LED_COUNT; ++i)
        {
            ls[i].ison = 0;
            ls[i].state = !led_blink_ls[i].on_state;
            rt_pin_mode(led_blink_ls[i].pin, PIN_MODE_OUTPUT);
        }
    }
}

static void led_process(int inc_ms)
{
    for (int i = 0; i < LED_COUNT; i++)
    {
        if (ls[i].state == BLINK)
        {
            ls[i].tick -= rt_tick_from_millisecond(inc_ms);
            if (ls[i].ison && ls[i].tick > 0)
            {
                //in ON STATE , do nothing
            }
            else if (ls[i].ison && ls[i].off_time == 0)
            {
                //offtime=0; just blink once
                ls[i].state = OFF;
                led_set_level(i, !led_blink_ls[i].on_state);
            }
            else if (ls[i].ison && ls[i].off_time > 0)
            {
                //ison = 0; offtime>0;  On state turn to off state
                ls[i].tick = ls[i].off_time;
                ls[i].ison = 0;
                led_set_level(i, !led_blink_ls[i].on_state);
            }
            else if (ls[i].ison == 0 && ls[i].tick > 0)
            {
                //OFF STATE , do nothing
            }
            else if (ls[i].ison == 0)
            {
                // off state turn to ON state
                ls[i].tick = ls[i].on_time;
                ls[i].ison = 1;
                led_set_level(i, led_blink_ls[i].on_state);
            }
        }
    }
}

static void led_thread_entry(void *params)
{
    while (1)
    {
        led_process(10);
        rt_thread_mdelay(10);
    }
}

static int led_blink_startup(void)
{
    led_blink_init();
    rt_thread_t t = rt_thread_create("led", led_thread_entry, RT_NULL, LED_THREAD_STACK_SIZE, LED_THREAD_PRIORITY, 20);
    if (t != RT_NULL)
        rt_thread_startup(t);
    return RT_EOK;
}
INIT_APP_EXPORT(led_blink_startup);
