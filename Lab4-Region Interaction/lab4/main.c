
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <math.h>
#include <time.h>
#include <sys/timeb.h>
// anything beginning with windows is the win32 library
#include <windows.h>
#include <wingdi.h>
#include <winuser.h>
#include <process.h>	/* needed for multithreading */
#include "resource.h"
#include "globals.h"

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPTSTR lpCmdLine, int nCmdShow)
	// the above args passed to the function are command line args
	// the last two args are similar to argc and argv
{
	MSG			msg; //event we are going to get from the OS
	HWND		hWnd;
	WNDCLASS	wc;
	// window class (below) - a set of rules about how we wanna draw stuff
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)WndProc; //pointer to a function. Event handler. 
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(hInstance, "ID_PLUS_ICON");
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = "ID_MAIN_MENU";
	wc.lpszClassName = "PLUS";

	//register with the OS
	if (!RegisterClass(&wc))
		return(FALSE);

	// use the window class to make a window of 400-by-400 size with horizontal and vertical scroll bars
	hWnd = CreateWindow("PLUS", "Region Interaction",
		WS_OVERLAPPEDWINDOW | WS_HSCROLL | WS_VSCROLL,
		CW_USEDEFAULT, 0, 400, 400, NULL, NULL, hInstance, NULL);
	if (!hWnd)
		return(FALSE);

	//show window on the screen
	ShowScrollBar(hWnd, SB_BOTH, FALSE);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	MainWnd = hWnd;

	//ShowPixelCoords = 0; //control whether to draw the red dots
	//ShowBigDots = 0;
	RegionPlay = 0;
	ColorsRed = 0;
	ColorsYellow = 0;
	ColorsBlue = 0;
	ColorsGreen = 0;
	ResetRegion = 0;
	RegionStep = 0;

	strcpy(filename, "");
	OriginalImage = NULL;
	ROWS = COLS = 0;

	InvalidateRect(hWnd, NULL, TRUE);
	UpdateWindow(hWnd);

	//event loop
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return(msg.wParam);
}



//takes in what window got the event, what type of message it was and parameters associated with the message 
//for example where was the button pressed on the window, which key was pressed 
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg,
	WPARAM wParam, LPARAM lParam)

