
#include <cstring>

#include "Map.hpp"

bool Map::g_allow_d3 = false;
bool Map::g_allow_b5 = true;

int const Map::m_directions[8][2] = {
    { 1,-1},    // 0 top right
    { 1, 0},    // 1 right
    { 1, 1},    // 2 bottom right
    { 0, 1},    // 3 bottom
    {-1, 1},    // 4 bottom left
    {-1, 0},    // 5 left
    {-1,-1},    // 6 top left
    { 0,-1},    // 7 top
};

int const Map::m_adversaries[2] = {
    Map::PLAYER_WHITE,
    Map::PLAYER_BLACK,
};

#ifdef DEBUG
bool Map::m_debug = true;
#endif


Map::Map()
{
    std::memset(&m_, 0, sizeof(m_));

    // no player on the map
    for (int y = 0; y < 19; ++y)
        for (int x = 0; x < 19; ++x)
            m_.map[y][x].player = PLAYER_NONE;
}

Map::Map(Map const& map)
{
    m_ = map.m_;
}

Map& Map::operator=(Map const& map)
{
    m_ = map.m_;
    return *this;
}

void Map::take(int x, int y)
{
    ASSERT(in_bounds(x, y));

#ifdef SERVER
    if (!in_bounds(x, y) || m_.map[y][x].player < 0)
        return;
#endif

    Position& pos(m_.map[y][x]);
    int player(pos.player);
    
    ASSERT(player == PLAYER_BLACK || player == PLAYER_WHITE);
    
    PlayerStats& stats(m_.stats[player]);
    
    DEBUG_COUT("taking " << x << "," << y << '\n');
    
    // break the rows
    for (int dir = 0; dir < 4; ++dir)
    {
        int const (&v)[2](m_directions[dir]);
        int prev_sz(pos.rows[dir].sz_total);
        int new_sz[2];
        
        new_sz[0] = pos.rows[dir].sz_current;
        new_sz[1] = pos.rows[dir].sz_total - new_sz[0] - 1;
       
        stats.nrows[((prev_sz > 5) ? 5 : prev_sz) - 1]--;
        for (int i = 0; i < 2; ++i)
            if (new_sz[i] > 0)
                stats.nrows[((new_sz[i] > 5) ? 5 : new_sz[i]) - 1]++;
        
        DEBUG_COUT("breaking " << prev_sz << " = " << new_sz[0] << " + 1 (taken) + " << new_sz[1] << '\n');
        
        {   // make the two resulting rows
            int start[2][2];

            start[0][0] = x - new_sz[0] * v[0];
            start[0][1] = y - new_sz[0] * v[1];
            start[1][0] = x + v[0];
            start[1][1] = y + v[1];

            for (int i = 0; i < 2; ++i)
            {
                int xp(start[i][0]);
                int yp(start[i][1]);

                for (int j = 0; j < new_sz[i]; ++j, xp += v[0], yp += v[1])
                {
                    Position& p(m_.map[yp][xp]);

                    p.rows[dir].sz_current = j;
                    p.rows[dir].sz_total = new_sz[i];
                }
            }
        }
        
        pos.rows[dir].sz_current = 0;
        pos.rows[dir].sz_total = 0;
    }

    pos.player = PLAYER_NONE;
    m_.stats[(player + 1) % 2].ntaken++;
}

