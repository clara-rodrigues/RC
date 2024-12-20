# Mastermind Game Client

This project is a **Mastermind game client** that communicates with a backend **Game Server** (GS) using both **UDP** and **TCP** protocols. The client sends various commands, including `TRY`, `START`, `QUIT`, `DEBUG`, `EXIT`, `SHOW_TRIALS`, and `SCOREBOARD`, to interact with the game server and receive feedback.

---

## Project Structure

The project is divided into two main components: **client** (the player-side application) and **server** (the backend game logic).

### **client**
The player-side application that communicates with the server via UDP and TCP protocols.

- **UDP**: Contains files responsible for handling **UDP** communication, used for real-time commands like `START`, `TRY`, `QUIT`, `EXIT`, and `DEBUG`.
    - `start/`: Implements the 'START' command to initiate a new game session.
    - `try/`: Implements the 'TRY' command, sending the player's guess to the server.
    - `quit/`: Implements the 'QUIT' command to quit the current game session.
    - `exit/`: Implements the 'EXIT' command to exit the game entirely.
    - `debug/`: Implements the 'DEBUG' command to create a game in debug mode.
    - `UDP.cpp`: Implements general **UDP** communication functions used by the client.
    - `UDP.hpp`: Header file for **UDP** communication functions.

- **TCP**: Contains files responsible for handling **TCP** communication, used for receiving files, scoreboard data, and trial guesses.
    - `scoreboard/`: Handles the 'SCOREBOARD' command to fetch the current scoreboard from the server.
    - `showTrials/`: Handles the 'SHOW_TRIALS' command to display trial guesses made by the player.
    - `TCP.cpp`: Implements general **TCP** communication functions for the client.
    - `TCP.hpp`: Header file for **TCP** communication functions.

- `player.cpp`: Main logic for the player, including game state management and interactions with the server.
- `player.hpp`: Header file for the **player** class and related functions.

---

### **server**
Contains the backend **Game Server** that handles game logic and communication with the client application.

- **UDP**: Contains files for handling **UDP** communication on the server side.
    - `start/`: Implements the 'START' command on the server, starting a new game session.
    - `try/`: Handles the 'TRY' command, processing guesses and determining correctness.
    - `quit/`: Implements the 'QUIT' command, handling player quit requests.
    - `exit/`: Implements the 'EXIT' command to close the game session.
    - `debug/`: Implements the 'DEBUG' command to create and manage a game in debug mode.
    - `UDP.cpp`: Implements general **UDP** communication functions used by the server.
    - `UDP.hpp`: Header file for **UDP** communication functions on the server.

- **TCP**: Contains files for handling **TCP** communication on the server side.
    - `scoreboard/`: Implements the 'SCOREBOARD' functionality, sending the scoreboard data to the client.
    - `showTrials/`: Handles the server-side logic for displaying trial guesses made by the player.
    - `TCP.cpp`: Implements general **TCP** communication functions for the server.
    - `TCP.hpp`: Header file for **TCP** communication functions on the server.

- `GS.cpp`: Main server logic, responsible for managing the game state and processing client commands.
- `GS.hpp`: Header file for `GS.cpp`, declaring server-side game logic.

---

## How to Run

To run the project, follow these steps:

1. **Build the Project**:
    - Open a terminal and navigate to the root directory of the project.
    - Run the following command to clean and build the project:
      ```bash
      make
      ```

2. **Run the Game Server (GS)**:
    - In the same terminal, run the **Game Server (GS)** by executing:
      ```bash
      ./GS [-p GSport] [-v]
      ```
      - **`GSport`** (Optional): The port number where the Game Server accepts both **UDP** and **TCP** requests. If omitted, it defaults to **58000 + GN**, where **GN** is the number of the group.
      - **`-v`** (Optional): Enables **verbose mode**, where the server outputs a short description of the received requests, including **PLID**, request type, and the originating **IP and port**.

3. **Run the Player Application**:
    - Open another terminal window and run the **Player application** by executing:
      ```bash
      ./player
      ```

---

## Commands

The following commands can be used by the client to interact with the game server:

- **`START`**: Starts a new game session.
- **`TRY`**: Submits a guess to the server.
- **`QUIT`**: Quits the current game session.
- **`EXIT`**: Exits the game entirely.
- **`DEBUG`**: Creates a game in Debug mode.
- **`SCOREBOARD`**: Requests the current scoreboard from the server.
- **`SHOW_TRIALS`**: Displays trial guesses made by the player.

---

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
