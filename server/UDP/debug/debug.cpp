#include <string>
#include <vector>
#include "../start/start.hpp"
#include <iostream>
#include "debug.hpp"
#include <sstream>



void handleDebug(int fd, struct sockaddr_in &client_addr, socklen_t client_len, std::istringstream &commandStream, std::string client_ip, int client_port){
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
        if (verbose) {
        std::cout << "[Verbose] [IP: "<< client_ip <<"] [PORT: "<< client_port <<"]Debug Command Received for player" << plid << std::endl;
    }

    }catch(const std::invalid_argument& e){
        std::cout << e.what() << std::endl;
        sendto(fd, "RDB ERR\n", 8, 0, (struct sockaddr *)&client_addr, client_len);
        return;
    }
    
    responseOk = debug(plid, maxPlaytime, guesses);

    if(responseOk){
        sendto(fd, "RDB OK\n", 7, 0, (struct sockaddr *)&client_addr, client_len);
    }else{
        sendto(fd, "RDB NOK\n", 8, 0, (struct sockaddr *)&client_addr, client_len);
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
        games[gameId].gameMode = "D";
        createPlayerFile(plid,gameId);
        return 1;
    }
    return 0;
    
}