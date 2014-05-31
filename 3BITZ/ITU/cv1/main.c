//WinAPI ITU skelet
//HINT - this is how the key areas of the code are marked

#include <windows.h>
#include <stdio.h>
#include <string.h>

#include <math.h>

// Global variable

//HINT - global variables are put here

HINSTANCE hInst;
POINT p, a, b, c;
HBRUSH brush, oldbrush;
HBRUSH bg, oldbg;
HBITMAP hBitmap = NULL; 
HPEN pen, oldpen;
int color = 0;
RECT windowRect;


// Function prototypes.
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int);
LRESULT CALLBACK MainWndProc(HWND, UINT, WPARAM, LPARAM);

void onPaint(HWND hWnd);

// Application entry point. 

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    MSG msg;
    BOOL bRet;
	   WNDCLASS wcx;          // register class
    HWND hWnd;
    hInst = hInstance;     // Save the application-instance handle.
    
	// Fill in the window class structure with parameters that describe the main window.

    wcx.style = CS_HREDRAW | CS_VREDRAW;              // redraw if size changes
    wcx.lpfnWndProc = (WNDPROC) MainWndProc;          // points to window procedure
    wcx.cbClsExtra = 0;                               // no extra class memory
    wcx.cbWndExtra = 0;                               // no extra window memory
    wcx.hInstance = hInstance;                        // handle to instance
    wcx.hIcon = LoadIcon(NULL, IDI_APPLICATION);      // predefined app. icon
    wcx.hCursor = LoadCursor(NULL, IDC_ARROW);        // predefined arrow
	   wcx.hbrBackground = GetStockObject(WHITE_BRUSH);     // white background brush
    wcx.lpszMenuName =  (LPCSTR) "MainMenu";          // name of menu resource
    wcx.lpszClassName = (LPCSTR) "MainWClass";        // name of window class

	hBitmap = (HBITMAP)LoadImage(hInst, "minion.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	
    // Register the window class.

    if (!RegisterClass(&wcx)) return FALSE;

    // create window of registered class

    hWnd = CreateWindow(
        "MainWClass",        // name of window class
        "ITU cv.1",          // title-bar string
        WS_OVERLAPPEDWINDOW, // top-level window
        50,                  // default horizontal position
        100,                 // default vertical position
        500,                 // default width
        500,                 // default height
        (HWND) NULL,         // no owner window
        (HMENU) NULL,        // use class menu
        hInstance,           // handle to application instance
        (LPVOID) NULL);      // no window-creation data
    if (!hWnd) return FALSE;

		
		//HINT - if you need to initialize anything - HERE is the right spot
  // Show the window and send a WM_PAINT message to the window procedure.
  // Record the current cursor position.

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    // loop of message processing

    while( (bRet = GetMessage( &msg, NULL, 0, 0 )) != 0)
    {
        if (bRet == -1)
        {
            // handle the error and possibly exit
        }
        else
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
		//HINT - dont put anything else here - it would be processed by the end of the application run
    return (int) msg.wParam;
}

LRESULT CALLBACK MainWndProc(
    HWND hWnd,        // handle to window
    UINT uMsg,        // message identifier
    WPARAM wParam,    // first message parameter
    LPARAM lParam)    // second message parameter
{
	
	//HINT - put new case branches to crate the reactions to the mouse/keyboard input
    switch (uMsg)
    {
        case WM_CREATE:
            // Initialize the window.
            return 0;

        case WM_SIZE:
            // Set the size and position of the window.
						      //HINT - do we want to update the window during the size change???
            GetWindowRect(hWnd, &windowRect);
            return 0;

		      case WM_KEYDOWN:
            switch (wParam) {
                case VK_LEFT:   // LEFT ARROW
					               b.x--;
                    break;

                case VK_RIGHT:  // RIGHT ARROW
					               b.x++;
                    break;

				            case VK_UP:
					               b.y--;
					               break;

				            case VK_DOWN:
					               b.y++;
					               break;

				            case 0x08: //backspace
					               color = 1;
					               break;

				            case 0x1B: //esc
					               color = 0;
					               break;
            }
			         InvalidateRect(hWnd, NULL, TRUE);
            break;

        case WM_DESTROY:
            // Clean up window-specific data objects.
            PostQuitMessage(0);
            return 0;

				    case WM_MOUSEMOVE:
					       //HINT - do we care wheter the mouse is moving?
					       GetCursorPos(&a);
					       ScreenToClient(hWnd, &a);
					       InvalidateRect(hWnd, NULL, TRUE);
					       return 0;

				    case WM_PAINT:
					       //HINT - the drawing of the window content MUST be placed only HERE - which function does that??
					       onPaint(hWnd);
					       return 0;

				    case WM_LBUTTONDOWN:
					       b.x = a.x;
				        b.y = a.y;
					       InvalidateRect(hWnd, NULL, TRUE);
					       break;

        //
        // Process other messages.
        //
        default:
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
}

void onPaint(HWND hWnd){
    BITMAP bitmap;
    HGDIOBJ oldBitmap;
	   PAINTSTRUCT ps;                 // information can be used to paint the client area of a window owned by that application
    HDC hDC, hdcMem;                // device context
    char text[100];

	   //HINT - THE CODE TO DRAW THE WINDOW CONTENT
	   //HINT - where should we call this function???
    hDC = BeginPaint(hWnd, &ps);    // prepares the specified window for painting
	
	   if (color == 0) {
		      brush = CreateSolidBrush(RGB(255, 20, 147));
		      oldbrush = SelectObject(hDC, brush); 
		      Rectangle(hDC, b.x, b.y, b.x+120, b.y+187);
	   }
	   else {
		      brush = CreateSolidBrush(RGB(148, 0, 211));
		      oldbrush = SelectObject(hDC, brush); 
		      Rectangle(hDC, b.x, b.y, b.x+120, b.y+187);
	   }

	   hdcMem = CreateCompatibleDC(hDC); 
    oldBitmap = SelectObject(hdcMem, hBitmap);
	   GetObject(hBitmap, sizeof(bitmap), &bitmap); 
    BitBlt(hDC, b.x+10, b.y+10, bitmap.bmWidth, bitmap.bmHeight, hdcMem, 0, 0, SRCCOPY); 
    SelectObject(hdcMem, oldBitmap); 
    DeleteDC(hdcMem);

	   if (color == 0) {
		      pen = CreatePen(0,3,RGB(148, 0, 211));
		      oldpen = SelectObject(hDC, pen);
	   }
	   else {
		      pen = CreatePen(0,3,RGB(255, 20, 147));
		      oldpen = SelectObject(hDC, pen);
	   }

	   //HINT  - some computation and use of drawing functions
	   //nakreslení èáry z bodu [ax, ay] do bodu [bx, by]
	   MoveToEx(hDC, b.x+60, b.y+93, NULL);
	   LineTo(hDC, a.x, a.y);

    sprintf(text, "Souradnice kurzoru: x: %d y: %d", a.x, a.y);
    TextOut(hDC, 0, windowRect.bottom-150, text, strlen(text));

	   //HINT  - also we sure need to discover the mouse location
		
    DeleteDC(hDC);                  // deletes the specified device context
    EndPaint(hWnd, &ps);            // marks the end of painting in the specified window

}
