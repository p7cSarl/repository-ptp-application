/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++}
{						OPENGL APP DEMO VERSION 1.0					        }
{       Writen by Leon de Boer, Perth, Western Australia, 2016.				}
{	  	contact: ldeboer@gateway.net.au										}
{																			}
{       Copyright released on Code Project Open License (CPOL) and use      }
{       and/or abuse is freely given :-)									}
{																			}
{        THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND		}
{++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*==========================================================================
BACKGROUND TO CODE

This code is is C syntax rather than C++ or C# code and contains no objects
or sophisticated elements. That is not because those techniques don't have
merit or a place but simply because this code is targetted as a learning
tool to the widest audience. Anyone proficient in C++ or C# could easily
convert this code to those formats.

==========================================================================*/

#define _WIN32_WINNT 0x0500
#include <windows.h>		// Standard windows headers
#include <tchar.h>			// Unicode support	.. we will use TCHAR rather than char	
#include <commctrl.h>		// Common controls dialogs unit
#include <gl\gl.h>			// Header File For The OpenGL32 Library
#include <gl\glu.h>			// Header File For The GLu32 Library


#include <stdio.h>
#include <strsafe.h>

// This is the lazy adding libraries via #pragma rather than in linker includes
// If you are not on visual studio you will need to add the librarys via your linker
#pragma comment(lib,"ComCtl32.lib")
#pragma comment(lib,"OpenGl32.lib")
#pragma comment(lib,"GLU32.lib")

typedef struct
{
	GLfloat Time;
	GLfloat P1;
	GLfloat P2;
	GLfloat P3;
	GLfloat P4;
	GLfloat P5;
	GLfloat P6;
	GLfloat P7;
	GLfloat P8;
	GLfloat P9;
	GLfloat P10;
	GLfloat P11;
	GLfloat P12;
	GLfloat P13;
	GLfloat P14;
	GLfloat P15;
} PTPData;

int ReadFromFile();
int DrawBackGround(BOOL draw, GLfloat MaxLimit);
int DrawAxis(BOOL draw, GLfloat MaxLimit);
void ReadPTPDataFromPTPFile(TCHAR* FileName);
void ReadFilePTP(TCHAR* FileName);
BOOL LoadFileAndRetrieveProfile(HWND hEdit, LPCTSTR pszFileName);


PTPData	 ListOfPTPData[6000] = { 0 };
int numLines = 0;
GLfloat ProfileDistance = 1.0f;
HWND hWndEdit;

BOOL DisplayBackground = 0;

BOOL DisplayProfil_1 = 1;
BOOL DisplayProfil_2 = 1;
BOOL DisplayProfil_3 = 1;
BOOL DisplayProfil_4 = 1;
BOOL DisplayProfil_5 = 1;
BOOL DisplayProfil_6 = 1;
BOOL DisplayProfil_7 = 1;
BOOL DisplayProfil_8 = 1;
BOOL DisplayProfil_9 = 1;
BOOL DisplayProfil_10 = 1;
BOOL DisplayProfil_11 = 1;
BOOL DisplayProfil_12 = 1;
BOOL DisplayProfil_13 = 1;
BOOL DisplayProfil_14 = 1;
BOOL DisplayProfil_15 = 1;

/***************************************************************************
APP SPECIFIC INTERNAL CONSTANTS
***************************************************************************/

/*--------------------------------------------------------------------------}
;{                   MAIN MENU COMMAND VALUE CONSTANTS			            }
;{-------------------------------------------------------------------------*/
#define IDC_BMPLOAD 101									// App menu to load bitmap
#define IDC_PTPLOAD 102									// App menu to load PTP File
#define IDC_EXIT 105									// App menu command to exit application
#define IDC_TIMERSTART 201								// App menu to start timer
#define IDC_TIMERSTOP 202								// App menu to stop timer
#define IDC_BUTTONPLUS 50								// Button Increment 
#define IDC_BUTTONMINUS 51								// Button Decrement 
#define IDC_PROFILEDISTANCE 52							// EDITBUTTON PROFILEDISTANCE 
#define IDC_PROFILEDISTANCE_PLUS 53							// EDITBUTTON PROFILEDISTANCE Increse 
#define IDC_PROFILEDISTANCE_MINUS 54							// EDITBUTTON PROFILEDISTANCE decrese

#define IDC_PROFILE_1_DISPLAY 300							// DISPLAY PROFILE 1
#define IDC_PROFILE_2_DISPLAY 301							// DISPLAY PROFILE 2
#define IDC_PROFILE_3_DISPLAY 302							// DISPLAY PROFILE 3

#define IDC_BACKGROUND_DISPLAY 350							// DISPLAY BACKGROUND 3
#define BUF_SIZE 40				

//------------Begin Read PTP File--------Open Dialog-------------
#define BUFFERSIZE 528000
DWORD g_BytesTransferred = 0;
VOID CALLBACK FileIOCompletionRoutine(__in  DWORD dwErrorCode, __in  DWORD dwNumberOfBytesTransfered,	__in  LPOVERLAPPED lpOverlapped);

VOID CALLBACK FileIOCompletionRoutine(__in  DWORD dwErrorCode, __in  DWORD dwNumberOfBytesTransfered,	__in  LPOVERLAPPED lpOverlapped)
{
	_tprintf(TEXT("Error code:\t%x\n"), dwErrorCode);
	_tprintf(TEXT("Number of bytes:\t%x\n"), dwNumberOfBytesTransfered);
	g_BytesTransferred = dwNumberOfBytesTransfered;
}

VOID CALLBACK readComplete(DWORD err, DWORD bytes, LPOVERLAPPED ovlp)
{
}
//-------------End Read PTP File----------------------------------


/*--------------------------------------------------------------------------}
;{                      APPLICATION STRING CONSTANTS			            }
;{-------------------------------------------------------------------------*/
const TCHAR* AppClassName = _T("OPENGL_DEMO_APP");
static const TCHAR* DATABASE_PROPERTY = _T("OurDataStructure");

/*---------------------------------------------------------------------------
OUR OPENGL DATA RECORD DEFINITION
---------------------------------------------------------------------------*/
typedef struct OpenGLData {
	HGLRC Rc;											// Our render context ** Always Needed
	GLuint glTexture;									// Our texture to draw
	GLfloat	xrot;										// X Rotation
	GLfloat	yrot;										// Y Rotation
} GLDATABASE;

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
COMMON DIALOG CALL ROUTINES
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*--------------------------------------------------------------------------
This uses file open common control from windows to select a file.
--------------------------------------------------------------------------*/
int OpenFileDialog(TCHAR* Name, unsigned short NameBufSize, TCHAR* String, TCHAR* Ext, TCHAR* Title, HWND Wnd) {
	int i;
	TCHAR FileName[256], DefExt[256], Filter[400];
	OPENFILENAME OpenFN;

	InitCommonControls();											// Initialize common dialogs
	Name[0] = 0;													// Preset name clear
	_tcscpy_s(&FileName[0], _countof(FileName), _T("*."));			// Tranfer "*."
	_tcscat_s(&FileName[0], _countof(FileName), Ext);				// Create "*.xxx" extension
	_tcscpy_s(Filter, _countof(Filter), String);					// Tranfer string
	i = _tcslen(Filter);											// Fetch that string length in TCHAR
	_tcscpy_s(&Filter[i + 1], _countof(Filter) - i - 1, &FileName[0]);	// Transfer "*.ext"
	i += (_tcslen(&FileName[0]) + 1);								// Advance to beyond end
	_tcscpy_s(&Filter[i + 1], _countof(Filter) - i - 1, _T("\0"));	// Must end with two 0 entries
	_tcscpy_s(&Filter[i + 2], _countof(Filter) - i - 2, _T("\0"));	// Must end with two 0 entries
	_tcscpy_s(DefExt, _countof(DefExt), Ext);						// Default ext name
	memset((void*)&OpenFN, 0, sizeof(OpenFN));					    // Zero unused fields
	OpenFN.lStructSize = sizeof(OPENFILENAME);					    // Size of structure
	OpenFN.hInstance = GetModuleHandle(NULL);						// Pointer to instance
	OpenFN.hwndOwner = Wnd;											// Owner window
	OpenFN.lpstrFilter = &Filter[0];								// Filter
	OpenFN.nFilterIndex = 1;										// 1st Filter String
	OpenFN.lpstrFile = &FileName[0];								// Return result
	OpenFN.nMaxFile = _countof(FileName);							// Max name length
	OpenFN.lpstrDefExt = Ext;										// Default extension
	OpenFN.lpstrFileTitle = &FileName[0];							// Default file title
	OpenFN.nMaxFileTitle = _countof(FileName);						// Max size of file title
	OpenFN.lpstrTitle = Title;	                     			    // Window title
	OpenFN.lpfnHook = NULL;											// No hook handler
	OpenFN.Flags = OFN_FILEMUSTEXIST | OFN_LONGNAMES;               // Set flag masks
	if (GetOpenFileName(&OpenFN) != 0) {
		_tcscpy_s(Name, NameBufSize, FileName);						// Return the name
		return OpenFN.nFilterIndex;									// Return filter type
	}
	else return 0;												// Dialog cancelled
};


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
OPENGL ROUTINES
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


/*-[ InitGL ]---------------------------------------------------------------

Initializes the OpenGL system for the provided window handle, this is a one
time call made for the window and the function returns the created Render
context for the window. The responsibility to delete the render context when
completed is placed on the caller. In this demo initialization will be called
from WM_CREATE from the application window. Failure of initialization will
return a render context handle equal to zero.

16Apr16 LdB
--------------------------------------------------------------------------*/
static HGLRC InitGL(HWND Wnd) {
	HGLRC ourOpenGLRC = 0;											// Preset render context to zero

																	//  We need to make sure the window create in a suitable DC format
	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, //Flags
		PFD_TYPE_RGBA,												// The kind of framebuffer. RGBA or palette.
		32,															// Colordepth of the framebuffer.
		0, 0, 0, 0, 0, 0,
		0,
		0,
		0,
		0, 0, 0, 0,
		24,															// Number of bits for the depthbuffer
		8,															// Number of bits for the stencilbuffer
		0,															// Number of Aux buffers in the framebuffer.
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	};

	HDC ourWindowHandleToDeviceContext = GetDC(Wnd);				// Get a DC for our window
	int letWindowsChooseThisPixelFormat = ChoosePixelFormat(ourWindowHandleToDeviceContext, &pfd); // Let windows select an appropriate pixel format
	if (SetPixelFormat(ourWindowHandleToDeviceContext, letWindowsChooseThisPixelFormat, &pfd)) { // Try to set that pixel format
		ourOpenGLRC = wglCreateContext(ourWindowHandleToDeviceContext);
		if (ourOpenGLRC != 0) {
			wglMakeCurrent(ourWindowHandleToDeviceContext, ourOpenGLRC); // Make our render context current
			glEnable(GL_TEXTURE_2D);								// Enable Texture Mapping
			glShadeModel(GL_SMOOTH);								// Enable Smooth Shading
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);					// Black Background
			glClearDepth(1.0f);										// Depth Buffer Setup
			glEnable(GL_DEPTH_TEST);								// Enables Depth Testing
			glDepthFunc(GL_LEQUAL);									// The Type Of Depth Testing To Do
			glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);		// Really Nice Perspective Calculations
		}
	}
	ReleaseDC(Wnd, ourWindowHandleToDeviceContext);					// Release the window device context we are done
	return (ourOpenGLRC);											// Return the render context
}


