#include <windows.h>
#include <cmath>

const double LANE_WIDTH_IN = 41.5;
const double BALL_DIAMETER_IN = 8.5;
const double PIN_OVERALL_DIA_IN = 4.766;
const double BALL_TO_PIN_IMPACT_IN = (BALL_DIAMETER_IN + PIN_OVERALL_DIA_IN) / 2;
const double PIN_NECK_IN = 1.797;
const int TOTAL_BOARDS = 39;
const double FOUL_LINE_TO_PIN_01_IN = 60 * 12;
const double LANE_LEN_IN = FOUL_LINE_TO_PIN_01_IN + 36.18754; // 60 feet from foul line to pins
const int APPROACH_LEN_IN = 15 * 12 + 8; // 15'-8" approach
const double LANE_AND_APPROACH_LEN_IN = LANE_LEN_IN + APPROACH_LEN_IN;
const int ARROW_SPACING_DY_BOARDS = 5;
const int ARROW_DISTANCE_PX = 16 * 12;  // Distance to the Center Arrow located on board 20
const int PIN_SPACING_IN = 12;
const double GUTTER_WIDTH = 9.25;
const int SCREEN_WIDTH = 5120;
const int SCREEN_WIDTH_MARGINS = SCREEN_WIDTH * 0.05;                   // 128 
const int SCREEN_WIDTH_MARGIN = SCREEN_WIDTH_MARGINS / 2;               // 64
const int SCREEN_WIDTH_AVAIL = SCREEN_WIDTH - SCREEN_WIDTH_MARGINS;     // 2432
const int SCREEN_HEIGHT = 1440;
const double SCALE = SCREEN_WIDTH_AVAIL / LANE_AND_APPROACH_LEN_IN;    // 2.575794725302138
const double LANE_WIDTH_PY = LANE_WIDTH_IN * SCALE;
const double BALL_DIAMETER_PX = BALL_DIAMETER_IN * SCALE;
const double PIN_OVERALL_DIA_PX = PIN_OVERALL_DIA_IN * SCALE;
const double PIN_NECK_DIA_PX = PIN_NECK_IN * SCALE;
const double BOARD_WIDTH_PY = LANE_WIDTH_PY / TOTAL_BOARDS;
const double CONV_MPH_TO_FPS = 5280.0 / 3600.0;  // mph to fps conversion
const double BALL_SPEED_MPH = 15;  // Ball speed in mph
double ball_speed_fps = BALL_SPEED_MPH * CONV_MPH_TO_FPS;  // Ball speed in feet per second
double ball_speed_px = ball_speed_fps * 12 * SCALE;  // Speed in pixels per second

// Define the origin as the foul line and the edge of lane
int ORIGIN_X = SCREEN_WIDTH_MARGIN + APPROACH_LEN_IN * SCALE;  // Foul line called zero
int ORIGIN_Y = (SCREEN_HEIGHT / 4) - (LANE_WIDTH_IN * SCALE / 2);   // bottom edge of the lane
double calc_ball_to_pin_dist_in;

// Set the initial ball location
double ball_x_scaled = ORIGIN_X;
double ball_y_scaled = ORIGIN_Y + (22.5 * BOARD_WIDTH_PY);
bool rolling = false;

