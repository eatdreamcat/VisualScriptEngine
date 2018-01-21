#include "NodeEditorControl.hpp"
#include "BitmapContextGdi.hpp"
#include "BitmapContextGdiplus.hpp"
#include "Direct2DContext.hpp"

#include "InputUINodes.hpp"
#include "ViewerUINodes.hpp"
#include "TestAppNodes.hpp"

MyCreateNodeCommand::MyCreateNodeCommand (NodeType nodeType, NUIE::NodeUIManager& uiManager, NUIE::NodeUIEnvironment& uiEnvironment, const std::wstring& name, const NUIE::Point& position) :
	NUIE::CreateNodeCommand (name, uiManager, uiEnvironment, position),
	nodeType (nodeType)
{

}

NUIE::UINodePtr MyCreateNodeCommand::CreateNode (const NUIE::Point& modelPosition)
{
	switch (nodeType) {
		case NodeType::Integer:
			return NUIE::UINodePtr (new NUIE::IntegerUpDownUINode (L"Integer", modelPosition, 0, 5));
		case NodeType::IntegerRange:
			return NUIE::UINodePtr (new NUIE::IntegerRangeNode (L"Range", modelPosition));
		case NodeType::Point:
			return NUIE::UINodePtr (new PointNode (L"Point", modelPosition));
		case NodeType::Line:
			return NUIE::UINodePtr (new LineNode (L"Line", modelPosition));
		case NodeType::Circle:
			return NUIE::UINodePtr (new CircleNode (L"Circle", modelPosition));
		case NodeType::Viewer:
			return NUIE::UINodePtr (new NUIE::MultiLineViewerUINode (L"Viewer", modelPosition, 5));
	}
	return nullptr;
}

AppEventHandlers::AppEventHandlers (wxPanel* panel) :
	panel (panel)
{

}

NUIE::CommandPtr AppEventHandlers::OnContextMenu (NUIE::NodeUIManager& uiManager, NUIE::NodeUIEnvironment& uiEnvironment, const NUIE::Point& position, const NUIE::CommandStructure& commands)
{
	// TODO: wx context menu
	NUIE::CommandStructure actualCommands = commands;
	NUIE::GroupCommandPtr createCommandGroup (new NUIE::GroupCommand (L"Add Node"));
	createCommandGroup->AddChildCommand (NUIE::CommandPtr (new MyCreateNodeCommand (MyCreateNodeCommand::NodeType::Integer, uiManager, uiEnvironment, L"Integer", position)));
	createCommandGroup->AddChildCommand (NUIE::CommandPtr (new MyCreateNodeCommand (MyCreateNodeCommand::NodeType::IntegerRange, uiManager, uiEnvironment, L"Range", position)));
	createCommandGroup->AddChildCommand (NUIE::CommandPtr (new MyCreateNodeCommand (MyCreateNodeCommand::NodeType::Point, uiManager, uiEnvironment, L"Point", position)));
	createCommandGroup->AddChildCommand (NUIE::CommandPtr (new MyCreateNodeCommand (MyCreateNodeCommand::NodeType::Line, uiManager, uiEnvironment, L"Line", position)));
	createCommandGroup->AddChildCommand (NUIE::CommandPtr (new MyCreateNodeCommand (MyCreateNodeCommand::NodeType::Circle, uiManager, uiEnvironment, L"Circle", position)));
	createCommandGroup->AddChildCommand (NUIE::CommandPtr (new MyCreateNodeCommand (MyCreateNodeCommand::NodeType::Viewer, uiManager, uiEnvironment, L"Viewer", position)));
	actualCommands.AddCommand (createCommandGroup);
	return UI::SelectCommandFromContextMenu ((HWND) panel->GetHandle (), position, actualCommands);
}

NUIE::CommandPtr AppEventHandlers::OnContextMenu (NUIE::NodeUIManager& uiManager, NUIE::NodeUIEnvironment& env, const NUIE::Point& position, const NUIE::UINodePtr& uiNode, const NUIE::CommandStructure& commands)
{
	return UI::SelectCommandFromContextMenu ((HWND) panel->GetHandle (), position, commands);
}

NUIE::CommandPtr AppEventHandlers::OnContextMenu (NUIE::NodeUIManager& uiManager, NUIE::NodeUIEnvironment& env, const NUIE::Point& position, const NE::OutputSlotPtr& outputSlot, const NUIE::CommandStructure& commands)
{
	return UI::SelectCommandFromContextMenu ((HWND) panel->GetHandle (), position, commands);
}

NUIE::CommandPtr AppEventHandlers::OnContextMenu (NUIE::NodeUIManager& uiManager, NUIE::NodeUIEnvironment& env, const NUIE::Point& position, const NE::InputSlotPtr& inputSlot, const NUIE::CommandStructure& commands)
{
	return UI::SelectCommandFromContextMenu ((HWND) panel->GetHandle (), position, commands);
}

