#include <maya/MFnPlugin.h>

#include "raycastNode.h"


MStatus initializePlugin(MObject obj) {
	MStatus status;
	MFnPlugin plugin(obj, "Robert Joosten", "1.0", "any");

	status = plugin.registerNode(
		RaycastNode::kName,
		RaycastNode::id,
		RaycastNode::creator,
		RaycastNode::initialize
	);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	return status;
}

MStatus uninitializePlugin(MObject obj) {
	MStatus status;
	MFnPlugin plugin(obj);

	status = plugin.deregisterNode(RaycastNode::id);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	return status;
}