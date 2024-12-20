#include <iostream>
#include <cstring>
#include <cstdlib>
#include "UDP/UDP.hpp"
#include "TCP/TCP.hpp"
#include "GS.hpp"
#include <sys/select.h>
#include <unistd.h>
#include <algorithm>  
#include <sys/socket.h>
#include <netinet/in.h>
#include <csignal>
#include <fstream>
#include <dirent.h>

// Global variables used for managing the players, colors, and games
std::vector<Player> players;  // List of players in the game
std::vector<std::string> colors = {"R", "G", "B", "Y", "O", "P"};  // List of valid colors for guesses
std::vector<Game> games;  // List of ongoing games
int verbose = 0;  // Verbose flag for detailed output
int udp_fd;  // UDP socket file descriptor
int tcp_fd;  // TCP socket file descriptor


namespace fs = std::filesystem;  // Alias for filesystem namespace

// Function to create a directory for a player and store game data
void createPlayerDir(int plid, Game &game) {
    struct tm *timeinfo;
    const std::string oldFile = "server/GAMES/GAME_" + std::to_string(plid) + ".txt";
    const std::string folder = "server/GAMES/" + std::to_string(plid);

    if (!fs::exists(folder)) {
        fs::create_directories(folder);
    }

    std::string newFileName;

    if (fs::exists(oldFile)) {
        timeinfo = gmtime(&game.startTime);
        newFileName = folder + "/" + 
                      std::to_string(timeinfo->tm_year + 1900) + 
                      std::to_string(timeinfo->tm_mon + 1) + 
                      std::to_string(timeinfo->tm_mday) + 
                      "_" + 
                      std::to_string(timeinfo->tm_hour) + 
                      std::to_string(timeinfo->tm_min) + 
                      std::to_string(timeinfo->tm_sec) + 
                      "_" +
                      game.finalSate + 
                      ".txt";

        std::cout << "Renaming " << oldFile << " to " << newFileName << std::endl;

        // Rename the old file to the new file
        fs::rename(oldFile, newFileName);
    } else {
        std::cerr << "File does not exist: " << oldFile << std::endl;
        return;
    }

    // Open the new file in append mode to avoid truncation
    std::ofstream file(newFileName, std::ios::app);
    if (file.is_open()) {
        file << timeinfo->tm_year + 1900 << "-"
             << timeinfo->tm_mon + 1 << "-"
             << timeinfo->tm_mday << " "
             << timeinfo->tm_hour << ":"
             << timeinfo->tm_min << ":"
             << timeinfo->tm_sec << std::endl;
    } else {
        std::cerr << "Error opening the file " << newFileName << std::endl;
    }
}

// Function to close a game and store its data
void closeGame(Player& player, Game& game) {
    player.isPlaying = false;
    createPlayerDir(player.plid, game);
}