MouseCaptureHandler::MouseCaptureHandler (wxPanel* panel) :
	panel (panel),
	counter (0)
{

}

void MouseCaptureHandler::OnMouseDown ()
{
	panel->CaptureMouse ();
	counter++;
}

void MouseCaptureHandler::OnMouseUp ()
{
	counter--;
	if (counter == 0) {
		panel->ReleaseMouse ();
	}
	counter = 0;
}

void MouseCaptureHandler::OnCaptureLost ()
{
	counter = 0;
}

NodeEditorControl::NodeEditorControl (wxWindow *parent, UpdateInterface& updateInterface, NE::EvaluationEnv& evaluationEnv) :
	wxPanel (parent, wxID_ANY, wxPoint (0, 0), wxSize (200, 200)),
	captureHandler (this),
	NUIE::NodeUIEnvironment (),
	updateInterface (updateInterface),
	evaluationEnv (evaluationEnv),
	drawingContext (),
	skinParams (),
	eventHandlers (this),
	nodeEditor (*this)
{
	HWND hwnd = GetHandle ();
	drawingContext.Init (hwnd);

	NUIE::NodeUIManager& uiManager = nodeEditor.GetNodeUIManager ();

	static const bool isStressTest = false;
	if (isStressTest) {
		static int count = 10;
		for (int i = 0; i < count; i++) {
			for (int j = 0; j < count; j++) {
				uiManager.AddNode (NUIE::UINodePtr (new NUIE::IntegerRangeNode (L"Range", NUIE::Point (i * 150, j * 150))), GetEvaluationEnv ());
			}
		}
		nodeEditor.Update ();
	} else {
		NUIE::UINodePtr startInputNode = uiManager.AddNode (NUIE::UINodePtr (new NUIE::IntegerUpDownUINode (L"Integer", NUIE::Point (70, 70), 20, 5)), GetEvaluationEnv ());
		NUIE::UINodePtr stepInputNode = uiManager.AddNode (NUIE::UINodePtr (new NUIE::IntegerUpDownUINode (L"Integer", NUIE::Point (70, 180), 20, 5)), GetEvaluationEnv ());
		NUIE::UINodePtr intRangeNodeX = uiManager.AddNode (NUIE::UINodePtr (new NUIE::IntegerRangeNode (L"Range", NUIE::Point (200, 100))), GetEvaluationEnv ());
		NUIE::UINodePtr inputNodeY = uiManager.AddNode (NUIE::UINodePtr (new NUIE::IntegerUpDownUINode (L"Integer", NUIE::Point (200, 220), 20, 5)), GetEvaluationEnv ());
		std::shared_ptr<PointNode> pointNode (new PointNode (L"Point", NUIE::Point (400, 150)));
		uiManager.AddNode (pointNode, GetEvaluationEnv ());
		pointNode->SetValueCombinationMode (NE::ValueCombinationMode::CrossProduct);
		NUIE::UINodePtr viewerNode = uiManager.AddNode (NUIE::UINodePtr (new NUIE::MultiLineViewerUINode (L"Viewer", NUIE::Point (600, 150), 5)), GetEvaluationEnv ());

		uiManager.ConnectOutputSlotToInputSlot (startInputNode->GetUIOutputSlot (NE::SlotId ("out")), intRangeNodeX->GetUIInputSlot (NE::SlotId ("start")));
		uiManager.ConnectOutputSlotToInputSlot (stepInputNode->GetUIOutputSlot (NE::SlotId ("out")), intRangeNodeX->GetUIInputSlot (NE::SlotId ("step")));
		uiManager.ConnectOutputSlotToInputSlot (intRangeNodeX->GetUIOutputSlot (NE::SlotId ("out")), pointNode->GetUIInputSlot (NE::SlotId ("x")));
		uiManager.ConnectOutputSlotToInputSlot (inputNodeY->GetUIOutputSlot (NE::SlotId ("out")), pointNode->GetUIInputSlot (NE::SlotId ("y")));
		uiManager.ConnectOutputSlotToInputSlot (pointNode->GetUIOutputSlot (NE::SlotId ("point")), viewerNode->GetUIInputSlot (NE::SlotId ("in")));
		nodeEditor.Update ();
	}
}

void NodeEditorControl::OnPaint (wxPaintEvent& evt)
{
	nodeEditor.Draw ();

	PAINTSTRUCT ps;
	HWND hwnd = (HWND) GetHandle ();
	HDC hdc = BeginPaint (hwnd, &ps);
	drawingContext.DrawToHDC (hdc);
	EndPaint (hwnd, &ps);
}

void NodeEditorControl::OnResize (wxSizeEvent& evt)
{
	wxSize size = evt.GetSize ();
	drawingContext.Resize (size.GetWidth (), size.GetHeight ());
}

