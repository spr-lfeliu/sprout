#include "feedback.h"
#include "application.h"

Feedback::Feedback(int pin_ok, int pin_great, int pin_notgood)
{
    Reset();
    m_pin_ok = pin_ok;
    m_pin_great = pin_great;
    m_pin_notgood = pin_notgood;
}

void Feedback::Reset() 
{
    m_feedback_button = none;
}

void Feedback::Setup()
{
    pinMode(m_pin_ok, OUTPUT);
    pinMode(m_pin_great, OUTPUT);
    pinMode(m_pin_notgood, OUTPUT);
}

void Feedback::TurnLedOn(Button button)
{
    TurnAllLedsOff();
    switch(button) {
        case ok:
            digitalWrite(m_pin_ok, HIGH);
            break;
        case great:
            digitalWrite(m_pin_great, HIGH);
            break;
        case notgood:
            digitalWrite(m_pin_notgood, HIGH);
            break;
    }
}

void Feedback::TurnAllLedsOff()
{
    digitalWrite(m_pin_ok, LOW);
    digitalWrite(m_pin_great, LOW);
    digitalWrite(m_pin_notgood, LOW);
}
