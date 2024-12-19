#include <iostream>
#include <string>
#include <sys/socket.h> 

#include "../../GS.hpp"

#include "../TCP.hpp"


#include <dirent.h>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <ctime>
#include <limits>

namespace fs = std::filesystem;


std::string readFile(int client_fd, const std::string &filename) {
    std::ifstream file(filename, std::ios::binary);
    std::vector<char> buffer;

    if (!file) {
        std::cerr << "[ERROR] Não foi possível abrir o arquivo: " << filename << std::endl;
        const std::string error_response = "RST NOK\n";
        write(client_fd, error_response.c_str(), error_response.size());
        return " ";
    }


    char file_buffer[BUFFER_SIZE];
    while (file.read(file_buffer, sizeof(file_buffer)) || file.gcount() > 0) {
        buffer.insert(buffer.end(), file_buffer, file_buffer + file.gcount());
    }
    file.close();

    return std::string(buffer.begin(), buffer.end());
}


bool FindLastGame(const std::string &PLID, std::string &fname) {
    std::string dirname = "server/GAMES/" + PLID + "/";
    std::cout << "DIRNAME: " << dirname << std::endl;

    struct dirent **filelist;
    int n_entries = scandir(dirname.c_str(), &filelist, nullptr, alphasort);
    bool found = false;

    if (n_entries <= 0) {
        std::cout << "N_ENTRIES: " << n_entries << std::endl;
        return false;
    }

    while (n_entries--) {
        std::string filename = filelist[n_entries]->d_name;
        std::cout << "N_ENTRIES: " << filename << std::endl;

        if (filename[0] != '.' && !found) {
            fname = dirname + filename;
            found = true;
            std::cout << "FNAME: " << fname << std::endl;
        }

        free(filelist[n_entries]);
    }

    free(filelist);

    std::cout << "FOUND: " << found << std::endl;
    return found;
}


void handleShowTrials(int client_fd, std::istringstream &commandStream, std::string client_ip, int client_port) {
    int plid;
        
    try{
        plid = validPLID(commandStream);
        checkExtraInput(commandStream);
        
        if (verbose) {
            std::cout << "[Verbose] [IP: " << client_ip << "] [PORT: " << client_port 
                    << "] Show Trials for PLID: " << plid << std::endl;
        }    
        }catch (const std::invalid_argument& e){
                std::cerr << "[ERROR] " << e.what() << std::endl;
        const std::string error_response = "RST ERR\n";
        write(client_fd, error_response.c_str(), error_response.size());
        return;
    }

    std::string plid_str = std::to_string(plid);
    char plid_char[7];
    std::strncpy(plid_char, plid_str.c_str(), sizeof(plid_char) - 1);
    plid_char[sizeof(plid_char) - 1] = '\0';
    std::string filename;


    Player *player = findPlayerById(plid);
    if (!player) {
        std::cerr << "[ERROR] Jogador com PLID " << plid << " não encontrado." << std::endl;
        const std::string error_response = "RST NOK\n";
        write(client_fd, error_response.c_str(), error_response.size());
        return;
    }

    time_t currentTime = time(0);
    auto &game = games[player->gameId];

    if (currentTime - game.startTime > game.maxPlaytime) {
        game.finalSate = 'T';
        closeGame(*player, game);
    }

    if (player->isPlaying) {
        std::string gameFile = "server/GAMES/GAME_" + std::to_string(plid) + ".txt";
        std::string trials_info = player->getActiveGameSummary(gameFile);
        std::ostringstream header;

        header << "RST ACT show_trials.txt" << " " << trials_info.length() << " ";

        std::cout << "Trials Info: " << trials_info << std::endl;
        std::cout << "File size: " << trials_info << std::endl;

        std::string header_str = header.str();
        
        sendToPlayer(client_fd, header_str, trials_info);
        

    } else {

        if(FindLastGame(plid_char, filename)){

            std::string trials_info = readFile(client_fd, filename);
            std::ostringstream header;

            std::cout << "FILENAME: " << filename << std::endl;

            header << "RST FIN show_trials.txt " << trials_info.length() << " ";

            std::cout << "Trials Info: " << trials_info << std::endl;
            std::cout << "File size: " << trials_info << std::endl;

            std::string header_str = header.str();
        
            sendToPlayer(client_fd, header_str, trials_info);

        } else {
            const std::string no_games_response = "RST NOK\n";
            write(client_fd, no_games_response.c_str(), no_games_response.size());
            
        }
    return;
    }
}

