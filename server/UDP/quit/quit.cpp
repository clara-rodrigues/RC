
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "quit.hpp"




void handleQuit(int fd, struct sockaddr_in &client_addr, socklen_t client_len, std::istringstream &commandStream){
    Player* currentPlayer = nullptr; 
    int plid;

    try{
        plid = validPLID(commandStream);
        checkExtraInput(commandStream);
    }catch(const std::invalid_argument& e){
        std::cout << e.what() << std::endl;
        sendto(fd, "RQT ERR", 7, 0, (struct sockaddr *)&client_addr, client_len);
        return;
    }

    std::cout<<"HELLPPP"<<std::endl;

    currentPlayer = findPlayerById(plid);

    if(currentPlayer->isPlaying){
        std::cout << "Player is playing" << std::endl;
        int gameId = currentPlayer->gameId;
        std::vector<std::string> secretKey = games[gameId].secretKey;
        currentPlayer->isPlaying = false;
        games[gameId].finalSate = 'Q';
        closeGame(*currentPlayer, games[gameId]);
        std::ostringstream oss;
        oss << "RQT OK " << secretKey[0] << " " << secretKey[1] << " " << secretKey[2] << " " << secretKey[3];
        std::string message = oss.str();
        sendto(fd, message.c_str(), 15, 0, (struct sockaddr *)&client_addr, client_len);
        
    }else{
        sendto(fd, "RQT NOK", 7, 0, (struct sockaddr *)&client_addr, client_len);
        
    }
}