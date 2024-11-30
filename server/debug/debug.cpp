#include <string>
#include <vector>
#include "../start/start.hpp"
#include <iostream>
#include "debug.hpp"


int debug(int plid, int maxPlaytime, std::vector<std::string> guesses){ 
    Player* currentPlayer = nullptr; 
    int gameId;
    int responseOK;

    responseOK = startNewGame(plid, maxPlaytime);

    if(responseOK){
        for (auto& player : players) {  
            if (player.plid == plid) {
                currentPlayer = &player;  
                break;
            }
        }

        gameId = currentPlayer->gameId;

        games[gameId].secretKey = guesses;
        return 1;
    }
    return 0;
    
}