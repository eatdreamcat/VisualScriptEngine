#ifndef UIITEMFINDER_HPP
#define UIITEMFINDER_HPP

#include "NodeUIManager.hpp"
#include "InputSlot.hpp"
#include "OutputSlot.hpp"

namespace NUIE
{

UINodePtr			FindNodeUnderPosition (NodeUIManager& uiManager, NodeUIEnvironment& env, const Point& viewPosition);
UIInputSlotPtr		FindInputSlotUnderPosition (NodeUIManager& uiManager, NodeUIEnvironment& env, const Point& viewPosition);
UIOutputSlotPtr		FindOutputSlotUnderPosition (NodeUIManager& uiManager, NodeUIEnvironment& env, const Point& viewPosition);

bool FindItemUnderPosition (NodeUIManager& uiManager, NodeUIEnvironment& env, const Point& viewPosition,
							const std::function<void (UINodePtr&)>& nodeFound,
							const std::function<void (UIOutputSlotPtr&)>& outputSlotFound,
							const std::function<void (UIInputSlotPtr&)>& inputSlotFound);

}

#endif
