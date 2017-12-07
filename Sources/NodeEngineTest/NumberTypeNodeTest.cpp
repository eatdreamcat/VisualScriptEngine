#include "SimpleTest.hpp"
#include "Node.hpp"
#include "NodeManager.hpp"
#include "InputSlot.hpp"
#include "OutputSlot.hpp"
#include "SingleValues.hpp"

using namespace NE;

namespace NumberTypeNodeTest
{

class IntNode : public Node
{
public:
	IntNode () :
		Node ()
	{
		
	}

	virtual void RegisterSlots () override
	{
		RegisterOutputSlot (OutputSlotPtr (new OutputSlot (SlotId ("out"))));
	}

	virtual ValuePtr Calculate (NE::EvaluationEnv&) const override
	{
		return ValuePtr (new IntValue (5));
	}

	virtual void CalculationPostProcess (const ValuePtr&, NE::EvaluationEnv&) const override
	{
	
	}
};

class DoubleNode : public Node
{
public:
	DoubleNode () :
		Node ()
	{
		
	}

	virtual void RegisterSlots () override
	{
		RegisterOutputSlot (OutputSlotPtr (new OutputSlot (SlotId ("out"))));
	}

	virtual ValuePtr Calculate (NE::EvaluationEnv&) const override
	{
		return ValuePtr (new DoubleValue (6.0));
	}

	virtual void CalculationPostProcess (const ValuePtr&, NE::EvaluationEnv&) const override
	{
	
	}
};

class NumberInputSlot : public InputSlot
{
public:
	NumberInputSlot (const SlotId& id) :
		InputSlot (id, ValuePtr (new IntValue (0)), OutputSlotConnectionMode::Single)
	{

	}
};

class NumberAdderNode : public Node
{
public:
	NumberAdderNode () :
		Node ()
	{
		
	}

	virtual void RegisterSlots () override
	{
		RegisterInputSlot (InputSlotPtr (new NumberInputSlot (SlotId ("first"))));
		RegisterInputSlot (InputSlotPtr (new NumberInputSlot (SlotId ("second"))));
	}

	virtual ValuePtr Calculate (NE::EvaluationEnv& env) const override
	{
		ValuePtr a = EvaluateSingleInputSlot (SlotId ("first"), env);
		ValuePtr b = EvaluateSingleInputSlot (SlotId ("second"), env);
		double result = NumberValue::ToDouble (a) + NumberValue::ToDouble (b);
		return ValuePtr (new DoubleValue (result));
	}

	virtual void CalculationPostProcess (const ValuePtr&, NE::EvaluationEnv&) const override
	{
	
	}
};

TEST (NumberConnectionTest)
{
	NodeManager manager;

	NodePtr intNode = manager.AddNode (NodePtr (new IntNode ()));
	NodePtr doubleNode = manager.AddNode (NodePtr (new DoubleNode ()));
	NodePtr adderNode = manager.AddNode (NodePtr (new NumberAdderNode ()));

	ASSERT (manager.ConnectOutputSlotToInputSlot (intNode->GetOutputSlot (SlotId ("out")), adderNode->GetInputSlot (SlotId ("first"))));
	ASSERT (manager.ConnectOutputSlotToInputSlot (doubleNode->GetOutputSlot (SlotId ("out")), adderNode->GetInputSlot (SlotId ("second"))));

	ValuePtr result = adderNode->Evaluate (NE::EmptyEvaluationEnv);
	ASSERT (Value::IsType<NumberValue> (result));
	ASSERT (Value::IsType<DoubleValue> (result));
	ASSERT (Value::Cast<NumberValue> (result.get ())->ToDouble () == 11.0);
	ASSERT (DoubleValue::Get (result) == 11.0);
}

}
