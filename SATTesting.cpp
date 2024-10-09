// SATTesting.cpp: A program using the TL-Engine

#include "TL-Engine11.h" // TL-Engine11 include file and namespace

#include <vector>
#include <iostream> // For debug to console

using namespace tle;

const int SquareNumCorners = 4;
const int SquareNumAxesToCheck = 2;
const float DegreesToRadians = 3.14159265359f / 180.0f;

struct Vector2
{
	float x;
	float y;

	void Normalise();
	void Reverse();
	Vector2 MultiplyScalar(const float& k) const;
	float Length() const;
	Vector2 Subtract(const Vector2& OtherVec) const;
	Vector2 Add(const Vector2& OtherVec) const;
	float DotProduct(const Vector2& OtherVec) const;
	Vector2 PerpendicularVector() const;
};

// This is for regular polygons for now.
struct Shape
{
	Model* mCentre;
	std::vector<Model*> mVertices;
	std::vector<Vector2> mVerticesPositions;
	std::vector<Vector2> mAxes;

	void InitialiseShape(Mesh* DummyMesh, Mesh* CornerMesh, const int NumSides, const float SideLength);
	void UpdateVerticesPosition();
	void UpdateAxes();
};

struct Circle
{
	Model* mCentre;
	float mRadius;
	Vector2 mCentrePosition;
	Vector2 mAxis;

	void InitialiseCircle(Mesh* CentreMesh, const float Radius);
	void UpdateCentrePos();
	void UpdateAxis(Shape& Poly);
};

struct Square
{
	Model* CentreDummy;
	Model* VerticesArray[SquareNumCorners];
	float SideLength;
	Vector2 VerticesPositionArray[SquareNumCorners];
	Vector2 AxesArray[SquareNumAxesToCheck];

	void InitialiseSquare(Mesh* DummyMesh, Mesh* CornerMesh, const int Side);
	void UpdateVerticesPosition();
	void UpdateAxesArray();
};

// Minimum information needed to resolve a collision
// Supported by https://research.ncl.ac.uk/game/mastersdegree/gametechnologies/previousinformation/physics4collisiondetection/2017%20Tutorial%204%20-%20Collision%20Detection.pdf
// Page 4-5
struct CollisionData
{
	float mPenetration; // minimum distance along the normal the intersecting object must move
	Vector2 mNormal; // the direction vector along which the intersecting object must move to resolve the collision
	//Vector2 mPointOnPlane; // the contact point where the collision is detected

	void InitialiseData();
	void UpdateData(const Vector2& Axis, const float& Min1, const float& Max1, const float& Min2, const float& Max2);
};

struct CoolCube
{
	Model* CubeModel;
	Model* Vertices[];
};

// SAT for Squares function prototype
bool TwoSquaresSAT(Square& Sq1, Square& Sq2);
bool CheckCollisionAxisSquares(const Vector2& Axis, const Square& Sq1, const Square& Sq2);
void GetMinMaxVertexOnAxisSquare(const Vector2& Axis, const Square& Sq, float& Min, float& Max);

// SAT for Shapes function prototypes
bool TwoShapesSAT(Shape& First, Shape& Second, CollisionData& Data);
bool CheckCollisionAxisShapes(const Vector2& Axis, const Shape& First, const Shape& Second, CollisionData& Data);
void GetMinMaxVertexOnAxisShape(const Vector2& Axis, const Shape& Shape, float& Min, float& Max);

// SAT for Circles prototypes
bool ShapeToCircleSAT(Shape& FirstPolygon, Circle& SecondCircle, CollisionData& Data);
bool CheckCollisionAxisShapeCircle(const Vector2& Axis, const Shape& Poly, const Circle& Circ, CollisionData& Data);
void GetMinMaxVertexOnAxisCircle(const Vector2& Axis, const Circle& Circ, float& Min, float& Max);

