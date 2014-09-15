#ifndef GAMETYPE_HPP
#define GAMETYPE_HPP
#include <map>
#include <iostream>
#include "Player.hpp"
#include "Map.hpp"
#include "bnet_server.hpp"
//#include "Map.hpp"

#define     NO_ERROR            1
/*#define     ERROR_CONNECTION    2
#define     ERROR_HELLO       3
*/
class GameType
{
public:
    enum		GameMode
    {
        P_VS_P,
        P_VS_IA,
        IA_VS_IA,
    };
public:
    GameType(GameMode, std::pair<bool, bool>, bnet_ai_hdl_t &_ai);
    ~GameType();
    GameMode                    getMode();
    std::pair<Player*, Player*> getPlayers();
    std::pair<bool, bool>       getRules();
    const Map&                  getMap();
    int                         getError();
    bnet_ai_hdl_t&              getAi();
private:
    GameMode                    _mode;
    std::pair<bool, bool>       _rules;
    Map _map;
    bnet_ai_hdl_t               &_ai;
    std::pair<Player*, Player*> _players;

    int                        _bnet_error;
};

#endif // GAMETYPE_HPP
