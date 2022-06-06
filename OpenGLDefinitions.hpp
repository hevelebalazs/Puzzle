#define WGL_DRAW_TO_WINDOW_ARB            0x2001
#define WGL_SUPPORT_OPENGL_ARB            0x2010
#define WGL_DOUBLE_BUFFER_ARB             0x2011
#define WGL_PIXEL_TYPE_ARB                0x2013
#define WGL_TYPE_RGBA_ARB                 0x202B
#define WGL_COLOR_BITS_ARB                0x2014
#define WGL_DEPTH_BITS_ARB                0x2022
#define WGL_STENCIL_BITS_ARB              0x2023
#define WGL_CONTEXT_MAJOR_VERSION_ARB     0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB     0x2092
#define WGL_CONTEXT_PROFILE_MASK_ARB      0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB  0x00000001
#define WGL_SAMPLES_ARB                   0x2042


typedef const char *WINAPI type_wglGetExtensionsStringEXT(void);
typedef BOOL WINAPI type_wglChoosePixelFormatARB(HDC hdc,
												 const int *piAttribIList,
												 const FLOAT *pfAttribFList,
												 UINT nMaxFormats,
												 int *piFormats,
												 UINT *nNumFormats);
typedef HGLRC WINAPI type_wglCreateContextAttribsARB(HDC hDC, HGLRC hshareContext, const int *attribList);

#define GlobalGLFunc(name) type_##name *name;
#define LoadGLFunc(name) {name = (type_##name *)wglGetProcAddress(#name); Assert(name);}

GlobalGLFunc(wglGetExtensionsStringEXT);
GlobalGLFunc(wglChoosePixelFormatARB);
GlobalGLFunc(wglCreateContextAttribsARB);
