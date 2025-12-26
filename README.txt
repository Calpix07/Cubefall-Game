# Cubefall

Cubefall is an arcade-style endless survival game developed using the C programming language and the Raylib library.

## 🎮 About the Game
You control an explorer escaping from a crumbling ancient temple. Your goal is to achieve the highest score by dodging columns and stones falling from above at random speeds and positions.

* **Increasing Difficulty:** The speed of falling blocks increases as the game progresses.
* **High Score System:** The top 5 scores are recorded and saved.
* **Physics-Based Gameplay:** Collisions between the character and blocks are calculated using custom algorithms.

## 🛠 Technical Details
This project was developed as a freshman Computer Engineering project and demonstrates the following technical concepts:

* **Language & Library:** C, Raylib.
* **Memory Management:** Game and player data are passed to functions via **pointers** to ensure memory efficiency.
* **Data Persistence (File I/O):** The scoreboard is stored in **Binary** format in the `rekor.dat` file.
* **Algorithms:** The **Bubble Sort** algorithm is utilized to sort the high scores.

## 🚀 Installation & Execution

To compile the project on your local machine, follow these steps:

1.  Clone or download this repository.
2.  Open the `Cubefall Game.dev` file using **Embarcadero Dev-C++ 6.3**.
3.  Ensure the compiler is set to **TDM-GCC 64-bit Release** mode.
4.  Verify that **Raylib** library linker parameters are correctly configured in project settings.
5.  Press **F11** to compile and run.

*Note: You can download the ready-to-play version from the "Releases" section.*