
#include "player.hpp"
#include "UDP/try/try.hpp"
#include "UDP/start/start.hpp"
#include "UDP/try/try.hpp"
#include "UDP/quit/quit.hpp"
#include "UDP/debug/debug.hpp"
#include "UDP/exit/exit.hpp"
#include "TCP/showTrials/showTrials.hpp"
#include "TCP/scoreBoard/scoreBoard.hpp"
#include <csignal>



extern bool running; 
bool running = true; 
std::string ip = "localhost";
std::string port = "58068";
Player player;

std::vector<std::string> colors = {"R", "G", "B", "Y", "O", "P"};

// Validates a PLID (Player ID) received as input.
// The PLID must contain exactly 6 numeric digits.
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

// Validates the maximum playtime provided by the user.
// It must be a value between 1 and 600 seconds.
int validMaxPlayTime(std::istream& input){
    int maxPlaytime;

    if (!(input >> maxPlaytime) || maxPlaytime <= 0 || maxPlaytime > 600) {
        throw std::invalid_argument("Invalid max_playtime. Must be between 1 and 600 seconds.");
    }
    return maxPlaytime;
}

// Validates a guess attempt for a sequence of colors.
// The sequence must contain exactly 4 valid colors.
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

// Checks for extra input that should not be present after the expected arguments.
void checkExtraInput(std::istream& input){
    std::string extra;
    if (input >> extra) {
        throw std::invalid_argument("Extra input detected.");
    }
}

// Processes the "start" command to initialize the game.
// Validates the PLID, maximum playtime, and sets up the player.
Player parseStartGame(const std::string& input, Player& player) {
    std::istringstream iss(input);
    std::string command;

    iss >> command;

    try{
        player.plid = validPLID(iss);
        player.maxPlaytime = validMaxPlayTime(iss);
        checkExtraInput(iss);
    }catch (const std::invalid_argument& e){
       throw std::invalid_argument(e);
    }
    player.numTrials = 1;

    return player;
}

// Processes the "try" command to record a guess attempt.
// Validates the provided colors and ensures the maximum number of attempts is not exceeded.
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

// Processes the "debug" command to configure a test game.
// Validates the PLID, maximum playtime, and color sequence.
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
}

// Ends the current game, resetting the player's state.
void closeGame(Player &player){
    player.isPlaying = false;
    player.numTrials = 1;
}

// Maps text commands to numeric IDs for easier processing.
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
    return (it != commandMap.end()) ? it->second : -1; 
}

void deleteFiles(){
    std::string folder = "client/";
    for (const auto & entry : std::filesystem::directory_iterator(folder)){
        if(entry.path().extension() == ".txt"){
            std::filesystem::remove(entry.path());
        }
    }

}



// Handles signals (such as SIGINT) to perform cleanup actions before exiting the program.
void signalHandler(int signum) {
    if (player.isPlaying){
        execute_exit(player, ip, port);
        
    }
    deleteFiles();
    std::exit(signum);
    
}

// The main function of the program.
// Sets up initial parameters, processes user commands, and controls the game flow.
int main(int argc, char* argv[]) {
    

    for (int i = 1; i < argc; i++) {
        if (std::string(argv[i]) == "-n") {
            ip = argv[i + 1];
        } else if (std::string(argv[i]) == "-p") {
            port = argv[i + 1];
         
        }
    }
    running = true;
    while (running) {
        std::cout << "Enter command: ";
        std::string input;
        std::getline(std::cin, input);

        std::istringstream iss(input);
        std::string command;
        iss >> command;

        int commandID = getCommandID(command);


        std::signal(SIGINT, signalHandler);

        switch (commandID) {
            case 1: { // "start"
                try {
                    parseStartGame(input, player);
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
                    int numTrials = player.numTrials;
                    execute_try(player, guesses, ip, port, numTrials);
                    player.numTrials = numTrials;
                } catch (const std::invalid_argument& e) {
                    std::cerr << "Error: " << e.what() << std::endl;
                }
                break;
            }
            case 3: { // "show_trials" or "st"
                show_trials(ip, port, player.plid);  
                break;
            }
            case 4: { // "scoreboard" or "sb"
                score_board(ip, port);
                break;
            }
            case 5: { // "quit" 
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
