#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
#include <utility>
#include <iostream>

#include "try.hpp"
#include <sstream>



int existDup(std::vector<Trial> trials, std::vector<std::string> guesses){
    for(const auto& trial : trials){
        if(trial.guesses == guesses){
            return 1;
        }
    }
    return 0;
}


void writeTrial(int plid, std::vector<std::string> guesses, std::pair<int, int> args, time_t currentTime, time_t startTime) {
    std::string folder = "server/GAMES";
    std::string filename = folder + "/GAME_" + std::to_string(plid) + ".txt";
    std::ofstream file(filename, std::ios::app);
    if (file.is_open()) {
        file << "T:";
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


void handleTry(int fd, struct sockaddr_in &client_addr, socklen_t client_len, std::istringstream &commandStream){
    std::vector<std::string> guesses;
    int numTrials;
    int gameId;
    int plid;

    try{
        plid = validPLID(commandStream);
        guesses = validGuess(commandStream);
        numTrials = checkNumTrials(commandStream);
        checkExtraInput(commandStream);

    }catch(const std::invalid_argument& e){
        std::cout << e.what() << std::endl;
        sendto(fd, "RTR ERR", 7, 0, (struct sockaddr *)&client_addr, client_len);
        return;
    }
    

    Player* currentPlayer = nullptr; 
    
    currentPlayer = findPlayerById(plid);

    // check if player is playing
    if(currentPlayer->isPlaying){
        gameId = currentPlayer->gameId;

        // check if trial number is correct
        if (games[gameId].numTrials +1 != numTrials){
            sendto(fd, "RTR INV", 7, 0, (struct sockaddr *)&client_addr, client_len);
            return;
        }
    }else{
        sendto(fd, "RTR NOK", 7, 0, (struct sockaddr *)&client_addr, client_len);
        return;
    }


    //check if time has been exceeded
    time_t currentTime = time(0);
    if (currentTime - games[gameId].startTime > games[gameId].maxPlaytime){
        std::ostringstream oss;
        std::vector<std::string> secretKey = games[gameId].secretKey;
        games[gameId].finalSate = 'T';
        currentPlayer->isPlaying = false;
        oss << "RTR ETM " << secretKey[0] << " " << secretKey[1] << " " << secretKey[2] << " " << secretKey[3];
        std::string message = oss.str();
        sendto(fd, message.c_str(), 15, 0, (struct sockaddr *)&client_addr, client_len);
        closeGame(*currentPlayer, games[gameId]);
        return ;
    }


    if (existDup(games[gameId].trials, guesses)){
        sendto(fd, "RTR DUP", 7, 0, (struct sockaddr *)&client_addr, client_len);
        return;
    }

    std::pair<int, int>  args = tryGuess(plid, guesses, gameId);

    writeTrial(plid, guesses, args,currentTime, games[gameId].startTime);

    if(args.first != 4 & numTrials >= games[gameId].MAX_NUM_TRIALS){
        std::ostringstream oss;
        std::vector<std::string> secretKey = games[gameId].secretKey;

        currentPlayer->isPlaying = false;
        oss << "RTR ENT " << secretKey[0] << " " << secretKey[1] << " " << secretKey[2] << " " << secretKey[3];
        std::string message = oss.str();
        games[gameId].finalSate = 'F';
        sendto(fd, message.c_str(), 15, 0, (struct sockaddr *)&client_addr, client_len);
        closeGame(*currentPlayer, games[gameId]);
        return ;
    }

    if (args.first == 4){
        std::ostringstream oss;
        std::vector<std::string> secretKey = games[gameId].secretKey;
        currentPlayer->isPlaying = false;
        oss << "RTR OK " << numTrials << " " << args.first << " " << args.second ;
        std::string message = oss.str();
        sendto(fd, message.c_str(), 12, 0, (struct sockaddr *)&client_addr, client_len);
        games[gameId].score = calcScore(games[gameId]);
        games[gameId].finalSate = 'W';
        closeGame(*currentPlayer, games[gameId]);


        return ;
    }

    std::ostringstream oss;
    oss << "RTR OK " << numTrials << " " << args.first << " " << args.second ;

    games[gameId].numTrials++;

    std::string message = oss.str();

    sendto(fd, message.c_str(), 12, 0, (struct sockaddr *)&client_addr, client_len);
   
}



std::pair<int, int> tryGuess(int plid, std::vector<std::string> guesses, int gameId) {
    int numBlack = 0;
    int numWhite = 0;
    std::vector<int> trialState(4, 0);

    std::cout << "PLID: " << plid << std::endl;
    std::cout << "Game ID: " << gameId << std::endl;
    std::cout << "Secret Key: " << games[gameId].secretKey[0] << games[gameId].secretKey[1] << games[gameId].secretKey[2] << games[gameId].secretKey[3] << std::endl;
    
    
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

    std::cout << "Trial Guess:" << newTrial.guesses[0] << newTrial.guesses[1] << newTrial.guesses[2] << newTrial.guesses[3] <<newTrial.numBlack <<newTrial.numWhite << std::endl;


    games[gameId].trials.push_back(newTrial);


    return std::make_pair(numBlack, numWhite);
}
