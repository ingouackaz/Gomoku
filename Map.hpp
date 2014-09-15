
#ifndef HPP_MAP
#define HPP_MAP

#include <vector>
#include <utility>
#include <bitset>

#define ASSERT(x)
#define DEBUG_COUT(x)

#if 0
#include <cassert>
#undef ASSERT
#define ASSERT(x)   assert(x)
#endif

#if defined(SERVER) && 0
#define DEBUG 1
#include <iostream>
#undef DEBUG_COUT
#define DEBUG_COUT(x) do {if (m_debug) {std::cout << x;}} while (0)
#endif

class Map
{
    public:
    enum Player
    {
        PLAYER_BLACK=0,
        PLAYER_WHITE=1,
        PLAYER_NONE=-1,
    };

    struct PlayerStats
    {
        short nrows[5]; // rows of 1/2/3/4/5+
        char ntaken;
    };

    struct Position
    {
        char player;// -1 if no player, 0 or 1 otherwise
        struct Direction
        {
            char sz_current;
            char sz_total;
        } rows[4];  // rows, from top left to bottom right
    };

    static inline bool in_bounds(int x, int y)
    {
        return ((x >= 0 && x < 19) &&
                (y >= 0 && y < 19));
    }
    
    static inline void set_double_three(bool forbidden)
    {
        g_allow_d3 = !forbidden;
    }

    static inline void set_breaking_five(bool enabled)
    {
        g_allow_b5 = enabled;
    }
    
    Map();

    Map(Map const& map);

    Map& operator=(Map const& map);
    
    inline PlayerStats const& player_stats(int player) const
    {
        ASSERT(player == 0 || player == 1);
        return m_.stats[player];
    }

    inline bool double_three() const
    {
        return g_allow_d3;
    }

    inline bool breaking_five() const
    {
        return g_allow_b5;
    }
    
    inline int player_at(int x, int y) const
    {
        return m_.map[y][x].player;
    }

    inline bool near(int x, int y) const
    {
        return m_.near[y][x];
    }

    inline bool is_valid_move(int player, int x, int y) const
    {
#ifdef SERVER
        std::vector< std::pair<int, int> > taken;
#endif
        Map cpy(*this);
        bool ret;

#ifdef DEBUG
        bool prev_debug(m_debug);

        m_debug = false;
#endif
        ret = cpy.play(player, x, y
#ifdef SERVER
            , taken
#endif
        );
#ifdef DEBUG
        m_debug = prev_debug;
#endif
        return ret;
    }

    void take(int x, int y);

    void put(int player, int x, int y);

    // player must be set to 0 or 1. unexpected value => undefined behavior.
    // x and y must be in the map bounds [0;19[. unexpected value => undefined behavior.
    // win set to true or false if the move makes the player win.
    // returns true for successful move, false for forbidden move.

    // XXX: the instance enters in an INVALID STATE once Map::play returns false (invalid move),
    // so do not reuse it if it occurs (always copy/use/delete the Map instance).
    // you can also use Map::is_valid_move before using this method.
    bool play(int player, int x, int y
#ifdef SERVER
        , std::vector< std::pair<int, int> >& taken
#endif
    );

    bool wins(int player) const;
    
    protected:
    static bool g_allow_d3;
    static bool g_allow_b5;

    bool _makes_open_three(int player, int x, int y, int dir, std::vector< std::pair<int, int> >* positions);
    bool _makes_double_three(int player, int x, int y, int except_dir);

    // cached values
    static int const m_directions[8][2];
    static int const m_adversaries[2];

#ifdef DEBUG
    static bool m_debug;
#endif

    struct
    {
        std::bitset<19> near[19];
        Position map[19][19];
        PlayerStats stats[2];
    } m_;
};

#endif