{
	HMENU				hMenu;
	OPENFILENAME		ofn;
	FILE* fpt;
	HDC					hDC;
	char				header[320]/*, text[320]*/;
	int					BYTES /*xPos, yPos*/;

	switch (uMsg)
	{
	case WM_COMMAND: //did we get a command from the menu
		switch (LOWORD(wParam))
		{
		case ID_COLORS_RED:
			ColorsRed = (ColorsRed + 1) % 2;
			ColorsYellow = ColorsBlue = ColorsGreen = 0;
			break;
		case ID_COLORS_YELLOW:
			ColorsYellow = (ColorsYellow + 1) % 2;
			ColorsRed = ColorsBlue = ColorsGreen = 0;
			break;
		case ID_COLORS_BLUE:
			ColorsBlue = (ColorsBlue + 1) % 2;
			ColorsYellow = ColorsRed = ColorsGreen = 0;
			break;
		case ID_COLORS_GREEN:
			ColorsGreen = (ColorsGreen + 1) % 2;
			ColorsYellow = ColorsBlue = ColorsRed = 0;
			break;
		case ID_RESETREGION:
			ResetRegion = (ResetRegion + 1) % 2;
			RegionPlay = 0;
			RegionStep = 0;
			PaintImage();
			break;
		case ID_DISPLAY_PLAY:
			RegionPlay = (RegionPlay + 1) % 2;
			ResetRegion = 0;
			RegionStep = 0;
			break;
		case ID_DISPLAY_STEP:
			RegionStep = (RegionStep + 1) % 2;
			ResetRegion = 0;
			RegionPlay = 0;
			break;
		case ID_GROWTHPREDICATES:
			DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG1), hWnd, AboutDlgProc);
			break;
		case ID_FILE_LOAD: //generates menu command with file load operation
			if (OriginalImage != NULL)
			{
				free(OriginalImage);
				OriginalImage = NULL;
			}
			memset(&(ofn), 0, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.lpstrFile = filename;
			filename[0] = 0;
			ofn.nMaxFile = MAX_FILENAME_CHARS;
			ofn.Flags = OFN_EXPLORER | OFN_HIDEREADONLY;
			ofn.lpstrFilter = "PPM files\0*.ppm\0All files\0*.*\0\0";
			if (!(GetOpenFileName(&ofn)) || filename[0] == '\0') //allows opening dialog box to load files
				break;		/* user cancelled load */
			if ((fpt = fopen(filename, "rb")) == NULL)
			{
				MessageBox(NULL, "Unable to open file", filename, MB_OK | MB_APPLMODAL);
				break;
			}
			fscanf(fpt, "%s %d %d %d", header, &COLS, &ROWS, &BYTES);
			if (strcmp(header, "P5") != 0 || BYTES != 255)
			{
				MessageBox(NULL, "Not a PPM (P5 greyscale) image", filename, MB_OK | MB_APPLMODAL);
				fclose(fpt);
				break;
			}
			OriginalImage = (unsigned char*)calloc(ROWS * COLS, 1);
			header[0] = fgetc(fpt);	/* whitespace character after header */
			fread(OriginalImage, 1, ROWS * COLS, fpt);
			fclose(fpt);
			SetWindowText(hWnd, filename);
			PaintImage();
			break;

		case ID_FILE_QUIT:
			DestroyWindow(hWnd);
			break;
		}
		break;
	case WM_SIZE:		  /* could be used to detect when window size changes */
		PaintImage();
		return(DefWindowProc(hWnd, uMsg, wParam, lParam));
		break;
	case WM_PAINT:
		PaintImage();
		return(DefWindowProc(hWnd, uMsg, wParam, lParam));
		break;
	case WM_LBUTTONDOWN:case WM_RBUTTONDOWN:
		if(RegionPlay==1)
		{
			ThreadCol = LOWORD(lParam);
			ThreadRow = HIWORD(lParam);
			ThreadRunning = 1;
			flag = 1;

			_beginthread(ExplosionThread, 0, MainWnd);
		}

		if (RegionStep == 1)
		{
			ThreadCol = LOWORD(lParam);
			ThreadRow = HIWORD(lParam);
			ThreadRunning = 1;
			flag = 1;

			_beginthread(ExplosionThread, 0, MainWnd);
		}

		//left and right mouse presses
		return(DefWindowProc(hWnd, uMsg, wParam, lParam));
		break;
	case WM_MOUSEMOVE: //move mouse
		//if (ShowPixelCoords == 1) //only when show pixel co-ordinates is true
		//{
		//	//pull x and y parameters from the message
		//	xPos = LOWORD(lParam);
		//	yPos = HIWORD(lParam);
		//	if (xPos >= 0 && xPos < COLS && yPos >= 0 && yPos < ROWS) //check if within the boundary of the image
		//	{
		//		sprintf(text, "%d,%d=>%d     ", xPos, yPos, OriginalImage[yPos * COLS + xPos]); //get the pixel value at the location
		//		hDC = GetDC(MainWnd);
		//		TextOut(hDC, 0, 0, text, strlen(text));		/* draw text on the window */
		//		if (ShowBigDots == 1) {
		//			for (int r = yPos - 2;r <= yPos + 2;r++) {
		//				for (int c = xPos - 2;c <= xPos + 2;c++) {
		//					SetPixel(hDC, c, r, RGB(255, 0, 0));
		//				}
		//			}
		//		}
		//		else {
		//			SetPixel(hDC, xPos, yPos, RGB(255, 0, 0));	/* color the cursor position red */
		//		}

		//		ReleaseDC(MainWnd, hDC);
		//	}
		//}
		return(DefWindowProc(hWnd, uMsg, wParam, lParam));
		break;
	case WM_KEYDOWN: //key presses
		if (wParam == 's' || wParam == 'S')
			PostMessage(MainWnd, WM_COMMAND, ID_SHOWPIXELCOORDS, 0);	  /* send message to self */
		if (wParam == 'j' || wParam == 'J') {
			flag = 1;
		}
		if ((TCHAR)wParam == '1') //1 key is going to create a timer that will get an event every 10ms using TIMER_SECOND
		{
			TimerRow = TimerCol = 0;
			SetTimer(MainWnd, TIMER_SECOND, 10, NULL);	/* start up 10 ms timer */
		}
		if ((TCHAR)wParam == '2')
		{
			KillTimer(MainWnd, TIMER_SECOND);			/* halt timer, stopping generation of WM_TIME events */
			PaintImage();								/* redraw original image, erasing animation */
		}
		if ((TCHAR)wParam == '3')
		{
			ThreadRunning = 1;
			_beginthread(AnimationThread, 0, MainWnd);	/* start up a child thread to do other work while this thread continues GUI */
			//_beginthread(ExplosionThread, 0, MainWnd);
		}
		if ((TCHAR)wParam == '4')
		{
			ThreadRunning = 0;							/* this is used to stop the child thread (see its code below) */
		}
		return(DefWindowProc(hWnd, uMsg, wParam, lParam));
		break;
	case WM_TIMER:	  /* this event gets triggered every time the timer goes off */
		hDC = GetDC(MainWnd);
		SetPixel(hDC, TimerCol, TimerRow, RGB(0, 0, 255));	/* color the animation pixel blue */
		ReleaseDC(MainWnd, hDC);
		TimerRow++;
		TimerCol += 2;
		break;
	case WM_HSCROLL:	  /* this event could be used to change what part of the image to draw */
		PaintImage();	  /* direct PaintImage calls eliminate flicker; the alternative is InvalidateRect(hWnd,NULL,TRUE); UpdateWindow(hWnd); */
		return(DefWindowProc(hWnd, uMsg, wParam, lParam));
		break;
	case WM_VSCROLL:	  /* this event could be used to change what part of the image to draw */
		PaintImage();
		return(DefWindowProc(hWnd, uMsg, wParam, lParam));
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return(DefWindowProc(hWnd, uMsg, wParam, lParam));
		break;
	}
	//this puts a check mark or uncheck mark on the show pixel co-ords option
	hMenu = GetMenu(MainWnd);
	if (ColorsRed == 1)
		CheckMenuItem(hMenu, ID_COLORS_RED, MF_CHECKED);
	else
		CheckMenuItem(hMenu, ID_COLORS_RED, MF_UNCHECKED);
	if (ColorsYellow == 1)
		CheckMenuItem(hMenu, ID_COLORS_YELLOW, MF_CHECKED);
	else
		CheckMenuItem(hMenu, ID_COLORS_YELLOW, MF_UNCHECKED);
	if (ColorsBlue == 1)
		CheckMenuItem(hMenu, ID_COLORS_BLUE, MF_CHECKED);
	else
		CheckMenuItem(hMenu, ID_COLORS_BLUE, MF_UNCHECKED);
	if (ColorsGreen == 1)
		CheckMenuItem(hMenu, ID_COLORS_GREEN, MF_CHECKED);
	else
		CheckMenuItem(hMenu, ID_COLORS_GREEN, MF_UNCHECKED);
	if (RegionPlay == 1)
		CheckMenuItem(hMenu, ID_DISPLAY_PLAY, MF_CHECKED);
	else
		CheckMenuItem(hMenu, ID_DISPLAY_PLAY, MF_UNCHECKED);
	if (RegionStep == 1)
		CheckMenuItem(hMenu, ID_DISPLAY_STEP, MF_CHECKED);
	else
		CheckMenuItem(hMenu, ID_DISPLAY_STEP, MF_UNCHECKED);
	if (ResetRegion == 1) {
		CheckMenuItem(hMenu, ID_RESETREGION, MF_BYCOMMAND);
		ThreadRunning = 0;
	}
		
	DrawMenuBar(hWnd);

	return(0L);
}

