#include "NodeId.hpp"

namespace NE
{

SerializationInfo NodeIdGenerator::serializationInfo (ObjectId ("{413878AA-B26B-4880-A0AE-35BD97ECDD17}"), ObjectVersion (1));
SerializationInfo NodeId::serializationInfo (ObjectId ("{8456D457-E5A6-48CB-BB55-310DBB3F6933}"), ObjectVersion (1));

NodeIdGenerator::NodeIdGenerator () :
	nextId (1)
{

}

NodeIdGenerator::~NodeIdGenerator ()
{

}

NodeIdType NodeIdGenerator::GenerateUniqueId ()
{
	NodeIdType newId = nextId++;
	return newId;
}

Stream::Status NodeIdGenerator::Read (InputStream& inputStream)
{
	ObjectHeader header (inputStream);
	NodeIdType nextIdFromStream = 0;
	inputStream.Read (nextIdFromStream);
	nextId = nextIdFromStream;
	return inputStream.GetStatus ();
}

Stream::Status NodeIdGenerator::Write (OutputStream& outputStream) const
{
	ObjectHeader header (outputStream, serializationInfo);
	outputStream.Write (nextId);
	return outputStream.GetStatus ();
}

NodeId::NodeId () :
	id (0)
{

}

NodeId::NodeId (const NodeIdType& uniqueId) :
	id (uniqueId)
{

}

NodeId::~NodeId ()
{

}

void NodeId::SetUniqueId (const NodeIdType& uniqueId)
{
	id = uniqueId;
}

size_t NodeId::GenerateHashValue () const
{
	return std::hash<size_t> {} (id);
}

bool NodeId::operator< (const NodeId& rhs) const
{
	return id < rhs.id;
}

bool NodeId::operator> (const NodeId& rhs) const
{
	return id > rhs.id;
}
	
bool NodeId::operator== (const NodeId& rhs) const
{
	return id == rhs.id;
}

bool NodeId::operator!= (const NodeId& rhs) const
{
	return !operator== (rhs);
}

Stream::Status NodeId::Read (InputStream& inputStream)
{
	ObjectHeader header (inputStream);
	inputStream.Read (id);
	return inputStream.GetStatus ();
}

Stream::Status NodeId::Write (OutputStream& outputStream) const
{
	ObjectHeader header (outputStream, serializationInfo);
	outputStream.Write (id);
	return outputStream.GetStatus ();
}

const NodeId NullNodeId;

}
