#include <string>
#include <iostream>
#include "../GS.hpp"
#include "start.hpp"

int startNewGame(const std::string& plid, int maxPlaytime) {

    for (const auto& player : players) {
        if (player.plid == plid & player.isPlaying) {
            std::cout << "Game already in progress." << std::endl;
            return 0;
        }
        else if (player.plid == plid & !player.isPlaying) {
            
            Game newGame;
            newGame.plid = plid;
            newGame.maxPlaytime = maxPlaytime;
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
    players.push_back(newPlayer);

    std::vector<std::string> secret_key = generateSecretKey();

    // Add the new game
    Game newGame;
    newGame.secretKey = secret_key;
    newGame.plid = plid;
    newGame.maxPlaytime = maxPlaytime;
    games.push_back(newGame);

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