/*-[ ReSizeGLScene ]--------------------------------------------------------

Rescales the OpenGL system for a given size of screen, called at anytime
the Application resizes the window . It will call once after InitGL and
in this demo it is called from WM_WINDOWPOSCHANGED from the application
main window.

15Apr16 LdB
--------------------------------------------------------------------------*/
static void ReSizeGLScene(HWND Wnd) {
	GLDATABASE* db = (GLDATABASE*)GetProp(Wnd, DATABASE_PROPERTY); // Fetch the data base
	if (db == 0) return;											// Cant resize .. no render context
	HDC Dc = GetWindowDC(Wnd);										// Get the window DC
	RECT r;
	GetWindowRect(Wnd, &r);											// Fetch the window size
	int Width = r.right - r.left;									// Window width
	int Height = r.bottom - r.top;									// Window height
	if (Height == 0) Height = 1;									// Stop divid by zero
	wglMakeCurrent(Dc, db->Rc);										// Make our render context current
	glViewport(0, 0, Width, Height);								// Reset The Current Viewport
	glMatrixMode(GL_PROJECTION);									// Select The Projection Matrix
	glLoadIdentity();												// Reset The Projection Matrix
																	// Calculate The Aspect Ratio Of The Window
	gluPerspective(45.0f, (GLfloat)Width / (GLfloat)Height, 0.1f, 100.0f);

	glMatrixMode(GL_MODELVIEW);										// Select The Modelview Matrix
	glLoadIdentity();												// Reset The Modelview Matrix
	ReleaseDC(Wnd, Dc);												// Release the window DC
}