int main()
{
	// Create a 3D engine (using TL11 engine here) and open a window for it
	TLEngine* myEngine = New3DEngine(TL11);
	myEngine->StartWindowed();

	// Add default folder for meshes and other media
	myEngine->AddMediaFolder("C:\\Users\\Public\\Documents\\TL-Engine11\\Media");
	//myEngine->AddMediaFolder(".\\Media");

	/**** Set up your scene here ****/
	Mesh* CubeMesh = myEngine->LoadMesh("Cube.fbx");
	/*Model* Cube = CubeMesh->CreateModel();*/

	Mesh* FloorMesh = myEngine->LoadMesh("Floor.fbx");
	Model* Floor = FloorMesh->CreateModel();

	Mesh* SphereMesh = myEngine->LoadMesh("Sphere.fbx");
	//Model* Sphere = SphereMesh->CreateModel();

	Mesh* BulletMesh = myEngine->LoadMesh("Bullet.x");
	/*Model* Bullet = BulletMesh->CreateModel();*/

	/*Mesh* TorusMesh = myEngine->LoadMesh("Torus.fbx");
	Model* Torus = TorusMesh->CreateModel();*/

	Camera* MyCamera = myEngine->CreateCamera(ManualCamera, 0.0f, 100.0f, 0.0f);
	MyCamera->RotateX(90.0f);

	const float MoveSpeed = 10.0f;
	const float RotateSpeed = 60.0f;

	// Cube test
	Shape Test;
	Test.InitialiseShape(BulletMesh, BulletMesh, 3, 10.0f);
	Test.mCentre->SetPosition(-50.0f, 0.0f, 0.0f);

	Shape Test2;
	Test2.InitialiseShape(BulletMesh, BulletMesh, 4, 20.0f);
	Test2.mCentre->SetPosition(50.0f, 0.0f, 0.0f);

	//// shape test
	//Shape Pentagon;
	//Pentagon.InitialiseShape(BulletMesh, BulletMesh, 5, 15.0f);

	// Circle test
	Circle MyCircle;
	MyCircle.InitialiseCircle(SphereMesh, 10.0f);

	MyCamera->AttachToParent(MyCircle.mCentre);

	// The main game loop, repeat until engine is stopped
	while (myEngine->IsRunning())
	{
		// Draw the scene
		myEngine->DrawScene();

		const float DeltaTime = myEngine->FrameTime();

		/**** Update your scene each frame here ****/

		//// Square control - rotate
		//if (myEngine->KeyHeld(Key_E))
		//{
		//	Test.CentreDummy->RotateY(DeltaTime * RotateSpeed);
		//}
		//if (myEngine->KeyHeld(Key_Q))
		//{
		//	Test.CentreDummy->RotateY(-DeltaTime * RotateSpeed);
		//}

		//// Square control - translate
		//if (myEngine->KeyHeld(Key_W))
		//{
		//	Test.CentreDummy->MoveLocalZ(DeltaTime * MoveSpeed);
		//}
		//if (myEngine->KeyHeld(Key_S))
		//{
		//	Test.CentreDummy->MoveLocalZ(-DeltaTime * MoveSpeed);
		//}
		//if (myEngine->KeyHeld(Key_A))
		//{
		//	Test.CentreDummy->MoveLocalX(-DeltaTime * MoveSpeed);
		//}
		//if (myEngine->KeyHeld(Key_D))
		//{
		//	Test.CentreDummy->MoveLocalX(DeltaTime * MoveSpeed);
		//}

		//// Check collision
		//if (TwoSquaresSAT(Test, Test2))
		//{
		//	Test.CentreDummy->SetSkin("RedBall.jpg");
		//	Test2.CentreDummy->SetSkin("RedBall.jpg");
		//}
		//else
		//{
		//	Test.CentreDummy->SetSkin("Grass1.jpg");
		//	Test2.CentreDummy->SetSkin("Grass1.jpg");
		//}

		//// Rotate the two other polygons
		/*Test.mCentre->RotateY(DeltaTime*RotateSpeed);
		Test2.mCentre->RotateY(DeltaTime * RotateSpeed);*/

		// Shape control - rotate
		if (myEngine->KeyHeld(Key_E))
		{
			MyCircle.mCentre->RotateY(DeltaTime * RotateSpeed);
		}
		if (myEngine->KeyHeld(Key_Q))
		{
			MyCircle.mCentre->RotateY(-DeltaTime * RotateSpeed);
		}

		// Square control - translate
		if (myEngine->KeyHeld(Key_W))
		{
			MyCircle.mCentre->MoveLocalZ(DeltaTime * MoveSpeed);
		}
		if (myEngine->KeyHeld(Key_S))
		{
			MyCircle.mCentre->MoveLocalZ(-DeltaTime * MoveSpeed);
		}
		if (myEngine->KeyHeld(Key_A))
		{
			MyCircle.mCentre->MoveLocalX(-DeltaTime * MoveSpeed);
		}
		if (myEngine->KeyHeld(Key_D))
		{
			MyCircle.mCentre->MoveLocalX(DeltaTime * MoveSpeed);
		}

		// Data for collision
		CollisionData ColData;
		ColData.InitialiseData();

		// Check circle collision with shapes
		if (ShapeToCircleSAT(Test, MyCircle, ColData))
		{
			Test.mCentre->SetSkin("RedBall.jpg");
			MyCircle.mCentre->SetSkin("RedBall.jpg");

			// Resolve collision
			MyCircle.mCentre->MoveX(-ColData.mPenetration * ColData.mNormal.x);
			MyCircle.mCentre->MoveZ(-ColData.mPenetration * ColData.mNormal.y);
		}
		else
		{
			Test.mCentre->SetSkin("Grass1.jpg");
			MyCircle.mCentre->SetSkin("Grass1.jpg");
		}

		if (ShapeToCircleSAT(Test2, MyCircle, ColData))
		{
			Test2.mCentre->SetSkin("RedBall.jpg");
			MyCircle.mCentre->SetSkin("RedBall.jpg");

			// Resolve collision
			MyCircle.mCentre->MoveX(-ColData.mPenetration * ColData.mNormal.x);
			MyCircle.mCentre->MoveZ(-ColData.mPenetration * ColData.mNormal.y);
		}
		else
		{
			Test2.mCentre->SetSkin("Grass1.jpg");
			MyCircle.mCentre->SetSkin("Grass1.jpg");
		}

		//// Check collision - First square and shape
		//if (TwoShapesSAT(Pentagon, Test, ColData))
		//{
		//	Test.mCentre->SetSkin("RedBall.jpg");
		//	Pentagon.mCentre->SetSkin("RedBall.jpg");

		//	// Resolve collision
		//	Pentagon.mCentre->MoveX(ColData.mPenetration * ColData.mNormal.x);
		//	Pentagon.mCentre->MoveZ(ColData.mPenetration * ColData.mNormal.y);

		//	std::cout << "Pen: " << ColData.mPenetration << " Normal X: " << ColData.mNormal.x << " Normal Z: " << ColData.mNormal.y << std::endl;
		//}
		//else
		//{
		//	Test.mCentre->SetSkin("Grass1.jpg");
		//	Pentagon.mCentre->SetSkin("Grass1.jpg");
		//}

		//// Check collision - Second square and shape
		//if (TwoShapesSAT(Pentagon, Test2, ColData))
		//{
		//	Test2.mCentre->SetSkin("RedBall.jpg");
		//	Pentagon.mCentre->SetSkin("RedBall.jpg");

		//	// Resolve collision
		//	Pentagon.mCentre->MoveX(ColData.mPenetration * ColData.mNormal.x);
		//	Pentagon.mCentre->MoveZ(ColData.mPenetration * ColData.mNormal.y);

		//	std::cout << "Pen: " << ColData.mPenetration << " Normal X: " << ColData.mNormal.x << " Normal Z: " << ColData.mNormal.y << std::endl;
		//}
		//else
		//{
		//	Test2.mCentre->SetSkin("Grass1.jpg");
		//	Pentagon.mCentre->SetSkin("Grass1.jpg");
		//}

		// Stop if the Escape key is pressed
		if (myEngine->KeyHit(Key_Escape))
		{
			myEngine->Stop();
		}
	}

	// Delete the 3D engine now we are finished with it
	myEngine->Delete();
}

