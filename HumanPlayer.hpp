

#ifndef		HUMANPLAYER_HPP
# define	HUMANPLAYER_HPP

#define    X_POS 172
#define    Y_POS 107
# define   CURSOR_IN_BOARD Input.GetMouseX() > 172 && Input.GetMouseX() < 628 && Input.GetMouseY() > 107  && Input.GetMouseY() < 559
// Le joueur humain, c'est la que tu gere les event clavier pour le tour du joueur

#include "Player.hpp"
#include <iostream>

class		HumanPlayer : public Player
{
public:
  HumanPlayer(Map::Player, Map &);
  ~HumanPlayer();
virtual bool play(const std::pair<int, int> coo,std::vector< std::pair<int, int> >  &taken);
private:
  HumanPlayer(const HumanPlayer &);
  HumanPlayer &operator=(const Player &);
};

#endif
