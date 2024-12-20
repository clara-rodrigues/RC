 
#include "try.hpp"    

#include "../UDP.hpp"

// Converts string representations of numbers (numTrials, numBlacks, numWhites) to integers.
// Returns a vector containing the converted integers.
// If conversion fails, an error message is displayed.
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

    return {numTrialsInt,numBlacksInt,numWhitesInt};   
}


// Maps server response status strings to numeric IDs for easier processing.
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
    return (it != commandMap.end()) ? it->second : -1; 
}


// Executes the "try" command by sending the player's guesses to the server and handling the response.
// Parameters:
// - player: The Player object containing game state information.
// - guesses: A vector of guesses (color codes) to send to the server.
// - ip, port: The server's IP address and port number.
// - numTrials: The current trial number, which may be incremented based on the server's response.
void execute_try(Player &player, const std::vector<std::string>& guesses, const std::string& ip, const std::string& port, int& numTrials) {
    std::string plid = player.plid;
    std::string msg = "TRY " + plid ;
   
    for (const auto& guess : guesses) {
        msg += " " + guess;
    }
    msg += " " + std::to_string(numTrials) + "\n";

    std::string response;
    
    if (send_UDP(msg, response, ip, port) == 1) {
        std::cout << "Server Response: " << response << std::endl;

    } else {
        std::cerr << "Failed to send 'try' command." << std::endl;
    }

    std::istringstream iss(response);
    std::string message,responseStatus,gameTrials,numBlacks,numWhites,guess1,guess2,guess3,guess4;
    std::vector<char> valid_colors = {'R', 'G', 'B', 'Y', 'O', 'P'};
    iss >> message >> responseStatus ;
    
    if(message != "RTR"){
        std::cerr << "Error: Invalid server response format (status)." << std::endl;
        return;
    }

    int responseId = getResponseId(responseStatus);

    switch (responseId) {
        case 1:{
            iss >> gameTrials >> numBlacks >> numWhites;
            int numBlacksInt = checkNums(gameTrials,numBlacks,numWhites)[1];
            if(numBlacksInt == 4){
                closeGame(player);
                std::cout << "WIN!" << std::endl;
            }else{
                numTrials++;    
            }
            
            break;
        }
        case 2:{
            iss >> guess1 >> guess2 >> guess3 >> guess4;
            char guess1_color = guess1[0];
            char guess2_color = guess2[0];
            char guess3_color = guess3[0];
            char guess4_color = guess4[0];

            if (std::find(valid_colors.begin(), valid_colors.end(),guess1_color) == valid_colors.end() || 
                std::find(valid_colors.begin(), valid_colors.end(), guess2_color) == valid_colors.end() || 
                std::find(valid_colors.begin(), valid_colors.end(), guess3_color) == valid_colors.end() || 
                std::find(valid_colors.begin(), valid_colors.end(), guess4_color) == valid_colors.end() ) {
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

}