/*-[ DrawGLScene ]----------------------------------------------------------

This is where all the OpenGL drawing is done for each frame. In this demo
it will be called from WM_PAINT messages to the window

15Apr16 LdB
--------------------------------------------------------------------------*/
void DrawGLScene(GLDATABASE* db, HDC Dc) {
	//if ((db == 0) || (db->glTexture == 0)) return;					// Cant draw .. no render context
	//wglMakeCurrent(Dc, db->Rc);										// Make our render context current

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);				// Clear The Screen And The Depth Buffer
	glLoadIdentity();												// Reset The View
	glTranslatef(-5.0f, -5.0f, -15.0f);

	//glRotatef(db->xrot, 1.0f, 0.0f, 0.0f);
	glRotatef(db->yrot, 0.0f, 1.0f, 0.0f);

	//glBindTexture(GL_TEXTURE_2D, db->glTexture);

	DrawAxis(TRUE, 50.0f);
	DrawBackGround(DisplayBackground,20.0f);
	
	ReadFromFile();
	//ProfileDistance = 0.2f;
	GLfloat Tempi = 0.1f;
	GLfloat Tempi1 = 0.1f;
	for (int i = 0; i < numLines; i++)
	{
		if (DisplayProfil_1)
		{
			//Profil 1
			glBegin(GL_LINES);
			glColor3f(1.0f, 1.0f, 0.0f);
			glVertex3f(ListOfPTPData[i].Time, ListOfPTPData[i].P1, ProfileDistance);
			glVertex3f(ListOfPTPData[i + 1].Time, ListOfPTPData[i + 1].P1, ProfileDistance);
			glEnd();
		}


		if (DisplayProfil_2)
		{
			//Profi	2
			glBegin(GL_LINES);
			glColor3f(1.0f, 1.0f, 0.4f);
			glVertex3f(ListOfPTPData[i].Time, ListOfPTPData[i].P2, 2 * ProfileDistance);
			glVertex3f(ListOfPTPData[i + 1].Time, ListOfPTPData[i + 1].P2, 2 * ProfileDistance);
			glEnd();
		}

		if (DisplayProfil_3)
		{
			//Profi	3
			glBegin(GL_LINES);
			glColor3f(1.0f, 0.7f, 0.0f);
			glVertex3f(ListOfPTPData[i].Time, ListOfPTPData[i].P3, 3 * ProfileDistance);
			glVertex3f(ListOfPTPData[i + 1].Time, ListOfPTPData[i + 1].P3, 3 * ProfileDistance);
			glEnd();
		}

		if (DisplayProfil_4)
		{

			//Profi	4
			glBegin(GL_LINES);
			glColor3f(1.0f, 1.0f, 0.0f);
			glVertex3f(ListOfPTPData[i].Time, ListOfPTPData[i].P4, 4 * ProfileDistance);
			glVertex3f(ListOfPTPData[i + 1].Time, ListOfPTPData[i + 1].P4, 4 * ProfileDistance);
			glEnd();
		}

		if (DisplayProfil_5)
		{
			//Profi	5
			glBegin(GL_LINES);
			glColor3f(1.0f, 0.0f, 4.0f);
			glVertex3f(ListOfPTPData[i].Time, ListOfPTPData[i].P5, 5 * ProfileDistance);
			glVertex3f(ListOfPTPData[i + 1].Time, ListOfPTPData[i + 1].P5, 5 * ProfileDistance);
			glEnd();
		}

		if (DisplayProfil_6)
		{
			//Profi	6
			glBegin(GL_LINES);
			glColor3f(1.0f, 0.0f, 7.0f);
			glVertex3f(ListOfPTPData[i].Time, ListOfPTPData[i].P6, 6 * ProfileDistance);
			glVertex3f(ListOfPTPData[i + 1].Time, ListOfPTPData[i + 1].P6, 6 * ProfileDistance);
			glEnd();
		}

		if (DisplayProfil_7)
		{
			//Profi	7
			glBegin(GL_LINES);
			glColor3f(1.0f, 0.0f, 0.0f);
			glVertex3f(ListOfPTPData[i].Time, ListOfPTPData[i].P7, 7 * ProfileDistance);
			glVertex3f(ListOfPTPData[i + 1].Time, ListOfPTPData[i + 1].P7, 7 * ProfileDistance);
			glEnd();
		}

		if (DisplayProfil_8)
		{
			//Profi	8
			glBegin(GL_LINES);
			glColor3f(0.0f, 0.0f, 0.4f);
			glVertex3f(ListOfPTPData[i].Time, ListOfPTPData[i].P8, 8 * ProfileDistance);
			glVertex3f(ListOfPTPData[i + 1].Time, ListOfPTPData[i + 1].P8, 8 * ProfileDistance);
			glEnd();

		}

		if (DisplayProfil_9)
		{
			//Profi	9
			glBegin(GL_LINES);
			glColor3f(0.0f, 0.0f, 0.7f);
			glVertex3f(ListOfPTPData[i].Time, ListOfPTPData[i].P9, 9 * ProfileDistance);
			glVertex3f(ListOfPTPData[i + 1].Time, ListOfPTPData[i + 1].P9, 9 * ProfileDistance);
			glEnd();
		}

		if (DisplayProfil_10)
		{
			//Profi	10
			glBegin(GL_LINES);
			glColor3f(0.0f, 0.0f, 1.0f);
			glVertex3f(ListOfPTPData[i].Time, ListOfPTPData[i].P10, 10 * ProfileDistance);
			glVertex3f(ListOfPTPData[i + 1].Time, ListOfPTPData[i + 1].P10, 10 * ProfileDistance);
			glEnd();
		}

		if (DisplayProfil_11)
		{
			//Profi	11
			glBegin(GL_LINES);
			glColor3f(0.0f, 0.4f, 1.0f);
			glVertex3f(ListOfPTPData[i].Time, ListOfPTPData[i].P11, 11 * ProfileDistance);
			glVertex3f(ListOfPTPData[i + 1].Time, ListOfPTPData[i + 1].P11, 11 * ProfileDistance);
			glEnd();
		}

		if (DisplayProfil_12)
		{
			//Profi	12
			glBegin(GL_LINES);
			glColor3f(0.0f, 0.7f, 1.0f);
			glVertex3f(ListOfPTPData[i].Time, ListOfPTPData[i].P12, 12 * ProfileDistance);
			glVertex3f(ListOfPTPData[i + 1].Time, ListOfPTPData[i + 1].P12, 12 * ProfileDistance);
			glEnd();
		}

		if (DisplayProfil_13)
		{
			//Profi	13
			glBegin(GL_LINES);
			glColor3f(0.4f, 1.0f, 1.0f);
			glVertex3f(ListOfPTPData[i].Time, ListOfPTPData[i].P13, 13 * ProfileDistance);
			glVertex3f(ListOfPTPData[i + 1].Time, ListOfPTPData[i + 1].P13, 13 * ProfileDistance);
			glEnd();
		}

		if (DisplayProfil_14)
		{
			//Profi	14
			glBegin(GL_LINES);
			glColor3f(0.7f, 1.0f, 1.0f);
			glVertex3f(ListOfPTPData[i].Time, ListOfPTPData[i].P14, 14 * ProfileDistance);
			glVertex3f(ListOfPTPData[i + 1].Time, ListOfPTPData[i + 1].P14, 14 * ProfileDistance);
			glEnd();
		}

		if (DisplayProfil_15)
		{
			//Profi	15
			glBegin(GL_LINES);
			glColor3f(1.0f, 1.0f, 1.0f);
			glVertex3f(ListOfPTPData[i].Time, ListOfPTPData[i].P14, 15 * ProfileDistance);
			glVertex3f(ListOfPTPData[i + 1].Time, ListOfPTPData[i + 1].P14, 15 * ProfileDistance);
			glEnd();
		}
	}
}


