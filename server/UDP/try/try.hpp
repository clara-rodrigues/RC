#ifndef TRY_HPP
#define TRY_HPP

#include <string>
#include "../../GS.hpp"
#include "../UDP.hpp"


int existDup(std::vector<Trial> trials, std::vector<std::string> guesses);

void createScoreFile(int plid, Game &game);

void writeTrial(int plid, std::vector<std::string> guesses, std::pair<int, int> args, time_t currentTime, time_t startTime) ;

void handleTry(int fd, struct sockaddr_in &client_addr, socklen_t client_len, std::istringstream &commandStream, std::string client_ip, int client_port);

int calcScore(const Game& game);

std::pair<int, int> tryGuess(int plid, std::vector<std::string> guesses, int gameId);

#endif