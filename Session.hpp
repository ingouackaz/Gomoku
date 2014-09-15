#ifndef SESSION_HPP
#define SESSION_HPP
#include "Gomoku.hpp"
#define X_IMAGE         325.f
#define Y_IMAGE         230.f
#define X_TEXT          100.f
#define Y_TEXT          40.f

class	Session
{
public:
  virtual ~Session();
  virtual Session *update(Gomoku &) = 0;
  virtual void draw(Gomoku &) = 0;
protected:
    void        underlinedSelectedText(unsigned int &i,unsigned int& MouseX,unsigned int& MouseY);
    void        cleanUnselectedText();
protected:
    std::vector<sf::String > _text;
    int                     _selected;
};

#endif
