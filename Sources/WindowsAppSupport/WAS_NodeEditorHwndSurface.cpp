#include "WAS_NodeEditorHwndSurface.hpp"
#include "WAS_BitmapContextGdi.hpp"
#include "WAS_WindowsAppUtils.hpp"
#include "NE_Debug.hpp"

namespace WAS
{

NodeEditorHwndSurface::NodeEditorHwndSurface () :
	NodeEditorHwndSurface (NUIE::NativeDrawingContextPtr (new BitmapContextGdi ()))
{

}

NodeEditorHwndSurface::NodeEditorHwndSurface (const NUIE::NativeDrawingContextPtr& nativeContext) :
	NativeNodeEditorControl (),
	nodeEditor (nullptr),
	nativeContext (nativeContext),
	windowHandle (NULL)
{

}

NodeEditorHwndSurface::~NodeEditorHwndSurface ()
{

}

bool NodeEditorHwndSurface::Init (NUIE::NodeEditor* nodeEditorPtr, void* nativeWindowHandle, int x, int y, int width, int height)
{
	nodeEditor = nodeEditorPtr;
	DBGASSERT (nodeEditor != nullptr);

	windowHandle = (HWND) nativeWindowHandle;
	if (DBGERROR (windowHandle == NULL)) {
		return false;
	}

	nativeContext->Init (windowHandle);
	MoveWindow (windowHandle, x, y, width, height, TRUE);

	return true;
}

void* NodeEditorHwndSurface::GetEditorNativeHandle () const
{
	return windowHandle;
}

bool NodeEditorHwndSurface::IsEditorFocused () const
{
	HWND focusedHwnd = GetFocus ();
	HWND editorHwnd = (HWND) GetEditorNativeHandle ();
	return focusedHwnd == editorHwnd;
}

void NodeEditorHwndSurface::Resize (int x, int y, int width, int height)
{
	if (windowHandle == NULL) {
		return;
	}
	MoveWindow (windowHandle, x, y, width, height, TRUE);
	if (nodeEditor != nullptr) {
		nodeEditor->OnResize (width, height);
	}
}

void NodeEditorHwndSurface::Invalidate ()
{
	if (windowHandle == NULL) {
		return;
	}
	InvalidateRect (windowHandle, NULL, FALSE);
}

NUIE::DrawingContext& NodeEditorHwndSurface::GetDrawingContext ()
{
	return *nativeContext;
}

NUIE::NodeEditor* NodeEditorHwndSurface::GetNodeEditor ()
{
	return nodeEditor;
}

void NodeEditorHwndSurface::Draw ()
{
	if (windowHandle == NULL) {
		return;
	}
	if (nodeEditor != nullptr) {
		nodeEditor->Draw ();
	}
	nativeContext->BlitToWindow (windowHandle);
}

}
