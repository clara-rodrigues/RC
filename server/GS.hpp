#ifndef GS_HPP
#define GS_HPP

#include <fstream>
#include <ctime>
#include <string>
#include <vector>
#include <ctime>

#define BUFFER_SIZE 4096

struct Player {
    int plid;
    bool isPlaying = false;
    int gameId;
    std::string getActiveGameSummary(std::string gameFile) const;
};

extern std::vector<Player> players;

extern std::vector<std::string> colors;
extern int verbose;

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
    static const int MAX_NUM_TRIALS = 8;
    std::vector<Trial> trials;
    time_t startTime;
    int score = 0;
    std::string gameMode;
    std:: string finalSate;

};

extern std::vector<Game> games;


void createPlayerDir(int plid, Game &game);

void closeGame(Player& player, Game& game);

void clearGamesDir(std::string directory);

void signalHandler(int signum);

Player* findPlayerById(int plid);

int validPLID(std::istream& input);

int validMaxPlayTime(std::istream& input);

std::vector<std::string> validGuess(std::istream& input);

void checkExtraInput(std::istream& input);

int checkNumTrials(std::istream& input);

Player* findPlayerById(int plid);

void serverLoop(int udp_fd, int tcp_fd);


#endif



