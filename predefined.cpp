#include "stdafx.h"

#include "predefined.h"

bool is_predefined(const std::string& name)
{
	// Types found in stdole2.tlb
	// Which are defined in OCIdl.h (Windows SDK)
	static const char* predefined[] =
	{
		"IFontDisp",
		"IFontEventsDisp",
		"IPictureDisp",
		"OLE_COLOR",
		"OLE_XPOS_PIXELS",
		"OLE_YPOS_PIXELS",
		"OLE_XSIZE_PIXELS",
		"OLE_YSIZE_PIXELS",
		"OLE_XPOS_HIMETRIC",
		"OLE_YPOS_HIMETRIC",
		"OLE_XSIZE_HIMETRIC",
		"OLE_YSIZE_HIMETRIC",
		"OLE_XPOS_CONTAINER",
		"OLE_YPOS_CONTAINER",
		"OLE_XSIZE_CONTAINER",
		"OLE_YSIZE_CONTAINER",
		"OLE_HANDLE",
		"OLE_OPTEXCLUSIVE",
		"OLE_CANCELBOOL",
		"OLE_ENABLEDEFAULTBOOL",
		"OLE_TRISTATE"
	};

	return std::ranges::find(predefined, name) != std::end(predefined);
}
