#ifndef OPTIONSESSION_HP
#define OPTIONSESSION_HP
#include "Session.hpp"

#define BACK_BUTTON 0
#define DOUBLE_3_BUTTON 1
#define BREAKABLE_5_BUTTON 2


class OptionSession : public Session
{
public:
    OptionSession(std::pair<bool,bool> rules);
    virtual ~OptionSession();
    virtual Session *update(Gomoku &);
    virtual void draw(Gomoku &);
private:
   // void        underlinedSelectedText(unsigned int &i,unsigned int& MouseX,unsigned int& MouseY);
   // void        cleanUnselectedText();
    bool        ActionBreakable5(const bool);
    bool        ActionDouble3(const bool);

    sf::Font _myFont;
    sf::Image _menuOptionImage;
    sf::Sprite _menuOptionSprite;
   // int     _selected;
};

#endif // OPTIONSESSION_HP
