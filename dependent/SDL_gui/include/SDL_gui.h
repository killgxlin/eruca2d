#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"

#ifndef SDL_GUI_H
#define SDL_GUI_H

#ifdef WIN32
# ifdef BUILDING_SDLGUI_DLL
#  define DLLIMPORT __declspec(dllexport)
# else
#  define DLLIMPORT /* __declspec (dllimport) */
# endif
#else
# define DLLIMPORT
#endif

#ifdef __cplusplus

class DLLIMPORT GUI_Exception
{
	protected:
		char *message;
	public:
		GUI_Exception(const char *fmt, ...);
		GUI_Exception(const GUI_Exception &err);
		virtual ~GUI_Exception(void);
		const char *GetMessage(void);
};

class DLLIMPORT GUI_Object
{
	private:
		int refcount;
		char *name;
	public:
		GUI_Object(const char *aname);
		virtual ~GUI_Object(void);
		
		void SetName(const char *s);
		const char *GetName(void);
		int CheckName(const char *aname);
		
		void IncRef(void);
		int DecRef(void);
};

class DLLIMPORT GUI_Callback : public GUI_Object
{
	protected:
	public:
		GUI_Callback(const char *aname);
		~GUI_Callback(void);
		
		virtual void Call(GUI_Object *object)=0;
};

template <class T> class DLLIMPORT GUI_EventHandler : public GUI_Callback
{
	typedef void handler(GUI_Object *sender);
	typedef handler (T::*method);
	protected:
		method f;
		T *obj;
	public:
		GUI_EventHandler<T>(T *t, method m) : GUI_Callback("event")
		{
			// FIXME keeps a borrowed reference
			obj = t;
			f = m;
		}
		virtual void Call(GUI_Object *sender)
		{
			(obj->*f)(sender);
		}
};

class DLLIMPORT GUI_Surface : public GUI_Object
{
	protected:
		SDL_Surface *surface;
	public:
		GUI_Surface(const char *aname, SDL_Surface *image);
		GUI_Surface(const char *aname, int f, int w, int h, int d, int rm, int gm, int bm, int am);
		GUI_Surface(const char *fn);
		virtual ~GUI_Surface(void);

		void DisplayFormat(void);
		void SetAlpha(Uint32 flag, Uint8 alpha);
		void SetColorKey(Uint32 c);
		void Blit(SDL_Rect *src_r, GUI_Surface *dst, SDL_Rect *dst_r);
		void UpdateRects(int n, SDL_Rect *rects);
		void UpdateRect(int x, int y, int w, int h);
		void Fill(SDL_Rect *r, Uint32 c);
		int GetWidth(void);
		int GetHeight(void);
		Uint32 MapRGB(int r, int g, int b);
		int IsDoubleBuffered(void);
		int IsHardware(void);
		void Flip(void);
		SDL_Surface *GetSurface(void);
		void SaveBMP(const char *filename);
};

class DLLIMPORT GUI_Font : public GUI_Object
{
	public:
		GUI_Font(const char *aname);
		virtual ~GUI_Font(void);

		// draw text onto a surface
		virtual void DrawText(GUI_Surface *surface, const char *s, int x, int y);

		// create a new surface with text on it using the fastest method
		virtual GUI_Surface *RenderFast(const char *s, SDL_Color fg);

		// create a new surface with text on it using the best quality
		virtual GUI_Surface *RenderQuality(const char *s, SDL_Color fg);
		
		// return the size of the text when using this font
		virtual SDL_Rect GetTextSize(const char *s);
};

class DLLIMPORT GUI_FastFont : public GUI_Font
{
	protected:
		GUI_Surface *image;
		int char_width;
		int char_height;
	public:
		GUI_FastFont(const char *fn);
		virtual ~GUI_FastFont(void);

		virtual void DrawText(GUI_Surface *surface, const char *s, int x, int y);
		virtual GUI_Surface *RenderFast(const char *s, SDL_Color fg);
		virtual GUI_Surface *RenderQuality(const char *s, SDL_Color fg);
		virtual SDL_Rect GetTextSize(const char *s);
		GUI_Surface *GetFontImage(void);
};

