#include "exit.hpp"

// Executes the "exit" command, which handles the process of quitting the game and stopping the program.
// Parameters:
// - player: The Player object containing the player's ID (plid) and game settings.
// - ip, port: The server's IP address and port number.
void execute_exit(Player &player,const std::string& ip, const std::string& port) {
   execute_quit(player, ip, port);
   running = false;
}