/*-[ BMP2GLTexture ]--------------------------------------------------------

This converts a bitmap file from the filename path and converts it to an
OpenGL texture. If the filename is invalid of the file not a bitmap the
routine will return a GLuint of zero.

15Apr16 LdB
--------------------------------------------------------------------------*/
GLuint BMP2GLTexture(TCHAR* fileName, HWND Wnd, GLDATABASE* db) {
	HBITMAP hBMP;                                                   // Handle Of The Bitmap
	BITMAP  BMP;                                                    // Bitmap Structure

	HDC Dc = GetWindowDC(Wnd);										// Fetch the window DC
	if ((db != 0) && (db->Rc != 0)) wglMakeCurrent(Dc, db->Rc);		// Make our render context current

	hBMP = (HBITMAP)LoadImage(GetModuleHandle(NULL), fileName,
		IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);	// Load the bitmap from file
	if (!hBMP) return (0);											// If bitmap does not exist return false
	GetObject(hBMP, sizeof(BMP), &BMP);								// Get The bitmap details
	int P2Width = (BMP.bmWidth) >> 2;								// Divid width by 4
	if ((P2Width << 2) < (BMP.bmWidth)) P2Width++;					// Inc by 1 if width x 4 is less than original
	P2Width = P2Width << 2;											// Power of two width
	long imageSize = (long)P2Width * (long)BMP.bmHeight * sizeof(RGBQUAD);
	BYTE* lpPixels = (BYTE*)malloc(imageSize);						// Create the pixel buffer					

																	// Create and fill BITMAPINFO structure to pass to GetDIBits
	BITMAPINFO bmi;
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = BMP.bmWidth;
	bmi.bmiHeader.biHeight = BMP.bmHeight;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 24;
	bmi.bmiHeader.biCompression = 0;
	bmi.bmiHeader.biSizeImage = imageSize;
	bmi.bmiHeader.biXPelsPerMeter = 0;
	bmi.bmiHeader.biYPelsPerMeter = 0;
	bmi.bmiHeader.biClrUsed = 0;
	bmi.bmiHeader.biClrImportant = 0;

	// Put DIBBits into memory buffer
	GetDIBits(Dc, hBMP, 0, BMP.bmHeight, lpPixels, &bmi, DIB_RGB_COLORS);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);							// Pixel Storage Mode (Word Alignment / 4 Bytes)
	GLuint texture;
	glGenTextures(1, &texture);										// Create a GL texture

																	// Create Nearest Filtered Texture
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, BMP.bmWidth, BMP.bmHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, lpPixels);

	free(lpPixels);													// Free allocated pixel memory
	ReleaseDC(Wnd, Dc);												// Release the window DC
	DeleteObject(hBMP);												// Delete The Object
	return (texture);												// Return the texture
}