class DLLIMPORT GUI_TrueTypeFont : public GUI_Font
{
	protected:
		TTF_Font *ttf;
	public:
		GUI_TrueTypeFont(const char *fn, int size);
		virtual ~GUI_TrueTypeFont(void);

		virtual GUI_Surface *RenderFast(const char *s, SDL_Color fg);
		virtual GUI_Surface *RenderQuality(const char *s, SDL_Color fg);
		virtual SDL_Rect GetTextSize(const char *s);
};

class DLLIMPORT GUI_Container;

class DLLIMPORT GUI_Layout : public GUI_Object
{
	public:
		GUI_Layout(const char *aname);
		virtual ~GUI_Layout(void);
		
		virtual void Layout(GUI_Container *container);
};

class DLLIMPORT GUI_VBoxLayout : public GUI_Layout
{
	public:
		GUI_VBoxLayout(const char *aname);
		virtual ~GUI_VBoxLayout(void);
		
		virtual void Layout(GUI_Container *container);
};

class DLLIMPORT GUI_Widget;

class DLLIMPORT GUI_Drawable : public GUI_Object
{
	protected:
		// FIXME make these private
		int flags;
		int flag_delta;
		SDL_Rect area;
		GUI_Callback *status_callback;
		void Keep(GUI_Widget **target, GUI_Widget *source);
		SDL_Rect Adjust(const SDL_Rect *area);
	public:
		GUI_Drawable(const char *aname, int x, int y, int w, int h);
		virtual ~GUI_Drawable(void);
		
		void DoUpdate(int force);
		virtual void Update(int force);
		virtual void Draw(GUI_Surface *image, const SDL_Rect *sr, const SDL_Rect *dr);
		virtual void Erase(const SDL_Rect *dr);
		virtual void Fill(const SDL_Rect *dr, SDL_Color c);
		virtual int Event(const SDL_Event *event, int xoffset, int yoffset);
		virtual void Clicked(int x, int y);
		virtual void RemoveWidget(GUI_Widget *widget);
		virtual void Notify(int mask);

		int GetWidth(void);
		int GetHeight(void);
		void TileImage(GUI_Surface *surface, const SDL_Rect *area, int x_offset, int y_offset);
		void MarkChanged(void);
		void WriteFlags(int andmask, int ormask);
		void SetFlags(int mask);
		void ClearFlags(int mask);
		SDL_Rect GetArea(void);
		void SetPosition(int x, int y);
		void SetStatusCallback(GUI_Callback *callback);
		int GetFlagDelta(void);
		int GetFlags(void);
};

class DLLIMPORT GUI_Widget : public GUI_Drawable
{
	protected:
		GUI_Drawable *parent;
	public:
		GUI_Widget(const char *name, int x, int y, int w, int h);
		virtual ~GUI_Widget(void);
		
		void SetAlign(int align);
		void SetTransparent(int trans);
		void SetEnabled(int flag);
		void SetState(int state);
		int GetState(void);

		virtual void Draw(GUI_Surface *image, const SDL_Rect *sr, const SDL_Rect *dr);
		virtual void Erase(const SDL_Rect *dr);
		virtual void Fill(const SDL_Rect *dr, SDL_Color c);

		void SetParent(GUI_Drawable *aparent);
		GUI_Drawable *GetParent(void);
};

class DLLIMPORT GUI_TextEntry : public GUI_Widget
{
	protected:
		GUI_Font *font;
		SDL_Color textcolor;
		GUI_Surface *normal_image;
		GUI_Surface *highlight_image;
		GUI_Surface *focus_image;
		GUI_Callback *focus_callback;
		GUI_Callback *unfocus_callback;
		int align;
		size_t buffer_size;
		size_t buffer_index;
		char *buffer;
	public:
		GUI_TextEntry(const char *name, int x, int y, int w, int h, GUI_Font *font, int size);
		~GUI_TextEntry(void);

