// compile w linux:
// g++ matr.cpp -o matr -lncurses

#include <ncurses.h>        // library to handle console output and control the display of characters.
#include <cstdlib>
#include <ctime>
#include <vector>
#include <unistd.h>


// Function prototypes
void initializeStreams();   // initializeStreams` function sets random starting positions for each rain stream.
void updateStreams();       // updateStreams` function moves the streams down the console.
void drawStreams();         // drawStreams` function clears the console and redraws the streams at their new positions. 
char getRandomKana();       // getRandomKana` function generates random half-width kana characters.


// Struct to track each independent Matrix rain stream
struct MatrixStream {
    int x;             // Fixed X coordinate for this stream
    int head_y;        // Current vertical position of the leading character
    int speed;         // Frames to wait before moving down (lower = faster)
    int speed_counter; // Counter to track speed delays
    int length;        // Length of the stream's trail
};


int main() {
    initializeStreams(); // Now this safely uses COLS and LINES    
    srand(time(0)); 
    initscr();     
    noecho();      
    curs_set(0);   

    // Setup Ncurses colors
    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK); // Standard Matrix Green
    init_pair(2, COLOR_WHITE, COLOR_BLACK); // White for the leading head

    initializeStreams(); 

    while (true) {
        updateStreams(); 
        drawStreams();   
        usleep(40000); // Global tick rate (40ms) for smoother rendering
    }

    endwin(); 
    return 0;
}



// Keep this global vector declaration, but leave it empty initially
std::vector<MatrixStream> streams; 

void initializeStreams() {
    // Resize the vector dynamically based on the terminal width
    int num_streams = COLS / 1; // Adjust divisor to change density
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

            // REPLACED: 'HEIGHT' becomes 'LINES'
            if (current_y >= 0 && current_y < LINES) {
                if (j == 0) {
                    attron(COLOR_PAIR(2) | A_BOLD);
                    mvaddch(current_y, streams[i].x, getRandomKana());
                    attroff(COLOR_PAIR(2) | A_BOLD);
                } else if (j < 4) {
                    attron(COLOR_PAIR(1) | A_BOLD);
                    mvaddch(current_y, streams[i].x, getRandomKana());
                    attroff(COLOR_PAIR(1) | A_BOLD);
                } else {
                    attron(COLOR_PAIR(1) | A_DIM);
                    mvaddch(current_y, streams[i].x, getRandomKana());
                    attroff(COLOR_PAIR(1) | A_DIM);
                }
            }
        }
    }
    refresh(); 
}


// Generate random half-width katakana characters
char getRandomKana() { 
    return static_cast<char>(0xFF61 + rand() % (0xFF9F - 0xFF61 + 1)); 
}