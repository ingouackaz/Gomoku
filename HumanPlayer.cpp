

#include "HumanPlayer.hpp"

HumanPlayer::HumanPlayer(Map::Player team, Map &referee)
    : Player(team, referee)
{
    _type = 0;
}
bool    HumanPlayer::play(const std::pair<int, int> coo,std::vector< std::pair<int, int> >  &taken)
{
    //std::vector< std::pair<int, int> >  taken;
    return _referee.is_valid_move(_team, coo.first, coo.second) &&_referee.play(_team, coo.first ,coo.second, taken);
}