		void SetFont(GUI_Font *afont);
		void SetTextColor(int r, int g, int b);
		void SetText(const char *text);
		const char *GetText(void);
		void SetNormalImage(GUI_Surface *surface);
		void SetHighlightImage(GUI_Surface *surface);
		void SetFocusImage(GUI_Surface *surface);
		void SetFocusCallback(GUI_Callback *callback);
		void SetUnfocusCallback(GUI_Callback *callback);

		virtual void Update(int force);
		virtual void Clicked(int x, int y);
		virtual int Event(const SDL_Event *event, int xoffset, int yoffset);
};

class DLLIMPORT GUI_Label : public GUI_Widget
{
	protected:
		GUI_Surface *text;
		GUI_Font *font;
		SDL_Color textcolor;
	public:
		GUI_Label(const char *aname, int x, int y, int w, int h, GUI_Font *afont, const char *s);
		virtual ~GUI_Label(void);

		void SetFont(GUI_Font *font);
		void SetTextColor(int r, int g, int b);
		void SetText(const char *s);

		virtual void Update(int force);
};

class DLLIMPORT GUI_Picture : public GUI_Widget
{
	protected:
		GUI_Surface *image;
		GUI_Widget *caption;
	public:
		GUI_Picture(const char *aname, int x, int y, int w, int h, GUI_Surface *an_image);
		virtual ~GUI_Picture(void);

		void SetImage(GUI_Surface *an_image);
		void SetCaption(GUI_Widget *a_caption);
		
		virtual void Update(int force);
		virtual void Erase(const SDL_Rect *area);
		virtual int Event(const SDL_Event *event, int xoffset, int yoffset);
};

class DLLIMPORT GUI_AbstractButton : public GUI_Widget
{
	protected:
		GUI_Widget *caption;
		GUI_Callback *click;
		virtual GUI_Surface *GetCurrentImage(void);
	public:
		GUI_AbstractButton(const char *aname, int x, int y, int w, int h);
		virtual ~GUI_AbstractButton(void);

		virtual void RemoveWidget(GUI_Widget *widget);
		void SetCaption(GUI_Widget *widget);
		void SetClick(GUI_Callback *handler);
		
		virtual void Erase(const SDL_Rect *dr);
		virtual void Fill(const SDL_Rect *dr, SDL_Color c);
		virtual void Update(int force);
		virtual void Notify(int mask);
		virtual void Clicked(int x, int y);
};

class DLLIMPORT GUI_Button : public GUI_AbstractButton
{
	protected:
		GUI_Surface *normal;
		GUI_Surface *highlight;
		GUI_Surface *pressed;
		GUI_Surface *disabled;
		virtual GUI_Surface *GetCurrentImage(void);
	public:
		GUI_Button(const char *aname, int x, int y, int w, int h);
		virtual ~GUI_Button(void);

		void SetNormalImage(GUI_Surface *surface);
		void SetHighlightImage(GUI_Surface *surface);
		void SetPressedImage(GUI_Surface *surface);
		void SetDisabledImage(GUI_Surface *surface);
};

class DLLIMPORT GUI_ToggleButton : public GUI_AbstractButton
{
	protected:
		GUI_Surface *off_normal;
		GUI_Surface *off_highlight;
		GUI_Surface *on_normal;
		GUI_Surface *on_highlight;
		virtual GUI_Surface *GetCurrentImage(void);
	public:
		GUI_ToggleButton(const char *aname, int x, int y, int w, int h);
		virtual ~GUI_ToggleButton(void);

		virtual void Clicked(int x, int y);
		void SetOnNormalImage(GUI_Surface *surface);
		void SetOffNormalImage(GUI_Surface *surface);
		void SetOnHighlightImage(GUI_Surface *surface);
		void SetOffHighlightImage(GUI_Surface *surface);
};

class DLLIMPORT GUI_ProgressBar : public GUI_Widget
{
	protected:
		GUI_Surface *image1;
		GUI_Surface *image2;
		double value;
	public:
		GUI_ProgressBar(const char *aname, int x, int y, int w, int h);
		virtual ~GUI_ProgressBar(void);
		
		void SetImage1(GUI_Surface *surface);
		void SetImage2(GUI_Surface *surface);
		void SetPosition(double value);

		virtual void Update(int force);
};

