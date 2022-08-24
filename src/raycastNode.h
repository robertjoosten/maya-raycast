#include <maya/MPxNode.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MTypes.h>
#include <maya/MStatus.h>
#include <maya/MMatrix.h>
#include <maya/MPoint.h>
#include <maya/MVector.h>
#include <maya/MVectorArray.h>
#include <maya/MFloatPoint.h>
#include <maya/MFloatVector.h>
#include <maya/MQuaternion.h>
#include <maya/MFnNurbsSurface.h>
#include <maya/MFnMesh.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnMatrixAttribute.h>
#include <maya/MFnNumericAttribute.h>


template <class T>
T getMatrixRow(MMatrix& matrix, const int& row);


template <class T>
void setMatrixRow(MMatrix& matrix, const int& row, const T& value);


class RaycastNode : public MPxNode {
public:
	RaycastNode();
	virtual ~RaycastNode();
	static void* creator();

	virtual MStatus compute(const MPlug& plug, MDataBlock& data);

	static MStatus initialize();
	static MTypeId id;
	static const MString kName;
	static MObject aEnvelope;
	static MObject aAlong;
	static MObject aBidirectional;
	static MObject aReverse;
	static MObject aUseDistance;
	static MObject aUseNormal;
	static MObject aInputMatrix;
	static MObject aInputMesh;
	static MObject aInputSurface;
	static MObject aOutputMatrix;
	static MObject aHit;
	static MObject aHitDistance;
};