int DrawAxis(BOOL draw, GLfloat MaxLimit)
{
	if (draw)  //  activate or deactive  axis drawing
	{
		// L axe des Xint
		glColor3f(1.0f, 0.0f, 0.0f);
		glLineWidth(5.0);
		glBegin(GL_LINES);  //X axis
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(MaxLimit, 0.0, 0.0);
		glEnd();

		GLfloat a = 1;
		for (GLfloat u = 0; u <= MaxLimit; u+=a)
		{
			glLineWidth(3.0);
			glBegin(GL_LINES);  //X axis
			glVertex3f(u, -0.1, 0.0);
			glVertex3f(u, 0.1, 0.0);
			glEnd();

			for (GLfloat unit = 0; unit <= 10; unit += 0.1)
			{
				glLineWidth(1.0);
				glBegin(GL_LINES);  //X axis
				glVertex3f(u + unit, -0.07, 0.0);
				glVertex3f(u + unit, 0.07, 0.0);
				glEnd();
			}
		}

		// L axe des Y
		glColor3f(0.0f, 1.0f, 0.0f);
		glLineWidth(5.0);
		glBegin(GL_LINES);  //X axis
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(0.0, MaxLimit, 0.0);
		glEnd();

		// L axe des Z
		glColor3f(0.0f, 0.0f, 1.0f);
		glLineWidth(5.0);
		glBegin(GL_LINES);  //X axis
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(0.0, 0.0, MaxLimit);
		glEnd();
	}
	else
	{
		// The Axis won't be draw
	}
}
int DrawBackGround(BOOL draw, GLfloat MaxLimit)
{
	if (draw)
	{
		GLUquadricObj *quadric;
		quadric = gluNewQuadric();
		gluSphere(quadric, 0.2, 0.2, 0.2);

		//glBegin(GL_QUADS);
		//glColor3f(0.2f, 1.0f, 0.5f);
		//glVertex3f(0.0f, 0.0f, 0.0f);     // Green
		//glVertex3f(MaxLimit, 0.0f, 0.0f);
		//glVertex3f(MaxLimit, 0.0f, MaxLimit);
		//glVertex3f(0.0f, 0.0f, MaxLimit);
		//glEnd();

		//glBegin(GL_QUADS);
		//glColor3f(1.0f, 0.0f, 1.0f);
		//glVertex3f(0.0f, 0.0f, 0.0f);     // Green
		//glVertex3f(0.0f, 0.0f, MaxLimit);
		//glVertex3f(0.0f, MaxLimit, MaxLimit);
		//glVertex3f(0.0f, MaxLimit, 0.0f);
		//glEnd();

		//glBegin(GL_QUADS);
		//glColor3f(1.0f, 1.0f, 0.0f);
		//glVertex3f(0.0f, 0.0f, MaxLimit);     // Green
		//glVertex3f(MaxLimit, 0.0f, 2.0f);
		//glVertex3f(MaxLimit, MaxLimit, MaxLimit);
		//glVertex3f(0.0f, MaxLimit, MaxLimit);

		glBegin(GL_QUADS);
		glColor3f(0.2f, 1.0f, 0.5f);
		glVertex3f(0.0f, 0.0f, 0.0f);     // Green
		glVertex3f(MaxLimit, 0.0f, 0.0f);
		glVertex3f(MaxLimit, 0.0f, MaxLimit);
		glVertex3f(0.0f, MaxLimit, 0.0f);
		glEnd();

		glBegin(GL_QUADS);
		glColor3f(1.0f, 0.0f, 1.0f);
		glVertex3f(0.0f, 0.0f, 0.0f);     // Green
		glVertex3f(0.0f, 0.0f, MaxLimit);
		glVertex3f(0.0f, MaxLimit, MaxLimit);
		glVertex3f(0.0f, MaxLimit, 0.0f);
		glEnd();

		glBegin(GL_QUADS);
		glColor3f(1.0f, 1.0f, 0.0f);
		glVertex3f(0.0f, 0.0f, 0.0f);     // Green
		glVertex3f(MaxLimit, 0.0f, 0.0f);
		glVertex3f(MaxLimit, 0.0f, MaxLimit);
		glVertex3f(0.0f, 0.0f, MaxLimit);
		glEnd();
	}
	else
	{
		// Background Environment no draw
	}
	
}