class DLLIMPORT GUI_ScrollBar : public GUI_Widget
{
	protected:
		GUI_Surface *background;
		GUI_Surface *knob;
		GUI_Callback *moved_callback;
		int position;
		int tracking_on;
		int tracking_start;
		int tracking_pos;
		int page_step;
	public:
		GUI_ScrollBar(const char *aname, int x, int y, int w, int h);
		virtual ~GUI_ScrollBar(void);

		void SetKnobImage(GUI_Surface *image);
		void SetBackgroundImage(GUI_Surface *image);
		int GetPosition(void);
		void SetPosition(int value);
		void SetPageStep(int value);
		void SetMovedCallback(GUI_Callback *callback);

		virtual void Update(int force);
		virtual void Erase(const SDL_Rect *rp);
		virtual int Event(const SDL_Event *event, int xoffset, int yoffset);
};

class DLLIMPORT GUI_Container : public GUI_Widget
{
	protected:
		int n_widgets;
		int s_widgets;
		GUI_Widget **widgets;
		int x_offset;
		int y_offset;
		GUI_Surface *background;
		SDL_Color bgcolor;
		virtual void UpdateLayout(void);
	public:
		GUI_Container(const char *name, int x, int y, int w, int h);
		virtual ~GUI_Container(void);

		int ContainsWidget(GUI_Widget *widget);
		void AddWidget(GUI_Widget *widget);
		virtual void RemoveWidget(GUI_Widget *widget);
		int GetWidgetCount(void);
		GUI_Widget *GetWidget(int index);

		void SetBackground(GUI_Surface *image);
		void SetBackgroundColor(SDL_Color c);
		void SetXOffset(int value);
		void SetYOffset(int value);
		
		virtual void Draw(GUI_Surface *image, const SDL_Rect *sr, const SDL_Rect *dr);
		virtual void Fill(const SDL_Rect *dr, SDL_Color c);
		virtual void Erase(const SDL_Rect *rp);
};

class DLLIMPORT GUI_Panel : public GUI_Container
{
	protected:
		GUI_Layout *layout;
		virtual void Update(int force);
		virtual void UpdateLayout(void);
	public:
		GUI_Panel(const char *name, int x, int y, int w, int h);
		virtual ~GUI_Panel(void);

		void SetLayout(GUI_Layout *a_layout);
		virtual int Event(const SDL_Event *event, int xoffset, int yoffset);
};

class DLLIMPORT GUI_CardStack : public GUI_Container
{
	protected:
		int visible_index;
		virtual void Update(int force);
	public:
		GUI_CardStack(const char *name, int x, int y, int w, int h);
		virtual ~GUI_CardStack(void);

		void Next(void);
		void Prev(void);
		int GetIndex(void);
		void ShowIndex(int index);
		void Show(const char *aname);

		virtual int Event(const SDL_Event *event, int xoffset, int yoffset);
};

class DLLIMPORT GUI_Screen : public GUI_Drawable
{
	protected:
		GUI_Surface *screen_surface;
		GUI_Surface *background;
		GUI_Widget *contents;
		GUI_Widget *focus_widget;
		Uint32 background_color;
		virtual void FlushUpdates(void);
		virtual void UpdateRect(const SDL_Rect *r);
		virtual void Update(int force);
	public:
		GUI_Screen(const char *aname, SDL_Surface *surface);
		virtual ~GUI_Screen(void);

		virtual void Draw(GUI_Surface *image, const SDL_Rect *sr, const SDL_Rect *dr);
		virtual void Erase(const SDL_Rect *dr);
		virtual void Fill(const SDL_Rect *dr, SDL_Color c);
		virtual int Event(const SDL_Event *event, int xoffset, int yoffset);
		virtual void RemoveWidget(GUI_Widget *widget);
		
		void SetContents(GUI_Widget *widget);
		void SetBackground(GUI_Surface *image);
		void SetFocusWidget(GUI_Widget *item);
		void ClearFocusWidget(void);
		void SetBackgroundColor(SDL_Color c);
		GUI_Widget *GetFocusWidget(void);
		GUI_Surface *GetSurface(void);
};

