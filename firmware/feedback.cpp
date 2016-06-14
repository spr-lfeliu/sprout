#include "feedback.h"
#include "application.h"

Feedback::Feedback(int pin_neutral, int pin_positive, int pin_negative)
{
    Reset();
    m_pin_neutral = pin_neutral;
    m_pin_positive = pin_positive;
    m_pin_negative = pin_negative;
    Setup();
}

void Feedback::Reset()
{
    m_feedback_button = none;
}

void Feedback::Setup()
{
    pinMode(m_pin_neutral, OUTPUT);
    pinMode(m_pin_positive, OUTPUT);
    pinMode(m_pin_negative, OUTPUT);
}

void Feedback::TurnLedOn(Button button)
{
    TurnAllLedsOff();
    switch(button) {
        case neutral:
            digitalWrite(m_pin_neutral, HIGH);
            break;
        case positive:
            digitalWrite(m_pin_positive, HIGH);
            break;
        case negative:
            digitalWrite(m_pin_negative, HIGH);
            break;
    }
}

void Feedback::TurnAllLedsOff()
{
    digitalWrite(m_pin_neutral, LOW);
    digitalWrite(m_pin_positive, LOW);
    digitalWrite(m_pin_negative, LOW);
}
