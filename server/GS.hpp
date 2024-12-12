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
    int score = 0;
    std::string gameMode;
    std:: string finalSate;

};

extern std::vector<Game> games;
Player* findPlayerById(int plid);
void serverLoop(int udp_fd, int tcp_fd);

int validPLID(std::istream& input);
int validMaxPlayTime(std::istream& input);
std::vector<std::string> validGuess(std::istream& input);
void checkExtraInput(std::istream& input);
int checkNumTrials(std::istream& input);
int calcScore(const Game& game);

void closeGame(Player& player, Game& game);
void createPlayerDir(int plid, Game &game);



#endif



