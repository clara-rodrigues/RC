#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <unordered_map>
#include "UDP/start/start.hpp"
#include "UDP/try/try.hpp"
#include "UDP/quit/quit.hpp"
#include "UDP/debug/debug.hpp"
#include "UDP/exit/exit.hpp"
#include "player.hpp"
#include "UDP/try/try.hpp"
#include "TCP/showTrials/showTrials.hpp"

extern bool running; // Declaration of the running variable
bool running = true; // Initialize the running variable

Player parseStartGame(const std::string& input) {
    std::istringstream iss(input);
    std::string command;
    Player player;

    iss >> command;

    if (!(iss >> player.plid) || player.plid.size() != 6) {
        throw std::invalid_argument("Invalid or missing PLID.");}

    for (char c : player.plid) {
        if (!std::isdigit(c)) {
            throw std::invalid_argument("Invalid or missing PLID.");
        }
    }  

    if (!(iss >> player.maxPlaytime) || player.maxPlaytime <= 0 || player.maxPlaytime > 600) {
        throw std::invalid_argument("Invalid max_playtime. Must be between 1 and 600 seconds.");}


    std::string extra;
    if (iss >> extra) {
        throw std::invalid_argument("Extra input detected. Expected format: 'start PLID max_playtime'.");
    }

    return player;
}

void parseTryGuess(const std::string& input, Player& player, std::vector<std::string>& guesses) {
    std::istringstream iss(input);
    std::string command;

    if (!(iss >> command) || command != "try") {
        throw std::invalid_argument("Invalid command. Expected 'try'.");}

    if (player.numTrials > Player::MAX_NUM_TRIALS) {
        throw std::invalid_argument("Max number of trials reached.");}

    std::string guess;
    while (iss >> guess) {
        guesses.push_back(guess);}

    if (guesses.size() != 4) {
        throw std::invalid_argument("Invalid number of guesses. Expected exactly 4 guesses.");
    }

}

int getCommandID(const std::string& command) {
    static std::unordered_map<std::string, int> commandMap = {
        {"start", 1},
        {"try", 2},
        {"show_trials", 3},
        {"st", 3}, 
        {"scoreboard", 4},
        {"sb", 4}, 
        {"quit", 5},
        {"exit", 6}, 
        {"debug", 7}
    };

    auto it = commandMap.find(command);
    return (it != commandMap.end()) ? it->second : -1; // Return -1 for unknown commands
}

// Main function
int main() {
    std::string ip = "localhost";  // Server's IP
    std::string port = "58000";     // Server's Port
    std::string port_tcp = "58000";
    Player player;

    running = true;
    while (running) {
        std::cout << "Enter command: ";
        std::string input;
        std::getline(std::cin, input);

        std::istringstream iss(input);
        std::string command;
        iss >> command;

        int commandID = getCommandID(command);

        switch (commandID) {
            case 1: { // "start"
                try {
                    player = parseStartGame(input);
                    std::cout << "PLID: " << player.plid << std::endl;
                    std::cout << "Max Playtime: " << player.maxPlaytime << " seconds" << std::endl;
                    execute_start(player.plid, player.maxPlaytime, ip, port);
                } catch (const std::invalid_argument& e) {
                    std::cerr << "Error: " << e.what() << std::endl;
                }
                break;
            }
            case 2: { // "try"
                try {
                    std::vector<std::string> guesses;
                    parseTryGuess(input, player, guesses);
                    std::cout << "Guess recorded successfully!" << std::endl;
                    std::cout << "Guesses: ";
                    for (const auto& g : guesses) {
                        std::cout << g << " ";
                    }
                    std::cout << std::endl;
                    int numTrials = player.numTrials;

                    execute_try(player.plid, guesses, ip, port, numTrials);
                    player.numTrials = numTrials;
                } catch (const std::invalid_argument& e) {
                    std::cerr << "Error: " << e.what() << std::endl;
                }
                break;
            }
            case 3: { // "show_trials" or "st"
                std::cout << "Number of trials made: " << player.numTrials << std::endl;
                show_trials(ip, port, player.plid);  
                break;
            }
            case 4: { // "scoreboard" or "sb"
                std::cout << "Scoreboard feature not implemented yet." << std::endl;
                break;
            }
            case 5: { // "quit" or "exit"
                std::cout << "Exiting the game. Goodbye!" << std::endl;
                execute_quit(player.plid,ip, port);
         
                break;
            }
            case 6: { // "exit"
                std::cout << "Exiting the game. Goodbye!" << std::endl;
                execute_exit(player.plid, ip, port);
         
                break;
            }
            case 7: { // "debug"
                std::cout << "Debug Information:" << std::endl;
                std::cout << "PLID: " << player.plid << std::endl;
                std::cout << "Max Playtime: " << player.maxPlaytime << std::endl;
                std::cout << "Trials: " << player.numTrials << std::endl;
                execute_debug(ip, port);
                break;
            }
            default: { // Unknown command
                std::cerr << "Error: Unknown command." << std::endl;
                break;
            }
        }
    }

    return 0;
}
