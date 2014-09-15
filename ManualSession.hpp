#ifndef MANUALSESSION_HPP
#define MANUALSESSION_HPP

#include "Session.hpp"

#define BACK_BUTTON 0

class ManualSession : public Session
{
public:
    ManualSession();
    virtual ~ManualSession();
    virtual Session *update(Gomoku &);
    virtual void draw(Gomoku &);
private:
    //void        underlinedSelectedText(unsigned int &i,unsigned int& MouseX,unsigned int& MouseY);
    //std::vector<sf::String > _text;
    sf::Font _myFont;
    sf::Image _menuManualImage;
    sf::Sprite _menuManualSprite;
    int     _selected;
};

#endif // MANUALSESSION_HPP
