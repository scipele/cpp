// ************ MAIN PROGRAM ***************************************************
//| Item	     | Main Program Documentation Notes                            |
//|--------------|-------------------------------------------------------------|
//| Filename     | math_fns_3d.cpp                                             |
//| EntryPoint   | main                                                        |
//| Purpose      | Graph 3D mathematical functions                             |
//| Inputs       | all hard coded                                              |
//| Outputs      | graph                                                       |
//| Dependencies | SDL2, SDL2_ttf                                              |
//| By Name,Date | T.Sciple, 02/07/2026                                        |
// Compile on Linux -> g++ -fdiagnostics-color=always -std=c++17 -g math_fns_3d.cpp -o math_fns_3d -I/usr/include/SDL2 -lSDL2 -lSDL2_ttf

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <fstream>
#include <string>

const int GRID_SIZE = 400;

struct Vec3 {
    float x, y, z;
};

struct Camera {
    Vec3 position = {0.0f, -4.0f, -12.0f};
    float focalLength = 300.0f;
};

struct Quad {
    Vec3 p00, p10, p01, p11;
    float avgDepth;
    int i, j;
};

struct AppState {
    bool running = true;
    Camera cam;
    float rotX = -145.0f;
    float rotY = 45.0f;
    float rotZ = -60.0f;
    int screenWidth = 1280;
    int screenHeight = 800;
};

// ─── Function Prototypes ───
bool project(const Vec3& world, const Camera& cam, int screenWidth, int screenHeight, int& sx, int& sy);
Vec3 rotateX(const Vec3& v, float angle_deg);
Vec3 rotateY(const Vec3& v, float angle_deg);
Vec3 rotateZ(const Vec3& v, float angle_deg);
SDL_Color hsvToRgb(float h, float s = 0.95f, float v = 0.98f);
void drawText(SDL_Renderer* renderer, TTF_Font* font, const char* text, int x, int y, SDL_Color color);
bool initSDL();
void getScreenDimensions(int& width, int& height);
bool createWindow(SDL_Window*& window, SDL_Renderer*& renderer, int width, int height);
TTF_Font* loadFont(const char* path, int size);
void generateGrid(std::vector<std::vector<Vec3>>& grid, float& minZ, float& maxZ);
void handleInput(SDL_Event& e, AppState& state);
void renderSurface(SDL_Renderer* renderer, const std::vector<Quad>& quads, const std::vector<std::vector<Vec3>>& grid, float minZ, float zRange, const Camera& cam, int screenWidth, int screenHeight);
void renderAxes(SDL_Renderer* renderer, TTF_Font* font, float rotX, float rotY, float rotZ, const Camera& cam, int screenWidth, int screenHeight);
void renderTitle(SDL_Renderer* renderer, TTF_Font* font);
void cleanup(SDL_Window* window, SDL_Renderer* renderer, TTF_Font* font);
void exportToOBJ(const std::vector<std::vector<Vec3>>& grid, float minZ, float zRange, const std::string& filename);


