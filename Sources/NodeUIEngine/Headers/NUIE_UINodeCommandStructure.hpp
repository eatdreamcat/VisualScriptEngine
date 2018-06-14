#ifndef UINODECOMMANDSTRUCTURE_HPP
#define UINODECOMMANDSTRUCTURE_HPP

#include "NUIE_UINode.hpp"
#include "NUIE_CommandStructure.hpp"
#include "NUIE_NodeUIManager.hpp"

namespace NUIE
{

class DeleteNodesCommand : public SingleCommand
{
public:
	DeleteNodesCommand (NodeUIManager& uiManager, NodeUIEnvironment& uiEnvironment, const NE::NodeCollection& relevantNodes);
	virtual ~DeleteNodesCommand ();

	virtual void Do () override;

private:
	NodeUIManager&		uiManager;
	NodeUIEnvironment&	uiEnvironment;
	NE::NodeCollection	relevantNodes;
};

class CopyNodesCommand : public SingleCommand
{
public:
	CopyNodesCommand (NodeUIManager& uiManager, const NE::NodeCollection& relevantNodes);
	virtual ~CopyNodesCommand ();

	virtual void Do () override;

private:
	NodeUIManager&		uiManager;
	NE::NodeCollection	relevantNodes;
};

class PasteNodesCommand : public SingleCommand
{
public:
	PasteNodesCommand (NodeUIManager& uiManager, NodeUIEnvironment& uiEnvironment, const Point& position);
	virtual ~PasteNodesCommand ();

	virtual void Do () override;

private:
	NodeUIManager&		uiManager;
	NodeUIEnvironment&	uiEnvironment;
	Point				position;
};

class UndoCommand : public SingleCommand
{
public:
	UndoCommand (NodeUIManager& uiManager, NodeUIEnvironment& uiEnvironment);
	virtual ~UndoCommand ();

	virtual void Do () override;

private:
	NodeUIManager&		uiManager;
	NodeUIEnvironment&	uiEnvironment;
};

class RedoCommand : public SingleCommand
{
public:
	RedoCommand (NodeUIManager& uiManager, NodeUIEnvironment& uiEnvironment);
	virtual ~RedoCommand ();

	virtual void Do () override;

private:
	NodeUIManager&		uiManager;
	NodeUIEnvironment&	uiEnvironment;
};

NE::NodeCollection	GetNodesForCommand (const NodeUIManager& uiManager, const UINodePtr& uiNode);

CommandStructure	CreateEmptyAreaCommandStructure (NodeUIManager& uiManager, NodeUIEnvironment& uiEnvironment, const Point& position);
CommandStructure	CreateNodeCommandStructure (NodeUIManager& uiManager, NodeUIEnvironment& uiEnvironment, const UINodePtr& uiNode);
CommandStructure	CreateOutputSlotCommandStructure (NodeUIManager& uiManager, NodeUIEnvironment& uiEnvironment, const UIOutputSlotPtr& outputSlot);
CommandStructure	CreateInputSlotCommandStructure (NodeUIManager& uiManager, NodeUIEnvironment& uiEnvironment, const UIInputSlotPtr& inputSlot);
CommandStructure	CreateNodeGroupCommandStructure (NodeUIManager& uiManager, NodeUIEnvironment& uiEnvironment, const UINodeGroupPtr& group);

}

#endif
