#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/gl.h>
#include <GL/glx.h>

typedef GLXContext (*glXCreateContextAttribsARBProc)(
	Display*,
	GLXFBConfig,
	GLXContext,
	bool,
	const int*
);

static bool ctxErrorOccurred = false;

/**
 * [isExtensionSupported description]
 * @param  extList   [description]
 * @param  extension [description]
 * @return           [description]
 */

static bool isExtensionSupported(const char *pExtList, const char *pExtension)
{
	const char *pStart;
	const char *pWhere;
	const char *pTerminator;

	pWhere = strchr(pExtension, ' ');
	
	if (pWhere || *pExtension == '\0')
	{
		return false;
	}

	for (pStart = pExtList;;) 
	{
		pWhere = strstr(pStart, pExtension);

		if (!pWhere)
		{
			break;
		}

		pTerminator = pWhere + strlen(pExtension);

		if (pWhere == pStart || *(pWhere - 1) == ' ')
		{
			if (*pTerminator == ' ' || *pTerminator == '\0')
			{
				return true;
			}
		}

		pStart = pTerminator;
	}

	return false;
}

/**
 * [ctxErrorHandler description]
 * @param  pDpy [description]
 * @param  pEv  [description]
 * @return      [description]
 */

static int ctxErrorHandler(Display *pDpy, XErrorEvent *pEv)
{
	ctxErrorOccurred = true;
	return 0;
}

/**
 * [main description]
 * @param  argc [description]
 * @param  argv [description]
 * @return      [description]
 */

int main(int argc, char** argv)
{
	// Open an X Display.

	Display *pDisplay = XOpenDisplay(NULL);

	if (!pDisplay)
	{
		printf("Failed to open X display\n");
		exit(1);
	}

	// Get GLX version.

	static int visualAttribs[] =
	{
		GLX_X_RENDERABLE, True,
		GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
		GLX_RENDER_TYPE, GLX_RGBA_BIT,
		GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
		GLX_RED_SIZE, 		8,
		GLX_GREEN_SIZE, 	8,
		GLX_BLUE_SIZE, 		8,
		GLX_ALPHA_SIZE, 	8,
		GLX_DEPTH_SIZE, 	24,
		GLX_STENCIL_SIZE, 	8,
		GLX_DOUBLEBUFFER, 	True,
		None
	};

	int glxMajor;
	int glxMinor;

	if (!glXQueryVersion(pDisplay, &glxMajor, &glxMinor) ||
		((glxMajor == 1) && (glxMinor < 3)) ||
		(glxMajor < 1)
	)
	{
		printf("Invalid GLX version\n");
		exit(1);
	}

	// Find the optimal framebuffer format for display.

	int fbCount;
	
	GLXFBConfig *pFbc = glXChooseFBConfig(
		pDisplay, 
		DefaultScreen(pDisplay), 
		visualAttribs,
		&fbCount
	);

	if (!pFbc)
	{
		printf("Failed to get framebuffer object\n");
		exit(1);
	}

	int bestFbcIdx 		= -1;
	int worstFbc 		= -1;
	int bestNumSamp 	= -1;
	int worstNumSamp 	= 999;

	for (int i = 0; i < fbCount; i++)
	{
		XVisualInfo *pVi = glXGetVisualFromFBConfig(pDisplay, pFbc[i]);

		{
			if (pVi)
			{
				int sampBuf;
				int samples;

				glXGetFBConfigAttrib(pDisplay, pFbc[i], GLX_SAMPLE_BUFFERS, &sampBuf);
				glXGetFBConfigAttrib(pDisplay, pFbc[i], GLX_SAMPLES, &samples);

				if (bestFbcIdx < 0 || sampBuf && samples > bestNumSamp)
				{
					bestFbcIdx = i;
					bestNumSamp = samples;
				}

				if (worstFbc < 0 || !sampBuf || samples < worstNumSamp)
				{
					worstFbc = i;
					worstNumSamp = samples;
				}
			}
		}

		XFree(pVi);
	}

	GLXFBConfig bestFbc = pFbc[bestFbcIdx];
	XFree(pFbc);

	XVisualInfo *pVi = glXGetVisualFromFBConfig(pDisplay, bestFbc);
	XSetWindowAttributes swa;
	Colormap cmap;

	swa.colormap = cmap = XCreateColormap(
		pDisplay,
		RootWindow(pDisplay, pVi->screen),
		pVi->visual, 
		AllocNone
	);

	swa.background_pixmap = None;
	swa.border_pixel = 0;
	swa.event_mask = StructureNotifyMask;

	// Create an XWindow.

	Window win = XCreateWindow(
		pDisplay,
		RootWindow(pDisplay, pVi->screen),
		0,
		0,
		100,
		100,
		0,
		pVi->depth,
		InputOutput,
		pVi->visual,
		CWBorderPixel | CWColormap | CWEventMask, &swa
	);

	if (!win)
	{
		printf("Failed to create window.\n");
		exit(1);
	}

	XFree(pVi);
	XStoreName(pDisplay, win, "GL 3.0 Window");
	XMapWindow(pDisplay, win);

	// Create an OpenGL context.

	const char *pGlxExts = glXQueryExtensionsString(
		pDisplay,
	    DefaultScreen(pDisplay)
	);

	glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 0;
	glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)glXGetProcAddressARB(
		(const GLubyte*)"glXCreateContextAttribsARB"
	);

	GLXContext ctx = 0;

	ctxErrorOccurred = false;
	int (*oldHandler)(Display*, XErrorEvent*) = XSetErrorHandler(&ctxErrorHandler);

	if (!isExtensionSupported(pGlxExts, "GLX_ARB_create_context") ||
	    !glXCreateContextAttribsARB
	)
	{
		ctx = glXCreateNewContext(pDisplay, bestFbc, GLX_RGBA_TYPE, 0, true);
	}
	else
	{
		int context_attribs[] =
		{
		    GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
		    GLX_CONTEXT_MINOR_VERSION_ARB, 0,
		    None
		};

		ctx = glXCreateContextAttribsARB(
			pDisplay, 
			bestFbc, 
			0,
		    True,
		    context_attribs
		);

		XSync(pDisplay, false);
	}

	XSync(pDisplay, false);
	XSetErrorHandler(oldHandler);

	if (ctxErrorOccurred || !ctx)
	{
		printf( "Failed to create an OpenGL context\n" );
		exit(1);
	}

	if (!glXIsDirect(pDisplay,ctx))
	{
		printf("Indirect GLX rendering context obtained\n");
	}
	else
	{
		printf("Direct GLX rendering context obtained\n");
	}
	
	glXMakeCurrent(pDisplay, win, ctx);

	glClearColor(0, 0.5, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glXSwapBuffers(pDisplay, win);

	sleep(1);

	glClearColor(1, 0.5, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glXSwapBuffers(pDisplay, win);

	sleep(1);

	glXMakeCurrent(pDisplay, 0, 0);
	
	while (1)
	{

	}

	glXDestroyContext(pDisplay, ctx);
	XDestroyWindow(pDisplay, win);
	XFreeColormap(pDisplay, cmap);
	XCloseDisplay(pDisplay);

	return 0;
}