int main() {
    // Initialize
    if (!initSDL()) return 1;

    AppState state;
    getScreenDimensions(state.screenWidth, state.screenHeight);

    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    if (!createWindow(window, renderer, state.screenWidth, state.screenHeight)) return 1;

    TTF_Font* font = loadFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 20);

    // Generate grid
    std::vector<std::vector<Vec3>> grid;
    float minZ = 0.0f, maxZ = 0.0f;
    generateGrid(grid, minZ, maxZ);
    float zRange = maxZ - minZ;
    if (zRange < 0.001f) zRange = 1.0f;

    exportToOBJ(grid, minZ, zRange, "surface_colored.obj");

    const float rotSpeed = 3.0f;
    const float moveSpeed = 0.4f;
    const float zoomSpeed = 40.0f;
    const float minFocal = 100.0f;
    const float maxFocal = 2000.0f;

    // Main loop
    while (state.running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            handleInput(e, state);
            
            if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_RESIZED) {
                SDL_GetWindowSize(window, &state.screenWidth, &state.screenHeight);
            }
        }

        SDL_SetRenderDrawColor(renderer, 8, 8, 18, 255);
        SDL_RenderClear(renderer);

        // ─── Collect & sort quads ───
        std::vector<Quad> quads;
        quads.reserve((GRID_SIZE-1)*(GRID_SIZE-1));

        for (int i = 0; i < GRID_SIZE - 1; ++i) {
            for (int j = 0; j < GRID_SIZE - 1; ++j) {
                Vec3 p00 = rotateZ(rotateX(rotateY(grid[i][j],     state.rotY), state.rotX), state.rotZ);
                Vec3 p10 = rotateZ(rotateX(rotateY(grid[i+1][j],   state.rotY), state.rotX), state.rotZ);
                Vec3 p01 = rotateZ(rotateX(rotateY(grid[i][j+1],   state.rotY), state.rotX), state.rotZ);
                Vec3 p11 = rotateZ(rotateX(rotateY(grid[i+1][j+1], state.rotY), state.rotX), state.rotZ);

                float vz00 = p00.z - state.cam.position.z;
                float vz10 = p10.z - state.cam.position.z;
                float vz01 = p01.z - state.cam.position.z;
                float vz11 = p11.z - state.cam.position.z;
                float avgDepth = (vz00 + vz10 + vz01 + vz11) / 4.0f;

                quads.push_back({p00, p10, p01, p11, avgDepth, i, j});
            }
        }

        std::sort(quads.begin(), quads.end(), [](const Quad& a, const Quad& b){
            return a.avgDepth > b.avgDepth;
        });

        renderSurface(renderer, quads, grid, minZ, zRange, state.cam, state.screenWidth, state.screenHeight);
        renderAxes(renderer, font, state.rotX, state.rotY, state.rotZ, state.cam, state.screenWidth, state.screenHeight);
        renderTitle(renderer, font);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    cleanup(window, renderer, font);
    return 0;
}


// ─── Initialization Functions ───
bool initSDL() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return false;
    }

    if (TTF_Init() == -1) {
        std::cerr << "TTF_Init failed: " << TTF_GetError() << std::endl;
        SDL_Quit();
        return false;
    }

    return true;
}

void getScreenDimensions(int& width, int& height) {
    SDL_DisplayMode dm;
    if (SDL_GetCurrentDisplayMode(0, &dm) == 0) {
        width  = dm.w * 88 / 100;
        height = dm.h * 88 / 100;
        width  = std::max(width,  960);
        height = std::max(height, 720);
    }
}

bool createWindow(SDL_Window*& window, SDL_Renderer*& renderer, int width, int height) {
    window = SDL_CreateWindow(
        "3D Mathematical Surface with Axes",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        width, height,
        SDL_WINDOW_RESIZABLE
    );

    if (!window) {
        std::cerr << "SDL_CreateWindow failed\n";
        TTF_Quit(); SDL_Quit(); 
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "SDL_CreateRenderer failed\n";
        SDL_DestroyWindow(window); TTF_Quit(); SDL_Quit(); 
        return false;
    }

    return true;
}

TTF_Font* loadFont(const char* path, int size) {
    TTF_Font* font = TTF_OpenFont(path, size);
    if (!font) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
    }
    return font;
}

void generateGrid(std::vector<std::vector<Vec3>>& grid, float& minZ, float& maxZ) {
    const float SCALE = 7.0f;
    const float STEP = (2.0f * SCALE) / (GRID_SIZE - 1.0f);

    grid.resize(GRID_SIZE, std::vector<Vec3>(GRID_SIZE));
    minZ = 1e9f;
    maxZ = -1e9f;
    
    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            float x = -SCALE + i * STEP;
            float y = -SCALE + j * STEP;
            float z = -1.5f * sinf(sqrtf(x*x + y*y) * 1.0f);

            grid[i][j] = {x, y, z};
            minZ = std::min(minZ, z);
            maxZ = std::max(maxZ, z);
        }
    }
}

