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

//std::time_t parseDateTime(const std::string &dateTimeStr) {
//    struct std::tm tm = {};
//    std::istringstream ss(dateTimeStr);
//    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");  
//    if (ss.fail()) {
//        throw std::runtime_error("Failed to parse date-time: " + dateTimeStr);
//    }
//    return std::mktime(&tm);
//}


//std::string getLastLine(const std::string &filePath) {
//    std::ifstream file(filePath, std::ios::ate);  
//    if (!file.is_open()) {
//        throw std::runtime_error("Failed to open file: " + filePath);
//    }
//    std::string line;
//    if (file.seekg(-1, std::ios::end)) {
//        char ch;
//        while (file.tellg() > 0) {
//            file.seekg(-1, std::ios::cur);
//            file.get(ch);
//            if (ch == '\n' && file.tellg() > 1) {
//                break;
//            }
//            file.seekg(-1, std::ios::cur);
//        }
//    }
//
//    std::getline(file, line);
//    return line;
//}


//std::string getLastGameSummary(int plid) {
//    std::string folder = "server/GAMES/" + std::to_string(plid);
//    std::time_t currentTime = std::time(nullptr);
//    std::string closestFile;
//    double minDifference = std::numeric_limits<double>::max();
//
//    if (!fs::exists(folder)) {
//        std::cerr << "[ERROR] Folder " << folder << " does not exist." << std::endl;
//        return "";
//    }
//
//    for (const auto &entry : fs::directory_iterator(folder)) {
//        if (entry.is_regular_file()) {
//            try {
//                std::string lastLine = getLastLine(entry.path().string());
//                
//                std::time_t fileTime = parseDateTime(lastLine);
//
//                double diff = std::difftime(std::abs(currentTime - fileTime), 0);
//                
//                if (diff < minDifference) {
//                    minDifference = diff;
//                    closestFile = entry.path().filename().string();
//                }
//            } catch (const std::exception &e) {
//                std::cerr << "[ERROR] Failed to process file " << entry.path() << ": " << e.what() << std::endl;
//            }
//        }
//    }
//
//    if (!closestFile.empty()) {
//        std::cout << "[INFO] Closest file found: " << closestFile << std::endl;
//    } else {
//        std::cerr << "[ERROR] No valid files found in folder." << std::endl;
//    }
//
//
//
//    return folder + "/" + closestFile;
//}

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

        // Free each entry
        free(filelist[n_entries]);
    }

    // Free the file list
    free(filelist);

    std::cout << "FOUND: " << found << std::endl;
    return found;
}




//void sendFile(int client_fd, const std::string &filename,std::string status) {
//    std::ifstream file(filename, std::ios::binary);
//
//    if (!file) {
//        std::cerr << "[ERROR] Não foi possível abrir o arquivo: " << filename << std::endl;
//        const std::string error_response = "RST NOK\n";
//        write(client_fd, error_response.c_str(), error_response.size());
//        return;
//    }
//
//    file.seekg(0, std::ios::end);
//    std::size_t file_size = file.tellg();
//    file.seekg(0, std::ios::beg);
//    std::cerr << "[DEBUG] sendFile: Tamanho do arquivo: " << file_size << " bytes" << std::endl;
//
//    std::ostringstream header;
//    header << "RST "+ status + " " << "show_trials.txt" << " " << file_size << " ";
//    std::string header_str = header.str();
//    std::vector<char> buffer(header_str.begin(), header_str.end()); // Start with header
//
//    // Read the file into the same buffer
//    char file_buffer[BUFFER_SIZE];
//    while (file.read(file_buffer, sizeof(file_buffer)) || file.gcount() > 0) {
//        buffer.insert(buffer.end(), file_buffer, file_buffer + file.gcount());
//    }
//    buffer.push_back('\n\n');  
//    file.close();
//    std::cerr << "[DEBUG] File content read into buffer successfully.\n";
//
//    sendToPlayer(client_fd, buffer);
//}


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
        

        //sendFile(client_fd, active_game_file,"ACT");
        sendToPlayer(client_fd, header_str, trials_info);
        

    } else {

        std::cout << "AQUIIIIII: " << std::endl;

        int result = FindLastGame(plid_char, filename);
        std::cout << "PAADKASDHASJDJAKS: " << std::endl;
        std::cout << "RESULT: " << result << std::endl;

        if(result) {

    
            //std::string gameFile =  getLastGameSummary(plid);
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

