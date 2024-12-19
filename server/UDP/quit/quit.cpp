
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "quit.hpp"




void handleQuit(int fd, struct sockaddr_in &client_addr, socklen_t client_len, std::istringstream &commandStream, std::string client_ip, int client_port){
    Player* currentPlayer = nullptr; 
    int plid;

    try{
        plid = validPLID(commandStream);
        checkExtraInput(commandStream);
        if (verbose) {
        std::cout << "[Verbose] [IP: "<< client_ip <<"] [PORT: "<< client_port <<"]Quit Game for PLID: " << plid << std::endl;
        }
    }catch(const std::invalid_argument& e){
        std::cout << e.what() << std::endl;
        sendto(fd, "RQT ERR\n", 8, 0, (struct sockaddr *)&client_addr, client_len);
        return;
    }

    currentPlayer = findPlayerById(plid);
    
    if(currentPlayer->isPlaying){

        int gameId = currentPlayer->gameId;
        std::vector<std::string> secretKey = games[gameId].secretKey;
        std::ostringstream oss;

        games[gameId].finalSate = 'Q';
        oss << "RQT OK " << secretKey[0] << " " << secretKey[1] << " " << secretKey[2] << " " << secretKey[3] << "\n";

        std::string message = oss.str();
        
        sendto(fd, message.c_str(), 15, 0, (struct sockaddr *)&client_addr, client_len);
        closeGame(*currentPlayer, games[gameId]);
        
    }else{
        sendto(fd, "RQT NOK\n", 8, 0, (struct sockaddr *)&client_addr, client_len);
        
    }
}