BOOL CALLBACK AboutDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam)
{
	BOOL *Val1, *Val2;
	switch (Message)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			predicateA = GetDlgItemInt(hDlg, IDC_EDIT1, &Val1, value);
			if (!Val1)
			{
				MessageBox(hDlg, "Couldn't retrieve Predicate 1", NULL, MB_OK);
				SendDlgItemMessage(hDlg, IDC_EDIT1, EM_SETSEL, 0, -1l);
			}
			predicateB = GetDlgItemInt(hDlg, IDC_EDIT2, &Val2, value);
			if (!Val2)
			{
				MessageBox(hDlg, "Couldn't retrieve Predicate 2", NULL, MB_OK);
				SendDlgItemMessage(hDlg, IDC_EDIT2, EM_SETSEL, 0, -1l);
			}
			EndDialog(hDlg, IDOK);
			return TRUE;
		case IDCANCEL:
			EndDialog(hDlg, IDCANCEL);
			return TRUE;
		}
		break;
	case WM_CLOSE:
		PostQuitMessage(0);
	default:
		return FALSE;
	}
	return FALSE;
}


void PaintImage()

{
	PAINTSTRUCT			Painter;
	HDC					hDC;
	BITMAPINFOHEADER	bm_info_header;
	BITMAPINFO* bm_info;
	int					i, r, c, DISPLAY_ROWS, DISPLAY_COLS;
	unsigned char* DisplayImage;

	if (OriginalImage == NULL)
		return;		/* no image to draw */

			  /* Windows pads to 4-byte boundaries.  We have to round the size up to 4 in each dimension, filling with black. */
	DISPLAY_ROWS = ROWS;
	DISPLAY_COLS = COLS;
	if (DISPLAY_ROWS % 4 != 0)
		DISPLAY_ROWS = (DISPLAY_ROWS / 4 + 1) * 4;
	if (DISPLAY_COLS % 4 != 0)
		DISPLAY_COLS = (DISPLAY_COLS / 4 + 1) * 4;
	DisplayImage = (unsigned char*)calloc(DISPLAY_ROWS * DISPLAY_COLS, 1);
	for (r = 0; r < ROWS; r++)
		for (c = 0; c < COLS; c++)
			DisplayImage[r * DISPLAY_COLS + c] = OriginalImage[r * COLS + c];

	BeginPaint(MainWnd, &Painter);
	hDC = GetDC(MainWnd);
	bm_info_header.biSize = sizeof(BITMAPINFOHEADER);
	bm_info_header.biWidth = DISPLAY_COLS;
	bm_info_header.biHeight = -DISPLAY_ROWS;
	bm_info_header.biPlanes = 1;
	bm_info_header.biBitCount = 8;
	bm_info_header.biCompression = BI_RGB;
	bm_info_header.biSizeImage = 0;
	bm_info_header.biXPelsPerMeter = 0;
	bm_info_header.biYPelsPerMeter = 0;
	bm_info_header.biClrUsed = 256;
	bm_info_header.biClrImportant = 256;
	bm_info = (BITMAPINFO*)calloc(1, sizeof(BITMAPINFO) + 256 * sizeof(RGBQUAD));
	bm_info->bmiHeader = bm_info_header;
	for (i = 0; i < 256; i++)
	{
		bm_info->bmiColors[i].rgbBlue = bm_info->bmiColors[i].rgbGreen = bm_info->bmiColors[i].rgbRed = i;
		bm_info->bmiColors[i].rgbReserved = 0;
	}

	SetDIBitsToDevice(hDC, 0, 0, DISPLAY_COLS, DISPLAY_ROWS, 0, 0,
		0, /* first scan line */
		DISPLAY_ROWS, /* number of scan lines */
		DisplayImage, bm_info, DIB_RGB_COLORS);
	ReleaseDC(MainWnd, hDC);
	EndPaint(MainWnd, &Painter);

	free(DisplayImage);
	free(bm_info);
}