void NodeEditorControl::OnMouseCaptureLost (wxMouseCaptureLostEvent& evt)
{
	captureHandler.OnCaptureLost ();
}

void NodeEditorControl::OnLeftButtonDown (wxMouseEvent& evt)
{
	// TODO: KeySet
	captureHandler.OnMouseDown ();
	nodeEditor.OnMouseDown (NUIE::EmptyKeySet, NUIE::MouseButton::Left, evt.GetX (), evt.GetY ());
}

void NodeEditorControl::OnLeftButtonUp (wxMouseEvent& evt)
{
	// TODO: KeySet
	nodeEditor.OnMouseUp (NUIE::EmptyKeySet, NUIE::MouseButton::Left, evt.GetX (), evt.GetY ());
	captureHandler.OnMouseUp ();
}

void NodeEditorControl::OnRightButtonDown (wxMouseEvent& evt)
{
	// TODO: KeySet
	captureHandler.OnMouseDown ();
	nodeEditor.OnMouseDown (NUIE::EmptyKeySet, NUIE::MouseButton::Right, evt.GetX (), evt.GetY ());
}

void NodeEditorControl::OnRightButtonUp (wxMouseEvent& evt)
{
	// TODO: KeySet
	nodeEditor.OnMouseUp (NUIE::EmptyKeySet, NUIE::MouseButton::Right, evt.GetX (), evt.GetY ());
	captureHandler.OnMouseUp ();
}

void NodeEditorControl::OnMiddleButtonDown (wxMouseEvent& evt)
{
	// TODO: KeySet
	captureHandler.OnMouseDown ();
	nodeEditor.OnMouseDown (NUIE::EmptyKeySet, NUIE::MouseButton::Middle, evt.GetX (), evt.GetY ());
}

void NodeEditorControl::OnMiddleButtonUp (wxMouseEvent& evt)
{
	// TODO: KeySet
	nodeEditor.OnMouseUp (NUIE::EmptyKeySet, NUIE::MouseButton::Middle, evt.GetX (), evt.GetY ());
	captureHandler.OnMouseUp ();
}

void NodeEditorControl::OnMouseMove (wxMouseEvent& evt)
{
	// TODO: KeySet
	nodeEditor.OnMouseMove (NUIE::EmptyKeySet, evt.GetX (), evt.GetY ());
}

void NodeEditorControl::OnMouseWheel (wxMouseEvent& evt)
{
	// TODO: KeySet
	NUIE::MouseWheelRotation rotation = evt.GetWheelRotation () > 0 ? NUIE::MouseWheelRotation::Forward : NUIE::MouseWheelRotation::Backward;
	nodeEditor.OnMouseWheel (NUIE::EmptyKeySet, rotation, evt.GetX (), evt.GetY ());
}

void NodeEditorControl::New ()
{
	nodeEditor.Clear ();
}
 
bool NodeEditorControl::Open (const std::wstring& fileName)
{
	if (!nodeEditor.Load (fileName)) {
		return false;
	}
	return true;
}
 
bool NodeEditorControl::Save (const std::wstring& fileName)
{
	if (!nodeEditor.Save (fileName)) {
		return false;
	}
	return true;
}

NUIE::DrawingContext& NodeEditorControl::GetDrawingContext ()
{
	return drawingContext;
}

NUIE::SkinParams& NodeEditorControl::GetSkinParams ()
{
	return skinParams;
}

NUIE::EventHandlers& NodeEditorControl::GetEventHandlers ()
{
	return eventHandlers;
}

NE::EvaluationEnv& NodeEditorControl::GetEvaluationEnv ()
{
	return evaluationEnv;
}

void NodeEditorControl::OnValuesRecalculated ()
{
	updateInterface.RedrawImage ();
}

void NodeEditorControl::OnRedrawRequest ()
{
	Refresh (false);
}

BEGIN_EVENT_TABLE(NodeEditorControl, wxPanel)
// TODO: double click events
EVT_PAINT (NodeEditorControl::OnPaint)
EVT_SIZE (NodeEditorControl::OnResize)
EVT_MOUSE_CAPTURE_LOST (NodeEditorControl::OnMouseCaptureLost)
EVT_LEFT_DOWN (NodeEditorControl::OnLeftButtonDown)
EVT_LEFT_UP (NodeEditorControl::OnLeftButtonUp)
EVT_RIGHT_DOWN (NodeEditorControl::OnRightButtonDown)
EVT_RIGHT_UP (NodeEditorControl::OnRightButtonUp)
EVT_MIDDLE_DOWN (NodeEditorControl::OnMiddleButtonDown)
EVT_MIDDLE_UP (NodeEditorControl::OnMiddleButtonUp)
EVT_MOUSEWHEEL (NodeEditorControl::OnMouseWheel)
EVT_MOTION (NodeEditorControl::OnMouseMove)
END_EVENT_TABLE ()
