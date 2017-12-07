#include "OutputSlot.hpp"
#include "Node.hpp"
#include "Debug.hpp"
#include "NodeManager.hpp"

namespace NE
{

DynamicSerializationInfo OutputSlot::serializationInfo (ObjectId ("{1F40DBE0-C463-46ED-A2CC-17C779C16802}"), ObjectVersion (1), OutputSlot::CreateSerializableInstance);

OutputSlot::OutputSlot () :
	Slot ()
{

}

OutputSlot::OutputSlot (const SlotId& id) :
	Slot (id)
{

}

OutputSlot::~OutputSlot ()
{

}

ValuePtr OutputSlot::Evaluate (EvaluationEnv& env) const
{
	return EvaluateOwnerNode (env);
}

Stream::Status OutputSlot::Read (InputStream& inputStream)
{
	ObjectHeader header (inputStream);
	Slot::Read (inputStream);
	return inputStream.GetStatus ();
}

Stream::Status OutputSlot::Write (OutputStream& outputStream) const
{
	ObjectHeader header (outputStream, serializationInfo);
	Slot::Write (outputStream);
	return outputStream.GetStatus ();
}

ValuePtr OutputSlot::EvaluateOwnerNode (EvaluationEnv& env) const
{
	if (DBGERROR (!HasOwnerNode ())) {
		return nullptr;
	}
	return ownerNode->Evaluate (env);
}

}
