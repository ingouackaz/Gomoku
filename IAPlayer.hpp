

#ifndef		IAPLAYER_HPP
# define	IAPLAYER_HPP

// l'ia, faut faire le dialogue via reseau (avec timeout ?) 

# include <cstdlib>
# include <unistd.h>

# include "Player.hpp"
# include "bnet_server.hpp"

class		IAPlayer : public Player
{
public:
  IAPlayer(Map::Player, Map &,bnet_ai_hdl_t &_ai);
  ~IAPlayer();
  virtual bool play(const std::pair<int, int> coo, std::vector< std::pair<int, int> >  &taken);
  bnet_ai_hdl_t &getAi();
private:
  bnet_ai_hdl_t     &_ai;
  IAPlayer(const IAPlayer &);
  IAPlayer &operator=(const Player &);
};

#endif
