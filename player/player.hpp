#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <vector>
#include <unordered_map>

struct Player {
    std::string plid;
    int maxPlaytime;
    int numTrials = 1;
    static const int MAX_NUM_TRIALS = 10;
    bool isPlaying = false;
};

extern bool running;

Player parseStartGame(const std::string& input);

void parseTryGuess(const std::string& input, Player& player, std::vector<std::string>& guesses);

int getCommandID(const std::string& command);

int main();

#endif // CLIENT_HPP
