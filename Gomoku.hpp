#ifndef		GOMOKU_HPP
# define	GOMOKU_HPP
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <map>
#include "bnet_server.hpp"
#include "GameType.hpp"

///#include "MenuSession.hpp"
// OK donc ca c'est la classe mere qui va tout contenir (2 players, 1 arbitre, 1 goban)

class       Session;

class		Gomoku
{
public:
    Gomoku();
    ~Gomoku();
    void       run();
    sf::RenderWindow& GetApp();
    const     int& GetMode();
    void      SetMode(int mode);
    const     std::pair<bool,bool> &GetRules();
    const     bnet_ai_hdl_t&GetbnetAi();
    void      SetRules(std::pair<bool,bool>);
    void      Set5Breakable(bool);
    void      SetDouble3(bool);
    void      endOfAi(GameType::GameMode, bool winner,bnet_end_reason reason);
    void      disconnectAi(GameType::GameMode);
    bnet_ai_hdl_t _ai;
private:
    Gomoku(const Gomoku &);
    Gomoku &operator=(const Gomoku &);
    void          initialize(void);
    void          update(void);
    void          draw(void);
protected:    
    sf::RenderWindow *_app;
    Session    *_session;
    int        _mode;
    std::pair<bool,bool> _rules;

protected:

};

#endif