void Map::put(int player, int x, int y)
{
    ASSERT(player == PLAYER_BLACK || player == PLAYER_WHITE);
    ASSERT(in_bounds(x, y));

#ifdef SERVER
    if (!in_bounds(x, y) || m_.map[y][x].player != PLAYER_NONE)
        return;
#endif

    PlayerStats& stats(m_.stats[player]);
    
    m_.map[y][x].player = player;
    
    for (int dir = 0; dir < 4; ++dir)
    {
        int const (&v)[2](m_directions[dir]);
        int xp(x);
        int yp(y);
        int bef_sz(0);
        int aft_sz(0);

        if (in_bounds(xp - v[0], yp - v[1]))
        {
            Position& pos(m_.map[yp - v[1]][xp - v[0]]);

            if (pos.player == player)
            {
                bef_sz = pos.rows[dir].sz_total;
                stats.nrows[((bef_sz > 5) ? 5 : bef_sz) - 1]--;
            }
        }

        if (in_bounds(xp + v[0], yp + v[1]))
        {
            Position& pos(m_.map[yp + v[1]][xp + v[0]]);

            if (pos.player == player)
            {
                aft_sz = pos.rows[dir].sz_total;
                stats.nrows[((aft_sz > 5) ? 5 : aft_sz) - 1]--;
            }
        }
        
        {   // make the new row
            int new_sz(bef_sz + aft_sz + 1);

            stats.nrows[((new_sz > 5) ? 5 : new_sz) - 1]++;
            DEBUG_COUT("merging " << bef_sz << " + 1 (joint) + " << aft_sz << " = " << new_sz << '\n');

            xp -= bef_sz * v[0];
            yp -= bef_sz * v[1];

            for (int i = 0; i < new_sz; ++i, xp += v[0], yp += v[1])
            {
                Position& pi(m_.map[yp][xp]);

                pi.rows[dir].sz_current = i;
                pi.rows[dir].sz_total = new_sz;
            }
        }
    }

#if 1
    // set near in every direction
    for (int dir = 0; dir < 8; ++dir)
    {
        int const (&v)[2](m_directions[dir]);
        int xp(x + v[0]);
        int yp(y + v[1]);
        
        for (int i = 0; in_bounds(xp, yp) && i < 2; ++i, xp += v[0], yp += v[1])
            m_.near[yp][xp] = true;
    }
#else
#define ZONE_SZ    5

    // set near on the ZONE_SZ * ZONE_SZ square
    for (int dy = y - ZONE_SZ / 2; dy <= y + ZONE_SZ / 2; ++dy)
    {
        for (int dx = x - ZONE_SZ / 2; dx <= x + ZONE_SZ / 2; ++dx)
        {
            if (in_bounds(dx, dy))
                m_.near[dy][dx] = true;
        }
    }
#undef ZONE_SZ
#endif
}

bool Map::play(int player, int x, int y
#ifdef SERVER
    , std::vector< std::pair<int, int> >& taken
#endif
)
{
    ASSERT(player == PLAYER_BLACK || player == PLAYER_WHITE);
    ASSERT(in_bounds(x, y));

    bool allow(false);

    DEBUG_COUT(player << " plays at: " << x << ' ' << y << '\n');

#ifdef SERVER
    if (!in_bounds(x, y))
        return false;
#endif

    // is it a free position?
    if (player_at(x, y) < 0)
    {
#ifdef SERVER
        taken.clear();
#else
        std::vector< std::pair<int, int> > taken;
#endif

        taken.reserve(8);
        
        for (int dir = 0; dir < 8; ++dir)
        {
            int const (&v)[2](m_directions[dir]);
            int xp(x + 2 * v[0]);
            int yp(y + 2 * v[1]);
            
            // look around to see what positions are taken in every direction
            if (in_bounds(xp + v[0], yp + v[1]) &&
                player_at(xp + v[0], yp + v[1]) == player &&
                player_at(xp, yp) == m_adversaries[player] &&
                m_.map[yp][xp].rows[dir % 4].sz_total == 2)
            {
                taken.push_back(std::make_pair<int, int>(xp       , yp       ));
                taken.push_back(std::make_pair<int, int>(xp - v[0], yp - v[1]));
            }
        }

        // let's assume the player plays there (required for later)
        put(player, x, y);

        // if nothing taken and double threes not allowed, check that is does not make one
        if (!(!taken.size() && !g_allow_d3 && _makes_double_three(player, x, y, -1)))
        {
            for (std::vector< std::pair<int, int> >::const_iterator it(taken.begin());
                it != taken.end(); ++it)
                take(it->first, it->second);
            
            allow = true;
        }
        else
        {
            // the map has not an invalid state, do not reuse it
        }
    }

    DEBUG_COUT(player << "'s stats: "   << (int)m_.stats[player].nrows[0] << ' '
                                        << (int)m_.stats[player].nrows[1] << ' '
                                        << (int)m_.stats[player].nrows[2] << ' '
                                        << (int)m_.stats[player].nrows[3] << '\n');
        
    return allow;
}