// ─── Input Handling ───
void handleInput(SDL_Event& e, AppState& state) {
    const float rotSpeed = 3.0f;
    const float moveSpeed = 0.4f;
    const float zoomSpeed = 40.0f;
    const float minFocal = 100.0f;
    const float maxFocal = 2000.0f;

    if (e.type == SDL_QUIT) {
        state.running = false;
    }

    if (e.type == SDL_KEYDOWN) {
        if (e.key.keysym.sym == SDLK_ESCAPE) {
            state.running = false;
        }

        switch (e.key.keysym.sym) {
            case SDLK_w: state.cam.position.z += moveSpeed; break;
            case SDLK_s: state.cam.position.z -= moveSpeed; break;
            case SDLK_a: state.cam.position.x -= moveSpeed; break;
            case SDLK_d: state.cam.position.x += moveSpeed; break;
            case SDLK_q: state.cam.position.y += moveSpeed; break;
            case SDLK_e: state.cam.position.y -= moveSpeed; break;

            case SDLK_LEFT:  state.rotX -= rotSpeed; state.rotZ -= rotSpeed; break;
            case SDLK_RIGHT: state.rotX += rotSpeed; state.rotZ += rotSpeed; break;
            case SDLK_UP:    state.rotX -= rotSpeed; break;
            case SDLK_DOWN:  state.rotX += rotSpeed; break;

            case SDLK_PLUS:
            case SDLK_KP_PLUS:
                state.cam.focalLength += zoomSpeed;
                if (state.cam.focalLength > maxFocal) state.cam.focalLength = maxFocal;
                break;

            case SDLK_MINUS:
            case SDLK_KP_MINUS:
                state.cam.focalLength -= zoomSpeed;
                if (state.cam.focalLength < minFocal) state.cam.focalLength = minFocal;
                break;

            case SDLK_PAGEUP:
                state.cam.position.y += 0.6f; break;
            case SDLK_PAGEDOWN:
                state.cam.position.y -= 0.6f; break;

            case SDLK_r:
                state.cam.position = {0.0f, -1.0f, -12.0f};
                state.cam.focalLength = 900.0f;
                state.rotX = -35.26f;
                state.rotY = 45.0f;
                state.rotZ = 0.0f;
                break;

            case SDLK_p:
                // Print current rotation and camera values
                std::cout << "Current View Settings:\n";
                std::cout << "rotX = " << state.rotX << "f;\n";
                std::cout << "rotY = " << state.rotY << "f;\n";
                std::cout << "rotZ = " << state.rotZ << "f;\n";
                std::cout << "cam.position = {" << state.cam.position.x << "f, " 
                          << state.cam.position.y << "f, " << state.cam.position.z << "f};\n";
                std::cout << "cam.focalLength = " << state.cam.focalLength << "f;\n";
                std::cout << std::endl;
                break;
        }
    }
}

