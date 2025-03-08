#include <windows.h>

const double LANE_WIDTH_INCHES = 41.5;
const double BALL_DIAMETER_INCHES = 8.5;
const double PIN_DIAMETER_INCHES = 4.766;
const int TOTAL_BOARDS = 39;
const double FOUL_LINE_TO_PIN_01_IN = 60 * 12;
const double LANE_LEN_IN = FOUL_LINE_TO_PIN_01_IN + 36.18754; // 60 feet from foul line to pins
const double APPROACH_LEN_IN = 15 * 12 + 8; // 15'-8" approach
const double LANE_AND_APPROACH_LEN_IN = LANE_LEN_IN + APPROACH_LEN_IN;
const int ARROW_SPACING_BOARDS = 5;
const int PIN_SPACING_INCHES = 12;
const double GUTTER_WIDTH = 9.25;


// Scale factor: 1 inch = X pixels
const int SCREEN_WIDTH = 2560;
const int SCREEN_WIDTH_MARGINS = SCREEN_WIDTH * 0.05;
const int SCREEN_WIDTH_AVAIL = SCREEN_WIDTH - SCREEN_WIDTH_MARGINS;
const int SCREEN_HEIGHT = 1440;
const int SCALE = SCREEN_WIDTH_AVAIL / LANE_AND_APPROACH_LEN_IN;
const int LANE_WIDTH_PX = LANE_WIDTH_INCHES * SCALE;
const int BALL_DIAMETER_PX = BALL_DIAMETER_INCHES * SCALE;
const int PIN_DIAMETER_PX = PIN_DIAMETER_INCHES * SCALE;
const int BOARD_WIDTH_PX = LANE_WIDTH_PX / TOTAL_BOARDS;

// Define the origin as the foul line and the edge of lane
int SCREEN_COORD_OFFSET_X = SCREEN_WIDTH_MARGINS / 2 + APPROACH_LEN_IN;
int ORIGIN_X = 0;  // Left edge of the lane
int ORIGIN_Y = 0;   // Foul line (adjust as needed)

int ballY = (APPROACH_LEN_IN + LANE_LEN_IN) * SCALE - 50;
bool rolling = false;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        // Draw the border
        for (int i = 0; i < TOTAL_BOARDS; ++i)
        {
            RECT board = {
                ORIGIN_X - APPROACH_LEN_IN * SCALE,
                ORIGIN_Y + i * BOARD_WIDTH_PX,
                ORIGIN_Y +  LANE_LEN_IN * SCALE,
                ORIGIN_X + (i + 1) * BOARD_WIDTH_PX
            };

            HBRUSH hBrush;
            hBrush = CreateSolidBrush(RGB(0, 0, 0)); 

            FillRect(hdc, &board, hBrush);
            DeleteObject(hBrush);
        }        

        // Draw the boards
        for (int i = 0; i < TOTAL_BOARDS; ++i)
        {
            RECT board = {
                ORIGIN_X - APPROACH_LEN_IN * SCALE,
                ORIGIN_Y + i * BOARD_WIDTH_PX,
                ORIGIN_Y +  LANE_LEN_IN * SCALE,
                ORIGIN_X + (i + 1) * BOARD_WIDTH_PX
            };

            HBRUSH hBrush;
            if (i % 2 == 0)
                hBrush = CreateSolidBrush(RGB(200, 150, 100)); // Light wood
            else
                hBrush = CreateSolidBrush(RGB(180, 130, 80)); // Dark wood

            FillRect(hdc, &board, hBrush);
            DeleteObject(hBrush);
        }

        // Draw approach markers (12' and 15')
        HPEN hPen = CreatePen(PS_DOT, 2, RGB(0, 0, 0));
        SelectObject(hdc, hPen);
        int approachOffset = APPROACH_LEN_IN * SCALE;

        // 15' marker
        MoveToEx(hdc, ORIGIN_X, ORIGIN_Y + approachOffset - (15 * 12 * SCALE), NULL);
        LineTo(hdc, ORIGIN_X + LANE_WIDTH_PX, ORIGIN_Y + approachOffset - (15 * 12 * SCALE));

        // 12' marker
        MoveToEx(hdc, ORIGIN_X, ORIGIN_Y + approachOffset - (12 * 12 * SCALE), NULL);
        LineTo(hdc, ORIGIN_X + LANE_WIDTH_PX, ORIGIN_Y + approachOffset - (12 * 12 * SCALE));
        DeleteObject(hPen);

        // Draw the pins (triangle formation)
        int pinBaseY = ORIGIN_Y + LANE_LEN_IN * SCALE;
        int pinBaseX = ORIGIN_X + (LANE_WIDTH_PX / 2);
        int rowSpacing = PIN_SPACING_INCHES * SCALE;
        int offset = PIN_SPACING_INCHES * SCALE / 2;

        for (int row = 0; row < 4; ++row)
        {
            for (int col = 0; col <= row; ++col)
            {
                int pinX = pinBaseX - (row * offset) + (col * PIN_SPACING_INCHES * SCALE);
                int pinY = pinBaseY - (row * rowSpacing);

                HBRUSH hPinBrush = CreateSolidBrush(RGB(255, 255, 255));
                Ellipse(hdc,
                    pinX - PIN_DIAMETER_PX / 2,
                    pinY - PIN_DIAMETER_PX / 2,
                    pinX + PIN_DIAMETER_PX / 2,
                    pinY + PIN_DIAMETER_PX / 2);
                DeleteObject(hPinBrush);
            }
        }

        // Draw the ball
        HBRUSH hBallBrush = CreateSolidBrush(RGB(0, 0, 0));
        Ellipse(hdc,
            ORIGIN_X + (LANE_WIDTH_PX / 2) - (BALL_DIAMETER_PX / 2),
            ballY,
            ORIGIN_X + (LANE_WIDTH_PX / 2) + (BALL_DIAMETER_PX / 2),
            ballY + BALL_DIAMETER_PX);
        DeleteObject(hBallBrush);

        EndPaint(hwnd, &ps);
        break;
    }

    case WM_KEYDOWN:
    {
        if (wParam == VK_SPACE && !rolling)
        {
            rolling = true;
            SetTimer(hwnd, 1, 30, NULL);
        }
        break;
    }

    case WM_TIMER:
    {
        if (rolling)
        {
            ballY -= 5;
            if (ballY <= 50)
            {
                KillTimer(hwnd, 1);
                rolling = false;
                ballY = (APPROACH_LEN_IN + LANE_LEN_IN) * SCALE - 50;
                MessageBox(hwnd, "Strike!", "Bowling Game", MB_OK);
            }
            InvalidateRect(hwnd, NULL, TRUE);
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
