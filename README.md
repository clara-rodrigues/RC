# **Mastermind Game Client**

This project is a client application for the Mastermind game. It communicates with a server using both UDP and TCP protocols to play the game, send commands, and receive responses. The client sends various commands such as `TRY`, `START`, `QUIT`, `DEBUG`, and `SCOREBOARD` to interact with the game and receive feedback.

## **Overview**

The project consists of two main components: the **Game Server (GS)** and the **Player Application (Player)**. The development of these components is intended to allow multiple instances of the Player application to connect to and interact with a single Game Server, which will be running on a machine with a known IP address and ports.

- **Game Server (GS)**: Hosts the game, processes commands from multiple Player instances, and maintains the state of the game.
- **Player Application (Player)**: A client application that communicates with the Game Server, sending commands like `START`, `TRY`, and `QUIT` to interact with the game.

Both the GS and Player Application will operate simultaneously on different machines connected via the Internet, allowing remote play and game management.

## **Features**

- **UDP Communication**: Used for real-time commands like `TRY` and `START`.
- **TCP Communication**: Used for file transfer and receiving data like the scoreboard.
- **Game Interaction**: The client can start the game, make guesses, quit the game, and view the scoreboard.
- **Server Response Handling**: The client processes responses from the server and performs actions based on the response status.

## **Requirements**

- C++11 or higher
- A C++ compiler (e.g., g++, clang++)
- A server that supports the Mastermind game protocol (usually a custom server)

## **Project Structure**

```plaintext
.
├── player
│   ├── udp
│   │   ├── start
│   │   │   ├── start.cpp        # Implements the 'START' command to start a new game session.
│   │   │   └── start.hpp        # Header file for 'start.cpp', declaring the 'START' function.
│   │   ├── try                  # Handles the 'TRY' command to send guesses to the server.
│   │   ├── quit                 # Implements the 'QUIT' command to quit the game.
│   │   ├── exit                 # Implements the 'EXIT' command to exit the game.
│   │   ├── udp.cpp              # General UDP communication functions, such as sending/receiving data.
│   │   └── udp.hpp              # Header file for 'udp.cpp', declaring functions for UDP communication.
│   ├── tcp
│   │   ├── scoreboard
│   │   │   ├── scoreboard.cpp   # Handles the 'SCOREBOARD' command to request scoreboard data.
│   │   │   └── scoreboard.hpp   # Header file for 'scoreboard.cpp', declaring functions for scoreboard handling.
│   │   ├── showTrials           # Handles displaying the player's trials and guesses.
│   │   ├── tcp.cpp              # Implements TCP communication functions, such as sending/receiving data.
│   │   └── tcp.hpp              # Header file for 'tcp.cpp', declaring functions for TCP communication.
│   ├── player.cpp               # Main player logic, managing player data, game state, and interactions with the server.
│   └── player.hpp               # Header file for 'player.cpp', declaring functions and player data structures.
└── server
    ├── udp
    │   ├── start
    │   │   ├── start.cpp        # Implements the 'START' command on the server, starting a new game session.
    │   │   └── start.hpp        # Header file for the server-side 'start' functionality.
    │   ├── try                  # Implements the 'TRY' command on the server, processing guesses from players.
    │   ├── quit                 # Implements the 'QUIT' command on the server, processing player quit requests.
    │   ├── exit                 # Implements the 'EXIT' command on the server, handling game exits.
    │   ├── udp.cpp              # General UDP communication functions for the server.
    │   └── udp.hpp              # Header file for the server-side 'udp.cpp', declaring UDP functions.
    ├── tcp
    │   ├── scoreboard
    │   │   ├── scoreboard.cpp   # Handles server-side 'SCOREBOARD' functionality, sending data to players.
    │   │   └── scoreboard.hpp   # Header file for 'scoreboard.cpp', declaring functions for scoreboard handling.
    │   ├── showTrials           # Implements the server-side logic to show trials and guesses.
    │   ├── tcp.cpp              # Implements TCP communication functions for handling requests from clients.
    │   └── tcp.hpp              # Header file for 'tcp.cpp', declaring functions for server-side TCP communication.
    ├── GS.cpp                    # Main server logic, managing game state, processing commands from clients, and handling game flow.
    └── GS.hpp                    # Header file for 'GS.cpp', declaring game server logic and commands.
