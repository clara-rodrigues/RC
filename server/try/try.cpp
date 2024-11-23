#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
#include <utility>
#include <iostream>
#include "../GS.hpp"
#include "try.hpp"


std::pair<int, int> tryGuess(int plid, std::vector<std::string> guesses, int gameId) {
    int numBlack = 0;
    int numWhite = 0;
    
    
    for (size_t i = 0; i < 4; i++) {
        if (guesses[i] == games[gameId].secretKey[i]) {
            numBlack++;
        } else if (std::find(games[gameId].secretKey.begin(), games[gameId].secretKey.end(), guesses[i]) != games[gameId].secretKey.end()) {
            numWhite++;
        }
    }
    return std::make_pair(numBlack, numWhite);


    games[gameId].trials.push_back(guesses);
    games[gameId].numTrials++;


    return std::make_pair(numBlack, numWhite);
}
