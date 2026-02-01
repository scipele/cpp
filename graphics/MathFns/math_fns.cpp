// ************ MAIN PROGRAM ***************************************************
//| Item	     | Main Program Documentation Notes                            |
//|--------------|-------------------------------------------------------------|
//| Filename     | math_fns.cpp                                                |
//| EntryPoint   | main                                                        |
//| Purpose      | Graph mathematical functions                                |
//| Inputs       | all hard coded                                              |
//| Outputs      | graph                                                       |
//| Dependencies | SDL2, SDL2_ttf                                              |
//| By Name,Date | T.Sciple, 01/31/2026                                        |
// g++ -fdiagnostics-color=always -std=c++17 -g${file} -o${fileDirname}/${fileBasenameNoExtension} -I/usr/include/SDL2 -lSDL2 -lSDL2_ttf

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <cmath>
#include <string>

struct line_coord_dbl {
    double x;
    double y;
    double x_prev;
    double y_prev;
};

struct line_coord_int {
    int x;
    int y;
    int x_prev;
    int y_prev;
};

struct color_rgb_opac {
    int r;
    int g;
    int b;
    int o; // opacity
};


// Function prototypes

bool init_window(SDL_Window*& window, SDL_Renderer*& rd, SDL_DisplayMode& disp);
void draw_axes(SDL_Renderer* rd, SDL_DisplayMode& disp);
void draw_line(SDL_Renderer* rd, line_coord_dbl& lc, color_rgb_opac& col);
void draw_line(SDL_Renderer* rd, line_coord_int& lc, color_rgb_opac& col);
double math_fn_a(double x);
double math_fn_b(double x);
double math_fn_c(double x);
double math_fn_d(double ya, double yb, double yc); 
void graph_math_fn(SDL_Renderer* rd, SDL_DisplayMode& disp);
void render_text(SDL_Renderer* rd, const char* text, int x, int y, int font_sz);
void wait_for_keypress();
line_coord_int conv_coord_to_screen_coord(double x, double y, line_coord_int& prev_coord, int sf, int x_offset, int y_offset);   


