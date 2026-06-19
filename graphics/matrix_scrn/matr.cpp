//| Item	     | Main Program Documentation Notes                            |
//|--------------|-------------------------------------------------------------|
//| Filename     | matr.cp                                                     |
//| EntryPoint   | main                                                        |
//| Purpose      | display Matrix rain effect in terminal                      |
//| Inputs       | none                                                        |
//| Outputs      | none                                                        |
//| Dependencies | ncurses library                                             |
//| By Name,Date | T.Sciple, 06/18/2026                                        |
//  compile w linux:
//  g++ matr.cpp -o matr -lncurses

#include <ncurses.h>        // library to handle console output and control the display of characters.
#include <cstdlib>
#include <ctime>
#include <vector>
#include <unistd.h>
#include <locale.h>
#include <iostream>


// Function prototypes
void initializeStreams();   // initializeStreams` function sets random starting positions for each rain stream.
void updateStreams();       // updateStreams` function moves the streams down the console.
void drawStreams();         // drawStreams` function clears the console and redraws the streams at their new positions. 
wchar_t getrandomkana();       // getrandomkana` function generates random half-width kana characters.


// Struct to track each independent Matrix rain stream
struct MatrixStream {
    int x;             // Fixed X coordinate for this stream
    int head_y;        // Current vertical position of the leading character
    int speed;         // Frames to wait before moving down (lower = faster)
    int speed_counter; // Counter to track speed delays
    int length;        // Length of the stream's trail
};

// Global vector declaration, left blank for dynamic resizing based on terminal width
std::vector<MatrixStream> streams; 


int main() {
    setlocale(LC_ALL, "");
    srand(time(0)); 
    initscr();     
    noecho();      
    curs_set(0);   

    std::cout << "Press 'q' to exit the Matrix rain effect." << std::endl;
    // pause for a moment to let the user read the message before clearing the screen
    usleep(1000000); // 1 second
    
    // Setup Ncurses colors
    // These are to display characters representing the Blond, Brunette, and Redhead (Not really)
    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK); // Standard Matrix Green
    init_pair(2, COLOR_WHITE, COLOR_BLACK); // White for the leading head

    initializeStreams(); 

    nodelay(stdscr, TRUE); // Make getch non-blocking
    int ch;

    while (true) {
        ch = getch();
        if (ch == 'q' || ch == 'Q') {
            break; // Exit the loop if 'q' or 'Q' is pressed
        }

        updateStreams();    // Update the position/speed of each stream
        drawStreams();      // Draw the streams at their new positions  
        usleep(40000);      // Global tick rate (40ms) for smoother rendering
    }

    endwin(); 
    return 0;
}


void initializeStreams() {
    // Resize the vector dynamically based on the terminal width
    int num_streams = COLS / 0.75; // Adjust divisor to change density
    streams.resize(num_streams);

    for (int i = 0; i < num_streams; ++i) {
        streams[i].x = rand() % COLS;
        streams[i].head_y = -(rand() % LINES); 
        streams[i].speed = 1 + (rand() % 4);
        streams[i].speed_counter = 0;
        streams[i].length = 10 + (rand() % 20);
    }
}


void updateStreams() {
    // Instead of: i < NUM_STREAMS
    for (size_t i = 0; i < streams.size(); ++i) { 
        streams[i].speed_counter++;
        
        if (streams[i].speed_counter >= streams[i].speed) {
            streams[i].head_y++;
            streams[i].speed_counter = 0;

            // Check if the stream has moved off the bottom of the screen, 
            // and reset it to the top with new random parameters
            if (streams[i].head_y - streams[i].length > LINES) {
                streams[i].x = rand() % COLS; 
                streams[i].head_y = -(rand() % 5); 
                streams[i].speed = 1 + (rand() % 4);
                streams[i].length = 10 + (rand() % 20);
            }
        }
    }
}


void drawStreams() {
    clear(); 

    // Instead of: i < NUM_STREAMS
    for (size_t i = 0; i < streams.size(); ++i) {
        for (int j = 0; j < streams[i].length; ++j) {
            int current_y = streams[i].head_y - j;

            // Check that the current_y is within the visible range of the terminal
            if (current_y >= 0 && current_y < LINES) {
                // Start with the leading character, which is the brightest and white
                wchar_t kana_char = getrandomkana(); 
                cchar_t wide_char;

                mvadd_wch(current_y, streams[i].x, &wide_char);
                if (j == 0) {
                    setcchar(&wide_char, &kana_char, A_BOLD, 2, NULL);
                    mvadd_wch(current_y, streams[i].x, &wide_char);
                // Case for the next few characters in the stream, which are brighter
                } else if (j < 4) {
                    setcchar(&wide_char, &kana_char, A_BOLD, 1, NULL);
                    mvadd_wch(current_y, streams[i].x, &wide_char);
                // Remaining characters in the stream are dimmer and standard green
                } else {
                    setcchar(&wide_char, &kana_char, A_DIM, 1, NULL);
                    mvadd_wch(current_y, streams[i].x, &wide_char);
                }
            }
        }
    }
    refresh(); 
}


wchar_t getrandomkana() { 
    return static_cast<wchar_t>(0xFF61 + rand() % (0xFF9F - 0xFF61 + 1)); 
}