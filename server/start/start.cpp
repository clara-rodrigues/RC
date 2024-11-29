#include <string>
#include <iostream>
#include "../GS.hpp"
#include "start.hpp"
#include <time.h>

int startNewGame(int plid, int maxPlaytime) {

    for (auto& player : players) {
        if ((player.plid == plid) & player.isPlaying) {
            std::cout << "Game already in progress." << std::endl;
            return 0;
        }
        else if ((player.plid == plid )& !player.isPlaying) {
            
            player.isPlaying = true;
            Game newGame;
            newGame.plid = plid;
            newGame.maxPlaytime = maxPlaytime;
            int newIndex = games.size() - 1;
            player.gameId = newIndex;
            std::cout << "Game ID in player: " << player.gameId << std::endl;

            games.push_back(newGame);
            std::cout << "New Game for player: " << plid
                      << " with max playtime: " << maxPlaytime << std::endl;

            return 1;
        }
    }



    // Add the new player
    Player newPlayer;
    newPlayer.plid = plid;
    newPlayer.isPlaying = true;

    std::vector<std::string> secret_key = generateSecretKey();

    // Add the new game
    Game newGame;
    newGame.secretKey = secret_key;
    newGame.plid = plid;
    newGame.maxPlaytime = maxPlaytime;
    newGame.startTime = time(0);
    games.push_back(newGame);

    int newIndex = games.size() - 1;
    newPlayer.gameId = newIndex;
    std::cout << "Game ID in player: " << newPlayer.gameId << std::endl;

    players.push_back(newPlayer);


    std::cout << "New game started for player: " << plid
              << " with max playtime: " << maxPlaytime
              << " Secret Key: ";
    for (const auto& key : secret_key) {
        std::cout << key << " ";
    }
    std::cout << std::endl;
    return 1;
}


std::vector<std::string> generateSecretKey(){
    std::vector<std::string> secretKey;
    
    srand(static_cast<unsigned>(time(0)));
    
    for (int i = 0; i < 4; i++) {
        secretKey.push_back(colors[rand() % colors.size()]);
    }

    return secretKey;
}