void AnimationThread(HWND AnimationWindowHandle)

{
	HDC		hDC;
	char	text[300];

	ThreadRow = ThreadCol = 0;
	while (ThreadRunning == 1)
	{
		hDC = GetDC(MainWnd);
		SetPixel(hDC, ThreadCol, ThreadRow, RGB(0, 255, 0));	/* color the animation pixel green */
		sprintf(text, "%d,%d     ", ThreadRow, ThreadCol);
		TextOut(hDC, 300, 0, text, strlen(text));		/* draw text on the window */
		ReleaseDC(MainWnd, hDC);
		ThreadRow += 3;
		ThreadCol++;
		Sleep(100);		/* pause 100 ms */
	}
}

void ExplosionThread(HWND AnimationWindowHandle)

{
	int CenterRow, CenterCol, BoxWidth;
	BoxWidth = 3;
	CenterCol = ThreadCol;
	CenterRow = ThreadRow;
	unsigned char* labels;
	int	Display_Rows, Display_Cols;
	int* indices, i;
	int	RegionSize, TotalRegions;
	//double	avg, var;

	Display_Rows = ROWS;
	Display_Cols = COLS;

	/* segmentation image = labels; calloc initializes all labels to 0 */
	labels = (unsigned char *)calloc(Display_Rows*Display_Cols, sizeof(unsigned char));

	/* used to quickly erase small grown regions */
	indices = (int *)calloc(Display_Rows*Display_Cols, sizeof(int));

	TotalRegions = 0;
	TotalRegions++;
	RegionGrow(OriginalImage, labels, Display_Rows, Display_Cols, CenterRow, CenterCol, 0, TotalRegions, indices, &RegionSize);

	if (RegionSize < 100)
	{	/* erase region (relabel pixels back to 0) */
		for (i = 0; i < RegionSize; i++)
			labels[indices[i]] = 0;
		TotalRegions--;
	}
}

