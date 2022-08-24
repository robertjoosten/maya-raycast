#include "raycastNode.h"
#include <math.h>


MTypeId RaycastNode::id(0x00000002);
MObject RaycastNode::aEnvelope;
MObject RaycastNode::aAlong;
MObject RaycastNode::aBidirectional;
MObject RaycastNode::aReverse;
MObject RaycastNode::aUseDistance;
MObject RaycastNode::aUseNormal;
MObject RaycastNode::aInputMatrix;
MObject RaycastNode::aInputMesh;
MObject RaycastNode::aInputSurface;
MObject RaycastNode::aOutputMatrix;
MObject RaycastNode::aHit;
MObject RaycastNode::aHitDistance;


const MString RaycastNode::kName("raycast");


template <class T>
T getMatrixRow(MMatrix& matrix, const int& row)
{
	T result;
	for (int i = 0; i < 3; i++) {
		result[i] = matrix[row][i];
	}
	return result;
}

template <class T>
void setMatrixRow(MMatrix& matrix, const int& row, const T& value)
{
	for (int i = 0; i < 3; i++) {
		matrix[row][i] = value[i];
	}
}


MStatus RaycastNode::initialize() {
	MStatus status;

	MFnEnumAttribute fnEnumAttr;
	MFnTypedAttribute fnTypedAttr;
	MFnMatrixAttribute fnMatrixAttr;
	MFnNumericAttribute fnNumericAttr;

	aEnvelope = fnNumericAttr.create("envelope", "envelope", MFnNumericData::kFloat, 1);
	fnNumericAttr.setMin(0);
	fnNumericAttr.setMax(1);
	fnNumericAttr.setStorable(true);
	fnNumericAttr.setKeyable(true);
	fnNumericAttr.setReadable(true);
	fnNumericAttr.setWritable(true);
	addAttribute(aEnvelope);

	aAlong = fnEnumAttr.create("along", "along", 0);
	fnEnumAttr.addField("x", 0);
	fnEnumAttr.addField("y", 1);
	fnEnumAttr.addField("z", 2);
	fnEnumAttr.setStorable(true);
	fnEnumAttr.setKeyable(true);
	fnEnumAttr.setReadable(true);
	fnEnumAttr.setWritable(true);
	addAttribute(aAlong);

	aBidirectional = fnNumericAttr.create("bidirectional", "bidirectional", MFnNumericData::kBoolean, true);
	fnNumericAttr.setStorable(true);
	fnNumericAttr.setKeyable(true);
	fnNumericAttr.setReadable(true);
	fnNumericAttr.setWritable(true);
	addAttribute(aBidirectional);

	aReverse = fnNumericAttr.create("reverse", "reverse", MFnNumericData::kBoolean, false);
	fnNumericAttr.setStorable(true);
	fnNumericAttr.setKeyable(true);
	fnNumericAttr.setReadable(true);
	fnNumericAttr.setWritable(true);
	addAttribute(aReverse);

	aUseDistance = fnNumericAttr.create("useDistance", "useDistance", MFnNumericData::kFloat, 1);
	fnNumericAttr.setMin(0);
	fnNumericAttr.setMax(1);
	fnNumericAttr.setStorable(true);
	fnNumericAttr.setKeyable(true);
	fnNumericAttr.setReadable(true);
	fnNumericAttr.setWritable(true);
	addAttribute(aUseDistance);

	aUseNormal = fnNumericAttr.create("useNormal", "useNormal", MFnNumericData::kFloat, 0);
	fnNumericAttr.setMin(0);
	fnNumericAttr.setMax(1);
	fnNumericAttr.setStorable(true);
	fnNumericAttr.setKeyable(true);
	fnNumericAttr.setReadable(true);
	fnNumericAttr.setWritable(true);
	addAttribute(aUseNormal);

	aInputMatrix = fnMatrixAttr.create("inputMatrix", "inputMatrix", MFnMatrixAttribute::kDouble);
	fnMatrixAttr.setStorable(true);
	fnMatrixAttr.setKeyable(true);
	fnMatrixAttr.setReadable(false);
	fnMatrixAttr.setWritable(true);
	addAttribute(aInputMatrix);

	aInputMesh = fnTypedAttr.create("inputMesh", "inputMesh", MFnData::kMesh);
	fnTypedAttr.setArray(true);
	fnTypedAttr.setStorable(true);
	fnTypedAttr.setKeyable(false);
	fnTypedAttr.setReadable(false);
	fnTypedAttr.setWritable(true);
	fnTypedAttr.setCached(false);
	addAttribute(aInputMesh);

	aInputSurface = fnTypedAttr.create("inputSurface", "inputSurface", MFnData::kNurbsSurface);
	fnTypedAttr.setArray(true);
	fnTypedAttr.setStorable(true);
	fnTypedAttr.setKeyable(false);
	fnTypedAttr.setReadable(false);
	fnTypedAttr.setWritable(true);
	fnTypedAttr.setCached(false);
	addAttribute(aInputSurface);

	aOutputMatrix = fnMatrixAttr.create("outputMatrix", "outputMatrix", MFnMatrixAttribute::kDouble);
	fnMatrixAttr.setStorable(false);
	fnMatrixAttr.setKeyable(false);
	fnMatrixAttr.setReadable(true);
	fnMatrixAttr.setWritable(false);
	addAttribute(aOutputMatrix);

	aHit = fnNumericAttr.create("hit", "hit", MFnNumericData::kBoolean, false);
	fnNumericAttr.setStorable(false);
	fnNumericAttr.setKeyable(false);
	fnNumericAttr.setReadable(true);
	fnNumericAttr.setWritable(false);
	addAttribute(aHit);

	aHitDistance = fnNumericAttr.create("hitDistance", "hitDistance", MFnNumericData::kFloat, -1);
	fnNumericAttr.setStorable(false);
	fnNumericAttr.setKeyable(false);
	fnNumericAttr.setReadable(true);
	fnNumericAttr.setWritable(false);
	addAttribute(aHitDistance);

	attributeAffects(aEnvelope, aOutputMatrix);
	attributeAffects(aAlong, aOutputMatrix);
	attributeAffects(aBidirectional, aOutputMatrix);
	attributeAffects(aReverse, aOutputMatrix);
	attributeAffects(aUseDistance, aOutputMatrix);
	attributeAffects(aUseNormal, aOutputMatrix);
	attributeAffects(aInputMatrix, aOutputMatrix);
	attributeAffects(aInputMesh, aOutputMatrix);
	attributeAffects(aInputSurface, aOutputMatrix);

	attributeAffects(aEnvelope, aHit);
	attributeAffects(aAlong, aHit);
	attributeAffects(aBidirectional, aHit);
	attributeAffects(aReverse, aHit);
	attributeAffects(aUseDistance, aHit);
	attributeAffects(aUseNormal, aHit);
	attributeAffects(aInputMatrix, aHit);
	attributeAffects(aInputMesh, aHit);
	attributeAffects(aInputSurface, aHit);

	attributeAffects(aEnvelope, aHitDistance);
	attributeAffects(aAlong, aHitDistance);
	attributeAffects(aBidirectional, aHitDistance);
	attributeAffects(aReverse, aHitDistance);
	attributeAffects(aUseDistance, aHitDistance);
	attributeAffects(aUseNormal, aHitDistance);
	attributeAffects(aInputMatrix, aHitDistance);
	attributeAffects(aInputMesh, aHitDistance);
	attributeAffects(aInputSurface, aHitDistance);

	return MS::kSuccess;
}


