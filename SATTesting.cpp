// SATTesting.cpp: A program using the TL-Engine

#include "TL-Engine11.h" // TL-Engine11 include file and namespace
using namespace tle;

const int SquareNumCorners = 4;
const int SquareNumAxesToCheck = 2;

struct Vector2
{
	float x;
	float y;

	void Normalise();
	float Length() const;
	Vector2 Subtract(const Vector2& OtherVec) const;
	float DotProduct(const Vector2& OtherVec) const;
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

struct CollisionData
{
	Vector2 mAxis;
	float mPenetration;
	Vector2 mPointOnPlane;
};

struct CoolCube
{
	Model* CubeModel;
	Model* Vertices[];
};

bool TwoSquaresSAT(Square& Sq1, Square& Sq2);
bool CheckCollisionAxisSquares(const Vector2& Axis, const Square& Sq1, const Square& Sq2);
void GetMinMaxVertexOnAxisSquare(const Vector2& Axis, const Square& Sq, float& Min, float& Max);

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

	/*Mesh* SphereMesh = myEngine->LoadMesh("Sphere.fbx");
	Model* Sphere = SphereMesh->CreateModel();*/

	Mesh* BulletMesh = myEngine->LoadMesh("Bullet.x");
	/*Model* Bullet = BulletMesh->CreateModel();*/

	/*Mesh* TorusMesh = myEngine->LoadMesh("Torus.fbx");
	Model* Torus = TorusMesh->CreateModel();*/

	Camera* MyCamera = myEngine->CreateCamera(ManualCamera, 0.0f, 100.0f, 0.0f);
	MyCamera->RotateX(90.0f);

	const float MoveSpeed = 10.0f;
	const float RotateSpeed = 60.0f;

	// Cube test
	Square Test;
	Test.InitialiseSquare(BulletMesh, BulletMesh, 10.0f);

	Square Test2;
	Test2.InitialiseSquare(BulletMesh, BulletMesh, 20.0f);
	Test2.CentreDummy->SetPosition(50.0f, 0.0f, 0.0f);

	// The main game loop, repeat until engine is stopped
	while (myEngine->IsRunning())
	{
		// Draw the scene
		myEngine->DrawScene();

		const float DeltaTime = myEngine->FrameTime();

		/**** Update your scene each frame here ****/

		// Square control - translate
		if (myEngine->KeyHeld(Key_W))
		{
			Test.CentreDummy->MoveLocalZ(DeltaTime * MoveSpeed);
		}
		if (myEngine->KeyHeld(Key_S))
		{
			Test.CentreDummy->MoveLocalZ(-DeltaTime * MoveSpeed);
		}
		if (myEngine->KeyHeld(Key_A))
		{
			Test.CentreDummy->MoveLocalX(-DeltaTime * MoveSpeed);
		}
		if (myEngine->KeyHeld(Key_D))
		{
			Test.CentreDummy->MoveLocalX(DeltaTime * MoveSpeed);
		}

		// Check collision
		if (TwoSquaresSAT(Test, Test2))
		{
			Test.CentreDummy->SetSkin("RedBall.jpg");
			Test2.CentreDummy->SetSkin("RedBall.jpg");
		}
		else
		{
			Test.CentreDummy->SetSkin("Grass1.jpg");
			Test2.CentreDummy->SetSkin("Grass1.jpg");
		}

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

// Returns the dot product between the two vectors.
float Vector2::DotProduct(const Vector2& OtherVec) const
{
	return (x * OtherVec.x + y * OtherVec.y);
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