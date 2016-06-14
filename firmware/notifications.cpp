#include "notifications.h"


void Notifications::Setup()
{
    pinMode(m_pin_rgb_red, OUTPUT);
    pinMode(m_pin_rgb_green, OUTPUT);
    pinMode(m_pin_rgb_blue, OUTPUT);
    m_led_toggle = false;
    m_display_water_led = false;
}

void Notifications::SetNotification(Notifications::Notification notification)
{
    if (m_water_notification == notification || m_non_water_notification == notification) {
        // do nothing, notification is already enacted
    } else {
        if (IsWaterNotification(notification)) {
            m_water_notification = notification;
        } else {
            m_non_water_notification = notification;
        }
        if (notification == all_ok) {
            m_water_notification = water_none;
        }
    }
}

bool Notifications::IsWaterNotification(Notifications::Notification notification) {
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
  if (m_water_notification == water_none) {
      m_display_water_led = false;
  }
}

void Notifications::DisplayLed()
{
  SetLedToggle();
  ToggleWaterLed();
  Notifications::Color color = Color();
  SetLedColor(&color);
  FadeIn(color);
  FadeOut(color);
}

void Notifications::FadeIn(Notifications::Color color)
{
  const bool is_increase_red = color.red_ > 0;
  const bool is_increase_green = color.green_ > 0;
  const bool is_increase_blue = color.blue_ > 0;
  for (int brightness = 0; brightness <= 255; brightness = brightness + 5) {
    if (is_increase_red) color.red_ = brightness;
    if (is_increase_green) color.green_ = brightness;
    if (is_increase_blue) color.blue_ = brightness;
    analogWrite(m_pin_rgb_red, color.red_);
    analogWrite(m_pin_rgb_green, color.green_);
    analogWrite(m_pin_rgb_blue, color.blue_);
    delay(30);
  }
}

void Notifications::FadeOut(Notifications::Color color)
{
  const bool is_decrease_red = color.red_ > 0;
  const bool is_decrease_green = color.green_ > 0;
  const bool is_decrease_blue = color.blue_ > 0;
  for (int brightness = 255; brightness > 0; brightness = brightness - 5) {
    if (is_decrease_red) color.red_ = brightness;
    if (is_decrease_green) color.green_ = brightness;
    if (is_decrease_blue) color.blue_ = brightness;
    analogWrite(m_pin_rgb_red, color.red_);
    analogWrite(m_pin_rgb_green, color.green_);
    analogWrite(m_pin_rgb_blue, color.blue_);
    delay(30);
  }
}

void Notifications::SetLedColor(Notifications::Color *color)
{
  if (m_display_water_led) {
      if (m_water_notification == water_deficit) {
          color->red_ = 0;
          color->green_ = 0;
          color->blue_ = 255;
      } else if (m_water_notification == water_excess) {
          color->red_ = 0;
          color->green_ = 255;
          color->blue_ = 255;
      }
  } else if (m_non_water_notification == all_ok) {
    color->red_ = 0;
    color->green_ = 255;
    color->blue_ = 0;
  } else if (m_non_water_notification == portal) {
    color->red_ = 255;
    color->green_ = 255;
    color->blue_ = 0;
  } else if (m_non_water_notification == failure) {
    color->red_ = 255;
    color->green_ = 0;
    color->blue_ = 0;
  } else {
    color->red_ = 0;
    color->green_ = 255;
    color->blue_ = 0;
  }
}
