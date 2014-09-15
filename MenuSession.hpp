#ifndef MENUSESSION_HPP
#define MENUSESSION_HPP

#include "Session.hpp"
#include "GameType.hpp"

#include <iostream>
#include <SFML/Window.hpp>
#include <map>
#include <vector>

//#include <SFML/System/
#define X_IMAGE         325.f
#define Y_IMAGE         230.f
#define X_TEXT          100.f
#define Y_TEXT          40.f

#define MENU_MODE       -1
#define SELECT_MODE     0
#define OPTION_MODE     1
#define MANUAL_MODE     2
#define EXIT_MODE       3


#define OFFSET          45.f

// Check ressources return in all Constructor

class MenuSession : public Session
{
public:
    MenuSession();

    virtual ~MenuSession();
    virtual Session *update(Gomoku &);
    virtual void draw(Gomoku &);
private:

    void        drawMenu(sf::RenderWindow &);
    void        drawManual(sf::RenderWindow &);
    void        drawOption(sf::RenderWindow &);
    void        drawSelectMode(sf::RenderWindow &);
    void        underlinedSelectedText(int &i,unsigned int& MouseX,unsigned int& MouseY);
private:
    sf::Image _menuImage;
    sf::Sprite _menuSprite;
    sf::Image _menuOptionImage;
    sf::Sprite _menuOptionSprite;
    sf::Image _menuManualImage;
    sf::Sprite _menuManualSprite;
    sf::Image _selectModeImage;
    sf::Sprite _selectModeSprite;
    sf::Font _myFont;
    std::vector<sf::String > _text;
    std::pair<bool, bool> _rules;
    bool    _firstUp;
    int     _mode;
    int     _selected;
};

#endif // MENUSESSION_HPP
