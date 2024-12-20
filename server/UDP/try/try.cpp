#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
#include <utility>
#include <iostream>

#include "try.hpp"
#include <sstream>
#include <iomanip>


// Function to check if a trial has been made before
int existDup(std::vector<Trial> trials, std::vector<std::string> guesses){
    for(const auto& trial : trials){
        if(trial.guesses == guesses){
            return 1;
        }
    }
    return 0;
}

// Function to create a score file for the player after the game
void createScoreFile(int plid, Game &game){
    struct tm *timeinfo;
    std::string folder = "server/SCORES/";
    std::string filename;

    timeinfo = gmtime(&game.startTime);
    filename = folder +"score_"+ std::to_string(plid) + "_"+
                    std::to_string(timeinfo->tm_year + 1900) + 
                    std::to_string(timeinfo->tm_mon + 1) + 
                    std::to_string(timeinfo->tm_mday) + 
                    "_" + 
                    std::to_string(timeinfo->tm_hour) + 
                    std::to_string(timeinfo->tm_min) + 
                    std::to_string(timeinfo->tm_sec) + 
                    ".txt";


    std::ofstream file(filename, std::ios::app);

    if (file.is_open()) {
        file << std::setw(3) << std::setfill('0') << game.score << " ";
        file << plid << " ";
        for (const auto& key : game.secretKey) {
            file << key ;
        }
        file << " ";
        file << game.numTrials << " ";
        file << game.gameMode<< std::endl;
    } else {
        std::cerr << "Erro ao abrir o arquivo " << filename << std::endl;
    }

}

// Function to write a trial attempt to the game file
void writeTrial(int plid, std::vector<std::string> guesses, std::pair<int, int> args, time_t currentTime, time_t startTime) {
    std::string folder = "server/GAMES";
    std::string filename = folder + "/GAME_" + std::to_string(plid) + ".txt";
    std::ofstream file(filename, std::ios::app);
    if (file.is_open()) {
        file << "T:" << " ";
        for (const auto& guess : guesses) {
            file << guess;
        }
        file << " ";
        file << args.first << " " << args.second << " ";
        file << currentTime-startTime << std::endl;
    } else {
        std::cerr << "Erro ao abrir o arquivo " << filename << std::endl;
    }
}

// Function to handle the try/guess request from the player
void handleTry(int fd, struct sockaddr_in &client_addr, socklen_t client_len, std::istringstream &commandStream, std::string client_ip, int client_port){
    std::vector<std::string> guesses;
    int numTrials;
    int gameId;
    int plid;

    try{
        plid = validPLID(commandStream);
        guesses = validGuess(commandStream);
        numTrials = checkNumTrials(commandStream);
        checkExtraInput(commandStream);
        if (verbose) {
            std::cout << "[Verbose] [IP: "<< client_ip <<"] [PORT: "<< client_port <<"]Try Guess: ";
            for (const auto& guess : guesses) {
                std::cout << guess << " ";  
            }
             std::cout << "for PLID: " << plid << std::endl;
        }

    }catch(const std::invalid_argument& e){
        std::cout << e.what() << std::endl;
        sendto(fd, "RTR ERR\n", 8, 0, (struct sockaddr *)&client_addr, client_len);
        return;
    }

    Player* currentPlayer = nullptr; 
    
    currentPlayer = findPlayerById(plid);

    if(currentPlayer->isPlaying){
        gameId = currentPlayer->gameId;

        if (games[gameId].numTrials +1 != numTrials){
            sendto(fd, "RTR INV\n", 8, 0, (struct sockaddr *)&client_addr, client_len);
            return;
        }
    }else{
        sendto(fd, "RTR NOK\n", 8, 0, (struct sockaddr *)&client_addr, client_len);
        return;
    }

    // Check if time has been exceeded
    time_t currentTime = time(0);
    if (currentTime - games[gameId].startTime > games[gameId].maxPlaytime){
        std::ostringstream oss;
        std::vector<std::string> secretKey = games[gameId].secretKey;
        games[gameId].finalSate = 'T';
       
        oss << "RTR ETM " << secretKey[0] << " " << secretKey[1] << " " << secretKey[2] << " " << secretKey[3] << "\n";
        std::string message = oss.str();
        sendto(fd, message.c_str(), 16, 0, (struct sockaddr *)&client_addr, client_len);
        closeGame(*currentPlayer, games[gameId]);
        return ;
    }

    if (existDup(games[gameId].trials, guesses)){
        sendto(fd, "RTR DUP\n", 8, 0, (struct sockaddr *)&client_addr, client_len);
        return;
    }

    std::pair<int, int>  args = tryGuess(guesses, gameId);

    writeTrial(plid, guesses, args,currentTime, games[gameId].startTime);

    if(args.first != 4 & numTrials >= games[gameId].MAX_NUM_TRIALS){
        std::ostringstream oss;
        std::vector<std::string> secretKey = games[gameId].secretKey;

        oss << "RTR ENT " << secretKey[0] << " " << secretKey[1] << " " << secretKey[2] << " " << secretKey[3] << "\n";
        std::string message = oss.str();

        games[gameId].finalSate = 'F';

        sendto(fd, message.c_str(), 16, 0, (struct sockaddr *)&client_addr, client_len);
        closeGame(*currentPlayer, games[gameId]);
        return ;
    }

    if (args.first == 4){
        std::ostringstream oss;
        std::vector<std::string> secretKey = games[gameId].secretKey;
        
        oss << "RTR OK " << numTrials << " " << args.first << " " << args.second << "\n";
        std::string message = oss.str();
        sendto(fd, message.c_str(), 13, 0, (struct sockaddr *)&client_addr, client_len);

        games[gameId].score = calcScore(games[gameId]);
        games[gameId].finalSate = 'W';
        games[gameId].numTrials++;

        closeGame(*currentPlayer, games[gameId]);
        createScoreFile(plid, games[gameId]);
        return ;
    }

    std::ostringstream oss;
    oss << "RTR OK " << numTrials << " " << args.first << " " << args.second << "\n";

    games[gameId].numTrials++;

    std::string message = oss.str();

    sendto(fd, message.c_str(), 13, 0, (struct sockaddr *)&client_addr, client_len);
   
}

// Function to calculate the score based on trials and time
int calcScore(const Game& game) {
    time_t currentTime = time(0);
    int gameTime = (currentTime - game.startTime);
    int numTrials = game.trials.size();
    int score_normalized = ((game.MAX_NUM_TRIALS - numTrials) * 50) / game.MAX_NUM_TRIALS  
    + ((600 - gameTime) * 50) / 600;

    return score_normalized;
}

// Function to process the guess and return black and white peg counts
std::pair<int, int> tryGuess( std::vector<std::string> guesses, int gameId) {
    int numBlack = 0;
    int numWhite = 0;
    std::vector<int> trialState(4, 0);
    
    for (size_t i = 0; i < 4; i++) {
        if (guesses[i] == games[gameId].secretKey[i]) {
            numBlack++;
            trialState[i] = 2;
        }
    }

    for (size_t i = 0; i < 4; i++) {
        if (trialState[i] != 2) {
            for (size_t j = 0; j < 4; j++) {
                if (trialState[j] == 0 && guesses[i] == games[gameId].secretKey[j]) {
                    trialState[j] = 1;
                    numWhite++;
                    break;
                }
            }
        }
    }
    
    Trial newTrial = {guesses, numBlack, numWhite};

    games[gameId].trials.push_back(newTrial);

    return std::make_pair(numBlack, numWhite);
}
