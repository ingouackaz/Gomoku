#include "GameType.hpp"
#include "HumanPlayer.hpp"
#include "IAPlayer.hpp"


GameType::GameType(GameMode mode,  std::pair<bool, bool> rules, bnet_ai_hdl_t &ai) : _mode(mode), _rules(rules) , _map() , _ai(ai)
{
    _map.set_double_three(_rules.first);
    _map.set_breaking_five(_rules.second);


    if(_mode == P_VS_P)
    {
        _bnet_error = NO_ERROR;
        _players.first = new HumanPlayer(Map::PLAYER_BLACK, _map);
        _players.second = new HumanPlayer(Map::PLAYER_WHITE, _map);
    }
    else if (_mode == P_VS_IA)
    {
        _players.first = new HumanPlayer(Map::PLAYER_BLACK, _map);
        _players.second = new IAPlayer(Map::PLAYER_WHITE, _map, ai);
    }
}

GameType::~GameType()
{
}

GameType::GameMode GameType::getMode()
{
    return _mode;
}

std::pair<bool, bool>   GameType::getRules()
{
    return _rules;
}

std::pair<Player*, Player*> GameType::getPlayers()
{
    return _players;
}

const Map& GameType::getMap()
{
    return _map;
}

int     GameType::getError()
{
    return _bnet_error;
}


bnet_ai_hdl_t &GameType::getAi()
{
    return _ai;
}
