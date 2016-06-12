#ifndef FEEDBACK_H
#define FEEDBACK_H

class Feedback
{
    
public:
    enum Button { neutral, positive, negative, none };

    Feedback(int pin_neutral, int pin_positive, int pin_negative);
    
    void ButtonPressed(Button button) { m_feedback_button = button; }
    void Reset();
 
    bool isNeutral() { return m_feedback_button == neutral; }
    bool isPositive() { return m_feedback_button == positive; }
    bool isNegative() { return m_feedback_button == negative; }
    bool isFeedback() { return m_feedback_button != none; }
    Button GetButtonPressed() { return m_feedback_button; }
    
private:

    int m_pin_neutral, m_pin_positive, m_pin_negative;
    Button m_feedback_button;
    
    void TurnLedOn(Button button);
    void TurnAllLedsOff();
    void Setup();

};

#endif