int ReadFromFile()
{
	numLines = 10;
	for (int i = 0; i < numLines; i++)
	{
		ListOfPTPData[i].Time = i;
		ListOfPTPData[i].P1 = i;
		ListOfPTPData[i].P2 = i/2;
		ListOfPTPData[i].P3 = i/3;
		ListOfPTPData[i].P4 = i/4;
		ListOfPTPData[i].P5 = i*2;
		ListOfPTPData[i].P6 = i%9;
		ListOfPTPData[i].P7 = i-2;
		ListOfPTPData[i].P8 = i;
		ListOfPTPData[i].P9 = i;
		ListOfPTPData[i].P10 = i;
		ListOfPTPData[i].P11 = i;
		ListOfPTPData[i].P12 = i;
		ListOfPTPData[i].P13 = i;
		ListOfPTPData[i].P14 = i;
	}
}
/*---------------------------------------------------------------------------
Application handler.
---------------------------------------------------------------------------*/
LRESULT CALLBACK OpenGLDemoHandler(HWND Wnd, UINT Msg, WPARAM wParam, LPARAM lParam) {

	switch (Msg) {
	case WM_CREATE: {											// WM_CREATE MESSAGE

																//  First manually build a menu for a window
		HMENU SubMenu, Menu;
		Menu = CreateMenu();								// Create a menu and populate it
		SubMenu = CreatePopupMenu();
		//AppendMenu(SubMenu, MF_STRING, IDC_BMPLOAD, _T("&Load Bitmap"));
		//AppendMenu(SubMenu, MF_SEPARATOR, 0, NULL);
		AppendMenu(SubMenu, MF_STRING, IDC_PTPLOAD, _T("&Load PTP"));
		AppendMenu(SubMenu, MF_SEPARATOR, 0, NULL);
		AppendMenu(SubMenu, MF_STRING, IDC_EXIT, _T("E&xit"));
		AppendMenu(Menu, MF_POPUP, (UINT_PTR)SubMenu, _T("&File"));
		SubMenu = CreatePopupMenu();
		//AppendMenu(SubMenu, MF_STRING, IDC_TIMERSTART, _T("&Start Timer"));
		//AppendMenu(SubMenu, MF_STRING, IDC_TIMERSTOP, _T("Stop &Timer"));
		//AppendMenu(Menu, MF_POPUP, (UINT_PTR)SubMenu, _T("&Timer"));
		//SubMenu = CreatePopupMenu();
		/*AppendMenu(SubMenu, MF_STRING, 301, _T("&Vectorize"));
		AppendMenu(Menu, MF_POPUP, (UINT_PTR)SubMenu, _T("&Process"));*/
		SetMenu(Wnd, Menu);									// Set the menu to window

															// Make a data structure, initialize it and attach to Wnd
		GLDATABASE* db = (GLDATABASE*)malloc(sizeof(GLDATABASE)); // Allocate structure
		db->Rc = InitGL(Wnd);								// Initialize OpenGL and get render context
		db->glTexture = 0;									// Zero the texture
		db->xrot = 0.0f;									// Zero x rotation
		db->yrot = 0.0f;									// Zero y rotation
		SetProp(Wnd, DATABASE_PROPERTY, (HANDLE)db);		// Set the database structure to a property on window
		ReSizeGLScene(Wnd);									// Rescale the OpenGL window

		HWND hwndButton = CreateWindow(
			L"BUTTON",  // Predefined class; Unicode assumed 
			L"PLUS",      // Button text 
			WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
			10,         // x position 
			10,         // y position 
			100,        // Button width
			50,        // Button height
			Wnd,     // Parent window
			IDC_BUTTONPLUS,       // No menu.
			(HINSTANCE)GetWindowLong(Wnd, GWL_HINSTANCE),
			NULL);      // Pointer not needed.

		HWND hwndButtonMinus = CreateWindow(
			L"BUTTON",  // Predefined class; Unicode assumed 
			L"MINUS",      // Button text 
			WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
			110,         // x position 
			10,         // y position 
			100,        // Button width
			50,        // Button height
			Wnd,     // Parent window
			IDC_BUTTONMINUS,       // No menu.
			(HINSTANCE)GetWindowLong(Wnd, GWL_HINSTANCE),
			NULL);      // Pointer not needed.

		HWND hwndButtonProfileDistancePlus = CreateWindow(
			L"BUTTON",  // Predefined class; Unicode assumed 
			L"+ P-Distance",      // Button text 
			WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
			220,         // x position 
			10,         // y position 
			100,        // Button width
			50,        // Button height
			Wnd,     // Parent window
			IDC_PROFILEDISTANCE_PLUS,       // No menu.
			(HINSTANCE)GetWindowLong(Wnd, GWL_HINSTANCE),
			NULL);      // Pointer not needed.

		HWND hwndButtonProfileDistanceMinus = CreateWindow(
			L"BUTTON",  // Predefined class; Unicode assumed 
			L"MINUS",      // Button text 
			WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
			330,         // x position 
			10,         // y position 
			100,        // Button width
			50,        // Button height
			Wnd,     // Parent window
			IDC_PROFILEDISTANCE_MINUS,       // No menu.
			(HINSTANCE)GetWindowLong(Wnd, GWL_HINSTANCE),
			NULL);      // Pointer not needed.

		HWND hwndButtonProfile1Display = CreateWindow(
			L"BUTTON",  // Predefined class; Unicode assumed 
			L"Profile 1",      // Button text 
			WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
			440,         // x position 
			10,         // y position 
			100,        // Button width
			50,        // Button height
			Wnd,     // Parent window
			IDC_PROFILE_1_DISPLAY,       // No menu.
			(HINSTANCE)GetWindowLong(Wnd, GWL_HINSTANCE),
			NULL);      // Pointer not needed.
		HWND hwndButtonProfile2Display = CreateWindow(
			L"BUTTON",  // Predefined class; Unicode assumed 
			L"Profile 2",      // Button text 
			WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
			550,         // x position 
			10,         // y position 
			100,        // Button width
			50,        // Button height
			Wnd,     // Parent window
			IDC_PROFILE_2_DISPLAY,       // No menu.
			(HINSTANCE)GetWindowLong(Wnd, GWL_HINSTANCE),
			NULL);      // Pointer not needed.

		HWND hwndButtonProfile3Display = CreateWindow(
			L"BUTTON",  // Predefined class; Unicode assumed 
			L"Profile 3",      // Button text 
			WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
			660,         // x position 
			10,         // y position 
			100,        // Button width
			50,        // Button height
			Wnd,     // Parent window
			IDC_PROFILE_3_DISPLAY,       // No menu.
			(HINSTANCE)GetWindowLong(Wnd, GWL_HINSTANCE),
			NULL);      // Pointer not needed.

		HWND hwndButtonBackGround = CreateWindow(
			L"BUTTON",  // Predefined class; Unicode assumed 
			L"Background",      // Button text 
			WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
			770,         // x position 
			10,         // y position 
			100,        // Button width
			50,        // Button height
			Wnd,     // Parent window
			IDC_BACKGROUND_DISPLAY,       // No menu.
			(HINSTANCE)GetWindowLong(Wnd, GWL_HINSTANCE),
			NULL);      // Pointer not needed.

		 hWndEdit = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("Edit"), TEXT("test"),
			WS_CHILD | WS_VISIBLE, 880, 20, 1400,
			50, Wnd, IDC_PROFILEDISTANCE, NULL, NULL);
	}
					break;
	case WM_DESTROY: {											// WM_DESTROY MESSAGE
																// We need to do some cleanups as program is going to exit	
		wglMakeCurrent(NULL, NULL);							// Make the rendering context not current 
		GLDATABASE* db = (GLDATABASE*)GetProp(Wnd, DATABASE_PROPERTY); // Fetch the data base
		if (db != 0) {
			if (db->Rc != 0) wglDeleteContext(db->Rc);		// If valid rendering context delete it
			if (db->glTexture != 0)
				glDeleteTextures(1, &db->glTexture);		// If valid gltexture delete it
			free(db);										// Release the data structure memory itself
		}
		PostQuitMessage(0);									// Post quit message
	}
					 break;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_BMPLOAD: {                                 // LOAD BITMAP COMMAND
			TCHAR FileName[256];
			int i = OpenFileDialog(&FileName[0], _countof(FileName),
				_T("BMP file:"), _T("BMP"), _T("BMP FILE TO LOAD"), Wnd);
			if (i != 0) {
				// Fetch that childs data base
				GLDATABASE* db = (GLDATABASE*)GetProp(Wnd, DATABASE_PROPERTY);
				if (db != 0) {
					// Now check if texture exists and if so delete it
					if (db->glTexture) {
						glDeleteTextures(1, &db->glTexture);
						db->glTexture = 0;
					}
					// Create new texture
					db->glTexture = BMP2GLTexture(&FileName[0], Wnd, db);
					InvalidateRect(Wnd, 0, TRUE);	// Force redraw of window
				}
			}
		}
						  break;
		case IDC_PTPLOAD: {                                 // LOAD PTP-File COMMAND
			TCHAR FileName[256];
			int i = OpenFileDialog(&FileName[0], _countof(FileName),
				_T("PTP file:"), _T("PTP"), _T("PTP FILE TO LOAD"), Wnd);
			if (i != 0) {
				// Fetch that childs data base

				//ReadPTPDataFromPTPFile(&FileName[0]);
				//ReadFilePTP(&FileName[0]);

				LoadTextFileToEdit(hWndEdit, &FileName[0]);
			}
		}
						  break;
		case IDC_EXIT:										// EXIT COMMAND
			PostMessage(Wnd, WM_CLOSE, 0, 0);				// Post close message 
			break;
		case IDC_TIMERSTART: {								// Menu item: Timer-->Start timer
			SetTimer(Wnd,								// handle to main window 
				1,										// timer identifier 
				100,									// 100 ms interval 
				0);										// timer callback null
		}
							 break;
		case IDC_TIMERSTOP: {								// Menu item: Timer-->Stop timer
			KillTimer(Wnd, 1);							// Kill the timer
		}

		//-----------Begin View configuration-------------------------------------------------------
		case IDC_BUTTONPLUS: {
			GLDATABASE* db = (GLDATABASE*)GetProp(Wnd, DATABASE_PROPERTY);// Fetch the data base
			//db->xrot += 5.0f;									// Inc x rotation
			db->yrot += 5.0f;									// Inc y rotation
			InvalidateRect(Wnd, 0, TRUE);						// We need a redraw now so invalidate us	
		}
		break;
		case IDC_BUTTONMINUS: {
			GLDATABASE* db = (GLDATABASE*)GetProp(Wnd, DATABASE_PROPERTY);// Fetch the data base
			//db->xrot -= 5.0f;									// Inc x rotation
			db->yrot -= 5.0f;									// Inc y rotation
			InvalidateRect(Wnd, 0, TRUE);						// We need a redraw now so invalidate us	
		}
		break;
		//-------------End View Configuration-------------------------------------------

		//----------- Begin Profile Configuration---------------------------------------
		case IDC_PROFILEDISTANCE: {
			 	
		}
		break;
		case IDC_PROFILEDISTANCE_PLUS: {
			if (ProfileDistance < 5.0f)
			{
				ProfileDistance += 0.05f;
				InvalidateRect(Wnd, 0, TRUE);						// We need a redraw now so invalidate us
			}
		}
		 break;
		case IDC_PROFILEDISTANCE_MINUS: {
			if (ProfileDistance > 0.0f)
			{
				ProfileDistance -= 0.05f;
				InvalidateRect(Wnd, 0, TRUE);						// We need a redraw now so invalidate us
			}
		}
		break;
		case IDC_PROFILE_1_DISPLAY: {
			if(DisplayProfil_1)
				DisplayProfil_1 = FALSE;
			else
				DisplayProfil_1 = TRUE;

			InvalidateRect(Wnd, 0, TRUE);						// We need a redraw now so invalidate us	
		}
		break;
		case IDC_PROFILE_2_DISPLAY: {
			if (DisplayProfil_2)
				DisplayProfil_2 = FALSE;
			else
				DisplayProfil_2 = TRUE;
			InvalidateRect(Wnd, 0, TRUE);						// We need a redraw now so invalidate us	
		}
		break; 
		case IDC_PROFILE_3_DISPLAY: {
			if (DisplayProfil_3)
				DisplayProfil_3 = FALSE;
			else
				DisplayProfil_3 = TRUE;
			InvalidateRect(Wnd, 0, TRUE);						// We need a redraw now so invalidate us	
		}
		break;
		case IDC_BACKGROUND_DISPLAY: {
			if (DisplayBackground)
				DisplayBackground = FALSE;
			else
				DisplayBackground = TRUE;
			InvalidateRect(Wnd, 0, TRUE);						// We need a redraw now so invalidate us	
		}
									break;
					
		
		//----------End Profile configuration-----------------------------------------------
								  
							  
		};
		break;
	case WM_PAINT: {											// WM_PAINT MESSAGE
		PAINTSTRUCT Ps;
		GLDATABASE* db = (GLDATABASE*)GetProp(Wnd, DATABASE_PROPERTY);// Fetch the data base
		BeginPaint(Wnd, &Ps);								// Begin paint
		DrawGLScene(db, Ps.hdc);							// Draw the OpenGL scene
		SwapBuffers(Ps.hdc);								// Swap buffers
		EndPaint(Wnd, &Ps);									// End paint
		return 0;
	}
				   break;
	case WM_TIMER: {											// WM_TIMER MESSAGE
		GLDATABASE* db = (GLDATABASE*)GetProp(Wnd, DATABASE_PROPERTY);// Fetch the data base
		db->xrot += 1.0f;									// Inc x rotation
		db->yrot += 1.0f;									// Inc y rotation
		InvalidateRect(Wnd, 0, TRUE);						// We need a redraw now so invalidate us			
	}
				   break;
	case WM_WINDOWPOSCHANGED:									// WM_WINDOWPOSCHANGED
																// Check if window size has changed .. window move doesnt change aspect ratio
		if ((lParam == 0) || ((((PWINDOWPOS)lParam)->flags & SWP_NOSIZE) == 0)) {
			ReSizeGLScene(Wnd);									// Rescale the GL window							
			InvalidateRect(Wnd, 0, TRUE);						// We need a redraw now so invalidate us
		}
		break;
	case WM_ERASEBKGND:											// WM_ERASEBKGND MESSAGE
		return (FALSE);
	default: return DefWindowProc(Wnd, Msg, wParam, lParam);	// Default handler
	};// end switch case
	return 0;
};


 void ReadPTPDataFromPTPFile(TCHAR* FileName )
{
	HANDLE hFile;
	DWORD  dwBytesRead = 0;
	char   ReadBuffer[BUFFERSIZE] = { 0 };
	OVERLAPPED ol = { 0 };
	
	printf("\n");	

	hFile = CreateFile(FileName,               // file to open
		GENERIC_READ,          // open for reading
		FILE_SHARE_READ,       // share for reading
		NULL,                  // default security
		OPEN_EXISTING,         // existing file only
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, // normal file
		NULL);                 // no attr. template

	if (hFile == INVALID_HANDLE_VALUE)
	{
		//DisplayError(TEXT("CreateFile"));
		_tprintf(TEXT("Terminal failure: unable to open file \"%s\" for read.\n"), FileName);
		return;
	}

	// Read one character less than the buffer size to save room for
	// the terminating NULL character. 
	 
	if (FALSE == ReadFileEx(hFile, ReadBuffer, BUFFERSIZE - 1, &ol, FileIOCompletionRoutine))
	{
		//DisplayError(TEXT("ReadFile"));
		printf("Terminal failure: Unable to read from file.\n GetLastError=%08x\n", GetLastError());
		CloseHandle(hFile);
		return;
	}
	SleepEx(5000, TRUE);
	dwBytesRead = g_BytesTransferred;
	// This is the section of code that assumes the file is ANSI text. 
	// Modify this block for other data types if needed.

	if (dwBytesRead > 0 && dwBytesRead <= BUFFERSIZE - 1)
	{
		ReadBuffer[dwBytesRead] = '\0'; // NULL character

		_tprintf(TEXT("Data read from %s (%d bytes): \n"), FileName, dwBytesRead);
		printf("%s\n", ReadBuffer);
	}
	else if (dwBytesRead == 0)
	{
		_tprintf(TEXT("No data read from file %s\n"), FileName);
	}
	else
	{
		printf("\n ** Unexpected value for dwBytesRead ** \n");
	}

	// It is always good practice to close the open file handles even though
	// the app will exit here and clean up open handles anyway.
	
	CloseHandle(hFile);
}

 void ReadFilePTP(TCHAR* FileName)
 {
	 HANDLE hout;
	 TCHAR buff2[BUF_SIZE] = { 'a','v' };
	 BOOL bResult = FALSE;

	 char inBuffer[BUF_SIZE];
	 DWORD nBytesToRead = BUF_SIZE;
	 hout = CreateFile(FileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	 if (hout == INVALID_HANDLE_VALUE)
	 {
		 
	 }
	 DWORD nRead;
	 bResult=ReadFile(hout, buff2, &nBytesToRead, &nRead, NULL);


	 if (bResult &&  nBytesToRead == 0)
	 {
		 DWORD text = buff2;
	 }
	 else
	 {
		 DWORD text = buff2;
	 }

	 CloseHandle(hout);
 }

 BOOL LoadTextFileToEdit(HWND hEdit, LPCTSTR pszFileName)
 {
	 HANDLE hFile;
	 BOOL bSuccess = FALSE;

	 hFile = CreateFile(pszFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	 if (hFile != INVALID_HANDLE_VALUE)
	 {
		 DWORD dwFileSize;
		 dwFileSize = GetFileSize(hFile, NULL);
		 if (dwFileSize != 0xFFFFFFFF)
		 {
			 LPSTR pszFileText;
			 pszFileText = (LPSTR)GlobalAlloc(GPTR, dwFileSize + 1);
			 if (pszFileText != NULL)
			 {
				 BYTE buffer[437];
				 DWORD dwBytesRead;
				 BOOL bSuccess = ReadFile(hFile, &buffer, 436, &dwBytesRead, NULL);
				 buffer[436] = "\0";
				 if (SetWindowText(hEdit, &buffer))
					 bSuccess = TRUE; // It worked!

				 if (!bSuccess)
				 {
					 //dwErrorCode = GetLastError();
				 }
				 while (bSuccess && dwBytesRead) // Hier sollen wir die Datei Zeile f�r Zeile lesen und  ListOfPTPData bef�hlen.
				 {
					 bSuccess = ReadFile(hFile, &buffer, 436, &dwBytesRead, NULL);
				 }				

				 //DWORD dwRead;
				 //if (ReadFile(hFile, pszFileText, dwFileSize, &dwRead, NULL))
				 //{
					// pszFileText[dwFileSize] = 0; // Add null terminator
					// if (SetWindowText(hEdit, pszFileText))
					//	 bSuccess = TRUE; // It worked!
				 //}
				 GlobalFree(pszFileText);
			 }
		 }
		 CloseHandle(hFile);
	 }
	 return bSuccess;
 }

 BOOL LoadFileAndRetrieveProfile(HWND hEdit, LPCTSTR pszFileName)
 {
	 HANDLE hFile;
	 BOOL bSuccess = FALSE;
	 hFile = CreateFile(pszFileName, GENERIC_READ,
		 FILE_SHARE_READ, NULL,
		 OPEN_EXISTING, 0, NULL);
	 if (hFile != INVALID_HANDLE_VALUE)
	 {
		 DWORD dwFileSize;
		 LPSTR pszFileText;
		 LPSTR psHeader; //771 DWORD = 1542Bytes

		 dwFileSize = GetFileSize(hFile, NULL);
		 pszFileText = (char *)GlobalAlloc(GPTR, dwFileSize + 1);
		 psHeader = (char *)GlobalAlloc(GPTR, 218);
		 if (pszFileText != NULL)
		 {
			 DWORD dwRead;
			 if (ReadFile(hFile, pszFileText, dwFileSize, &dwRead, NULL))
			 {
				 pszFileText[dwFileSize] = '\0';
				 for (int t = 0; t < 128; t++)
				 {
					 //test(pszFileText);
					 psHeader[t] = pszFileText[t];
				 }
				 //psHeader = pszFileText;
				 //test(pszFileText);
				 if (SetWindowText(hEdit, psHeader))
					 bSuccess = TRUE;
			 }
			 GlobalFree(pszFileText);
		 }
	 }
	 return bSuccess;
 }

/* ------------------------------------------------------------------------
The application entry point
-----------------------------------------------------------------------*/
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	MSG Msg;
	RECT R;
	HWND Wnd;
	WNDCLASSEX WndClass;

	memset(&WndClass, 0, sizeof(WNDCLASSEX));						// Clear the class record
	WndClass.cbSize = sizeof(WNDCLASSEX);							// Size of this record
	WndClass.style = CS_OWNDC;										// Class styles
	WndClass.lpfnWndProc = OpenGLDemoHandler;						// Handler for this class
	WndClass.cbClsExtra = 0;										// No extra class data
	WndClass.cbWndExtra = 0;										// No extra window data
	WndClass.hInstance = GetModuleHandle(NULL);						// This instance
	WndClass.hIcon = LoadIcon(0, IDI_APPLICATION);					// Set icon
	WndClass.hCursor = LoadCursor(0, IDC_ARROW);					// Set cursor
	WndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);	// Set background brush
	WndClass.lpszMenuName = NULL;									// No menu yet
	WndClass.lpszClassName = AppClassName;							// Set class name
	RegisterClassEx(&WndClass);										// Register the class
	GetClientRect(GetDesktopWindow(), &R);							// Get desktop area					
	Wnd = CreateWindowEx(0, AppClassName, _T("OpenGL Demo Program"),
		WS_VISIBLE | WS_OVERLAPPEDWINDOW, R.left + 50, R.top + 50,
		R.right - R.left - 100, R.bottom - R.top - 100,
		0, 0, 0, NULL);												// Create a window
	while (GetMessage(&Msg, 0, 0, 0)) {								// Get messages
		TranslateMessage(&Msg);										// Translate each message
		DispatchMessage(&Msg);										// Dispatch each message
	};
	return (0);
}