int main(int argc, char const *argv[])
{
    SDL_Window* window = nullptr;
    SDL_Renderer* rd = nullptr;
    SDL_DisplayMode disp;
    if (!init_window(window, rd, disp)) {
        return 1;
    }

    draw_axes(rd, disp);
    graph_math_fn(rd, disp);
    render_text(rd, "Press any key to exit...", 2000, 1200, 16);
    wait_for_keypress();
   
    // Cleanup
    SDL_DestroyRenderer(rd);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

// Overloaded function for integer
void draw_line(SDL_Renderer* rd, line_coord_dbl& lc, color_rgb_opac& col) {
    int x1 = static_cast<int>(lc.x_prev);
    int y1 = static_cast<int>(lc.y_prev);
    int x2 = static_cast<int>(lc.x);
    int y2 = static_cast<int>(lc.y);
    SDL_SetRenderDrawColor(rd, col.r, col.g, col.b, col.o);
    SDL_RenderDrawLine(rd, x1, y1, x2, y2);
};

// Overloaded function for double
void draw_line(SDL_Renderer* rd, line_coord_int& lc, color_rgb_opac& col) {
    int x1 = lc.x_prev;
    int y1 = lc.y_prev;
    int x2 = lc.x;
    int y2 = lc.y;
    SDL_SetRenderDrawColor(rd, col.r, col.g, col.b, col.o);
    SDL_RenderDrawLine(rd, x1, y1, x2, y2);
};


double math_fn_a(double x) {
    return 4 * sin(x);
}

double math_fn_b(double x) {
    return 4 * sin(x + 2*M_PI/3);
}

double math_fn_c(double x) {
    return 4 * sin(x + 4*M_PI/3);
}

double math_fn_d(double ya, double yb, double yc) {
    return abs(ya) + abs(yb) + abs(yc);
}


// Helper function to initialize SDL, window, and renderer
bool init_window(SDL_Window*& window, SDL_Renderer*& rd, SDL_DisplayMode& disp) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return false;
    }
    if (SDL_GetCurrentDisplayMode(0, &disp) != 0) {
        SDL_Log("Could not get display mode: %s", SDL_GetError());
        SDL_Quit();
        return false;
    }
    window = SDL_CreateWindow(
        "Graph of Mathematical Functions",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        disp.w,
        disp.h,
        0
    );
    if (!window) {
        SDL_Log("Could not create window: %s", SDL_GetError());
        SDL_Quit();
        return false;
    }
    rd = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!rd) {
        SDL_Log("Could not create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }
    return true;
}


void draw_axes(SDL_Renderer* rd, SDL_DisplayMode& disp) {

    // Set draw background color (optional, e.g., white)
    SDL_SetRenderDrawColor(rd, 255, 255, 255, 255);
    SDL_RenderClear(rd);

    // Draw the x and y axis
    SDL_SetRenderDrawColor(rd, 0, 0, 0, 255);
    line_coord_int li  = {disp.w, disp.h / 2, 0, disp.h / 2};    // x axis

    std::string w = "screen width = " + std::to_string(disp.w);
    render_text(rd, w.c_str(), 120, 140, 12);
    std::string h = "screen height = " + std::to_string(disp.h);
    render_text(rd, h.c_str(), 120, 160, 12);
 
    color_rgb_opac col_axes = {30, 30, 30, 255}; // Black for axes
    draw_line(rd, li, col_axes);

    li  = {disp.w / 2, 0, disp.w / 2, disp.h};    // y axis
    draw_line(rd, li, col_axes);

    // Draw Border Line aound the screen
    SDL_SetRenderDrawColor(rd, 0.05 * disp.h / 2, 0, 0, 255); // Dark red for border
    // draw tick marks on axes
    int tick_len = 0.002 * disp.w; // length factor for tick marks
    bool show_labels = false;

    for (int i = -disp.w / 2; i <= disp.w / 2; i += 20) {
        // make the major tick marks longer
        if (i % 100 == 0) {
            tick_len = 0.004 * disp.w;
            show_labels = true;
        } else {
            tick_len = 0.002 * disp.w;
            show_labels = false;
        }

        li = {disp.w / 2 - tick_len, disp.h / 2 + i, disp.w / 2 + tick_len, disp.h / 2 + i};
        draw_line(rd, li, col_axes); // x axis ticks
        if (show_labels) {
            render_text(rd, std::to_string(i / 20).c_str(), disp.w / 2 + tick_len + 2, disp.h / 2 - i - 8, 12);
        }

        li = {disp.w / 2 + i, disp.h / 2 - tick_len, disp.w / 2 + i, disp.h / 2 + tick_len};
        draw_line(rd, li, col_axes); // y axis ticks
        if (show_labels && i != 0) {
            render_text(rd, std::to_string(i / 20).c_str(), disp.w / 2 + i - 5, disp.h / 2 + tick_len + 2, 12);
        }
    }
}


void graph_math_fn(SDL_Renderer* rd, SDL_DisplayMode& disp) {
    double xmin = -20.0;
    double xmax = 20;
    
    // Calculate the scale factor based on the display size and x range
    int sf = static_cast<int>(disp.w / (xmax - xmin));


    int x_offset = disp.w / 2;
    int y_offset = disp.h / 2;

    color_rgb_opac col_a = {255, 0, 0, 255}; // Red
    color_rgb_opac col_b = {0, 255, 0, 255}; // Green
    color_rgb_opac col_c = {0, 0, 255, 255}; // Blue
    color_rgb_opac col_d = {0, 0, 128, 255}; // Blue

    render_text(rd, "Graph of Sin Waves", 120, 80, 36);
    std::string scale_str = "Scale Factor: " + std::to_string(sf);
    render_text(rd, scale_str.c_str(), 120, 120, 12);

    // set initial previous coordinates
    double ya, yb, yc, yd;
    ya = math_fn_a(xmin);
    yb = math_fn_b(xmin);
    yc = math_fn_c(xmin);
    yd = math_fn_d(ya, yb, yc);

    line_coord_int line_a = conv_coord_to_screen_coord(xmin, ya, line_a, sf, x_offset, y_offset);
    line_coord_int line_b = conv_coord_to_screen_coord(xmin, yb, line_b, sf, x_offset, y_offset);
    line_coord_int line_c = conv_coord_to_screen_coord(xmin, yc, line_c, sf, x_offset, y_offset);
    line_coord_int line_d = conv_coord_to_screen_coord(xmin, yd, line_d, sf, x_offset, y_offset);    

    SDL_SetRenderDrawColor(rd, 0, 0, 255, 255); // blue line
    for (double x = xmin; x < xmax; x += 0.01) {
        // Example mathematical function to plot

        ya = math_fn_a(x);
        yb = math_fn_b(x);
        yc = math_fn_c(x);
        yd = math_fn_d(ya, yb, yc);

        line_a = conv_coord_to_screen_coord(x, ya, line_a, sf, x_offset, y_offset);
        line_b = conv_coord_to_screen_coord(x, yb, line_b, sf, x_offset, y_offset);
        line_c = conv_coord_to_screen_coord(x, yc, line_c, sf, x_offset, y_offset);
        line_d = conv_coord_to_screen_coord(x, yd, line_d, sf, x_offset, y_offset);

        draw_line(rd, line_a, col_a);
        draw_line(rd, line_b, col_b);
        draw_line(rd, line_c, col_c );
        draw_line(rd, line_d, col_d );
   }

}


void render_text(SDL_Renderer* rd, const char* text, int x, int y, int font_sz) {
    if (TTF_Init() == -1) {
        SDL_Log("Unable to initialize SDL_ttf: %s", TTF_GetError());
    } else {
        TTF_Font* font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", font_sz);
        if (font) {
            SDL_Color color = {0, 0, 255, 255};
            SDL_Surface* textSurface = TTF_RenderText_Blended(font, text, color);
            if (textSurface) {
                SDL_Texture* textTexture = SDL_CreateTextureFromSurface(rd, textSurface);
                SDL_Rect dstRect = {x, y, textSurface->w, textSurface->h};
                SDL_RenderCopy(rd, textTexture, NULL, &dstRect);
                SDL_RenderPresent(rd);
                SDL_DestroyTexture(textTexture);
                SDL_FreeSurface(textSurface);
            }
            TTF_CloseFont(font);
        }
        TTF_Quit();
    }
}


void wait_for_keypress() {
    SDL_Event e;
    bool quit = false;
    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT || e.type == SDL_KEYDOWN) {
                quit = true;
            }
        }
    }
}


line_coord_int conv_coord_to_screen_coord(double x, double y, line_coord_int& prev_coord, int sf, int x_offset, int y_offset) {
    int screen_x = static_cast<int>(x * sf) + x_offset;
    int screen_y = y_offset - static_cast<int>(y * sf);
    return {screen_x, screen_y, prev_coord.x, prev_coord.y};  
}