// Normalise a vector by dividing each component by the length of the vector.
// Alters the original vector!
void Vector2::Normalise()
{
	float Size = Length();

	x /= Size;
	y /= Size;
}

// Swaps the direction of the vector
void Vector2::Reverse()
{
	x = -x;
	y = -y;
}

Vector2 Vector2::MultiplyScalar(const float& k) const
{
	return Vector2(k*x, k*y);
}

// Returns the length of the vector.
float Vector2::Length() const
{
	return sqrt(x * x + y * y);
}

// Subtracts the passed in vector from the vector and returns the result.
Vector2 Vector2::Subtract(const Vector2& OtherVec) const
{
	return Vector2((x - OtherVec.x), (y - OtherVec.y));
}

Vector2 Vector2::Add(const Vector2& OtherVec) const
{
	return Vector2(x + OtherVec.x, y + OtherVec.y);
}

// Returns the dot product between the two vectors.
float Vector2::DotProduct(const Vector2& OtherVec) const
{
	return (x * OtherVec.x + y * OtherVec.y);
}

// Returns a vector perpendicular to the current vector (clockwise)
Vector2 Vector2::PerpendicularVector() const
{
	return Vector2(-(this->y), this->x);
}

// Sets up the square
void Square::InitialiseSquare(Mesh* DummyMesh, Mesh* CornerMesh, const int Side)
{
	// Create centre dummy model
	CentreDummy = DummyMesh->CreateModel();

	// Create corners with correct local position to the centre of the cube
	SideLength = Side;
	float HalfSide = 0.5f * SideLength;
	for (int i = 0; i < SquareNumCorners; i++)
	{
		// Create corner mesh
		VerticesArray[i] = CornerMesh->CreateModel();

		// Attach to centre
		VerticesArray[i]->AttachToParent(CentreDummy);

		// Change local position - 0-+, 1++, 2+-, 3--
		if (i == 0 || i == 3)
		{
			VerticesArray[i]->SetLocalX(-HalfSide);
		}
		else
		{
			VerticesArray[i]->SetLocalX(HalfSide);
		}

		if (i == 0 || i == 1)
		{
			VerticesArray[i]->SetLocalZ(HalfSide);
		}
		else
		{
			VerticesArray[i]->SetLocalZ(-HalfSide);
		}
	}
}

