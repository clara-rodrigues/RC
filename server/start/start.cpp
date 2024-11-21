#include <string>
#include <iostream>
#include "../GS.hpp"

void startNewGame(const std::string& plid, int maxPlaytime) {

    for (const auto& player : players) {
        if (player.plid == plid) {
            std::cout << "Player " << plid << " already exists. Game not started." << std::endl;
            return;
        }
    }

    // Add the new player
    Player newPlayer;
    newPlayer.plid = plid;
    newPlayer.maxPlaytime = maxPlaytime;
    players.push_back(newPlayer);

    std::cout << "New game started for player: " << plid
              << " with max playtime: " << maxPlaytime << std::endl;
}


