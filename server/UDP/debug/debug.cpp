#include <string>
#include <vector>
#include "../start/start.hpp"
#include <iostream>
#include "debug.hpp"
#include <sstream>



void handleDebug(int fd, struct sockaddr_in &client_addr, socklen_t client_len, std::istringstream &commandStream){
    int maxPlaytime;
    std::vector<std::string> guesses;
    std::string guess;
    int responseOk;
    int plid;
    
    try{
        plid = validPLID(commandStream);
        maxPlaytime = validMaxPlayTime(commandStream);
        guesses = validGuess(commandStream);
        checkExtraInput(commandStream);

    }catch(const std::invalid_argument& e){
        std::cout << e.what() << std::endl;
        sendto(fd, "RDG ERR", 7, 0, (struct sockaddr *)&client_addr, client_len);
        return;
    }
    
    responseOk = debug(plid, maxPlaytime, guesses);

    if(responseOk){
        sendto(fd, "RDG OK", 6, 0, (struct sockaddr *)&client_addr, client_len);
    }else{
        sendto(fd, "RDG NOK", 7, 0, (struct sockaddr *)&client_addr, client_len);
    }
}





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