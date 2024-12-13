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

std::vector<Player> players;
std::vector<std::string> colors = {"R", "G", "B", "Y", "O", "P"};
std::vector<Game> games;
int verbose = 0;

namespace fs = std::filesystem;

void createPlayerDir(int plid, Game &game) {
    struct tm *timeinfo;
    const std::string oldFile = "server/GAMES/GAME_" + std::to_string(plid) + ".txt";
    const std::string folder = "server/GAMES/" + std::to_string(plid);

    // Create the folder if it doesn't exist
    if (!fs::exists(folder)) {
        fs::create_directories(folder);
    }

    std::string newFileName;

    // If the old file exists, rename it
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
        std::cerr << "Old file does not exist: " << oldFile << std::endl;
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


void closeGame(Player& player, Game& game) {
    player.isPlaying = false;
    createPlayerDir(player.plid, game);
}

void clearGamesDir() {
    const std::string directory = "server/GAMES";

    try {
        if (fs::exists(directory) && fs::is_directory(directory)) {
            for (const auto& entry : fs::directory_iterator(directory)) {
                fs::remove_all(entry.path());
                std::cout << "Removed: " << entry.path() << std::endl;
            }
            std::cout << "All files and folders in " << directory << " have been removed.\n";
        } else {
            std::cerr << "The directory " << directory << " does not exist or is not a valid directory.\n";
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

void signalHandler(int signum) {
    std::cout << "\nSinal (" << signum << ") recebido. Limpando a diretoria GAMES...\n";
    clearGamesDir();
    std::exit(signum);
}


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


int validMaxPlayTime(std::istream& input){
    int maxPlaytime;

    if (!(input >> maxPlaytime) || maxPlaytime <= 0 || maxPlaytime > 600) {
        throw std::invalid_argument("Invalid max_playtime. Must be between 1 and 600 seconds.");
    }
    return maxPlaytime;
}

std::vector<std::string> validGuess(std::istream& input){
    std::string guess;
    std::vector<std::string> guesses;


    for (size_t i = 0; i < 4; i++){
        input >> guess;
        if (std::find(colors.begin(), colors.end(), guess) == colors.end()) {
             throw std::invalid_argument("Invalid guess. One or more guesses are not valid.");
        }
        guesses.push_back(guess);
    }
    return guesses;
}


void checkExtraInput(std::istream& input){
    std::string extra;
    if (input >> extra) {
        throw std::invalid_argument("Extra input detected.");
    }
}

int checkNumTrials(std::istream& input){
    int trials;
    if (!(input >> trials) ) {
        throw std::invalid_argument("Invalid trial number.");
    }

    return trials;

}


Player* findPlayerById(int plid) {
    for (auto& player : players) { 
        if (player.plid == plid) { 
            return &player; 
        }
    }
    return nullptr;
}

bool Player::hasFinishedGames() const {
    for (const Game& game : games) {
        if (game.plid == plid && !game.trials.empty() && !isPlaying) {
            return true;
        }
    }
    return false;
}

int calcScore(const Game& game) {
    int score = 0;
    time_t currentTime = time(0);
    int remainingTime = game.maxPlaytime - (currentTime - game.startTime);
    int numTrials = game.trials.size();
    int remainingTrials = game.MAX_NUM_TRIALS - numTrials;
    score = remainingTime + remainingTrials;
    std::cout << "Score: " << score << std::endl;
    return score;
}

std::string Player::getActiveGameSummary() const {
    const Game& activeGame = games[gameId]; 
    std::string filename = "active_game_" + std::to_string(plid) + ".txt";

    std::ofstream file(filename);
    if (file.is_open()) {
        for (const Trial& trial : activeGame.trials) {
            for (const std::string& guess : trial.guesses) {
                file << guess << " ";
            }
            file << trial.numBlack << " " << trial.numWhite << "\n";
        }

        time_t currentTime = time(0);
        int remainingTime = activeGame.maxPlaytime - (currentTime - activeGame.startTime);
        file << "Time remaining: " << remainingTime << " seconds\n";

        file.close();
    }
    return filename;
}

std::string Player::getLastFinishedGameSummary() const {
    std::string filename = "finished_game_" + std::to_string(plid) + ".txt";

    std::ofstream file(filename);
    if (file.is_open()) {
        for (const Game& game : games) {
            if (game.plid == plid && !game.trials.empty() && !isPlaying) {
                for (const Trial& trial : game.trials) {
                    for (const std::string& guess : trial.guesses) {
                        file << guess << " ";
                    }
                    file << trial.numBlack << " " << trial.numWhite << "\n";
                }
                file.close();
                break;
            }
        }
    }
    return filename;
}


void serverLoop(int udp_fd, int tcp_fd) {
    fd_set inputs, testfds;
    struct timeval timeout;
    int out_fds;
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    FD_ZERO(&inputs);  // Limpa os descritores
    FD_SET(0, &inputs);  // A entrada padrão (teclado)
    FD_SET(udp_fd, &inputs);  // O socket UDP
    FD_SET(tcp_fd, &inputs);  // O socket TCP

    while (1) {
        testfds = inputs;  // Recarrega a máscara de entrada
        timeout.tv_sec = 10;  // Timeout de 10 segundos
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

        // Se houver dados no teclado
        if (FD_ISSET(0, &testfds)) {
            fgets(buffer, sizeof(buffer), stdin);
            printf("Input recebido no teclado: %s\n", buffer);
        }

        // Se houver dados no socket UDP
        if (FD_ISSET(udp_fd, &testfds)) {
            int ret = recvfrom(udp_fd, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_addr, &client_len);
            if (ret >= 0) {
                buffer[ret] = '\0';  
                printf("Mensagem recebida no socket UDP: %s\n", buffer);
                handleUserMessage(udp_fd, client_addr, client_len, buffer, ret);
            }
        }

        // Se houver dados no socket TCP
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

            if (pid == 0) {  // Processo filho
                close(tcp_fd);  // O filho não precisa do socket de escuta
                handlePlayerRequest(client_fd);
                close(client_fd);
                exit(0);
            } else {  // Processo pai
                close(client_fd);  // O pai não precisa manter a conexão com o cliente
            }
        }
    }
}


int main(int argc, char* argv[]) {
    int tcp_fd,udp_fd;
    std::string port = "58068";


    for (int i = 1; i < argc; i++) {
        if (std::string(argv[i]) == "-p") {
            port = argv[i + 1];
        } else if (std::string(argv[i]) == "-v") {
            verbose = 1;
        }
    }

    std::signal(SIGINT, signalHandler);
    std::cout << "Iniciando servidor UDP..." << std::endl;
    udp_fd = startUDP(port);
    std::cout << "Iniciando servidor TCP..." << std::endl;
    tcp_fd = startTCPServer(port);  
    
    serverLoop(udp_fd,tcp_fd);
    

}
