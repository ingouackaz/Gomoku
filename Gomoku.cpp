#include "Gomoku.hpp"
#include "MenuSession.hpp"

Gomoku::Gomoku()
{
    // Regles par defaut
    _rules.first = false;
    _rules.second = true;
    _ai = 0;
}

Gomoku::~Gomoku()
{
}

void     Gomoku::run()
{
    initialize();
    while (_app->IsOpened())
    {
        update();
        draw();
    }
}

sf::RenderWindow& Gomoku::GetApp()
{
    return *_app;
}

void    Gomoku::initialize()
{
    _app = new  sf::RenderWindow(sf::VideoMode(800, 600, 32), "SFML Events",4);
    if (_app == NULL)
    {
        _app = NULL;
        _session = NULL;
    }
    _app->SetFramerateLimit(30);
    _session = new MenuSession();
}

void    Gomoku::update()
{
    Session       *old = 0;

    if (!_session)
        _app->Close();
    else
    {
        old = _session;
        try{
            if ((_session = _session->update(*this)) != old)
                delete old;
        }
        catch (std::exception& e){
            std::cerr << e.what() << std::endl;
            _session = 0;
        }
    }
}

void    Gomoku::draw()
{
    if (!_session)
        _app->Close();
    else
    {
        _app->Clear();
        _session->draw(*this);
        _app->Display();
    }
}

void    Gomoku::SetMode(int mode)
{
    _mode = mode;
}

void    Gomoku::SetRules(std::pair<bool,bool> rules)
{
    _rules = rules;
}

void    Gomoku::SetDouble3(bool double3)
{
    _rules.first = double3;
}

void    Gomoku::Set5Breakable(bool breakable)
{
    _rules.second = breakable;
}

const std::pair<bool,bool>& Gomoku::GetRules()
{
    return _rules;
}

const int& Gomoku::GetMode()
{
    return _mode;
}

const     bnet_ai_hdl_t& Gomoku::GetbnetAi()
{
    return _ai;
}



void        Gomoku::endOfAi(GameType::GameMode mode , bool winner,  bnet_end_reason reason)
{
    if (mode == GameType::P_VS_IA)
    {
        if(_ai > 0)
        {
            bnet_tell_end(_ai, winner,reason);
            bnet_close_ai(_ai);
            _ai = 0;
        }
    }
}

void        Gomoku::disconnectAi(GameType::GameMode mode)
{
    if (mode == GameType::P_VS_IA)
    {
        if(_ai > 0)
        {
            bnet_close_ai(_ai);
            _ai = 0;
        }
    }
}
