# **Mastermind Game Client**

This project is a client application for the Mastermind game. It communicates with a server using both UDP and TCP protocols to play the game, send commands, and receive responses. The client sends various commands such as `TRY`, `START`, `QUIT`, `DEBUG`, and `SCOREBOARD` to interact with the game and receive feedback.

## **Project Structure**

- **player**: The client-side application folder that communicates with the server using UDP and TCP protocols.
- **server**: The backend Game Server folder that handles game logic and client communication.
- **Makefile**: The build system for compiling both the player and the server applications.

### **Directory Layout**

#### **player**
Contains the client-side application that interacts with the game server using UDP and TCP communication.

- **udp**: Contains files responsible for handling UDP communication, which is used for real-time commands like `START`, `TRY`, `QUIT`, and `EXIT`.
    - `start/`: Folder containing files for the 'START' command.
        - `start.cpp`: Implements the 'START' command to initiate a new game session.
        - `start.hpp`: Header file for `start.cpp`, declaring the 'START' function.
    - `try/`: Folder containing files for the 'TRY' command.
        - `try.cpp`: Implements the 'TRY' command, sending the player's guess to the server.
        - `try.hpp`: Header file for `try.cpp`, declaring the 'TRY' function.
    - `quit/`: Folder containing files for the 'QUIT' command.
        - `quit.cpp`: Implements the 'QUIT' command to quit the game.
        - `quit.hpp`: Header file for `quit.cpp`, declaring the 'QUIT' function.
    - `exit/`: Folder containing files for the 'EXIT' command.
        - `exit.cpp`: Implements the 'EXIT' command to exit the game completely.
        - `exit.hpp`: Header file for `exit.cpp`, declaring the 'EXIT' function.
    - `udp.cpp`: Implements general UDP communication functions used throughout the `player` application.
    - `udp.hpp`: Header file for `udp.cpp`, declaring functions for UDP communication.
  
- **tcp**: Contains files responsible for handling TCP communication, which is used for receiving files and scoreboard data.
    - `scoreboard/`: Folder containing files to handle the 'SCOREBOARD' command.
        - `scoreboard.cpp`: Implements the 'SCOREBOARD' command to fetch the current scoreboard from the server.
        - `scoreboard.hpp`: Header file for `scoreboard.cpp`, declaring functions for scoreboard handling.
    - `showTrials/`: Folder containing files to handle the 'SHOWTRIALS' command.
        - `showTrials.cpp`: Implements the 'SHOWTRIALS' command to display trial guesses made by the player.
        - `showTrials.hpp`: Header file for `showTrials.cpp`, declaring functions for showing trial guesses.
    - `tcp.cpp`: Implements general TCP communication functions, such as sending and receiving data over TCP.
    - `tcp.hpp`: Header file for `tcp.cpp`, declaring functions for TCP communication.

- `player.cpp`: Contains the main logic for the player, including game state management and interactions with the server.
- `player.hpp`: Header file for `player.cpp`, declaring the player class and related functions.

#### **server**
Contains the backend Game Server that handles game logic and communicates with the client application.

- **udp**: Folder containing files for handling UDP communication on the server side.
    - `start/`: Folder containing files for the 'START' command on the server.
        - `start.cpp`: Implements the 'START' command on the server, starting a new game session.
        - `start.hpp`: Header file for the server-side 'START' functionality.
    - `try/`: Folder containing files for the 'TRY' command on the server.
        - `try.cpp`: Handles the 'TRY' command, processing guesses from the player and determining if they're correct.
        - `try.hpp`: Header file for `try.cpp`, declaring functions for processing 'TRY' commands.
    - `quit/`: Folder containing files for the 'QUIT' command on the server.
        - `quit.cpp`: Implements the 'QUIT' command on the server, handling player quit requests.
        - `quit.hpp`: Header file for `quit.cpp`, declaring functions for handling 'QUIT' commands.
    - `exit/`: Folder containing files for the 'EXIT' command on the server.
        - `exit.cpp`: Implements the 'EXIT' command on the server, closing the game session and cleaning up.
        - `exit.hpp`: Header file for `exit.cpp`, declaring functions for handling 'EXIT' commands.
    - `udp.cpp`: Implements general UDP communication functions used by the server.
    - `udp.hpp`: Header file for `udp.cpp`, declaring UDP communication functions on the server side.

- **tcp**: Folder for handling TCP communication on the server side.
    - `scoreboard/`: Folder containing files for the 'SCOREBOARD' command on the server.
        - `scoreboard.cpp`: Implements the 'SCOREBOARD' functionality on the server, sending the scoreboard data to the player.
        - `scoreboard.hpp`: Header file for `scoreboard.cpp`, declaring functions for scoreboard handling.
    - `showTrials/`: Folder containing files for the 'SHOWTRIALS' command on the server.
        - `showTrials.cpp`: Handles the server-side logic to display trial guesses made by the player.
        - `showTrials.hpp`: Header file for `showTrials.cpp`, declaring functions for managing trials data.
    - `tcp.cpp`: Implements general TCP communication functions for the server to handle requests from clients.
    - `tcp.hpp`: Header file for `tcp.cpp`, declaring server-side TCP communication functions.

- `GS.cpp`: Main server logic, responsible for managing the game state, processing client commands, and handling game flow.
- `GS.hpp`: Header file for `GS.cpp`, declaring the game server logic and the functions for handling various commands.

## **How to Run**

To run the project, you only need to navigate to the root directory (the one you cloned from GitHub).

1. **Build the Project**:
    - Open a terminal window and navigate to the root directory of the project.
    - Run the following command to clean and build the project:
      ```bash
      make
      ```

2. **Run the Game Server (GS)**:
    - In the same terminal, run the Game Server by executing:
      ```bash
      ./server
      ```

3. **Run the Player Application**:
    - Open another terminal window and run the Player application by executing:
      ```bash
      ./player1
      ```

4. **Game Interaction**:
    - After starting both the server and the player application, the player can interact with the game by sending commands such as `START`, `TRY`, `QUIT`, and `SCOREBOARD`.

## **Commands**

- **START**: Starts a new game session.
- **TRY**: Submits a guess to the server.
- **QUIT**: Quits the current game session.
- **EXIT**: Exits the game entirely.
- **SCOREBOARD**: Requests the current scoreboard from the server.

## **License**

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
