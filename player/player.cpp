
#include "player.hpp"
#include "UDP/try/try.hpp"
#include "UDP/start/start.hpp"
#include "UDP/try/try.hpp"
#include "UDP/quit/quit.hpp"
#include "UDP/debug/debug.hpp"
#include "UDP/exit/exit.hpp"
#include "TCP/showTrials/showTrials.hpp"
#include "TCP/scoreBoard/scoreBoard.hpp"



extern bool running; // Declaration of the running variable
bool running = true; // Initialize the running variable
std::vector<std::string> colors = {"R", "G", "B", "Y", "O", "P"};



std::string validPLID(std::istream& input){
    std::string plid;

    if (!(input >> plid) || plid.size() != 6) {
        throw std::invalid_argument("Invalid or missing PLID.");}

    for (char c : plid) {
        if (!std::isdigit(c)) {
            throw std::invalid_argument("Invalid or missing PLID.");
        }
    }
    return plid;
}


int validMaxPlayTime(std::istream& input){
    int maxPlaytime;

    if (!(input >> maxPlaytime) || maxPlaytime <= 0 || maxPlaytime > 600) {
        throw std::invalid_argument("Invalid max_playtime. Must be between 1 and 600 seconds.");
    }
    return maxPlaytime;
}

std::vector<std::string> validGuess(std::istream& input) {
    std::vector<std::string> rawGuesses;
    std::string guess;

    while (input >> guess) {
        rawGuesses.push_back(guess);
        if (rawGuesses.size() == 4) {
            break;
        }
    }

    if (rawGuesses.size() != 4) {
        throw std::invalid_argument("Invalid input. Must provide exactly 4 guesses.");
    }

    for (const auto& g : rawGuesses) {
        if (std::find(colors.begin(), colors.end(), g) == colors.end()) {
            throw std::invalid_argument("Invalid guess. One or more guesses are not valid.");
        }
    }

    return rawGuesses;
}

void checkExtraInput(std::istream& input){
    std::string extra;
    if (input >> extra) {
        throw std::invalid_argument("Extra input detected.");
    }
}


Player parseStartGame(const std::string& input) {
    std::istringstream iss(input);
    std::string command;
    Player player;

    iss >> command;

    try{
        player.plid = validPLID(iss);
        player.maxPlaytime = validMaxPlayTime(iss);
        checkExtraInput(iss);
    }catch (const std::invalid_argument& e){
       throw std::invalid_argument(e);
    }

    return player;
}


void parseTryGuess(const std::string& input, Player& player, std::vector<std::string>& guesses) {
    std::istringstream iss(input);
    std::string command;

    iss >> command;

    if (player.numTrials > Player::MAX_NUM_TRIALS) {
        throw std::invalid_argument("Max number of trials reached.");}

    try{
        guesses = validGuess(iss);
        checkExtraInput(iss);
    }catch (const std::invalid_argument& e){
       throw std::invalid_argument(e);
    }
}


void parseDebug(const std::string& input, Player& player, std::vector<std::string>& guesses){
    std::istringstream iss(input);
    std::string command;
    std::string plid;

    iss >> command;
    try{
        player.plid = validPLID(iss);
        player.maxPlaytime = validMaxPlayTime(iss);
        guesses = validGuess(iss);
        checkExtraInput(iss);
    }catch (const std::invalid_argument& e){
       throw std::invalid_argument(e);
    }

    std::cout << "Player Id: " << player.plid <<std::endl;
    std::cout << "Max Play time: " << player.maxPlaytime <<std::endl;
    std::cout << " Valid Guess: " << guesses[0] << guesses[1] << guesses[2] << guesses[3] <<std::endl;

}

void closeGame(Player &player){
    player.isPlaying = false;
    player.numTrials = 1;
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
int main(int argc, char* argv[]) {
    std::string ip = "localhost";  // Server's IP
    std::string port = "58068";     // Server's Port

    Player player;

    for (int i = 1; i < argc; i++) {
        if (std::string(argv[i]) == "-n") {
            ip = argv[i + 1];
        } else if (std::string(argv[i]) == "-p") {
            port = argv[i + 1];
         
        }
    }

    std::cout << "PORT: " << port << std::endl;
    std::cout << "IP: " << ip << std::endl;

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
                    if(player.isPlaying && player.numTrials > 1){
                        std::cerr << "Error: There is an ongoing game." << std::endl;
                        break;
                    }
                    player = parseStartGame(input);
                    std::cout << "PLID: " << player.plid << std::endl;
                    std::cout << "Max Playtime: " << player.maxPlaytime << " seconds" << std::endl;
                    execute_start(player, ip, port);
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

                    execute_try(player, guesses, ip, port, numTrials);
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
                score_board(ip, port);
                break;
            }
            case 5: { // "quit" or "exit"
                if(player.isPlaying){
                    execute_quit(player,ip, port);
                }
                std::cout << "Exiting the game. Goodbye!" << std::endl;
         
                break;
            }
            case 6: { // "exit"
                if (player.isPlaying){
                    execute_exit(player, ip, port);
                }
                std::cout << "Exiting the game. Goodbye!" << std::endl;
                running = false;
         
                break;
            }
            case 7: { // "debug"
                if(player.isPlaying && player.numTrials > 1){
                    std::cerr << "Error: There is an ongoing game." << std::endl;
                    break;
                }
                std::vector<std::string> guesses;
                try{
                    parseDebug(input,player,guesses);
                }catch (const std::invalid_argument& e){
                    std::cerr << "Error: " << e.what() << std::endl;
                    break;
                }
                execute_debug(player, ip, port, guesses);
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
