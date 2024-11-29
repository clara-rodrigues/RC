#ifndef GS_HPP
#define GS_HPP

#include <fstream>
#include <ctime>
#include <string>
#include <vector>
#include <ctime>

struct Player {
    int plid;
    bool isPlaying = false;
    int gameId;

    bool hasFinishedGames() const;
    std::string getActiveGameSummary() const;
    std::string getLastFinishedGameSummary() const;
};

extern std::vector<Player> players;

extern std::vector<std::string> colors;

struct Trial {
    std::vector<std::string> guesses;
    int numBlack;
    int numWhite;
};

struct Game {
    std::vector<std::string> secretKey;
    int plid;
    int maxPlaytime;
    int numTrials = 0;
    static const int MAX_NUM_TRIALS = 3;
    std::vector<Trial> trials;
    time_t startTime;
};

extern std::vector<Game> games;
Player* findPlayerById(int plid);
void serverLoop(int udp_fd, int tcp_fd);
#endif
