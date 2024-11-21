#ifndef GS_HPP
#define GS_HPP

#include <string>

struct Player{
    std::string plid;
    int maxPlaytime;
    int numTrials = 0;
    static const int MAX_NUM_TRIALS = 10;
};

extern std::vector<Player> players;

#endif