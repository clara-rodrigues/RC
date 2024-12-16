#include <iostream>
#include <vector>
#include <string>
#include <sstream> // Add this line
#include "try.hpp"    

#include "../UDP.hpp"


std::vector<int> checkNums(std::string numTrials,std::string numBlacks, std::string numWhites){
    int numTrialsInt, numBlacksInt, numWhitesInt; 

    try
    {   
        numTrialsInt = std::stoi(numTrials);
        numBlacksInt = std::stoi(numBlacks);
        numWhitesInt = std::stoi(numWhites);
    }
    catch(const std::exception& e)
    {
        std::cerr <<"Error: Invalid server response format (status)." << '\n';
    }

    return {numTrialsInt,numBlacksInt,numWhitesInt}; // Return the variables as a vector
    
}



int getResponseId(const std::string& command) {
    static std::unordered_map<std::string, int> commandMap = {
        {"OK", 1},
        {"ENT", 2},
        {"ETM", 2},
        {"INV", 3}, 
        {"DUP", 3},
        {"ERR", 3}, 
        {"NOK", 3},
    };

    auto it = commandMap.find(command);
    return (it != commandMap.end()) ? it->second : -1; // Return -1 for unknown commands
}



void execute_try(Player &player, const std::vector<std::string>& guesses, const std::string& ip, const std::string& port, int& numTrials) {
    
    std::string plid = player.plid;
    std::string msg = "TRY " + plid ;
   
    
    for (const auto& guess : guesses) {
        msg += " " + guess;
    }
    msg += " " + std::to_string(numTrials) + "\n";

    std::cout << "Sending 'try' command: " << msg << std::endl;

    std::string response;
    
    if (send_UDP(msg, response, ip, port) == 1) {
        std::cout << "Server Response: " << response << std::endl;

    } else {
        std::cerr << "Failed to send 'try' command1." << std::endl;
    }


    std::istringstream iss(response);
    std::string message,responseStatus,gameTrials,numBlacks,numWhites,guess1,guess2,guess3,guess4;
    std::vector<char> valid_colors = {'R', 'G', 'B', 'Y', 'O', 'P'};
    iss >> message >> responseStatus >> gameTrials >> numBlacks >> numWhites >> guess1 >> guess2 >> guess3 >> guess4; 
    
    if(message != "RTR"){
        std::cerr << "Error: Invalid server response format (status)." << std::endl;
        return;
    }

    int responseId = getResponseId(responseStatus);

    switch (responseId) {
        case 1:{
            int numBlacksInt = checkNums(gameTrials,numBlacks,numWhites)[1];
            if(numBlacksInt == 4){
                closeGame(player);
                std::cout << "WINNNN!" << std::endl;
            }else{
                numTrials++;    
            }
            
            break;
        }
        case 2:{
            char guess1_color = guess1[0];
            char guess2_color = guess2[0];
            char guess3_color = guess3[0];
            char guess4_color = guess4[0];

            if (std::find(valid_colors.begin(), valid_colors.end(),guess1_color) == valid_colors.end() || 
                std::find(valid_colors.begin(), valid_colors.end(), guess2_color) == valid_colors.end() || 
                std::find(valid_colors.begin(), valid_colors.end(), guess3_color) == valid_colors.end() || 
                std::find(valid_colors.begin(), valid_colors.end(), guess4_color) == valid_colors.end() ) {
                    std::cout << "AQUIIIII" << std::endl;
                    std::cerr << "Error: Invalid server response format (status)." << std::endl;
                    return;
            }

            closeGame(player);
            break;
        }
            
        case 3:{
            break;
        }
        default:{
            std::cerr << "Error: Invalid server response format (status)." << std::endl;
            return;
        }

    }

    

    //if (responseStatus == "OK") {
    //    if(numBlacksInt == 4){
    //        closeGame(player);
    //        std::cout << "WINNNN!" << std::endl;
    //    }else{
    //        numTrials++;    
    //    }
    //}else if (responseStatus == "ENT" || responseStatus == "ETM") {
//
//
    //    
    //}
}