// ─── Rendering Functions ───
void renderSurface(SDL_Renderer* renderer, const std::vector<Quad>& quads, const std::vector<std::vector<Vec3>>& grid, float minZ, float zRange, const Camera& cam, int screenWidth, int screenHeight) {
    for (const auto& q : quads) {
        int x00,y00, x10,y10, x01,y01, x11,y11;
        if (!project(q.p00, cam, screenWidth, screenHeight, x00,y00)) continue;
        if (!project(q.p10, cam, screenWidth, screenHeight, x10,y10)) continue;
        if (!project(q.p01, cam, screenWidth, screenHeight, x01,y01)) continue;
        if (!project(q.p11, cam, screenWidth, screenHeight, x11,y11)) continue;

        float h00 = grid[q.i][q.j].z;
        float h10 = grid[q.i+1][q.j].z;
        float h01 = grid[q.i][q.j+1].z;
        float h11 = grid[q.i+1][q.j+1].z;
        float avgHeight = (h00 + h10 + h01 + h11) / 4.0f;

        float norm = (avgHeight - minZ) / zRange;
        norm = std::clamp(norm, 0.0f, 1.0f);

        float hue = 240.0f - norm * 240.0f;
        SDL_Color color = hsvToRgb(hue, 0.95f, 0.98f);

        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);

        int minX = std::min({x00,x10,x01,x11});
        int maxX = std::max({x00,x10,x01,x11});
        int minY = std::min({y00,y10,y01,y11});
        int maxY = std::max({y00,y10,y01,y11});

        if (maxX > minX && maxY > minY) {
            SDL_Rect rect = {minX, minY, maxX-minX+1, maxY-minY+1};
            SDL_RenderFillRect(renderer, &rect);
        }
    }
}

void renderAxes(SDL_Renderer* renderer, TTF_Font* font, float rotX, float rotY, float rotZ, const Camera& cam, int screenWidth, int screenHeight) {
    Vec3 origin = rotateZ(rotateX(rotateY({0,0,0}, rotY), rotX), rotZ);
    Vec3 px = rotateZ(rotateX(rotateY({5.0f, 0, 0}, rotY), rotX), rotZ);
    Vec3 py = rotateZ(rotateX(rotateY({0, 5.0f, 0}, rotY), rotX), rotZ);
    Vec3 pz = rotateZ(rotateX(rotateY({0, 0, 5.0f}, rotY), rotX), rotZ);

    int ox, oy;
    if (!project(origin, cam, screenWidth, screenHeight, ox, oy)) return;

    int x1, y1;

    // X axis - red
    if (project(px, cam, screenWidth, screenHeight, x1, y1)) {
        SDL_SetRenderDrawColor(renderer, 220, 40, 40, 255);
        SDL_RenderDrawLine(renderer, ox, oy, x1, y1);
        if (font) drawText(renderer, font, "X", x1 + 8, y1 - 10, {220, 40, 40, 255});
    }

    // Y axis - green
    if (project(py, cam, screenWidth, screenHeight, x1, y1)) {
        SDL_SetRenderDrawColor(renderer, 40, 220, 40, 255);
        SDL_RenderDrawLine(renderer, ox, oy, x1, y1);
        if (font) drawText(renderer, font, "Y", x1 + 8, y1 - 10, {40, 220, 40, 255});
    }

    // Z axis - blue
    if (project(pz, cam, screenWidth, screenHeight, x1, y1)) {
        SDL_SetRenderDrawColor(renderer, 40, 40, 220, 255);
        SDL_RenderDrawLine(renderer, ox, oy, x1, y1);
        if (font) drawText(renderer, font, "Z", x1 + 8, y1 - 10, {40, 40, 220, 255});
    }
}

void renderTitle(SDL_Renderer* renderer, TTF_Font* font) {
    if (font) {
        drawText(renderer, font, "3D Graph: z = -1.5 * sin(sqrt(x^2 + y^2) * 1.0)", 20, 20, {200, 200, 200, 255});
    }
}

void cleanup(SDL_Window* window, SDL_Renderer* renderer, TTF_Font* font) {
    if (font) TTF_CloseFont(font);
    TTF_Quit();
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}

// ─── Math & Utility Functions ───
bool project(const Vec3& world, const Camera& cam, int screenWidth, int screenHeight, int& sx, int& sy) {
    float vx = world.x - cam.position.x;
    float vy = world.y - cam.position.y;
    float vz = world.z - cam.position.z;

    if (vz <= 0.01f) return false;

    float scale = cam.focalLength / vz;
    sx = screenWidth  / 2 + (int)(vx * scale);
    sy = screenHeight / 2 - (int)(vy * scale);
    return true;
}

