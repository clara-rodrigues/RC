#ifndef TRY_HPP
#define TRY_HPP

#include <string>
#include "../../GS.hpp"
#include "../UDP.hpp"

std::pair<int, int> tryGuess(int plid, std::vector<std::string> guesses, int gameId);

void handleTry(int fd, struct sockaddr_in &client_addr, socklen_t client_len, std::istringstream &commandStream);

#endif