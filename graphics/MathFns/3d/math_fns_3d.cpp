// ************ MAIN PROGRAM ***************************************************
//| Item	     | Main Program Documentation Notes                            |
//|--------------|-------------------------------------------------------------|
//| Filename     | math_fns.cpp                                                |
//| EntryPoint   | main                                                        |
//| Purpose      | Graph 3D mathematical functions                             |
//| Inputs       | all hard coded                                              |
//| Outputs      | graph                                                       |
//| Dependencies | SDL2, SDL2_ttf                                              |
//| By Name,Date | T.Sciple, 02/06/2026                                        |
// Compile on Linux -> g++ -fdiagnostics-color=always -std=c++17 -g math_fns.cpp -o math_fns -I/usr/include/SDL2 -lSDL2 -lSDL2_ttf

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>

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


// Function Prototypes
bool project(const Vec3& world, const Camera& cam, int screenWidth, int screenHeight, int& sx, int& sy);
Vec3 rotateX(const Vec3& v, float angle_deg);
Vec3 rotateY(const Vec3& v, float angle_deg);
SDL_Color hsvToRgb(float h, float s = 0.95f, float v = 0.98f);
void drawText(SDL_Renderer* renderer, TTF_Font* font, const char* text, int x, int y, SDL_Color color);


int main() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    if (TTF_Init() == -1) {
        std::cerr << "TTF_Init failed: " << TTF_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // Load a font (you can change the path or use a system font)
    TTF_Font* font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 20);
    // Alternative: use a small built-in font or download one
    // TTF_Font* font = TTF_OpenFont("arial.ttf", 18);
    if (!font) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        // continue without labels or exit
    }

    int screenWidth = 1280;
    int screenHeight = 800;

    SDL_DisplayMode dm;
    if (SDL_GetCurrentDisplayMode(0, &dm) == 0) {
        screenWidth  = dm.w * 88 / 100;
        screenHeight = dm.h * 88 / 100;
        screenWidth  = std::max(screenWidth,  960);
        screenHeight = std::max(screenHeight, 720);
    }

    SDL_Window* window = SDL_CreateWindow(
        "3D Mathematical Surface with Axes",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        screenWidth, screenHeight,
        SDL_WINDOW_RESIZABLE
    );

    if (!window) {
        std::cerr << "SDL_CreateWindow failed\n";
        TTF_Quit(); SDL_Quit(); return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "SDL_CreateRenderer failed\n";
        SDL_DestroyWindow(window); TTF_Quit(); SDL_Quit(); return 1;
    }

    bool running = true;
    Camera cam;
    float rotY = 0.0f;
    float rotX = 0.0f;
    const float rotSpeed = 3.0f;
    const float moveSpeed = 0.4f;
    const float zoomSpeed = 40.0f;
    const float minFocal = 100.0f;
    const float maxFocal = 2000.0f;

    // Grid settings
    const int GRID_SIZE = 500;
    const float SCALE = 5.0f;
    const float STEP = (2.0f * SCALE) / (GRID_SIZE - 1.0f);

    std::vector<std::vector<Vec3>> grid(GRID_SIZE, std::vector<Vec3>(GRID_SIZE));
    float minZ =  1e9f, maxZ = -1e9f;
    
    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            float x = -SCALE + i * STEP;
            float y = -SCALE + j * STEP;
            float z = sinf(sqrtf(x*x + y*y) * 2.0f) * 1.2f;
            grid[i][j] = {x, z, y};
            minZ = std::min(minZ, z);
            maxZ = std::max(maxZ, z);
        }
    }
    float zRange = maxZ - minZ;
    if (zRange < 0.001f) zRange = 1.0f;

    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_ESCAPE) running = false;

                switch (e.key.keysym.sym) {
                    case SDLK_w: cam.position.z += moveSpeed; break;
                    case SDLK_s: cam.position.z -= moveSpeed; break;
                    case SDLK_a: cam.position.x -= moveSpeed; break;
                    case SDLK_d: cam.position.x += moveSpeed; break;
                    case SDLK_q: cam.position.y += moveSpeed; break;
                    case SDLK_e: cam.position.y -= moveSpeed; break;

                    case SDLK_LEFT:  rotY -= rotSpeed; break;
                    case SDLK_RIGHT: rotY += rotSpeed; break;
                    case SDLK_UP:    rotX -= rotSpeed; break;
                    case SDLK_DOWN:  rotX += rotSpeed; break;

                    case SDLK_PLUS:
                    case SDLK_KP_PLUS:
                        cam.focalLength += zoomSpeed;
                        if (cam.focalLength > maxFocal) cam.focalLength = maxFocal;
                        break;

                    case SDLK_MINUS:
                    case SDLK_KP_MINUS:
                        cam.focalLength -= zoomSpeed;
                        if (cam.focalLength < minFocal) cam.focalLength = minFocal;
                        break;

                    case SDLK_PAGEUP:
                        cam.position.y += 0.6f; break;
                    case SDLK_PAGEDOWN:
                        cam.position.y -= 0.6f; break;

                    case SDLK_r:
                        cam.position = {0.0f, -4.0f, -12.0f};
                        cam.focalLength = 600.0f;
                        rotX = 0.0f;
                        rotY = 0.0f;
                        break;
                }
            }

            if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_RESIZED) {
                SDL_GetWindowSize(window, &screenWidth, &screenHeight);
            }
        }

        SDL_SetRenderDrawColor(renderer, 8, 8, 18, 255);
        SDL_RenderClear(renderer);

        // ─── Collect & sort quads ───
        std::vector<Quad> quads;
        quads.reserve((GRID_SIZE-1)*(GRID_SIZE-1));

        for (int i = 0; i < GRID_SIZE - 1; ++i) {
            for (int j = 0; j < GRID_SIZE - 1; ++j) {
                Vec3 p00 = rotateX(rotateY(grid[i][j],     rotY), rotX);
                Vec3 p10 = rotateX(rotateY(grid[i+1][j],   rotY), rotX);
                Vec3 p01 = rotateX(rotateY(grid[i][j+1],   rotY), rotX);
                Vec3 p11 = rotateX(rotateY(grid[i+1][j+1], rotY), rotX);

                float vz00 = p00.z - cam.position.z;
                float vz10 = p10.z - cam.position.z;
                float vz01 = p01.z - cam.position.z;
                float vz11 = p11.z - cam.position.z;
                float avgDepth = (vz00 + vz10 + vz01 + vz11) / 4.0f;

                quads.push_back({p00, p10, p01, p11, avgDepth, i, j});
            }
        }

        std::sort(quads.begin(), quads.end(), [](const Quad& a, const Quad& b){
            return a.avgDepth > b.avgDepth;
        });

        // Draw surface
        for (const auto& q : quads) {
            int x00,y00, x10,y10, x01,y01, x11,y11;
            if (!project(q.p00, cam, screenWidth, screenHeight, x00,y00)) continue;
            if (!project(q.p10, cam, screenWidth, screenHeight, x10,y10)) continue;
            if (!project(q.p01, cam, screenWidth, screenHeight, x01,y01)) continue;
            if (!project(q.p11, cam, screenWidth, screenHeight, x11,y11)) continue;

            float h00 = grid[q.i][q.j].y;
            float h10 = grid[q.i+1][q.j].y;
            float h01 = grid[q.i][q.j+1].y;
            float h11 = grid[q.i+1][q.j+1].y;
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

            SDL_SetRenderDrawColor(renderer, 35, 35, 70, 140);
            // SDL_RenderDrawLine(renderer, x00,y00, x10,y10);
            // SDL_RenderDrawLine(renderer, x10,y10, x11,y11);
            // SDL_RenderDrawLine(renderer, x11,y11, x01,y01);
            // SDL_RenderDrawLine(renderer, x01,y01, x00,y00);
        }

        // ─── Draw axes with labels ───
        Vec3 origin = rotateX(rotateY({0,0,0}, rotY), rotX);
        Vec3 px = rotateX(rotateY({5.0f, 0, 0}, rotY), rotX);   // X - red
        Vec3 py = rotateX(rotateY({0, 5.0f, 0}, rotY), rotX);   // Y - green (height)
        Vec3 pz = rotateX(rotateY({0, 0, 5.0f}, rotY), rotX);   // Z - blue

        int ox, oy;
        if (project(origin, cam, screenWidth, screenHeight, ox, oy)) {
            
            // X axis - red
            int x1, y1;
            if (project(px, cam, screenWidth, screenHeight, x1, y1)) {
                SDL_SetRenderDrawColor(renderer, 220, 40, 40, 255);
                SDL_RenderDrawLine(renderer, ox, oy, x1, y1);

                // Label "X"
                if (font) {
                    drawText(renderer, font, "X", x1 + 8, y1 - 10, {220, 40, 40, 255});
                }
            }

            // Z axis - green
            if (project(py, cam, screenWidth, screenHeight, x1, y1)) {
                SDL_SetRenderDrawColor(renderer, 40, 220, 40, 255);
                SDL_RenderDrawLine(renderer, ox, oy, x1, y1);

                // Label "Z"
                if (font) {
                    drawText(renderer, font, "Z", x1 + 8, y1 - 10, {40, 220, 40, 255});
                }
            }

            // y axis - blue
            if (project(pz, cam, screenWidth, screenHeight, x1, y1)) {
                SDL_SetRenderDrawColor(renderer, 40, 40, 220, 255);
                SDL_RenderDrawLine(renderer, ox, oy, x1, y1);

                // Label "Y"
                if (font) {
                    drawText(renderer, font, "Y", x1 + 8, y1 - 10, {40, 40, 220, 255});
                }
            }
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    if (font) TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}


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


// HSV to RGB
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
