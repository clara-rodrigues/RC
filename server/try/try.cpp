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
    std::vector<int> trialState(4, 0);
    
    
    for (size_t i = 0; i < 4; i++) {
        if (guesses[i] == games[gameId].secretKey[i]) {
            numBlack++;
            trialState[i] = 2;
        }
    }

    for (size_t i = 0; i < 4; i++) {
        if (trialState[i] != 2) {
            for (size_t j = 0; j < 4; j++) {
                if (trialState[j] == 0 && guesses[i] == games[gameId].secretKey[j]) {
                    trialState[j] = 1;
                    numWhite++;
                    break;
                }
            }
        }
    }
    

    Trial newTrial = {guesses, numBlack, numWhite};

    std::cout << "Trial Guess:" << newTrial.guesses[0] << newTrial.guesses[1] << newTrial.guesses[2] << newTrial.guesses[3] <<newTrial.numBlack <<newTrial.numWhite << std::endl;


    games[gameId].trials.push_back(newTrial);


    return std::make_pair(numBlack, numWhite);
}