// Set Head Pin X and Y Coordinates
const double PIN_BASE_X_SCALED = ORIGIN_X + FOUL_LINE_TO_PIN_01_IN * SCALE;
const double PIN_BASE_X_TRUE = PIN_BASE_X_SCALED / SCALE;
const double PIN_BASE_Y_SCALED = ORIGIN_Y + (LANE_WIDTH_PY / 2);
const double PIN_BASE_Y_TRUE = PIN_BASE_Y_SCALED / SCALE;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        // Set the pen
        HPEN hPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0)); // Black outline, 2-pixel thick
        SelectObject(hdc, hPen);

        // Draw the boards for lane and past the lane + 18
        for (int i = 0; i < TOTAL_BOARDS + 18; ++i)
        {
            
            RECT board = {
                ORIGIN_X - APPROACH_LEN_IN * SCALE,
                ORIGIN_Y + i * BOARD_WIDTH_PY - 9 * BOARD_WIDTH_PY,
                ORIGIN_X + LANE_LEN_IN * SCALE,
                ORIGIN_Y + (i + 1) * BOARD_WIDTH_PY - 9 * BOARD_WIDTH_PY
            };

            HBRUSH hBrush;
            if (i % 2 == 0)
                hBrush = CreateSolidBrush(RGB(200, 150, 100)); // Light wood
            else
                hBrush = CreateSolidBrush(RGB(180, 130, 80)); // Dark wood
            FillRect(hdc, &board, hBrush);
            DeleteObject(hBrush);
        }

        // Draw the gutters
        for (int i = 0; i < 2; ++i)
        {
            RECT gutter = {
                ORIGIN_X,
                ORIGIN_Y + (i * (LANE_WIDTH_IN + GUTTER_WIDTH) * SCALE),
                ORIGIN_X + LANE_LEN_IN * SCALE,
                ORIGIN_Y - GUTTER_WIDTH * SCALE + (i * (LANE_WIDTH_IN + GUTTER_WIDTH) * SCALE)
            };

            HBRUSH hBrush;
            hBrush = CreateSolidBrush(RGB(0, 0, 0)); // Black

            FillRect(hdc, &gutter, hBrush);
            DeleteObject(hBrush);
        }

        // Draw the target arrows spaced 5 boards apart
        const double ARROW_SPACING_ACROSS_LANE_WIDTH = 5 * BOARD_WIDTH_PY; // 5 boards apart
        const double ARROW_SPACING_TIP_TO_TIP = 6 * SCALE; 
        const double ARROW_LEN_PX = 6 * SCALE;
        const double ARROW_DISTANCE = 16 * 12 * SCALE; // Distance from foul line to arrows (12-15 feet)
        const double ARROW_HALF_BASE_WIDTH_PX = 1.25 / 2 * SCALE;
        
        for (int i = 0; i < 7; ++i)
        {
            // Calculate the X/Y position of each arrow
            double arrowX = ORIGIN_X + ARROW_DISTANCE;
            double arrowY = ORIGIN_Y + ( (i + 1) * ARROW_SPACING_ACROSS_LANE_WIDTH);

            // Adjust X position to form the "V" shape
            if (i == 0 || i == 6) arrowX -= ARROW_SPACING_TIP_TO_TIP * 3;
            if (i == 1 || i == 5) arrowX -= ARROW_SPACING_TIP_TO_TIP * 2;
            if (i == 2 || i == 4) arrowX -= ARROW_SPACING_TIP_TO_TIP * 1;
            // Middle arrow stays centered

            POINT triangle[] = {
                { arrowX, arrowY },
                { arrowX - ARROW_LEN_PX, arrowY - ARROW_HALF_BASE_WIDTH_PX },
                { arrowX - ARROW_LEN_PX, arrowY + ARROW_HALF_BASE_WIDTH_PX }
            };

            // Create a solid brush (black arrows)
            HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0));
            HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

            // Draw the arrow
            Polygon(hdc, triangle, 3);

            // Clean up
            SelectObject(hdc, hOldBrush);
            DeleteObject(hBrush);
        }

        // Draw approach markers (12' and 15')
        SelectObject(hdc, hPen);

        // Foul Line
        MoveToEx(hdc, ORIGIN_X, ORIGIN_Y, NULL);
        LineTo(  hdc, ORIGIN_X, ORIGIN_Y + LANE_WIDTH_PY);
        
        // -12' markers
        MoveToEx(hdc, ORIGIN_X - (12 * 12 * SCALE), ORIGIN_Y, NULL);
        LineTo(  hdc, ORIGIN_X - (12 * 12 * SCALE), ORIGIN_Y + LANE_WIDTH_PY);

        // -15' markers
        MoveToEx(hdc, ORIGIN_X - (15 * 12 * SCALE), ORIGIN_Y, NULL);
        LineTo(  hdc, ORIGIN_X - (15 * 12 * SCALE), ORIGIN_Y + LANE_WIDTH_PY);

        DeleteObject(hPen);

        // Draw the pins (triangle formation)
        const double pi_const = 3.14159265358979323846;
        const double rowSpacing = cos(30 * pi_const/180.0) * PIN_SPACING_IN * SCALE;
        const double offset = PIN_SPACING_IN * SCALE / 2;

        for (int row = 0; row < 4; ++row)
        {
            for (int col = 0; col <= row; ++col)
            {
                int pinX = PIN_BASE_X_SCALED + (row * rowSpacing);
                int pinY = PIN_BASE_Y_SCALED - (row * offset) + (col * PIN_SPACING_IN * SCALE);

               // Create a solid white brush
               HBRUSH hPinBrush = CreateSolidBrush(RGB(255, 255, 255));
               HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hPinBrush);

               double current_relative_dia;
               for (int i=0; i < 2; i++) {
                    current_relative_dia = (i==0) ? PIN_OVERALL_DIA_PX : PIN_NECK_DIA_PX;

                    Ellipse(hdc,
                            pinX - current_relative_dia / 2,
                            pinY - current_relative_dia / 2,
                            pinX + current_relative_dia / 2,
                            pinY + current_relative_dia / 2);
                    DeleteObject(hPinBrush);
            }
        }
        }

        // Draw the ball
        HBRUSH hBallBrush = CreateSolidBrush(RGB(0, 0, 0));
        Ellipse(hdc,
            ball_x_scaled - (BALL_DIAMETER_PX / 2),
            ORIGIN_Y + (22.5 * BOARD_WIDTH_PY) - (BALL_DIAMETER_PX / 2),
            ball_x_scaled + (BALL_DIAMETER_PX / 2),
            ORIGIN_Y + (22.5 * BOARD_WIDTH_PY) + (BALL_DIAMETER_PX / 2));
        DeleteObject(hBallBrush);

        EndPaint(hwnd, &ps);
        break;

    }

    case WM_KEYDOWN:
    {
        if (wParam == VK_SPACE && !rolling)
        {
            rolling = true;
            SetTimer(hwnd, 1, .5, NULL); // Start the timer with 1 ms interval
        }
        break;
    }

    case WM_TIMER:
    {
        if (rolling)
        {
            // Update the ball's position based on speed and time per frame (X ms)
            ball_x_scaled += ball_speed_px * 0.005;  // Move the ball forward

            double ball_x_true = ball_x_scaled / SCALE;
            double ball_y_true = ball_y_scaled / SCALE;

            // Determine if the ball is hitting the head pin for now TODO *********  EXPAND THE CALC FOR ALL THE PINS
            calc_ball_to_pin_dist_in = sqrt( pow((abs(PIN_BASE_X_TRUE - ball_x_true)),2.0) +
                                             pow((abs(PIN_BASE_Y_TRUE - ball_y_true)),2.0) 
                                           );


            // Check if the ball has reached the pins
            if (calc_ball_to_pin_dist_in <= BALL_TO_PIN_IMPACT_IN )
            {
                KillTimer(hwnd, 1);  // Stop the timer
                rolling = false;     // Ball stops rolling
                ball_x_scaled = ORIGIN_X;   // Reset ball's position
                MessageBox(hwnd, "Strike!", "Bowling Game", MB_OK);  // Display message
            }

            InvalidateRect(hwnd, NULL, TRUE);  // Redraw the window
        }
        break;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    const char CLASS_NAME[] = "BowlingLaneWindow";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(0, CLASS_NAME, "Bowling Lane",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
        SCREEN_WIDTH, SCREEN_HEIGHT, NULL, NULL, hInstance, NULL);

    ShowWindow(hwnd, nCmdShow);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
