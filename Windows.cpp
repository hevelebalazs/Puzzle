#include <Windows.h>
#include <gl/gl.h>

#include "OpenGLDefinitions.hpp"

#define func
#define decl
#define tdef

#define Assert(condition) if(!(condition)) {DebugBreak();}

static bool global_running;

static int
func GetStringLength(char *string)
{
	int len = 0;
	for(int i = 0; string[i]; i++)
	{
		len++;
	}

	return len;
}

static bool
func StringsEqual(char *string1, char *string2, int length)
{
	for(int i = 0; i < length; i++)
	{
		if(string1[i] == 0 || string2[i] == 0)
		{
			return false;
		}

		if(string1[i] != string2[i])
		{
			return false;
		}
	}

	return true;
}

static bool
func HasExtension(char *extensions, char *extension)
{
	int extension_length = GetStringLength(extension);

	for(int i = 0; extensions[i]; i++)
	{
		if(i == 0 || extensions[i] == ' ')
		{
			if(StringsEqual(&extensions[i + 1], extension, extension_length))
			{
				return true;
			}
		}
	}

	return false;
}

static void
func LoadWglExtensions(HINSTANCE instance)
{
	WNDCLASS win_class = {};
	win_class.style = CS_OWNDC;
	win_class.lpfnWndProc = DefWindowProcA;
	win_class.hInstance = instance;
	win_class.lpszClassName = "DummyWindowClass";

	ATOM atom_result = RegisterClass(&win_class);
	Assert(atom_result);

	HWND window = CreateWindowEx(
		0,
		win_class.lpszClassName,
		"DummyWindow",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		0,
		0,
		instance,
		0
	);

	Assert(window != 0);

	HDC dc = GetDC(window);

	PIXELFORMATDESCRIPTOR pixel_format = {};
	pixel_format.nSize = sizeof(pixel_format);
	pixel_format.nVersion = 1;
	pixel_format.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
	pixel_format.cColorBits = 32;
	pixel_format.cAlphaBits = 8;
	pixel_format.iLayerType = PFD_MAIN_PLANE;

	int pixel_format_index = ChoosePixelFormat(dc, &pixel_format);
	Assert(pixel_format_index != 0);

	PIXELFORMATDESCRIPTOR used_pixel_format;
	DescribePixelFormat(dc, pixel_format_index, sizeof(used_pixel_format), &used_pixel_format);
	BOOL bool_result = SetPixelFormat(dc, pixel_format_index, &used_pixel_format);
	Assert(bool_result);

	HGLRC glrc = wglCreateContext(dc);

	atom_result = wglMakeCurrent(dc, glrc);
	Assert(atom_result);

	LoadGLFunc(wglGetExtensionsStringEXT);

	char *extensions = (char *)wglGetExtensionsStringEXT();
	Assert(HasExtension(extensions, "WGL_ARB_pixel_format"));
	Assert(HasExtension(extensions, "WGL_ARB_create_context"));
	Assert(HasExtension(extensions, "WGL_ARB_multisample"));

	LoadGLFunc(wglChoosePixelFormatARB);
	LoadGLFunc(wglCreateContextAttribsARB);

	wglMakeCurrent(0, 0);
        
	wglDeleteContext(glrc);
    ReleaseDC(window, dc);
	DestroyWindow(window);
}

static LRESULT CALLBACK
func WinCallback(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	LRESULT result = 0;

	switch(message)
	{
		case WM_SETCURSOR:
		{
			HCURSOR cursor = LoadCursor(0, IDC_ARROW);
			SetCursor(cursor);
			break;
		}
		case WM_DESTROY:
		case WM_CLOSE:
		{
			global_running = false;
			break;
		}
		default:
		{
			result = DefWindowProc(window, message, wparam, lparam);
			break;
		}
	}
	return result;
}

static void 
func InitOpenGL(HINSTANCE instance)
{
	LoadWglExtensions(instance);

	WNDCLASS win_class = {};
	win_class.style = CS_OWNDC;
	win_class.lpfnWndProc = WinCallback;
	win_class.hInstance = instance;
	win_class.lpszClassName = "OpenGLClass";

	ATOM atom_result = RegisterClass(&win_class);
	Assert(atom_result);

	HWND window = CreateWindowEx(
		0,
		win_class.lpszClassName,
		"OpenGL",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		0,
		0,
		instance,
		0
	);

	Assert(window != 0);

	HDC dc = GetDC(window);

	int choose_pixel_format_attributes[] =
	{
		WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
		WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
		WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
		WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
		WGL_COLOR_BITS_ARB, 32,
		WGL_DEPTH_BITS_ARB, 24,
		WGL_STENCIL_BITS_ARB, 8,
		WGL_SAMPLES_ARB, 8,
		0
	};

	int pixel_format_id = 0;
	UINT num_formats = 0;

	BOOL bool_result = wglChoosePixelFormatARB(dc, choose_pixel_format_attributes, 0, 1, &pixel_format_id, &num_formats);
	Assert(bool_result);

	PIXELFORMATDESCRIPTOR pixel_format = {};
    DescribePixelFormat(dc, pixel_format_id, sizeof(pixel_format), &pixel_format);
    bool_result = SetPixelFormat(dc, pixel_format_id, &pixel_format);
	Assert(bool_result);

	int create_context_attributes[] =
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
		WGL_CONTEXT_MINOR_VERSION_ARB, 3,
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		0
	};

	HGLRC glrc = wglCreateContextAttribsARB(dc, 0, create_context_attributes);
	Assert(glrc);

	atom_result = wglMakeCurrent(dc, glrc);
	Assert(atom_result);

	glFrontFace(GL_CW);
}

int CALLBACK
func WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show)
{
	global_running = true;

	InitOpenGL(instance);

	LARGE_INTEGER counter_frequency;
	QueryPerformanceFrequency(&counter_frequency);

	LARGE_INTEGER last_counter;
	QueryPerformanceCounter(&last_counter);

	// glEnable(GL_MULTISAMPLE);

	MSG message = {};
	while(global_running)
	{		
		while(PeekMessage(&message, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}

		LARGE_INTEGER counter;
		QueryPerformanceCounter(&counter);
		long long microseconds = counter.QuadPart - last_counter.QuadPart;
		float milliseconds = ((float)microseconds * 1000.0f) / ((float)counter_frequency.QuadPart);
		float seconds = 0.001f * milliseconds;
		last_counter = counter;

		float max_seconds = 1.0f / 30.0f;
		if(seconds > max_seconds)
		{
			seconds = max_seconds;
		}

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 

		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);  
		glDepthFunc(GL_LEQUAL); 

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glDisable(GL_DEPTH_TEST);

		HDC dc = wglGetCurrentDC();
		SwapBuffers(dc);
	}

	return 0;
}