#ifndef SELECTMODESESSION_HPP
#define SELECTMODESESSION_HPP
#include "Session.hpp"

#define BACK_BUTTON 0
#define HUMAN_VS_HUMAN_BUTTON 1
#define HUMAN_VS_IA_BUTTON 2
#define ERROR_CONNECTION 1
#define ERROR_TIMEOUT   2


class SelectModeSession: public Session
{
public:
    SelectModeSession();
    virtual ~SelectModeSession();
    virtual Session *update(Gomoku &);
    virtual void draw(Gomoku &);
private:
    void    updateModeAi(Gomoku &);
    void    drawModeAi(Gomoku &game);
private:
    sf::Font _myFont;
    sf::Image _selectModeImage;
    sf::Sprite _selectModeSprite;
    bool      _isModeAi;
     bool     _aiConnected;
    int       _error;
};

#endif // SELECTMODESESSION_HPP
