#ifndef PLAYER_HPP
#define PLAYER_HPP




#include <string>
#include <vector>
#include <unordered_map>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <unordered_map>
#include <algorithm>


struct Player {
    std::string plid;
    int maxPlaytime;
    int numTrials = 1;
    static const int MAX_NUM_TRIALS = 10;
    bool isPlaying = false;
};

extern bool running;
extern std::vector<std::string> colors;

void closeGame(Player &player);

Player parseStartGame(const std::string& input);

void parseTryGuess(const std::string& input, Player& player, std::vector<std::string>& guesses);

int getCommandID(const std::string& command);

int main(int argc, char* argv[]);

#endif // CLIENT_HPP
