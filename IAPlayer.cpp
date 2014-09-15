

#include "IAPlayer.hpp"
#include <iostream>

IAPlayer::IAPlayer(Map::Player team, Map &referee, bnet_ai_hdl_t  &_ai)
    : Player(team, referee) , _ai(_ai)
{
    _type = 1;
}

IAPlayer::~IAPlayer()
{

}

bool IAPlayer::play(std::pair<int, int> pos,std::vector< std::pair<int, int> >  &taken)
{  

    if (!bnet_let_play(_ai, pos))
    {
        bnet_tell_end(_ai,false,BNET_END_TIMEOUT);
        return false;
    }
    else
    {
        if(!_referee.play(_team, pos.first, pos.second ,taken))
        {
            bnet_tell_end(_ai,false,BNET_END_RULEERR);
            return false;
        }
        else
            return true;       
    }
}

bnet_ai_hdl_t&      IAPlayer::getAi()
{
    return _ai;
}