void Square::UpdateVerticesPosition()
{
	for (int i = 0; i < SquareNumCorners; i++)
	{
		VerticesPositionArray[i] = { VerticesArray[i]->GetX(), VerticesArray[i]->GetZ() };
	}
}

// Only need to check 2 of the 4 axes for each square, because there are 2 parallel pairs of axes.
// Check the axis perpendicular to top and right sides.
// Top: Corner[1] - Corner[2]
// Right: Corner[2] - Corner[3]
void Square::UpdateAxesArray()
{
	for (int i = 0; i < SquareNumAxesToCheck; i++)
	{
		AxesArray[i] = VerticesPositionArray[i + 1].Subtract(VerticesPositionArray[i + 2]);
		AxesArray[i].Normalise();
	}
}

bool TwoSquaresSAT(Square& Sq1, Square& Sq2)
{
	// Udpate vertices positions of both squares
	Sq1.UpdateVerticesPosition();
	Sq2.UpdateVerticesPosition();

	// Update axes of first square
	Sq1.UpdateAxesArray();

	// Check each axis for collision. If any return false then there is no collision.
	for (int i = 0; i < SquareNumAxesToCheck; i++)
	{
		if (!CheckCollisionAxisSquares(Sq1.AxesArray[i], Sq1, Sq2))
		{
			return false;
		}
	}

	// Update axes of second square
	Sq2.UpdateAxesArray();

	// Check each axis for collision.
	for (int i = 0; i < SquareNumAxesToCheck; i++)
	{
		if (!CheckCollisionAxisSquares(Sq2.AxesArray[i], Sq1, Sq2))
		{
			return false;
		}
	}

	// Must be colliding if reach this point!
	return true;
}

