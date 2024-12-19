#include <string>
#include <iostream>

#include "start.hpp"
#include <time.h>
#include <sstream>
#include <fstream>


void handleStartGame( int fd, struct sockaddr_in &client_addr, socklen_t client_len, std::istringstream &commandStream,  std::string client_ip, int client_port){
    int responseOK;
    int plid;
    int maxPlaytime;

    try{
        plid = validPLID(commandStream);
        maxPlaytime = validMaxPlayTime(commandStream);
        checkExtraInput(commandStream);
        if (verbose) {
        std::cout << "[Verbose] [IP: "<< client_ip <<"] [PORT: "<< client_port <<"] Start Game for PLID: " << plid << std::endl;
        }
    }catch(const std::invalid_argument& e){
        std::cout << e.what() << std::endl;
        sendto(fd, "RSG ERR\n", 8, 0, (struct sockaddr *)&client_addr, client_len);
        return;
    }

    std::cout << "Starting new game for Player ID: " << plid 
                << ", Max Playtime: " << maxPlaytime << std::endl;
    responseOK = startNewGame(plid, maxPlaytime);

    
    if(responseOK){
        sendto(fd, "RSG OK\n", 7, 0, (struct sockaddr *)&client_addr, client_len);
    }else{
        sendto(fd, "RSG NOK\n", 8, 0, (struct sockaddr *)&client_addr, client_len);
    }

}


void createPlayerFile(int plid,int gameId){
    std::string folder = "server/GAMES";
    std::string filename = folder + "/GAME_" + std::to_string(plid) + ".txt";
    struct tm * timeinfo;


    if (!std::filesystem::exists(folder)) {
        std::filesystem::create_directories(folder);
    }

    std::cout << "Creating player file: " << filename << std::endl;


    timeinfo = gmtime(&games[gameId].startTime);

    std::ofstream file(filename);
    if (file.is_open()) {
        file <<  plid << " "<< games[gameId].gameMode << " " ;
        for (const auto& key : games[gameId].secretKey) {
            file << key ;
        }
        file << " "<< games[gameId].maxPlaytime << " ";
        file <<  timeinfo->tm_year +1900<< "-"; 
        file << timeinfo->tm_mon+1 << "-";
        file << timeinfo->tm_mday << " " ;
        file <<timeinfo->tm_hour << ":";
        file << timeinfo->tm_min<< ":" ;
        file << timeinfo->tm_sec <<" "  ;
        file << std::to_string(time(0))<< std::endl;
    
        file.close();
    } else {
        std::cerr << "Error: Could not open file for writing!" << std::endl;
    }

}

int startNewGame(int plid, int maxPlaytime) {
    std::vector<std::string> secret_key = generateSecretKey();
    Player* currentPlayer = findPlayerById(plid);

    if (!currentPlayer) {
        // Criar um novo jogador e iniciar um novo jogo
        Player newPlayer;
        newPlayer.plid = plid;
        newPlayer.isPlaying = true;

        Game newGame;
        newGame.secretKey = secret_key;
        newGame.plid = plid;
        newGame.maxPlaytime = maxPlaytime;
        newGame.startTime = time(0);
        newGame.gameMode = "P";
        games.push_back(newGame);

        int newIndex = games.size() - 1;
        newPlayer.gameId = newIndex;
        players.push_back(newPlayer);

        // Criar arquivo do jogador
        createPlayerFile(plid, newIndex);

        std::cout << "New game started for player: " << plid
                  << " with max playtime: " << maxPlaytime
                  << " Secret Key: ";
        for (const auto& key : secret_key) {
            std::cout << key << " ";
        }
        std::cout << std::endl;

        return 1;

    } else {
        if (currentPlayer->isPlaying) {
            Game& currentGame = games[currentPlayer->gameId];
            time_t currentTime = time(0);
            if (currentTime - currentGame.startTime > currentGame.maxPlaytime) {
                currentGame.finalSate = 'T';
                closeGame(*currentPlayer, currentGame);
            } else {
                std::cout << "Game already in progress." << std::endl;
                return 0;
            }
        }

        currentPlayer->isPlaying = true;

        Game newGame;
        newGame.plid = plid;
        newGame.maxPlaytime = maxPlaytime;
        newGame.gameMode = "P";
        newGame.startTime = time(0);
        newGame.secretKey = secret_key;
        games.push_back(newGame);

        int newIndex = games.size() - 1;
        currentPlayer->gameId = newIndex;

        createPlayerFile(plid, newIndex);

        std::cout << "New game started for player: " << plid
                  << " with max playtime: " << maxPlaytime
                  << " Secret Key: ";
        for (const auto& key : secret_key) {
            std::cout << key << " ";
        }
        std::cout << std::endl;

        return 1;
    }
}



std::vector<std::string> generateSecretKey(){
    std::vector<std::string> secretKey;
    
    srand(static_cast<unsigned>(time(0)));
    
    for (int i = 0; i < 4; i++) {
        secretKey.push_back(colors[rand() % colors.size()]);
    }

    return secretKey;
}

