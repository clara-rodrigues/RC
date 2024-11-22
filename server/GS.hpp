#ifndef GS_HPP
#define GS_HPP

#include <string>

struct Player{
    std::string plid;
    bool isPlaying = false;
};

extern std::vector<Player> players;

extern std::vector<std::string> colors;



struct Game{
    std::vector<std::string> secretKey;
    std::string plid;
    int maxPlaytime;
    int numTrials = 0;
    static const int MAX_NUM_TRIALS = 10;
};

extern std::vector<Game> games;





#endif