// Using this link for the outline of the implementation. 
// https://research.ncl.ac.uk/game/mastersdegree/gametechnologies/previousinformation/physics4collisiondetection/2017%20Tutorial%204%20-%20Collision%20Detection.pdf
bool CheckCollisionAxisSquares(const Vector2& Axis, const Square& Sq1, const Square& Sq2)
{
	// point A = min on shape 1, point B = max on shape 1.
	// point C = min on shape 2, point D = max on shape 2.

	// get A,B,C,D
	float A, B, C, D;
	GetMinMaxVertexOnAxisSquare(Axis, Sq1, A, B);
	GetMinMaxVertexOnAxisSquare(Axis, Sq2, C, D);

	// Overlap test - first way (A < C AND B > C)
	if (A <= C && B >= C)
	{
		return true;
	}

	// Overlap test - second way (C < A AND D > A)
	if (C <= A && D >= A)
	{
		return true;
	}

	return false;
}

void GetMinMaxVertexOnAxisSquare(const Vector2& Axis, const Square& Sq, float& Min, float& Max)
{
	// Assume initial min/max
	Min = Sq.VerticesPositionArray[0].DotProduct(Axis);
	Max = Min;

	// Loop through remaining vertices to find min/max
	for (int i = 1; i < SquareNumCorners; i++)
	{
		float Projection = Sq.VerticesPositionArray[i].DotProduct(Axis);

		if (Projection < Min)
		{
			Min = Projection;
		}

		if (Projection > Max)
		{
			Max = Projection;
		}
	}
}

void Shape::InitialiseShape(Mesh* DummyMesh, Mesh* CornerMesh, const int NumSides, const float SideLength)
{
	// Create centre dummy model
	mCentre = DummyMesh->CreateModel();

	// Calculate how many degrees to turn to each corner
	const float DegreesToTurn = 360.0f / NumSides;
	const float RadiansToTurn = DegreesToTurn * DegreesToRadians;

	// Create corners with correct local position to the centre
	for (int i = 0; i < NumSides; i++)
	{
		// Create corner and attach to centre
		mVertices.push_back(CornerMesh->CreateModel());
		mVertices.at(i)->AttachToParent(mCentre);

		// Change corner's local position
		mVertices.at(i)->MoveLocalX(SideLength * sin(i * RadiansToTurn));
		mVertices.at(i)->MoveLocalZ(SideLength * cos(i * RadiansToTurn));

		// Reserve space in VerticesPositions and Axes vectors.
		mVerticesPositions.push_back({ mVertices.at(i)->GetX(), mVertices.at(i)->GetZ() });
		mAxes.push_back({ 0.0f, 0.0f });
	}
}

void Shape::UpdateVerticesPosition()
{
	for (int i = 0; i < mVertices.size(); i++)
	{
		mVerticesPositions.at(i) = { mVertices.at(i)->GetX(), mVertices.at(i)->GetZ() };
	}
}

// Axes are the normals to each side of the shape. There will be the same number of axes as vertices.
void Shape::UpdateAxes()
{
	for (int i = 0; i < mVertices.size(); i++)
	{
		if (i == mVertices.size() - 1)
		{
			mAxes.at(i) = mVerticesPositions.at(0).Subtract(mVerticesPositions.at(i));
		}
		else
		{
			mAxes.at(i) = mVerticesPositions.at(i + 1).Subtract(mVerticesPositions.at(i));
		}

		mAxes.at(i).Normalise();
		mAxes.at(i) = mAxes.at(i).PerpendicularVector();
	}
}


