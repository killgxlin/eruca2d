#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "SDL_gui.h"

GUI_Callback::GUI_Callback(const char *aname)
: GUI_Object(aname)
{
}

GUI_Callback::~GUI_Callback()
{
}

class GUI_Callback_C : public GUI_Callback
{
	protected:
		GUI_CallbackFunction *function;
		GUI_CallbackFunction *freefunc;
		void *data;
	public:
		GUI_Callback_C(GUI_CallbackFunction *function, GUI_CallbackFunction *freefunc, void *data);
		virtual ~GUI_Callback_C(void);
		virtual void Call(GUI_Object *object);
};

GUI_Callback_C::GUI_Callback_C(GUI_CallbackFunction *func, GUI_CallbackFunction *ffunc, void *p)
: GUI_Callback("callback")
{
	function = func;
	freefunc = ffunc;
	data = p;
}

GUI_Callback_C::~GUI_Callback_C()
{
	if (freefunc)
		freefunc(data);
}

void GUI_Callback_C::Call(GUI_Object *)
{
	if (function)
		function(data);
}

extern "C"
{

GUI_Callback *GUI_CallbackCreate(GUI_CallbackFunction *function, GUI_CallbackFunction *freefunc, void *data)
{
	return new GUI_Callback_C(function, freefunc, data);
}

void GUI_CallbackCall(GUI_Callback *callback)
{
	callback->Call(0);
}

}
