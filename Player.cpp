

#include "Player.hpp"

Player::Player(Map::Player team, Map &referee)
  : _team(team), _referee(referee)
{
    _type = -1;
}

const Map::Player& Player::getTeam()
{
    return _team;
}
int   Player::getType() const
{
    return _type;
}

Player::~Player()
{

}