bool TwoShapesSAT(Shape& First, Shape& Second, CollisionData& Data)
{
	// Udpate vertices positions of both squares
	First.UpdateVerticesPosition();
	Second.UpdateVerticesPosition();

	// Update axes of first square
	First.UpdateAxes();

	// Check each axis for collision. If any return false then there is no collision.
	for (int i = 0; i < First.mAxes.size(); i++)
	{
		if (!CheckCollisionAxisShapes(First.mAxes.at(i), First, Second, Data))
		{
			return false;
		}
	}

	// Update axes of second square
	Second.UpdateAxes();

	// Check each axis for collision.
	for (int i = 0; i < Second.mAxes.size(); i++)
	{
		if (!CheckCollisionAxisShapes(Second.mAxes.at(i), First, Second, Data))
		{
			return false;
		}
	}

	// Must be colliding if reach this point!
	return true;
}

// Using this link for the outline of the implementation. 
// https://research.ncl.ac.uk/game/mastersdegree/gametechnologies/previousinformation/physics4collisiondetection/2017%20Tutorial%204%20-%20Collision%20Detection.pdf
bool CheckCollisionAxisShapes(const Vector2& Axis, const Shape& First, const Shape& Second, CollisionData& Data)
{
	// point A = min on shape 1, point B = max on shape 1.
	// point C = min on shape 2, point D = max on shape 2.

	// get A,B,C,D
	float Min1, Max1, Min2, Max2;
	GetMinMaxVertexOnAxisShape(Axis, First, Min1, Max1);
	GetMinMaxVertexOnAxisShape(Axis, Second, Min2, Max2);

	// Overlap test 
	// First way (A < C AND B > C)
	// Second way (C < A AND D > A)
	if ((Min1 <= Min2 && Max1 >= Min2) || (Min2 <= Min1 && Max2 >= Min1))
	{
		// If they are overlapping, update collision data.
		Data.UpdateData(Axis, Min1, Max1, Min2, Max2);
		
		Vector2 NormalDirection(First.mCentre->GetX() - Second.mCentre->GetX(), First.mCentre->GetZ() - Second.mCentre->GetZ());
		if (NormalDirection.DotProduct(Data.mNormal) < 0.0f)
		{
			Data.mNormal.Reverse();
		}

		return true;
	}

	return false;
}

void GetMinMaxVertexOnAxisShape(const Vector2& Axis, const Shape& Shape, float& Min, float& Max)
{
	// Assume initial min/max
	Min = Shape.mVerticesPositions.at(0).DotProduct(Axis);
	Max = Min;

	// Loop through remaining vertices to find min/max
	for (int i = 1; i < Shape.mVerticesPositions.size(); i++)
	{
		float Projection = Shape.mVerticesPositions.at(i).DotProduct(Axis);

		if (Projection < Min)
		{
			Min = Projection;
		}

		if (Projection > Max)
		{
			Max = Projection;
		}
	}
}

bool ShapeToCircleSAT(Shape& FirstPolygon, Circle& SecondCircle, CollisionData& Data)
{
	// Udpate vertices positions of polygon and centre position of circle
	FirstPolygon.UpdateVerticesPosition();
	SecondCircle.UpdateCentrePos();

	// Update axes of polygon
	FirstPolygon.UpdateAxes();

	// Check each axis for collision. If any return false then there is no collision.
	for (int i = 0; i < FirstPolygon.mAxes.size(); i++)
	{
		if (!CheckCollisionAxisShapeCircle(FirstPolygon.mAxes.at(i), FirstPolygon, SecondCircle, Data))
		{
			return false;
		}
	}

	// Update axis of circle
	SecondCircle.UpdateAxis(FirstPolygon);

	// Check axis for collision
	if (!CheckCollisionAxisShapeCircle(SecondCircle.mAxis, FirstPolygon, SecondCircle, Data))
	{
		return false;
	}

	// Must be colliding if reach this point!
	return true;
}

