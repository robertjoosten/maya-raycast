#include "raycastNode.h"
#include <math.h>


MTypeId RaycastNode::id(0x00000002);
MObject RaycastNode::aEnvelope;
MObject RaycastNode::aAlong;
MObject RaycastNode::aBidirectional;
MObject RaycastNode::aReverse;
MObject RaycastNode::aUseTranslate;
MObject RaycastNode::aUseRotate;
MObject RaycastNode::aInputMatrix;
MObject RaycastNode::aInputMesh;
MObject RaycastNode::aInputSurface;
MObject RaycastNode::aOutputMatrix;
MObject RaycastNode::aHit;
MObject RaycastNode::aHitDistance;


const MString RaycastNode::kName("raycast");


template <class T>
T getMatrixRow(MMatrix &matrix, const int row)
{
	T result;
	for (int i = 0; i < 3; i++) {
		result[i] = matrix[row][i];
	}
	return result;
}

template <class T>
void setMatrixRow(MMatrix &matrix, const int row, T value)
{
	for (int i = 0; i < 3; i++) {
		matrix[row][i] = value[i];
	}
}


MStatus RaycastNode::initialize() {
	MStatus status;

	MFnEnumAttribute enumAttrFn;
	MFnTypedAttribute typedAttrFn;
	MFnMatrixAttribute matrixAttrFn;
	MFnNumericAttribute numericAttrFn;

	aEnvelope = numericAttrFn.create("envelope", "envelope", MFnNumericData::kDouble, 1);
	numericAttrFn.setMin(0);
	numericAttrFn.setMax(1);
	numericAttrFn.setStorable(true);
	numericAttrFn.setKeyable(true);
	numericAttrFn.setReadable(true);
	numericAttrFn.setWritable(true);
	addAttribute(aEnvelope);

	aAlong = enumAttrFn.create("along", "along", 0);
	enumAttrFn.addField("x", 0);
	enumAttrFn.addField("y", 1);
	enumAttrFn.addField("z", 2);
	enumAttrFn.setStorable(true);
	enumAttrFn.setKeyable(true);
	enumAttrFn.setReadable(true);
	enumAttrFn.setWritable(true);
	addAttribute(aAlong);

	aBidirectional = numericAttrFn.create("bidirectional", "bidirectional", MFnNumericData::kBoolean, true);
	numericAttrFn.setStorable(true);
	numericAttrFn.setKeyable(true);
	numericAttrFn.setReadable(true);
	numericAttrFn.setWritable(true);
	addAttribute(aBidirectional);

	aReverse = numericAttrFn.create("reverse", "reverse", MFnNumericData::kBoolean, false);
	numericAttrFn.setStorable(true);
	numericAttrFn.setKeyable(true);
	numericAttrFn.setReadable(true);
	numericAttrFn.setWritable(true);
	addAttribute(aReverse);

	aUseTranslate = numericAttrFn.create("useTranslate", "useTranslate", MFnNumericData::kBoolean, true);
	numericAttrFn.setStorable(true);
	numericAttrFn.setKeyable(true);
	numericAttrFn.setReadable(true);
	numericAttrFn.setWritable(true);
	addAttribute(aUseTranslate);

	aUseRotate = numericAttrFn.create("useRotate", "useRotate", MFnNumericData::kBoolean, false);
	numericAttrFn.setStorable(true);
	numericAttrFn.setKeyable(true);
	numericAttrFn.setReadable(true);
	numericAttrFn.setWritable(true);
	addAttribute(aUseRotate);

	aInputMatrix = matrixAttrFn.create("inputMatrix", "inputMatrix", MFnMatrixAttribute::kDouble);
	matrixAttrFn.setStorable(true);
	matrixAttrFn.setKeyable(true);
	matrixAttrFn.setReadable(false);
	matrixAttrFn.setWritable(true);
	addAttribute(aInputMatrix);

	aInputMesh = typedAttrFn.create("inputMesh", "inputMesh", MFnData::kMesh);
	typedAttrFn.setArray(true);
	typedAttrFn.setStorable(true);
	typedAttrFn.setKeyable(false);
	typedAttrFn.setReadable(false);
	typedAttrFn.setWritable(true);
	typedAttrFn.setCached(false);
	addAttribute(aInputMesh);

	aInputSurface = typedAttrFn.create("inputSurface", "inputSurface", MFnData::kNurbsSurface);
	typedAttrFn.setArray(true);
	typedAttrFn.setStorable(true);
	typedAttrFn.setKeyable(false);
	typedAttrFn.setReadable(false);
	typedAttrFn.setWritable(true);
	typedAttrFn.setCached(false);
	addAttribute(aInputSurface);

	aOutputMatrix = matrixAttrFn.create("outputMatrix", "outputMatrix", MFnMatrixAttribute::kDouble);
	matrixAttrFn.setStorable(false);
	matrixAttrFn.setKeyable(false);
	matrixAttrFn.setReadable(true);
	matrixAttrFn.setWritable(false);
	addAttribute(aOutputMatrix);

	aHit = numericAttrFn.create("hit", "hit", MFnNumericData::kBoolean, false);
	numericAttrFn.setStorable(false);
	numericAttrFn.setKeyable(false);
	numericAttrFn.setReadable(true);
	numericAttrFn.setWritable(false);
	addAttribute(aHit);

	aHitDistance = numericAttrFn.create("hitDistance", "hitDistance", MFnNumericData::kDouble, -1);
	numericAttrFn.setStorable(false);
	numericAttrFn.setKeyable(false);
	numericAttrFn.setReadable(true);
	numericAttrFn.setWritable(false);
	addAttribute(aHitDistance);

	attributeAffects(aEnvelope, aOutputMatrix);
	attributeAffects(aAlong, aOutputMatrix);
	attributeAffects(aBidirectional, aOutputMatrix);
	attributeAffects(aReverse, aOutputMatrix);
	attributeAffects(aUseTranslate, aOutputMatrix);
	attributeAffects(aUseRotate, aOutputMatrix);
	attributeAffects(aInputMatrix, aOutputMatrix);
	attributeAffects(aInputMesh, aOutputMatrix);
	attributeAffects(aInputSurface, aOutputMatrix);

	attributeAffects(aEnvelope, aHit);
	attributeAffects(aAlong, aHit);
	attributeAffects(aBidirectional, aHit);
	attributeAffects(aReverse, aHit);
	attributeAffects(aUseTranslate, aHit);
	attributeAffects(aUseRotate, aHit);
	attributeAffects(aInputMatrix, aHit);
	attributeAffects(aInputMesh, aHit);
	attributeAffects(aInputSurface, aHit);

	attributeAffects(aEnvelope, aHitDistance);
	attributeAffects(aAlong, aHitDistance);
	attributeAffects(aBidirectional, aHitDistance);
	attributeAffects(aReverse, aHitDistance);
	attributeAffects(aUseTranslate, aHitDistance);
	attributeAffects(aUseRotate, aHitDistance);
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
	if (plug != aOutputMatrix && plug != aHit) {
		return MS::kUnknownParameter;
	}

	// variables
	bool hit = false;
	float hitDistance = 9999;
	MStatus status;
	MVector point;
	MVector normal;

	// input attributes
	MDataHandle dEvelope = data.inputValue(aEnvelope, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	double envelope = dEvelope.asDouble();

	MDataHandle dAlong = data.inputValue(aAlong, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	int along = dAlong.asShort();

	MDataHandle dReverse = data.inputValue(aReverse, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	bool reverse = dReverse.asBool();

	MDataHandle dBidirectional = data.inputValue(aBidirectional, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	bool bidirectional = dBidirectional.asBool();

	MDataHandle dUseTranslate = data.inputValue(aUseTranslate, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	bool useTranslate = dUseTranslate.asBool();

	MDataHandle dUseRotate = data.inputValue(aUseRotate, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	bool useRotate = dUseRotate.asBool();

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

	// calculate origin and direction
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

	// calculate intersections
	if (envelope) {
		for (int i = 0; i < dInputMeshArray.elementCount(); i++) {
			MDataHandle dInputMesh = dInputMeshArray.inputValue();
			MObject oInputMesh = dInputMesh.asMesh();

			if (!oInputMesh.isNull()) {
				MFnMesh fnInputMesh(oInputMesh, &status);
				CHECK_MSTATUS_AND_RETURN_IT(status);

				float hitRayParam;
				float hitBary1;
				float hitBary2;
				int hitFace;
				int hitTriangle;
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

					if (useRotate) {
						status = fnInputMesh.getPolygonNormal(hitFace, normal, MSpace::kWorld);
						CHECK_MSTATUS_AND_RETURN_IT(status);
					}
				}
			}
			dInputMeshArray.next();
		}
		for (int i = 0; i < dInputSurfaceArray.elementCount(); i++) {
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

						if (useRotate) {
							normal = fnInputSurface.normal(uParameter, vParameter, MSpace::kWorld, &status);
							CHECK_MSTATUS_AND_RETURN_IT(status);
						}
					}
				}
			}
			dInputSurfaceArray.next();
		}
	}
	
	// calculate output matrix
	MMatrix outputMatrix = MMatrix(inputMatrix);
	if (hit && useTranslate) {
		if (envelope != 1.0) {
			point -= (point - origin) * (1 - envelope);
		}
		setMatrixRow(outputMatrix, 3, point);
	}
	if (hit && useRotate) {
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

		if (envelope != 1.0) {
			MQuaternion rotateQuat = normal.rotateTo(up);
			MQuaternion slerpQuat = slerp(MQuaternion::identity, rotateQuat, 1 - envelope);
			normal *= slerpQuat.asMatrix();
		}

		up = normal.normal() * upLength;
		side = (normal ^ forward).normal() * sideLength;
		forward = (side ^ normal).normal() * forwardLength;

		setMatrixRow(outputMatrix, along, up);
		setMatrixRow(outputMatrix, sideIndex, side);
		setMatrixRow(outputMatrix, forwardIndex, forward);
	}

	// set outputs
	dOutputMatrix.setMMatrix(outputMatrix);
	data.setClean(aOutputMatrix);

	dHit.setBool(hit);
	data.setClean(aHit);

	dHitDistance.setFloat(hitDistance);
	data.setClean(aHitDistance);

	return MS::kSuccess;
}
