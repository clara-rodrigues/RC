#ifndef GS_HPP
#define GS_HPP

#include <string>

struct Player{
    int plid;
    bool isPlaying = false;
    int gameId;
};

extern std::vector<Player> players;

extern std::vector<std::string> colors;




struct Game{
    std::vector<std::string> secretKey;
    int plid;
    int maxPlaytime;
    int numTrials = 0;
    static const int MAX_NUM_TRIALS = 10;
    std::vector<std::vector<std::string>>trials;

};

extern std::vector<Game> games;





#endif