Vec3 rotateX(const Vec3& v, float angle_deg) {
    float rad = angle_deg * M_PI / 180.0f;
    float c = cosf(rad), s = sinf(rad);
    return {v.x, v.y * c - v.z * s, v.y * s + v.z * c};
}

Vec3 rotateY(const Vec3& v, float angle_deg) {
    float rad = angle_deg * M_PI / 180.0f;
    float c = cosf(rad), s = sinf(rad);
    return {v.x * c - v.z * s, v.y, v.x * s + v.z * c};
}

Vec3 rotateZ(const Vec3& v, float angle_deg) {
    float rad = angle_deg * M_PI / 180.0f;
    float c = cosf(rad), s = sinf(rad);
    return {v.x * c - v.y * s, v.x * s + v.y * c, v.z};
}

SDL_Color hsvToRgb(float h, float s, float v) {
    h = fmodf(h, 360.0f);
    if (h < 0) h += 360.0f;
    int i = (int)(h / 60.0f);
    float f = h / 60.0f - i;
    float p = v * (1 - s);
    float q = v * (1 - f * s);
    float t = v * (1 - (1 - f) * s);

    switch (i) {
        case 0: return {(Uint8)(v*255), (Uint8)(t*255), (Uint8)(p*255), 255};
        case 1: return {(Uint8)(q*255), (Uint8)(v*255), (Uint8)(p*255), 255};
        case 2: return {(Uint8)(p*255), (Uint8)(v*255), (Uint8)(t*255), 255};
        case 3: return {(Uint8)(p*255), (Uint8)(q*255), (Uint8)(v*255), 255};
        case 4: return {(Uint8)(t*255), (Uint8)(p*255), (Uint8)(v*255), 255};
        default:return {(Uint8)(v*255), (Uint8)(p*255), (Uint8)(q*255), 255};
    }
}

void drawText(SDL_Renderer* renderer, TTF_Font* font, const char* text, int x, int y, SDL_Color color) {
    SDL_Surface* surface = TTF_RenderText_Blended(font, text, color);
    if (!surface) return;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        SDL_FreeSurface(surface);
        return;
    }

    SDL_Rect rect = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &rect);

    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}


void exportToOBJ(const std::vector<std::vector<Vec3>>& grid, float minZ, float zRange, const std::string& filename) {
    std::ofstream out(filename);
    if (!out) {
        std::cerr << "Failed to open " << filename << std::endl;
        return;
    }

    int rows = grid.size();
    int cols = grid[0].size();

    // Write vertices with colors
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            const Vec3& v = grid[i][j];

            // Compute color exactly like in renderSurface()
            float norm = (v.z - minZ) / zRange;
            norm = std::clamp(norm, 0.0f, 1.0f);
            float hue = 240.0f - norm * 240.0f;           // blue → cyan → green → yellow → red
            SDL_Color color = hsvToRgb(hue, 0.95f, 0.98f);

            // Convert SDL_Color (0-255) to float 0.0-1.0
            float r = color.r / 255.0f;
            float g = color.g / 255.0f;
            float b = color.b / 255.0f;

            out << "v " << v.x << " " << v.y << " " << v.z
                << " " << r << " " << g << " " << b << "\n";
        }
    }

    // Write triangular faces (same as before)
    for (int i = 0; i < rows - 1; ++i) {
        for (int j = 0; j < cols - 1; ++j) {
            int v1 = i * cols + j + 1;
            int v2 = (i + 1) * cols + j + 1;
            int v3 = i * cols + (j + 1) + 1;
            int v4 = (i + 1) * cols + (j + 1) + 1;

            // Triangle 1
            out << "f " << v1 << " " << v2 << " " << v3 << "\n";
            // Triangle 2
            out << "f " << v2 << " " << v4 << " " << v3 << "\n";
        }
    }

    out.close();
    std::cout << "Exported colored mesh to " << filename << std::endl;
}