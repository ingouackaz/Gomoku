
#include <iostream>

#include "bnet_ai_main.hpp"
#include "IA.hpp"

int	IA::play()
{
  IA::t_mmx	data;
  int		pos[2];

  data.player = 1;
  data.zero_score = 100000000;
  data.one_score = -100000000;
  for (int i = 1; i < 9000; ++i)
    {
      std::cout << std::endl << "STARTING EVAL OF DEPTH " << i << std::endl;
      IA::minimax(bnet_g__map, i, data, true);
      std::cout << "RETURN (" << data.move.first << "," << data.move.second << ")" << std::endl;
      pos[0] = data.move.first;
      pos[1] = data.move.second;
      bnet_commit_play(pos);
    }
  return (0);
}

int	IA::heuristic_eval(const Map &map, int player)
{
  int	eval(0);
  
  if (map.wins(player))
    eval = 2000000;
  eval += map.player_stats(player).ntaken * 1;
  eval += map.player_stats(player).nrows[1] * 1;
  eval += map.player_stats(player).nrows[2] * 2;
  eval += map.player_stats(player).nrows[3] * 3;
  eval += map.player_stats(player).nrows[4] * 4;

  return (!player? -eval : eval);
}

void	IA::minimax(const Map &map, int depth, IA::t_mmx &data, bool first_iter)
{
  if (!depth--)
    data.score = IA::heuristic_eval(map, (data.player));
  else
    {
      std::list<std::pair<t_coor, Map *> > moves;

      for (int x = 0; x < 19; ++x)
	for (int y = 0; y < 19; ++y)
	  if (map.near(x, y))
	    {
	      Map *new_map = new Map(map);
	      if (new_map->play(data.player, x, y))
		moves.push_back(std::make_pair(std::make_pair(x, y), new_map));
	      else
		delete new_map;
	    }

        int	best_score(data.player ? -100000001 : 100000001);
        t_coor	best_move;

        for (std::list<std::pair<t_coor, Map *> >::iterator it = moves.begin(); it != moves.end(); ++it)
      	{
      	  IA::t_mmx	new_data;
	  
      	  new_data.player = !(data.player);
      	  new_data.zero_score = data.player ? data.zero_score : best_score;
      	  new_data.one_score = !(data.player) ? data.one_score : best_score;

      	  if ((*it).second->wins(1))
      	    new_data.score = 20000000;
      	  else if ((*it).second->wins(0))
      	    new_data.score = -20000000;
      	  else
      	    IA::minimax(*((*it).second), depth, new_data, false);
      	  if (first_iter && 
      	      ((data.player && new_data.score >= best_score && new_data.score >= data.zero_score) ||
      	       (!(data.player) && new_data.score <= best_score && new_data.score <= data.one_score)))
      	    break;
      	  if ((data.player && new_data.score > best_score) ||
      	      (!(data.player) && new_data.score < best_score))
      	    {
      	      best_score = new_data.score;
      	      best_move = (*it).first;
      	    }
	  delete (*it).second;
      	}
        data.score = best_score;
        data.move = best_move;
      }

};