// Function to clear all files in a directory
void clearGamesDir(std::string directory) {

    try {
        if (fs::exists(directory) && fs::is_directory(directory)) {
            for (const auto& entry : fs::directory_iterator(directory)) {
                fs::remove_all(entry.path());
            }
            std::cout << "All files and folders in " << directory << " have been removed.\n";
        } else {
            std::cerr << "The directory " << directory << " does not exist or is not a valid directory.\n";
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

// Signal handler for cleaning up resources when the server is stopped
void signalHandler(int signum) {
    clearGamesDir("server/GAMES");
    clearGamesDir("server/SCORES");
    close(udp_fd);
    close(tcp_fd);
    std::exit(signum);
}

// Function to validate the player's ID (PLID)
int validPLID(std::istream& input){
    std::string plid;
    int digitPLID;

    if (!(input >> plid) || plid.size() != 6) {
        throw std::invalid_argument("Invalid or missing PLID.");}

    for (char c : plid) {
        if (!std::isdigit(c)) {
            throw std::invalid_argument("Invalid or missing PLID.");
        }
    }
    digitPLID = std::stoi(plid);

    return digitPLID;
}

// Function to validate the maximum playtime
int validMaxPlayTime(std::istream& input){
    int maxPlaytime;

    if (!(input >> maxPlaytime) || maxPlaytime <= 0 || maxPlaytime > 600) {
        throw std::invalid_argument("Invalid max_playtime. Must be between 1 and 600 seconds.");
    }
    return maxPlaytime;
}

// Function to validate the player's guesses
std::vector<std::string> validGuess(std::istream& input) {
    std::vector<std::string> rawGuesses;
    std::string guess;

    for (size_t i = 0; i < 4; ++i) {
        if (!(input >> guess)) {
            throw std::invalid_argument("Invalid input. Must provide exactly 4 guesses.");
        }
        rawGuesses.push_back(guess);
    }

    for (const auto& g : rawGuesses) {
        if (std::find(colors.begin(), colors.end(), g) == colors.end()) {
            throw std::invalid_argument("Invalid guess. One or more guesses are not valid.");
        }
    }

    return rawGuesses;
}

// Function to check if there is any extra input after valid guesses
void checkExtraInput(std::istream& input){
    std::string extra;
    if (input >> extra) {
        throw std::invalid_argument("Extra input detected.");
    }
}

// Function to check the number of trials
int checkNumTrials(std::istream& input){
    int trials;
    if (!(input >> trials) ) {
        throw std::invalid_argument("Invalid trial number.");
    }

    return trials;

}

// Function to find a player by their PLID
Player* findPlayerById(int plid) {
    for (auto& player : players) { 
        if (player.plid == plid) { 
            return &player; 
        }
    }
    return nullptr;
}

// Function to get a summary of an active game for a player
std::string Player::getActiveGameSummary(std::string gameFile) const {
    const Game& activeGame = games[gameId];
    time_t currentTime = time(0);
    char buffer[BUFFER_SIZE] = {0};  

    std::ifstream file(gameFile);
    if (!file.is_open()) {
        std::cerr << "Failed to open file " << gameFile << std::endl;
        return "";
    }

    std::ostringstream trialsStream;  
    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string token;
        
        if (ss >> token) {
            if (token == "T:") {
                trialsStream << line << '\n';
            }
        }
    }
    file.close();

    int remainingTime = activeGame.maxPlaytime - (currentTime - activeGame.startTime);
    if (remainingTime < 0) {
        remainingTime = 0;
    }

    trialsStream << "Time remaining: " << remainingTime << " seconds\n";

    std::string trialsContent = trialsStream.str();
    size_t copySize = std::min(trialsContent.size(), static_cast<size_t>(BUFFER_SIZE - 1));
    std::copy(trialsContent.begin(), trialsContent.begin() + copySize, buffer);
    buffer[copySize] = ('\0'); 

    std::cout << buffer << std::endl;

    return std::string(buffer);
}

// Main server loop function
void serverLoop(int udp_fd, int tcp_fd) {
    fd_set inputs, testfds;
    struct timeval timeout;
    int out_fds;
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    FD_ZERO(&inputs);  
    FD_SET(0, &inputs);  
    FD_SET(udp_fd, &inputs);  
    FD_SET(tcp_fd, &inputs); 

    while (1) {
        testfds = inputs;  
        timeout.tv_sec = 10; 
        timeout.tv_usec = 0;

        out_fds = select(FD_SETSIZE, &testfds, NULL, NULL, &timeout);

        if (out_fds == -1) {
            perror("select error");
            exit(1);
        }

        if (out_fds == 0) {
            printf("Timeout: Nenhuma atividade detectada.\n");
            continue;
        }

        if (FD_ISSET(0, &testfds)) {
            fgets(buffer, sizeof(buffer), stdin);
            printf("Input recebido no teclado: %s\n", buffer);
        }

        if (FD_ISSET(udp_fd, &testfds)) {
            int ret = recvfrom(udp_fd, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_addr, &client_len);
            if (ret >= 0) {
                buffer[ret] = '\0';  
                printf("Mensagem recebida no socket UDP: %s\n", buffer);
                handleUserMessage(udp_fd, client_addr, client_len, buffer, ret);
            }
        }

        if (FD_ISSET(tcp_fd, &testfds)) {
            int client_fd = accept(tcp_fd, (struct sockaddr *)&client_addr, &client_len);
            if (client_fd == -1) {
                perror("Erro ao aceitar conexão TCP");
                continue;
            }

            printf("Nova conexão TCP aceita.\n");

            pid_t pid = fork();
            if (pid == -1) {
                perror("Erro ao criar processo filho");
                close(client_fd);
                continue;
            }

            if (pid == 0) {  
                close(tcp_fd); 
                handlePlayerRequest(client_fd, client_addr);
                close(client_fd);
                exit(0);
            } else {  
                close(client_fd);  
            }
        }
    }
}

// Main function to start the server
int main(int argc, char* argv[]) {
    std::string port = "58068"; // Default port


    for (int i = 1; i < argc; i++) {
        std::cout << argv[i];
        if (std::string(argv[i]) == "-p") {
            port = argv[i + 1];
        } else if (std::string(argv[i]) == "-v") {
            verbose = 1;
            std::cout << " VERBOSE";
        }
    }

    std::signal(SIGINT, signalHandler);
    udp_fd = startUDP(port);
    tcp_fd = startTCPServer(port);  
    
    serverLoop(udp_fd,tcp_fd);
    

}