class DLLIMPORT GUI_RealScreen : public GUI_Screen
{
	protected:
		int n_updates;
		SDL_Rect *updates;
		virtual void FlushUpdates(void);
		virtual void UpdateRect(const SDL_Rect *r);
		virtual void Update(int force);
	public:
		GUI_RealScreen(const char *aname, SDL_Surface *surface);
		virtual ~GUI_RealScreen(void);
};

#else

typedef void GUI_Object;
typedef struct guiSurface GUI_Surface;
typedef struct guiScreen GUI_Screen;
typedef struct guiFont GUI_Font;
typedef struct guiWidget GUI_Widget;
typedef struct guiCallback GUI_Callback;
typedef struct guiLayout GUI_Layout;

#endif

/* flags */

#define WIDGET_PRESSED         0x00000001
#define WIDGET_INSIDE          0x00000002
#define WIDGET_HIDDEN          0x00000004
#define WIDGET_CHANGED         0x00000008
#define WIDGET_TRANSPARENT     0x00000010
#define WIDGET_HAS_FOCUS       0x00000020
#define WIDGET_WANTS_FOCUS     0x00000040
#define WIDGET_TURNED_ON       0x00000080

#define WIDGET_ALIGN_MASK      0x00000F00
#define WIDGET_HORIZ_MASK      0x00000300
#define WIDGET_HORIZ_CENTER    0x00000000
#define WIDGET_HORIZ_RIGHT     0x00000100
#define WIDGET_HORIZ_LEFT      0x00000200
#define WIDGET_HORIZ_STRETCH   0x00000300
#define WIDGET_VERT_MASK       0x00000C00
#define WIDGET_VERT_CENTER     0x00000000
#define WIDGET_VERT_TOP        0x00000400
#define WIDGET_VERT_BOTTOM     0x00000800
#define WIDGET_VERT_STRETCH    0x00000C00

#define WIDGET_DISABLED        0x00001000

#define SCREEN_DEBUG_BLIT      0x10000000
#define SCREEN_DEBUG_UPDATE    0x20000000

