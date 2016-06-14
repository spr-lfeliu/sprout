#ifndef NOTIFICATIONS_H
#define NOTIFICATIONS_H

#include "application.h"

class Notifications
{

public:
    Notifications(int pin_rgb_led_red, int pin_rgb_led_green,int pin_rgb_led_blue) :
        m_pin_rgb_red(pin_rgb_led_red),
        m_pin_rgb_green(pin_rgb_led_green),
        m_pin_rgb_blue(pin_rgb_led_blue),
        m_non_water_notification(all_ok),
        m_water_notification(water_none)
    {
        Setup();
    }
    enum Notification { all_ok, portal, water_deficit, water_excess, failure, water_none };
    void SetNotification(Notification);
    void DisplayLed();

private:
    struct Color {
      int red_;
      int green_;
      int blue_;
    };

    volatile Notification m_water_notification;
    volatile Notification m_non_water_notification;
    volatile bool m_led_toggle;
    volatile bool m_display_water_led;
    int m_pin_rgb_red;
    int m_pin_rgb_green;
    int m_pin_rgb_blue;

    void Setup();
    bool IsWaterNotification(Notification);
    void SetLedColor(Color *);
    void SetLedToggle();
    void ToggleWaterLed();
    void FadeIn(Color);
    void FadeOut(Color);
};

#endif
