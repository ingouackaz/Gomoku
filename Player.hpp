

#ifndef		PLAYER_HPP
# define	PLAYER_HPP

// Les joueurs, c'est une interface/classe abstraite, faut en faire heriter HumanPlayer et IAPlayer
#include <SFML/Window.hpp>
# include "Map.hpp"

class		Player
{
public:
  Player(Map::Player, Map &);
  ~Player();
  virtual bool play(std::pair<int, int> pos,std::vector< std::pair<int, int> >  &taken) = 0;
  const Map::Player& getTeam();
   int   getType() const;
protected:
  int               _type;
  Map::Player	_team;
  Map		&_referee;
private:
  Player(const Player &);
  Player &operator=(const Player &);
};

#endif