void* RaycastNode::creator() {
	return new RaycastNode();
}


RaycastNode::RaycastNode() {
}


RaycastNode::~RaycastNode() {
}


MStatus RaycastNode::compute(const MPlug& plug, MDataBlock& data) {
	if (plug != aOutputMatrix && plug != aHit && plug != aHitDistance) {
		return MS::kUnknownParameter;
	}

	// variables
	bool hit = false;
	float hitDistance = 9999;
	MStatus status;
	MVector point;
	MVector normal;

	// input attributes
	MDataHandle dEnvelope = data.inputValue(aEnvelope, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	float envelope = dEnvelope.asFloat();

	MDataHandle dAlong = data.inputValue(aAlong, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	short along = dAlong.asShort();

	MDataHandle dReverse = data.inputValue(aReverse, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	bool reverse = dReverse.asBool();

	MDataHandle dBidirectional = data.inputValue(aBidirectional, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	bool bidirectional = dBidirectional.asBool();

	MDataHandle dUseDistance = data.inputValue(aUseDistance, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	float useDistance = dUseDistance.asFloat() * envelope;

	MDataHandle dUseNormal = data.inputValue(aUseNormal, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	float useNormal = dUseNormal.asFloat() * envelope;

	MDataHandle dInputMatrix = data.inputValue(aInputMatrix, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	MMatrix inputMatrix = dInputMatrix.asMatrix();

	MArrayDataHandle dInputSurfaceArray = data.inputArrayValue(aInputSurface, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	MArrayDataHandle dInputMeshArray = data.inputArrayValue(aInputMesh, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// output attributes
	MDataHandle dOutputMatrix = data.outputValue(aOutputMatrix, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	MDataHandle dHit = data.outputValue(aHit, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	MDataHandle dHitDistance = data.outputValue(aHitDistance, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// get origin and directions
	MVector origin = getMatrixRow<MVector>(inputMatrix, 3);
	MVector direction;
	MVectorArray directions;

	direction[along] = 1;
	direction *= inputMatrix;
	direction *= (reverse) ? -1 : 1;
	directions.append(direction);
	if (bidirectional) {
		directions.append(direction * -1);
	}

	// loop all connected geometry and populate the hit, hitDistance, point 
	// and normal variables using the closest intersection. 
	if (useDistance || useNormal) {
		for (unsigned int i = 0; i < dInputSurfaceArray.elementCount(); i++) {
			MDataHandle dInputSurface = dInputSurfaceArray.inputValue();
			MObject oInputSurface = dInputSurface.asNurbsSurface();

			if (!oInputSurface.isNull()) {
				MFnNurbsSurface fnInputSurface(oInputSurface, &status);
				CHECK_MSTATUS_AND_RETURN_IT(status);

				double distance;
				double uParameter = 0;
				double vParameter = 0;
				MPoint hitPoint;

				for (int j = 0; j < directions.length(); j++) {
					bool _hit = fnInputSurface.intersect(
						MPoint(origin),
						directions[j],
						uParameter,
						vParameter,
						hitPoint,
						kMFnNurbsEpsilon,
						MSpace::kWorld,
						true,
						&distance,
						false,
						NULL,
						&status
					);
					CHECK_MSTATUS_AND_RETURN_IT(status);

					if (_hit && distance < hitDistance) {
						point = MVector(hitPoint);
						hit = true;
						hitDistance = distance;

						if (useNormal) {
							normal = fnInputSurface.normal(uParameter, vParameter, MSpace::kWorld, &status);
							CHECK_MSTATUS_AND_RETURN_IT(status);
						}
					}
				}
			}
			dInputSurfaceArray.next();
		}
		for (unsigned int i = 0; i < dInputMeshArray.elementCount(); i++) {
			MDataHandle dInputMesh = dInputMeshArray.inputValue();
			MObject oInputMesh = dInputMesh.asMesh();

			if (!oInputMesh.isNull()) {
				MFnMesh fnInputMesh(oInputMesh, &status);
				CHECK_MSTATUS_AND_RETURN_IT(status);

				int hitFace;
				int hitTriangle;
				float hitBary1;
				float hitBary2;
				float hitRayParam;

				MFloatPoint hitPoint;
				MMeshIsectAccelParams accelerator = fnInputMesh.autoUniformGridParams();

				bool _hit = fnInputMesh.closestIntersection(
					MFloatPoint(origin),
					MFloatVector(direction),
					NULL,
					NULL,
					false,
					MSpace::kWorld,
					hitDistance,
					bidirectional,
					&accelerator,
					hitPoint,
					&hitRayParam,
					&hitFace,
					&hitTriangle,
					&hitBary1,
					&hitBary2,
					(float)1e-6,
					&status
				);
				CHECK_MSTATUS_AND_RETURN_IT(status);

				if (_hit) {
					point = MVector(hitPoint);
					hit = true;
					hitDistance = (point - origin).length();

					if (useNormal) {
						status = fnInputMesh.getPolygonNormal(hitFace, normal, MSpace::kWorld);
						CHECK_MSTATUS_AND_RETURN_IT(status);
					}
				}
			}
			dInputMeshArray.next();
		}
	}

	// calculate output matrix
	MMatrix outputMatrix = MMatrix(inputMatrix);

	// perform a linear blend operation between the input point and the point 
	// of closest intersection using the useDistance multiplier.
	if (hit && useDistance) {
		if (useDistance != 1.0) {
			point -= (point - origin) * (1 - useDistance);
		}
		setMatrixRow(outputMatrix, 3, point);
	}

	// perform a slerp operation between the input normal and the normal of 
	// the closest intersection using the useNormal multiplier.
	if (hit && useNormal) {
		int sideIndex = (along + 2) % 3;
		int forwardIndex = (along + 1) % 3;

		MVector up = getMatrixRow<MVector>(inputMatrix, along);
		MVector side = getMatrixRow<MVector>(inputMatrix, sideIndex);
		MVector forward = getMatrixRow<MVector>(inputMatrix, forwardIndex);

		float upLength = up.length();
		float sideLength = side.length();
		float forwardLength = forward.length();

		if (normal.angle(up) > M_PI * 0.5) {
			normal *= -1;
		}

		if (useNormal != 1.0) {
			MQuaternion rotateQuat = normal.rotateTo(up);
			MQuaternion slerpQuat = slerp(MQuaternion::identity, rotateQuat, 1 - useNormal);
			normal *= slerpQuat.asMatrix();
		}

		up = normal.normal() * upLength;
		side = (normal ^ forward).normal() * sideLength;
		forward = (side ^ normal).normal() * forwardLength;

		setMatrixRow(outputMatrix, along, up);
		setMatrixRow(outputMatrix, sideIndex, side);
		setMatrixRow(outputMatrix, forwardIndex, forward);
	}

	// set outputs and clean plugs
	dOutputMatrix.setMMatrix(outputMatrix);
	data.setClean(aOutputMatrix);

	dHit.setBool(hit);
	data.setClean(aHit);

	dHitDistance.setFloat((hit) ? hitDistance : -1);
	data.setClean(aHitDistance);

	return MS::kSuccess;
}
