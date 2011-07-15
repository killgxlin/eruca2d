# Makefile.am for the SDL gui library

lib_LTLIBRARIES = libSDL_gui.la

libSDL_gui_la_SOURCES =	\
	SDL_gui.cc Exception.cc Object.cc Surface.cc Font.cc Callback.cc \
	Drawable.cc Screen.cc Widget.cc Container.cc \
	FastFont.cc TrueTypeFont.cc \
	Layout.cc Panel.cc CardStack.cc \
	Label.cc Picture.cc TextEntry.cc \
	AbstractButton.cc Button.cc ToggleButton.cc \
	ProgressBar.cc ScrollBar.cc \
	VBoxLayout.cc RealScreen.cc

DEFS = @DEFS@ -DBUILDING_SDLGUI_DLL
libSDL_gui_la_LDFLAGS = -release $(SDLGUI_VERSION)
libSDL_gui_la_LIBADD = -lSDL_ttf