void CollisionData::InitialiseData()
{
	mPenetration = FLT_MAX; // Initialise to a large number so we find correct minimum
	mNormal = { 0.0f, 0.0f };
	//mPointOnPlane = { 0.0f, 0.0f };
}

// With help from https://youtu.be/SUyG3aV_vpM?si=cCNq6pM6ntW2Tt8
void CollisionData::UpdateData(const Vector2& Axis, const float& Min1, const float& Max1, const float& Min2, const float& Max2)
{
	float AxisDepth = Max2 - Min1;
	if ((Max1 - Min2) < AxisDepth)
	{
		AxisDepth = Max1 - Min2;
	}

	if (AxisDepth < mPenetration)
	{
		mPenetration = AxisDepth;
		mNormal = Axis;
	}
}

void Circle::InitialiseCircle(Mesh* CentreMesh, const float Radius)
{
	mCentre = CentreMesh->CreateModel();
	mRadius = Radius;
	mCentrePosition = { 0.0f, 0.0f };
}

void Circle::UpdateCentrePos()
{
	mCentrePosition.x = mCentre->GetX();
	mCentrePosition.y = mCentre->GetZ();
}

// Axis to use for a circle is from the centre of the circle to the closest point on the polygon.
void Circle::UpdateAxis(Shape& Poly)
{
	float MinDist = FLT_MAX;
	int ClosestIndex = -1;

	for (int i = 0; i < Poly.mVerticesPositions.size(); i++)
	{
		float CurrentDist = (Poly.mVerticesPositions.at(i).Subtract(mCentrePosition)).Length();

		if (CurrentDist < MinDist)
		{
			MinDist = CurrentDist;
			ClosestIndex = i;
		}
	}

	mAxis = Poly.mVerticesPositions.at(ClosestIndex).Subtract(mCentrePosition);
	mAxis.Normalise();
}

// Using this video for outline of implementation https://youtu.be/vWs33LVrs74?si=OyFbAbT5qoq8Um0w
bool CheckCollisionAxisShapeCircle(const Vector2& Axis, const Shape& Poly, const Circle& Circ, CollisionData& Data)
{
	// point A = min on shape 1, point B = max on shape 1.
	// point C = min on shape 2, point D = max on shape 2.

	// get A,B,C,D
	float Min1, Max1, Min2, Max2;
	GetMinMaxVertexOnAxisShape(Axis, Poly, Min1, Max1);
	GetMinMaxVertexOnAxisCircle(Axis, Circ, Min2, Max2);

	// Overlap test 
	// First way (A < C AND B > C)
	// Second way (C < A AND D > A)
	if ((Min1 <= Min2 && Max1 >= Min2) || (Min2 <= Min1 && Max2 >= Min1))
	{
		// If they are overlapping, update collision data.
		Data.UpdateData(Axis, Min1, Max1, Min2, Max2);

		Vector2 NormalDirection(Poly.mCentre->GetX() - Circ.mCentre->GetX(), Poly.mCentre->GetZ() - Circ.mCentre->GetZ());
		if (NormalDirection.DotProduct(Data.mNormal) < 0.0f)
		{
			Data.mNormal.Reverse();
		}

		return true;
	}

	return false;
}

void GetMinMaxVertexOnAxisCircle(const Vector2& Axis, const Circle& Circ, float& Min, float& Max)
{
	Vector2 RadiusAlongAxis = Axis.MultiplyScalar(Circ.mRadius);

	Vector2 MaxPoint = Circ.mCentrePosition.Add(RadiusAlongAxis);
	Vector2 MinPoint = Circ.mCentrePosition.Subtract(RadiusAlongAxis);

	Max = MaxPoint.DotProduct(Axis);
	Min = MinPoint.DotProduct(Axis);

	// Check values are correct way around, swap if not.
	if (Min > Max)
	{
		float temp = Min;
		Min = Max;
		Max = temp;
	}
}