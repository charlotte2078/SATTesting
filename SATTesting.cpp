// SATTesting.cpp: A program using the TL-Engine

#include "TL-Engine11.h" // TL-Engine11 include file and namespace
using namespace tle;

struct CoolCube
{
	Model* CubeModel;
	Model* Vertices[];
};

int main()
{
	// Create a 3D engine (using TL11 engine here) and open a window for it
	TLEngine* myEngine = New3DEngine( TL11 );
	myEngine->StartWindowed();

	// Add default folder for meshes and other media
	myEngine->AddMediaFolder( "C:\\Users\\Public\\Documents\\TL-Engine11\\Media" );

	/**** Set up your scene here ****/
	Mesh* CubeMesh = myEngine->LoadMesh("Cube.fbx");
	Model* Cube = CubeMesh->CreateModel();

	Mesh* FloorMesh = myEngine->LoadMesh("Floor.fbx");
	Model* Floor = FloorMesh->CreateModel();

	/*Mesh* SphereMesh = myEngine->LoadMesh("Sphere.fbx");
	Model* Sphere = SphereMesh->CreateModel();*/

	Mesh* BulletMesh = myEngine->LoadMesh("Bullet.x");
	Model* Bullet = BulletMesh->CreateModel();

	/*Mesh* TorusMesh = myEngine->LoadMesh("Torus.fbx");
	Model* Torus = TorusMesh->CreateModel();*/

	Camera* MyCamera = myEngine->CreateCamera(ManualCamera, 0.0f, 100.0f, 0.0f);
	MyCamera->RotateX(90.0f);

	const float MoveSpeed = 10.0f;
	const float RotateSpeed = 60.0f;

	// The main game loop, repeat until engine is stopped
	while (myEngine->IsRunning())
	{
		// Draw the scene
		myEngine->DrawScene();

		const float DeltaTime = myEngine->FrameTime();

		/**** Update your scene each frame here ****/

		// Cube control - translate
		if (myEngine->KeyHeld(Key_W))
		{
			Cube->MoveLocalZ(DeltaTime * MoveSpeed);
		}
		if (myEngine->KeyHeld(Key_S))
		{
			Cube->MoveLocalZ(-DeltaTime * MoveSpeed);
		}
		// Cube control - rotate
		if (myEngine->KeyHeld(Key_A))
		{
			Cube->RotateY(-DeltaTime * RotateSpeed);
		}
		if (myEngine->KeyHeld(Key_D))
		{
			Cube->RotateY(DeltaTime * RotateSpeed);
		}


		// Stop if the Escape key is pressed
		if (myEngine->KeyHit( Key_Escape ))
		{
			myEngine->Stop();
		}
	}

	// Delete the 3D engine now we are finished with it
	myEngine->Delete();
}
