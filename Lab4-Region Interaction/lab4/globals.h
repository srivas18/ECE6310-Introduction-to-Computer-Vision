
#define SQR(x) ((x)*(x))	/* macro for square */
#ifndef M_PI			/* in case M_PI not found in math.h */
#define M_PI 3.1415927
#endif
#ifndef M_E
#define M_E 2.718282
#endif
#define MAX_QUEUE 10000	/* max perimeter size (pixels) of border wavefront */
#define MAX_FILENAME_CHARS	320

char	filename[MAX_FILENAME_CHARS];

HWND	MainWnd;

		// Display flags
int		ColorsRed,ColorsYellow,ColorsBlue,ColorsGreen,RegionPlay,RegionStep,ResetRegion;
int		flag = 0;
		// Image data
unsigned char	*OriginalImage;
int				ROWS,COLS;

#define TIMER_SECOND	1			/* ID of timer used for animation */

		// Drawing flags
int		TimerRow,TimerCol;
int		ThreadRow,ThreadCol;
int		ThreadRunning;
int		predicateA, predicateB;
BOOL	value;


//FILE* fpt;
//char		header[80];
		// Function prototypes
LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);
INT_PTR CALLBACK AboutDlgProc(HWND, UINT, WPARAM, LPARAM);
void PaintImage();
void AnimationThread(void *);		/* passes address of window */
void ExplosionThread(void *);
void RegionGrow();