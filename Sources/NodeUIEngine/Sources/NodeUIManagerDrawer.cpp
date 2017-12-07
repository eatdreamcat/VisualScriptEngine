#include "NodeUIManagerDrawer.hpp"
#include "ContextDecorators.hpp"
#include "SkinParams.hpp"

namespace NUIE
{

NodeUIManagerDrawer::NodeUIManagerDrawer (const NodeUIManager& uiManager) :
	uiManager (uiManager)
{

}
	
void NodeUIManagerDrawer::Draw (NodeUIEnvironment& env, const NodeDrawingExtension* drawExt) const
{
	InitNodesToDraw (env);

	NodeDrawingContext& drawingContext = env.GetDrawingContext ();
	drawingContext.BeginDraw ();

	DrawBackground (env);
	{
		TextSkipperNodeContextDecorator textSkipperContext (drawingContext, uiManager.IsPreviewMode ());
		ViewBoxNodeContextDecorator viewBoxContext (textSkipperContext, uiManager.GetViewBox ());
		NodeUIEnvironmentContextDecorator drawEnv (env, viewBoxContext);
		DrawConnections (drawEnv, drawExt);
		DrawNodes (drawEnv);
	}
	DrawSelectionRect (env, drawExt);

	drawingContext.EndDraw ();
}

void NodeUIManagerDrawer::DrawBackground (NodeUIEnvironment& env) const
{
	DrawingContext& drawingContext = env.GetDrawingContext ();
	Rect contextRect (0.0, 0.0, drawingContext.GetWidth (), drawingContext.GetHeight ());
	drawingContext.FillRect (contextRect, env.GetSkinParams ().GetBackgroundColor ());
}

void NodeUIManagerDrawer::DrawConnections (NodeUIEnvironment& env, const NodeDrawingExtension* drawExt) const
{
	for (const auto& it : nodeIdToNodeMap) {
		const UINode* uiNode = it.second;
		uiNode->EnumerateOutputSlots ([&] (const NE::OutputSlotConstPtr& outputSlot) {
			Point beg = uiNode->GetOutputSlotConnPosition (env, outputSlot->GetId ());
			uiManager.EnumerateConnectedInputSlots (outputSlot, [&] (const NE::InputSlotConstPtr& inputSlot) {
				const UINode* endNode = FindNodeById (inputSlot->GetOwnerNodeId ());
				if (DBGERROR (endNode == nullptr)) {
					return;
				}
				Point end = endNode->GetInputSlotConnPosition (env, inputSlot->GetId ());
				if (IsConnectionVisible (env, beg, end)) {
					DrawConnection (env, beg, end);
				}
			});
			return true;
		});
	}

	if (drawExt != nullptr) {
		drawExt->EnumerateTemporaryConnections ([&] (const Point& beg, const Point& end) {
			DrawConnection (env, beg, end);
		});
	}
}

void NodeUIManagerDrawer::DrawConnection (NodeUIEnvironment& env, const Point& beg, const Point& end) const
{
	const SkinParams& skinParams = env.GetSkinParams ();
	const Pen& pen = skinParams.GetConnectionLinePen ();

	double bezierOffsetVal = beg.DistanceTo (end) / 2.0;
	Point bezierOffset (bezierOffsetVal, 0.0);
	env.GetDrawingContext ().DrawBezier (beg, beg + bezierOffset, end - bezierOffset, end, pen);
}

void NodeUIManagerDrawer::DrawNodes (NodeUIEnvironment& env) const
{
	ColorBlenderNodeContextDecorator selectionContext (env.GetDrawingContext (), env.GetSkinParams ().GetSelectionBlendColor ());
	NodeUIEnvironmentContextDecorator selectionEnv (env, selectionContext);
	const NodeUIManager::SelectedNodes& selectedNodes = uiManager.GetSelectedNodes ();
	for (const UINode* uiNode : nodesToDraw) {
		const NE::NodeId& nodeId = uiNode->GetId ();
		NE::Checksum drawingImageChecksum = uiNode->GetDrawingImageChecksum (env);

		NodeUIEnvironment& envToUse = selectedNodes.Contains (nodeId) ? selectionEnv : env;
		NodeDrawingContext& contextToUse = envToUse.GetDrawingContext ();

		contextToUse.OnNodeDrawingBegin (nodeId, drawingImageChecksum, uiNode->GetNodeRect (envToUse));
		uiNode->Draw (envToUse);
		contextToUse.OnNodeDrawingEnd (nodeId);
	}
}

void NodeUIManagerDrawer::DrawSelectionRect (NodeUIEnvironment& env, const NodeDrawingExtension* drawExt) const
{
	if (drawExt != nullptr) {
		drawExt->EnumerateSelectionRectangles ([&] (const Rect& viewRect) {
			env.GetDrawingContext ().DrawRect (viewRect, env.GetSkinParams ().GetSelectionRectPen ());
		});
	}
}

void NodeUIManagerDrawer::InitNodesToDraw (NodeUIEnvironment& env) const
{
	uiManager.EnumerateUINodes ([&] (const UINodeConstPtr& uiNode) {
		const UINode* uiNodePtr = uiNode.get ();
		if (IsNodeVisible (env, uiNodePtr)) {
			nodesToDraw.push_back (uiNodePtr);
		}
		nodeIdToNodeMap.insert ({ uiNodePtr->GetId (), uiNodePtr });
		return true;
	});
	std::sort (nodesToDraw.begin (), nodesToDraw.end (), [&] (const UINode* a, const UINode* b) -> bool {
		return a->GetId () < b->GetId ();
	});
}

bool NodeUIManagerDrawer::IsConnectionVisible (NodeUIEnvironment& env, const Point& beg, const Point& end) const
{
	Rect connectionRect = Rect::FromTwoPoints (beg, end);
	return IsRectVisible (env, connectionRect);
}

bool NodeUIManagerDrawer::IsNodeVisible (NodeUIEnvironment& env, const UINode* uiNode) const
{
	Rect nodeRect = uiNode->GetNodeRect (env);
	return IsRectVisible (env, nodeRect);
}

bool NodeUIManagerDrawer::IsRectVisible (NodeUIEnvironment& env, const Rect& rect) const
{
	const ViewBox& viewBox = uiManager.GetViewBox ();
	const DrawingContext& context = env.GetDrawingContext ();
	return Rect::IsInBounds (viewBox.ModelToView (rect), context.GetWidth (), context.GetHeight ());
}

const UINode* NodeUIManagerDrawer::FindNodeById (const NE::NodeId& nodeId) const
{
	auto found = nodeIdToNodeMap.find (nodeId);
	if (found == nodeIdToNodeMap.end ()) {
		return nullptr;
	}
	return found->second;
}

}
