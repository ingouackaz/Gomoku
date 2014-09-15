#include <unistd.h>
#include "Session.hpp"

Session::~Session(){
    //-
}

void        Session::underlinedSelectedText(unsigned int &i, unsigned int &MouseX, unsigned int &MouseY)
{
    sf::Vector2f pos =  _text[i].GetPosition();
    if(MouseX >= pos.x && MouseX <= pos.x + X_TEXT && MouseY >= pos.y && MouseY <= pos.y + Y_TEXT)
    {
        _text[i].SetSize(25);
        _text[i].SetColor(sf::Color(255, 255, 0));
        _selected = i;
    }
    else
    {
        _selected = -1;
    }
}

void    Session::cleanUnselectedText()
{
    for(unsigned int i=0; i < _text.size();++i)
    {
        _text[i].SetSize(22);
        _text[i].SetColor(sf::Color(255, 255, 255));
    }
    _selected = -1;
}


