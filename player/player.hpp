#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include "UDP/try/try.hpp"
#include "TCP/showTrials/showTrials.hpp"
#include "TCP/scoreBoard/scoreBoard.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <unordered_map>
#include "UDP/start/start.hpp"
#include "UDP/try/try.hpp"
#include "UDP/quit/quit.hpp"
#include "UDP/debug/debug.hpp"
#include "UDP/exit/exit.hpp"

struct Player {
    std::string plid;
    int maxPlaytime;
    int numTrials = 1;
    static const int MAX_NUM_TRIALS = 10;
    bool isPlaying = false;
};

extern bool running;
extern std::vector<std::string> colors;

Player parseStartGame(const std::string& input);

void parseTryGuess(const std::string& input, Player& player, std::vector<std::string>& guesses);

int getCommandID(const std::string& command);

int main();

#endif // CLIENT_HPP
