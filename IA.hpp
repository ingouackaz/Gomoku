

#ifndef		IA_HPP
# define	IA_HPP

#include <list>
#include <utility>
#include <climits>

#include "Map.hpp"

typedef	std::pair<int, int> t_coor;

class		IA
{
public:
  typedef struct	s_mmx
  {
    int			player;
    int			score;
    int			zero_score;
    int			one_score;
    t_coor		move;
  }			t_mmx;

public:
  static int	play();

private:
  static int	heuristic_eval(const Map &, int);
  static void	minimax(const Map &, int, t_mmx &, bool);

private:
  IA();
  ~IA();
  IA(const IA &);
  IA &operator=(const IA &);
};


#endif
