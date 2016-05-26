#ifndef FEEDBACK_H
#define FEEDBACK_H

class Feedback
{
    
public:
    enum Button { ok, great, notgood, none };

    Feedback(int pin_ok, int pin_great, int pin_notgood);
    
    void ButtonPressed(Button button) { m_feedback_button = button; }
    void Reset();
 
    bool isOk() { return m_feedback_button == ok; }
    bool isGreat() { return m_feedback_button == great; }
    bool isNotGood() { return m_feedback_button == notgood; }
    bool isFeedback() { return m_feedback_button != none; }
    Button GetButtonPressed() { return m_feedback_button; }
    
private:

    int m_pin_ok, m_pin_great, m_pin_notgood;
    Button m_feedback_button;
    
    void TurnLedOn(Button button);
    void TurnAllLedsOff();
    void Setup();

};

#endif
