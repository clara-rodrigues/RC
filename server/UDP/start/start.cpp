#include <string>
#include <iostream>

#include "start.hpp"
#include <time.h>
#include <sstream>
#include <fstream>

// Function to handle the "start" command from the client
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

    responseOK = startNewGame(plid, maxPlaytime);

    
    if(responseOK){
        sendto(fd, "RSG OK\n", 7, 0, (struct sockaddr *)&client_addr, client_len);
    }else{
        sendto(fd, "RSG NOK\n", 8, 0, (struct sockaddr *)&client_addr, client_len);
    }

}

// Function to create a file for the player with game details
void createPlayerFile(int plid,int gameId){
    std::string folder = "server/GAMES";
    std::string filename = folder + "/GAME_" + std::to_string(plid) + ".txt";
    struct tm * timeinfo;


    if (!std::filesystem::exists(folder)) {
        std::filesystem::create_directories(folder);
    }

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

// Function to start a new game for the player
int startNewGame(int plid, int maxPlaytime) {
    std::vector<std::string> secret_key = generateSecretKey();
    Player* currentPlayer = findPlayerById(plid);

    if (!currentPlayer) {
        // Create a new player and game
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

        createPlayerFile(plid, newIndex);

        return 1;

    } else {
        // Verify if the player is already playing a game
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

        // Start a new game for player
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

        return 1;
    }
}

// Function to generate a secret key for the game
std::vector<std::string> generateSecretKey(){
    std::vector<std::string> secretKey;
    
    srand(static_cast<unsigned>(time(0)));
    
    for (int i = 0; i < 4; i++) {
        secretKey.push_back(colors[rand() % colors.size()]);
    }

    return secretKey;
}

