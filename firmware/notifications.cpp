#include "notifications.h"
#include "application.h"

Notifications::Notifications()
{
  
}

Notifications::Notifications(int pin_rgb_led_red, int pin_rgb_led_green,int pin_rgb_led_blue)
{
    m_pin_rgb_red = pin_rgb_led_red;
    m_pin_rgb_green = pin_rgb_led_green;
    m_pin_rgb_blue = pin_rgb_led_blue;
    m_non_water_notification = all_ok;
    m_water_notification = water_none;
    Setup();
}

void Notifications::Setup()
{
    pinMode(m_pin_rgb_red, OUTPUT);
    pinMode(m_pin_rgb_green, OUTPUT);
    pinMode(m_pin_rgb_blue, OUTPUT);
    m_led_toggle = false;
    m_display_water_led = false;
}

void Notifications::SetNotification(Notification notification)
{
    if (IsWaterNotification(notification)) {
        m_water_notification = notification;
    } else {
        m_non_water_notification = notification;
    }
    Timer timer(1530, &Notifications::DisplayLed, *this);
    timer.start();
}

bool Notifications::IsWaterNotification(Notification notification) {
    return notification == water_deficit || notification == water_excess || notification == water_none;
}

void Notifications::SetLedToggle()
{
  if (m_water_notification == water_none || m_non_water_notification == all_ok) {
    m_led_toggle = false;
  } else {
    m_led_toggle = true;
  }
}

void Notifications::ToggleWaterLed()
{
  if (m_led_toggle) {
    m_display_water_led = !m_display_water_led;
  }
}

void Notifications::DisplayLed()
{
  SetLedToggle();
  ToggleWaterLed();
  Notifications::Color color = Color();
  SetLedColor(color);
  FadeIn(color);
  FadeOut(color);
}

void Notifications::FadeIn(Notifications::Color color)
{
  for (int brightness = 0; brightness <= 255; brightness = brightness+ 5) {
    analogWrite(m_pin_rgb_red, brightness);
    analogWrite(m_pin_rgb_green, brightness);
    analogWrite(m_pin_rgb_blue, brightness);
    delay(15);
  }
}

void Notifications::FadeOut(Notifications::Color color)
{
  for (int brightness = 255; brightness >= 0; brightness = brightness - 5) {
    analogWrite(m_pin_rgb_red, brightness);
    analogWrite(m_pin_rgb_green, brightness);
    analogWrite(m_pin_rgb_blue, brightness);
    delay(15);
  }
}

void Notifications::SetLedColor(Notifications::Color color)
{
  if (m_display_water_led) {
    color.blue_ = 255;
  } else if (m_non_water_notification == all_ok) {
    color.green_ = 255;
  } else if (m_non_water_notification == portal) {
    color.red_ = 255;
    color.green_ = 255;
  } else if (m_non_water_notification == failure) {
    color.red_ = 255;
  }
  analogWrite(m_pin_rgb_red, color.red_);
  analogWrite(m_pin_rgb_green, color.green_);
  analogWrite(m_pin_rgb_blue, color.blue_);
}