/*
** Given an image, a starting point, and a label, this routine
** paint-fills (8-connected) the area with the given new label
** according to the given criteria (pixels close to the average
** intensity of the growing region are allowed to join).
*/
void RegionGrow(unsigned char* OriginalImage,	/* image data */
	unsigned char* labels,	/* segmentation labels */
	int ROWS, int COLS,	/* size of image */
	int r, int c,		/* pixel to paint from */
	int paint_over_label,	/* image label to paint over */
	int new_label,		/* image label for painting */
	int* indices,		/* output:  indices of pixels painted */
	int* count)		/* output:  count of pixels painted */
{
	HDC hDC;
	int	r2, c2;
	int	queue[MAX_QUEUE], qh, qt, distance;
	int	average, total;	/* average and total intensity in growing region */

	*count = 0;
	if (labels[r * COLS + c] != paint_over_label)
		return;
	labels[r * COLS + c] = new_label;
	average = total = (int)OriginalImage[r * COLS + c];
	if (indices != NULL)
		indices[0] = r * COLS + c;
	queue[0] = r * COLS + c;
	qh = 1;	/* queue head */
	qt = 0;	/* queue tail */
	(*count) = 1;
	while (qt != qh && ThreadRunning == 1)
	{
		if (flag == 1) {
			if ((*count) % 50 == 0)	/* recalculate average after each 50 pixels join */
			{
				average = total / (*count);
				// printf("new avg=%d\n",average);
			}
			for (r2 = -1; r2 <= 1; r2++)
				for (c2 = -1; c2 <= 1; c2++)
				{
					if (r2 == 0 && c2 == 0)
						continue;
					if ((queue[qt] / COLS + r2) < 0 || (queue[qt] / COLS + r2) >= ROWS ||
						(queue[qt] % COLS + c2) < 0 || (queue[qt] % COLS + c2) >= COLS)
						continue;
					if (labels[(queue[qt] / COLS + r2) * COLS + queue[qt] % COLS + c2] != paint_over_label)
						continue;
					/* test criteria to join region */
					if (abs((int)(OriginalImage[(queue[qt] / COLS + r2) * COLS + queue[qt] % COLS + c2])
						- average) > predicateA)
						continue;
					distance = sqrt(pow((r - (queue[qt] / COLS + r2)), 2) + pow((c - (queue[qt] % COLS + c2)), 2));
					if (distance > predicateB)
						continue;
					labels[(queue[qt] / COLS + r2) * COLS + queue[qt] % COLS + c2] = new_label;

					hDC = GetDC(MainWnd);
					if (ColorsRed == 1)
						SetPixel(hDC, (queue[qt] % COLS + c2), (queue[qt] / COLS + r2), RGB(255, 0, 0));
					else if (ColorsYellow == 1)
						SetPixel(hDC, (queue[qt] % COLS + c2), (queue[qt] / COLS + r2), RGB(255, 255, 0));
					else if (ColorsBlue == 1)
						SetPixel(hDC, (queue[qt] % COLS + c2), (queue[qt] / COLS + r2), RGB(0, 0, 255));
					else if (ColorsGreen == 1)
						SetPixel(hDC, (queue[qt] % COLS + c2), (queue[qt] / COLS + r2), RGB(34, 139, 34));
					ReleaseDC(MainWnd, hDC);
					if (indices != NULL)
						indices[*count] = (queue[qt] / COLS + r2) * COLS + queue[qt] % COLS + c2;
					total += OriginalImage[(queue[qt] / COLS + r2) * COLS + queue[qt] % COLS + c2];
					(*count)++;
					queue[qh] = (queue[qt] / COLS + r2) * COLS + queue[qt] % COLS + c2;
					qh = (qh + 1) % MAX_QUEUE;

					if (qh == qt)
					{
						printf("Max queue size exceeded\n");
						return;
					}
				}
			qt = (qt + 1) % MAX_QUEUE;
			if (RegionPlay == 1)
				Sleep(1);
			else if (RegionStep == 1)
				flag = 0;
		}
		if (RegionPlay == 1)
			flag = 1;
	}
}