#include <iostream>
bool Map::wins(int player) const
{
    ASSERT(player == PLAYER_BLACK || player == PLAYER_WHITE);
    
    PlayerStats const& stats(m_.stats[player]);

    if (stats.ntaken >= 10)
    {
        DEBUG_COUT("win by 10 takes\n");
        return true;
    }
    
    if (stats.nrows[4] >= 1)
    {
        if (!g_allow_b5)
        {
            return true;
        }

        for (int y = 0; y < 19; ++y)   // x and y
        {
            for (int x = 0; x < 19; ++x)
            {
                Position const& pos(m_.map[y][x]);

                if (pos.player == player)
                {
                    for (int dir = 0; dir < 4; ++dir)   // row's direction
                    {
                        if (pos.rows[dir].sz_current == 0 && pos.rows[dir].sz_total >= 5)   // if it's the start of a 5+
                        {
                            int const (&v)[2](m_directions[dir]);
                            int nsecure(0);
                            int xp(x);
                            int yp(y);

                            //std::cout << "\n\nstart of 5 at " << xp << ' ' << yp << std::endl;
                            
                            while (in_bounds(xp, yp) && player_at(xp, yp) == player)   // look around, loop around
                            {
                                bool secure(true);

                                //std::cout << "check secure at " << xp << ' ' << yp << std::endl;
                                for (int dir2 = 0; dir2 < 8; ++dir2)
                                {
                                    int const (&v2)[2](m_directions[dir2]);
                                    int expected[2][4];

                                    expected[0][0] = PLAYER_NONE;
                                    expected[0][1] = player;
                                    expected[0][2] = player;
                                    expected[0][3] = m_adversaries[player];

                                    expected[1][0] = m_adversaries[player];
                                    expected[1][1] = player;
                                    expected[1][2] = player;
                                    expected[1][3] = PLAYER_NONE;

                                    for (int i = 0; i < 2; ++i)
                                    {
                                        bool found(true);
                                        int xe(xp - v2[0]);
                                        int ye(yp - v2[1]);

                                        for (int j = 0; j < 4; ++j, xe += v2[0], ye += v2[1])
                                        {
                                            if (!(in_bounds(xe, ye) && player_at(xe, ye) == expected[i][j]))
                                            {
                                                found = false;
                                                break;
                                            }
                                        }

                                        if (found)
                                            secure = false;
                                    }
                                }
                                
                                if (secure)
                                {
                                    if (++nsecure == 5)
                                    {
                                        DEBUG_COUT("win by 5\n");
                                        return true;
                                    }
                                }
                                else
                                    nsecure = 0;

                                xp += v[0];
                                yp += v[1];
                            }
                        }
                    }
                }
            }
        }
    }
    return false;
}

// forme-t-il un trois libre ?
// trois libre = scan de [-4,4] qui verif:
//  - 0 ou 1 espace libre dans l'alignement
//  - adversaire absent aux extremites
/*  double-three configurations
     012345678
         V              ? = [ ] || [o]  
     ?oo o?  
     ?o oo?  
      ?ooo?  
      ?o oo? 
       ?ooo? 
       ?oo o?
        ?ooo?
        ?oo o?
        ?o oo?
*/

bool Map::_makes_open_three(int player, int x, int y, int dir, std::vector< std::pair<int, int> >* positions)
{
    int const (&v)[2](m_directions[dir]);
    std::vector< std::pair<int, int> > positions_dummy;
    std::vector< std::pair<int, int> >& p((positions) ? *positions : positions_dummy);
    bool ready(false);
    bool space(false);

    x -= 4 * v[0];
    y -= 4 * v[1];
    for (int i = 0; i <= 8; ++i)
    {
        if (in_bounds(x, y) && player_at(x, y) != m_adversaries[player])
        {
            if (ready)
            {
                if (p.size() == 3)
                    return true;
                
                if (m_.map[y][x].player == player)
                    p.push_back(std::make_pair<int, int>(x, y));
                else if (space)
                {
                    p.clear();
                    space = false;
                }
                else if (p.size())
                    space = true;
            }
            
            ready = true;
        }
        else
        {
            ready = false;
            space = false;
            p.clear();
        }

        x += v[0];
        y += v[1];
    }

    return false;
}


bool Map::_makes_double_three(int player, int x, int y, int except_dir)
{
    bool double_three(false);

    for (int dir = 0; dir < 4 && !double_three; ++dir)
    {
        if (dir != except_dir)
        {
            std::vector< std::pair<int, int> > positions;

            // it makes an open three
            if (_makes_open_three(player, x, y, dir, &positions))
            {
                if (except_dir < 0) // this is the first call, find another one
                {
                    // making the second call on every position
                    for (std::vector< std::pair<int, int> >::iterator it(positions.begin());
                        it != positions.end(); ++it)
                    {
                        // if a second one is found, return true
                        if (_makes_double_three(player, it->first, it->second, dir))
                            double_three = true;
                    }
                }
                else    // this is the second call, return true
                    double_three = true;
            }
        }
    }
    return double_three;
}