#ifdef __cplusplus
extern "C" {
#endif

typedef void GUI_CallbackFunction(void *);

/* GUI API */

DLLIMPORT int GUI_Init(void);
DLLIMPORT void GUI_SetScreen(GUI_Screen *);
DLLIMPORT GUI_Screen *GUI_GetScreen(void);
DLLIMPORT void GUI_Run(void);
DLLIMPORT void GUI_Quit(void);

DLLIMPORT int GUI_MustLock(void);
DLLIMPORT int GUI_Lock(void);
DLLIMPORT int GUI_Unlock(void);

DLLIMPORT void GUI_SetThread(Uint32 id);
DLLIMPORT int GUI_GetRunning(void);
DLLIMPORT void GUI_SetRunning(int value);
DLLIMPORT int GUI_ClipRect(SDL_Rect *sr, SDL_Rect *dr, SDL_Rect *clip);
DLLIMPORT void GUI_TriggerUpdate(void);

/* Object API */

DLLIMPORT GUI_Object *GUI_ObjectCreate(const char *s);
DLLIMPORT const char *GUI_ObjectGetName(GUI_Object *object);
DLLIMPORT void GUI_ObjectSetName(GUI_Object *object, const char *s);
DLLIMPORT void GUI_ObjectIncRef(GUI_Object *object);
DLLIMPORT int GUI_ObjectDecRef(GUI_Object *object);
DLLIMPORT int GUI_ObjectKeep(GUI_Object **target, GUI_Object *source);

/* Surface API */

DLLIMPORT GUI_Surface *GUI_SurfaceLoad(const char *fn);
DLLIMPORT GUI_Surface *GUI_SurfaceCreate(const char *aname, int f, int w, int h, int d, int rm, int gm, int bm, int am);
DLLIMPORT void GUI_SurfaceBlit(GUI_Surface *src, SDL_Rect *src_r, GUI_Surface *dst, SDL_Rect *dst_r);
DLLIMPORT void GUI_SurfaceUpdateRects(GUI_Surface *surface, int n, SDL_Rect *rects);
DLLIMPORT void GUI_SurfaceUpdateRect(GUI_Surface *surface, int x, int y, int w, int h);
DLLIMPORT void GUI_SurfaceFill(GUI_Surface *surface, SDL_Rect *r, Uint32 c);
DLLIMPORT int GUI_SurfaceGetWidth(GUI_Surface *surface);
DLLIMPORT int GUI_SurfaceGetHeight(GUI_Surface *surface);
DLLIMPORT Uint32 GUI_SurfaceMapRGB(GUI_Surface *surface, int r, int g, int b);

/* Font API */

DLLIMPORT GUI_Surface *GUI_FontRenderFast(GUI_Font *font, const char *s, SDL_Color fg);
DLLIMPORT GUI_Surface *GUI_FontRenderQuality(GUI_Font *font, const char *s, SDL_Color fg);
DLLIMPORT void GUI_FontDrawText(GUI_Font *font, GUI_Surface *surface, const char *s, int x, int y);
DLLIMPORT SDL_Rect GUI_FontGetTextSize(GUI_Font *font, const char *s);

/* Widget API */

DLLIMPORT void GUI_WidgetUpdate(GUI_Widget *widget, int force);
DLLIMPORT void GUI_WidgetDraw(GUI_Widget *widget, GUI_Surface *image, const SDL_Rect *sr, const SDL_Rect *dr);
DLLIMPORT void GUI_WidgetErase(GUI_Widget *widget, const SDL_Rect *dr);
DLLIMPORT void GUI_WidgetFill(GUI_Widget *widget, const SDL_Rect *dr, SDL_Color c);
DLLIMPORT int GUI_WidgetEvent(GUI_Widget *widget, const SDL_Event *event, int xoffset, int yoffset);
DLLIMPORT void GUI_WidgetClicked(GUI_Widget *widget, int x, int y);
DLLIMPORT void GUI_WidgetSetAlign(GUI_Widget *widget, int align);
DLLIMPORT void GUI_WidgetMarkChanged(GUI_Widget *widget);
DLLIMPORT void GUI_WidgetSetTransparent(GUI_Widget *widget, int trans);
DLLIMPORT void GUI_WidgetSetEnabled(GUI_Widget *widget, int flag);
DLLIMPORT void GUI_WidgetTileImage(GUI_Widget *widget, GUI_Surface *surface, const SDL_Rect *area, int x_offset, int y_offset);
DLLIMPORT void GUI_WidgetSetFlags(GUI_Widget *widget, int mask);
DLLIMPORT void GUI_WidgetClearFlags(GUI_Widget *widget, int mask);
DLLIMPORT void GUI_WidgetSetState(GUI_Widget *widget, int state);
DLLIMPORT int GUI_WidgetGetState(GUI_Widget *widget);
DLLIMPORT SDL_Rect GUI_WidgetGetArea(GUI_Widget *widget);
DLLIMPORT void GUI_WidgetSetPosition(GUI_Widget *widget, int x, int y);

/* Container API */

DLLIMPORT int GUI_ContainerContains(GUI_Widget *container, GUI_Widget *widget);
DLLIMPORT void GUI_ContainerAdd(GUI_Widget *container, GUI_Widget *widget);
DLLIMPORT void GUI_ContainerRemove(GUI_Widget *container, GUI_Widget *widget);
DLLIMPORT int GUI_ContainerGetCount(GUI_Widget *container);
DLLIMPORT GUI_Widget *GUI_ContainerGetChild(GUI_Widget *container, int index);

/* Screen API */

DLLIMPORT GUI_Screen *GUI_ScreenCreate(int w, int h, int d, int f);
DLLIMPORT void GUI_ScreenSetContents(GUI_Screen *screen, GUI_Widget *contents);
DLLIMPORT void GUI_ScreenSetBackground(GUI_Screen *screen, GUI_Surface *surface);
DLLIMPORT void GUI_ScreenSetFocusWidget(GUI_Screen *screen, GUI_Widget *item);
DLLIMPORT void GUI_ScreenClearFocusWidget(GUI_Screen *screen);
DLLIMPORT void GUI_ScreenSetBackgroundColor(GUI_Screen *screen, SDL_Color c);
DLLIMPORT GUI_Widget *GUI_ScreenGetFocusWidget(GUI_Screen *screen);

/* Button Widget */

DLLIMPORT GUI_Widget *GUI_ButtonCreate(const char *name, int x, int y, int w, int h);
DLLIMPORT int GUI_ButtonCheck(GUI_Widget *widget);
DLLIMPORT void GUI_ButtonSetNormalImage(GUI_Widget *widget, GUI_Surface *surface);
DLLIMPORT void GUI_ButtonSetHighlightImage(GUI_Widget *widget, GUI_Surface *surface);
DLLIMPORT void GUI_ButtonSetPressedImage(GUI_Widget *widget, GUI_Surface *surface);
DLLIMPORT void GUI_ButtonSetDisabledImage(GUI_Widget *widget, GUI_Surface *surface);
DLLIMPORT void GUI_ButtonSetCaption(GUI_Widget *widget, GUI_Widget *caption);
DLLIMPORT void GUI_ButtonSetClick(GUI_Widget *widget, GUI_Callback *callback);

/* Callback API */

DLLIMPORT GUI_Callback *GUI_CallbackCreate(GUI_CallbackFunction *function, GUI_CallbackFunction *freefunc, void *data);
DLLIMPORT void GUI_CallbackCall(GUI_Callback *callback);

/* FastFont object */

DLLIMPORT GUI_Font *GUI_FontLoadBitmap(char *fn);

/* TTF Font object */

DLLIMPORT GUI_Font *GUI_FontLoadTrueType(char *fn, int size);

/* Panel Widget API */

DLLIMPORT GUI_Widget *GUI_PanelCreate(const char *name, int x, int y, int w, int h);
DLLIMPORT int GUI_PanelCheck(GUI_Widget *widget);
DLLIMPORT void GUI_PanelSetBackground(GUI_Widget *widget, GUI_Surface *surface);
DLLIMPORT void GUI_PanelSetBackgroundColor(GUI_Widget *widget, SDL_Color c);
DLLIMPORT void GUI_PanelSetXOffset(GUI_Widget *widget, int value);
DLLIMPORT void GUI_PanelSetYOffset(GUI_Widget *widget, int value);
DLLIMPORT void GUI_PanelSetLayout(GUI_Widget *widget, GUI_Layout *layout);

/* VBox Layout object */

DLLIMPORT GUI_Layout *GUI_VBoxLayoutCreate(void);

/* ToggleButton Widget API */

DLLIMPORT GUI_Widget *GUI_ToggleButtonCreate(const char *name, int x, int y, int w, int h);
DLLIMPORT int GUI_ToggleButtonCheck(GUI_Widget *widget);
DLLIMPORT void GUI_ToggleButtonSetOnNormalImage(GUI_Widget *widget, GUI_Surface *surface);
DLLIMPORT void GUI_ToggleButtonSetOnHighlightImage(GUI_Widget *widget, GUI_Surface *surface);
DLLIMPORT void GUI_ToggleButtonSetOffNormalImage(GUI_Widget *widget, GUI_Surface *surface);
DLLIMPORT void GUI_ToggleButtonSetOffHighlightImage(GUI_Widget *widget, GUI_Surface *surface);
DLLIMPORT void GUI_ToggleButtonSetCaption(GUI_Widget *widget, GUI_Widget *caption);
DLLIMPORT void GUI_ToggleButtonSetClick(GUI_Widget *widget, GUI_Callback *callback);

/* Label Widget API */

DLLIMPORT GUI_Widget *GUI_LabelCreate(const char *name, int x, int y, int w, int h, GUI_Font *font, const char *text);
DLLIMPORT int GUI_LabelCheck(GUI_Widget *widget);
DLLIMPORT void GUI_LabelSetFont(GUI_Widget *widget, GUI_Font *font);
DLLIMPORT void GUI_LabelSetTextColor(GUI_Widget *widget, int r, int g, int b);
DLLIMPORT void GUI_LabelSetText(GUI_Widget *widget, const char *text);

/* Picture Widget API */

DLLIMPORT GUI_Widget *GUI_PictureCreate(const char *name, int x, int y, int w, int h, GUI_Surface *image);
DLLIMPORT int GUI_PictureCheck(GUI_Widget *widget);
DLLIMPORT void GUI_PictureSetImage(GUI_Widget *widget, GUI_Surface *image);
DLLIMPORT void GUI_PictureSetCaption(GUI_Widget *widget, GUI_Widget *caption);

/* ProgressBar widget API */

DLLIMPORT GUI_Widget *GUI_ProgressBarCreate(const char *name, int x, int y, int w, int h);
DLLIMPORT int GUI_ProgressBarCheck(GUI_Widget *widget);
DLLIMPORT void GUI_ProgressBarSetImage1(GUI_Widget *widget, GUI_Surface *image);
DLLIMPORT void GUI_ProgressBarSetImage2(GUI_Widget *widget, GUI_Surface *image);
DLLIMPORT void GUI_ProgressBarSetPosition(GUI_Widget *widget, double value);

/* ScrollBar widget API */

DLLIMPORT GUI_Widget *GUI_ScrollBarCreate(const char *name, int x, int y, int w, int h);
DLLIMPORT int GUI_ScrollBarCheck(GUI_Widget *widget);
DLLIMPORT void GUI_ScrollBarSetKnobImage(GUI_Widget *widget, GUI_Surface *image);
DLLIMPORT void GUI_ScrollBarSetBackgroundImage(GUI_Widget *widget, GUI_Surface *image);
DLLIMPORT void GUI_ScrollBarSetPosition(GUI_Widget *widget, int value);
DLLIMPORT void GUI_ScrollBarSetPageStep(GUI_Widget *widget, int value);
DLLIMPORT void GUI_ScrollBarSetMovedCallback(GUI_Widget *widget, GUI_Callback *callback);
DLLIMPORT int GUI_ScrollBarGetPosition(GUI_Widget *widget);

/* CardStack widget API */

DLLIMPORT GUI_Widget *GUI_CardStackCreate(const char *name, int x, int y, int w, int h);
DLLIMPORT int GUI_CardStackCheck(GUI_Widget *widget);
DLLIMPORT void GUI_CardStackSetBackground(GUI_Widget *widget, GUI_Surface *surface);
DLLIMPORT void GUI_CardStackSetBackgroundColor(GUI_Widget *widget, SDL_Color c);
DLLIMPORT void GUI_CardStackNext(GUI_Widget *widget);
DLLIMPORT void GUI_CardStackPrev(GUI_Widget *widget);
DLLIMPORT void GUI_CardStackShowIndex(GUI_Widget *widget, int index);
DLLIMPORT void GUI_CardStackShow(GUI_Widget *widget, const char *name);
DLLIMPORT int GUI_CardStackGetIndex(GUI_Widget *widget);

/* TextEntry widget API */

DLLIMPORT GUI_Widget *GUI_TextEntryCreate(const char *name, int x, int y, int w, int h, GUI_Font *font, int size);
DLLIMPORT int GUI_TextEntryCheck(GUI_Widget *widget);
DLLIMPORT void GUI_TextEntrySetFont(GUI_Widget *widget, GUI_Font *font);
DLLIMPORT void GUI_TextEntrySetTextColor(GUI_Widget *widget, int r, int g, int b);
DLLIMPORT void GUI_TextEntrySetText(GUI_Widget *widget, const char *text);
DLLIMPORT const char *GUI_TextEntryGetText(GUI_Widget *widget);
DLLIMPORT void GUI_TextEntrySetNormalImage(GUI_Widget *widget, GUI_Surface *surface);
DLLIMPORT void GUI_TextEntrySetHighlightImage(GUI_Widget *widget, GUI_Surface *surface);
DLLIMPORT void GUI_TextEntrySetFocusImage(GUI_Widget *widget, GUI_Surface *surface);
DLLIMPORT void GUI_TextEntrySetFocusCallback(GUI_Widget *widget, GUI_Callback *callback);
DLLIMPORT void GUI_TextEntrySetUnfocusCallback(GUI_Widget *widget, GUI_Callback *callback);

#ifdef __cplusplus
};
#endif

#endif
