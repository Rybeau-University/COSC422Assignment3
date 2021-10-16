#include <iostream>
#include <fstream>
#include <GL/freeglut.h>
#include "loadTGA.h"
using namespace std;

#include <assimp/cimport.h>
#include <assimp/types.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "assimp_extras.h"
#define GL_CLAMP_TO_EDGE 0x812F
//----------Globals----------------------------
const aiScene* scene = NULL;
aiAnimation* anim;
aiVector3D scene_min, scene_max, scene_center;
float scene_scale;
bool modelRotn = true;
int tDuration;       //Animation duration in ticks.
int currTick = 0;    //current tick
float fps;
int timeStep;
float M_PI = 3.14159265;
//Light/Shadow Variables
float lightPosn[4] = { -5, 10, 10, 1 };
float shadowMat[16] = { lightPosn[1], 0, 0 ,0
						-lightPosn[0], 0, -lightPosn[2], -1,	
						0, 0,lightPosn[1], 0,
						0, 0, 0, lightPosn[1] };

//Textures
GLuint textures[3];

//Env Variables
float curtainPos = 0;

//Camera Variables
float camY = 2;
float camX = 0;
float camZ = 20;
float MOVEMENT_SPEED = 0.5;
float ROTATION_SPEED = 5;
float angle = 0;


// ------A recursive function to traverse scene graph and render each mesh----------
void render(const aiScene* sc, const aiNode* nd)
{
	aiMatrix4x4 m = nd->mTransformation;
	aiMesh* mesh;
	aiFace* face;
	float materialCol[4] = { 1, 0, 1, 1 };
	int meshIndex;

	m.Transpose();      //Convert to column-major order
	glPushMatrix();
	glMultMatrixf((float*)&m);   //Multiply by the transformation matrix for this node

	if ((strcmp((nd->mName).data, "Chest") == 0))
	{
		glPushMatrix();
			glColor3f(1, 0, 0);
			glTranslatef(0, 7, 0);
			glScalef(14, 20, 4);
			glutSolidCube(1);
		glPopMatrix();
	}
	else if ((strcmp((nd->mName).data, "Hips") == 0))
	{
		glPushMatrix();
		glColor3f(1, 0.5, 0.5);
		glScalef(14, 4, 4);
		glutSolidCube(1);
		glPopMatrix();
	}
	else if ((strcmp((nd->mName).data, "RightCollar") == 0) || (strcmp((nd->mName).data, "LeftCollar") == 0))
	{
		int side = (strcmp((nd->mName).data, "RightCollar") == 0) ? -1: 1;
		glPushMatrix();
		glColor3f(0.5, 1, 0.5);
		glTranslatef(side * 7, 0, 0);
		glutSolidSphere(3, 20, 20);
		glPopMatrix();
	}
	else if ((strcmp((nd->mName).data, "RightUpLeg") == 0) || (strcmp((nd->mName).data, "LeftUpLeg") == 0))
	{
		glPushMatrix();
		glColor3f(0, 1, 0);
		glTranslatef(0, -9, 0);
		glScalef(3, 18, 3);
		glutSolidCube(1);
		glPopMatrix();

		glPushMatrix();
		glColor3f(0.5, 1, 0.5);
		glTranslatef(0, -18, 0);
		glutSolidSphere(3, 20, 20);
		glPopMatrix();
	}
	else if ((strcmp((nd->mName).data, "RightLowLeg") == 0) || (strcmp((nd->mName).data, "LeftLowLeg") == 0))
	{
		glPushMatrix();
		glColor3f(0, 0, 1);
		glTranslatef(0, -9, 0);
		glScalef(3, 18, 3);
		glutSolidCube(1);
		glPopMatrix();
	}
	else if ((strcmp((nd->mName).data, "RightFoot") == 0) || (strcmp((nd->mName).data, "LeftFoot") == 0))
	{
		glPushMatrix();
		glColor3f(0, 1, 1);
		glTranslatef(0, -1.5, 4);
		glScalef(3, 3, 10);
		glutSolidCube(1);
		glPopMatrix();
	}
	else if ((strcmp((nd->mName).data, "RightUpArm") == 0) || (strcmp((nd->mName).data, "LeftUpArm") == 0))
	{
		glPushMatrix();
		glColor3f(1, 0, 1);
		glTranslatef(0, -6.5, 0);
		glScalef(2.5, 13, 2.5);
		glutSolidCube(1);
		glPopMatrix();

		glPushMatrix();
		glColor3f(0.5, 1, 0.5);
		glTranslatef(0, -13, 0);
		glutSolidSphere(3, 20, 20);
		glPopMatrix();
	}
	else if ((strcmp((nd->mName).data, "RightHand") == 0) || (strcmp((nd->mName).data, "LeftHand") == 0))
	{
		glPushMatrix();
		glColor3f(0.5, 0.5, 1);
		glTranslatef(0, 0, 0);
		glScalef(2.5, 14, 2.5);
		glutSolidCube(1);
		glPopMatrix();

		glPushMatrix();
		glColor3f(0.5, 1, 0.5);
		glTranslatef(0, -8, 0);
		glutSolidSphere(3, 20, 20);
		glPopMatrix();
	}
	else if ((strcmp((nd->mName).data, "Neck") == 0))
	{
		glPushMatrix();
		glColor3f(0.5, 0.5, 1);
		glTranslatef(0, 3, 0);
		glRotatef(90, 1, 0, 0);
		glutSolidCylinder(1.5, 5, 50, 10);
		glPopMatrix();
	}
	else if ((strcmp((nd->mName).data, "Head") == 0))
	{
		glPushMatrix();
			glColor3f(0.5, 0.5, 1);
			glTranslatef(0, 2, 0);
			glutSolidSphere(5, 20, 20);
		glPopMatrix();
	}

	//else
	//{
	//	// Draw all meshes assigned to this node
	//	for (int n = 0; n < nd->mNumMeshes; n++)
	//	{
	//		meshIndex = nd->mMeshes[n];          //Get the mesh indices from the current node
	//		mesh = scene->mMeshes[meshIndex];    //Using mesh index, get the mesh object
	//		glColor4fv(materialCol);   //Default material colour

	//		//Get the polygons from each mesh and draw them
	//		for (int k = 0; k < mesh->mNumFaces; k++)
	//		{
	//			face = &mesh->mFaces[k];
	//			glBegin(GL_TRIANGLES);
	//			for (int i = 0; i < face->mNumIndices; i++) {
	//				int vertexIndex = face->mIndices[i];
	//				if (mesh->HasNormals())
	//					glNormal3fv(&mesh->mNormals[vertexIndex].x);

	//				glVertex3fv(&mesh->mVertices[vertexIndex].x);
	//			}
	//			glEnd();
	//		}
	//}

	// Recursively draw all children of the current node
	for (int i = 0; i < nd->mNumChildren; i++)
		render(sc, nd->mChildren[i]);

	glPopMatrix();
}

void loadTexture()
{

	glGenTextures(3, textures);

	glBindTexture(GL_TEXTURE_2D, textures[0]);
	loadTGA("curtains.tga");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

void updateNodeMatrices(int tick) {
	aiNodeAnim* chnl;
	aiVector3D posn;
	aiQuaternion rotn;
	aiMatrix4x4 matPos;
	aiMatrix3x3 matRotn3;
	aiMatrix4x4 matRot;
	aiMatrix4x4 matprod;
	aiNode* node;
	int length = anim->mNumChannels;
	for (int i = 0; i < length; i++) {
		chnl = anim->mChannels[i];
		if (chnl->mNumPositionKeys > 1) {
			posn = chnl->mPositionKeys[tick].mValue;
		}
		else {
			posn = chnl->mPositionKeys[0].mValue;
		}
		if (chnl->mNumRotationKeys > 1) {
			rotn = chnl->mRotationKeys[tick].mValue;
		}
		else {
			rotn = chnl->mRotationKeys[0].mValue;
		}

		matPos.Translation(posn, matPos);
		matRotn3 = rotn.GetMatrix();
		matRot = aiMatrix4x4(matRotn3);
		matprod = matPos * matRot;

		node = scene->mRootNode->FindNode(chnl->mNodeName);
		node->mTransformation = matprod;
	}
}

void update(int value) {
	if (curtainPos < 3 && currTick <= tDuration) {
		updateNodeMatrices(0);
		curtainPos += 0.1;
		glutTimerFunc(timeStep, update, 0);
	}
	else if (currTick <= tDuration) {
		updateNodeMatrices(currTick);
		currTick++;	
		glutTimerFunc(timeStep, update, 0);
	}
	else {
		if (curtainPos > 0) {
			curtainPos -= 0.1;
			glutTimerFunc(timeStep, update, 0);
		}
		else {
			currTick = 0;
			updateNodeMatrices(0);
			glutTimerFunc(1000, update, 0);
		}
	}
	
	glutPostRedisplay();
}


//--------------------OpenGL initialization------------------------
void initialise()
{
	float ambient[4] = { 0.2, 0.2, 0.2, 1.0 };  //Ambient light
	float white[4] = { 1, 1, 1, 1 };		    //Light's colour
	float black[4] = { 0, 0, 0, 1 };
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_SPECULAR, black);   //Disable specular light
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(40, 1, 1.0, 500.0);

	//---- Load the model ------
	scene = aiImportFile("Dance.bvh", aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_Debone);
	anim = scene->mAnimations[0];
	fps = anim->mTicksPerSecond;
	tDuration = anim->mDuration;
	timeStep = int(1000 / fps);

	//Textures
	loadTexture();

	if (scene == NULL) exit(1);
	//printSceneInfo(scene);
	//printTreeInfo(scene->mRootNode);

	get_bounding_box(scene, &scene_min, &scene_max);
	scene_center = (scene_min + scene_max) * 0.5f;
	aiVector3D scene_diag = scene_max - scene_center;
	scene_scale = 1.0 / scene_diag.Length();
}

/* drawFloor:
 * Draws the floor for the scene.
 * Draws the Floor as many quads of the same colour so that spotlight can appear on the floor.
 * Textures the Quads
*/
void drawFloor() {
	glNormal3f(0, 1, 0);
	glColor4f(0.7, 0.7, 0.7, 1.0);
	glBegin(GL_QUADS);
	for (int i = -50; i < 50; i++)
	{
		for (int j = -50; j < 50; j++)
		{
			glNormal3f(0.0, 1.0, 0.0);
			glVertex3f(i, 0, j);
			glVertex3f(i, 0, j + 1);
			glVertex3f(i + 1, 0, j + 1);
			glVertex3f(i + 1, 0, j);
		}
	}
	glEnd();
}

void drawMaracas() {
	glPushMatrix();
		glColor3f(0, 1, 1);
		glRotatef(90, 1, 0, 0);
		glutSolidCylinder(1, 100, 50, 10);
	glPopMatrix();
}


void drawCurtains()
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	glBegin(GL_QUADS);
		glNormal3f(0, 0, 1);
		glTexCoord2f(0.0, 1.0); glVertex3f(-3, 3.6, 3);
		glTexCoord2f(0.0, 0.0); glVertex3f(-3, 0, 3);
		glTexCoord2f(1.0 - curtainPos / 3, 0.0); glVertex3f(0 - curtainPos, 0, 3);
		glTexCoord2f(1.0 - curtainPos / 3, 1.0); glVertex3f(0 - curtainPos, 3.6, 3);
		
		glTexCoord2f(0.0, 1.0); glVertex3f(3, 3.6, 3);
		glTexCoord2f(0.0, 0.0); glVertex3f(3, 0, 3);
		glTexCoord2f(1.0 - curtainPos / 3, 0.0); glVertex3f(0 + curtainPos, 0, 3);
		glTexCoord2f(1.0 - curtainPos / 3, 1.0); glVertex3f(0 + curtainPos, 3.6, 3);
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

void drawFront()
{
	glColor3f(1, 0, 0);
	glBegin(GL_QUADS);
		glNormal3f(0, 0, 1);
		glVertex3f(-5, 3.5, 3.2);
		glVertex3f(-5, 0, 3.2);
		glVertex3f(-2.9, 0, 3.2);
		glVertex3f(-2.9, 3.5, 3.2);

		glVertex3f(5, 3.5, 3.2);
		glVertex3f(5, 0, 3.2);
		glVertex3f(2.9, 0, 3.2);
		glVertex3f(2.9, 3.5, 3.2);
	glEnd();
	glBegin(GL_TRIANGLES);
		glNormal3f(0, 0, 1);
		glVertex3f(-5, 3.5, 3.2);
		glVertex3f(5, 3.5, 3.2);
		glVertex3f(0, 5, 3.2);
	glEnd();
}

void drawBack()
{
	glColor3f(1, 0, 0);
	glBegin(GL_QUADS);
		glNormal3f(0, 0, -1);
		glVertex3f(-5, 3.5, -5);
		glVertex3f(-5, 0, -5);
		glVertex3f(5, 0, -5);
		glVertex3f(5, 3.5, -5);
	glEnd();
	glBegin(GL_TRIANGLES);
		glNormal3f(0, 0, -1);
		glVertex3f(-5, 3.5, -5);
		glVertex3f(5, 3.5, -5);
		glVertex3f(0, 5, -5);
	glEnd();
}

void drawSides() 
{
	glColor3f(1, 0, 0);
	glBegin(GL_QUADS);
	glNormal3f(-1, 0, 0);
	glVertex3f(-5, 3.5, 3.2);
	glVertex3f(-5, 0, 3.2);
	glVertex3f(-5, 0, -5);
	glVertex3f(-5, 3.5, -5);

	glNormal3f(1, 0, 0);
	glVertex3f(5, 3.5, 3.2);
	glVertex3f(5, 0, 3.2);
	glVertex3f(5, 0, -5);
	glVertex3f(5, 3.5, -5);
	glEnd();
}

void drawRoof()
{
	glColor3f(1, 0, 0);
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);
	glVertex3f(-5, 3.5, 3.2);
	glVertex3f(0, 5, 3.2);
	glVertex3f(0, 5, -5);
	glVertex3f(-5, 3.5, -5);

	glVertex3f(5, 3.5, 3.2);
	glVertex3f(0, 5, 3.2);
	glVertex3f(0, 5, -5);
	glVertex3f(5, 3.5, -5);
	glEnd();
}

void drawStage()
{
	drawCurtains();
	drawFront();
	drawBack();
	drawSides();
	drawRoof();
}

//------The main display function---------
void display()
{
	float CDR = M_PI / 180.0;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(camX, camY, camZ, 0, camY, 0, 0, 1, 0);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosn);

	glPushMatrix();
		glTranslatef(-scene_center.x, -scene_center.y, -scene_center.z);
		glRotatef(angle, 0, 1, 0);
		glTranslatef(scene_center.x, scene_center.y, scene_center.z);
		drawStage();
		drawFloor();
		glScalef(scene_scale, scene_scale, scene_scale);
		glTranslatef(-scene_center.x, -scene_center.y, -scene_center.z);
		//drawMaracas();
		render(scene, scene->mRootNode);
	glPopMatrix();
	glutSwapBuffers();
}





void rotateCamera(int direction) {
	angle += direction * ROTATION_SPEED;
	if (angle >= 360) {
		angle = 0;
	}
	else if (angle <= 0) {
		angle = 360;
	}
}

void zoomCamera(int direction) {
	float moveX = -camX;
	float moveZ = -camZ;
	if (abs(moveX) > abs(moveZ)) {
		moveZ /= abs(moveX);
		moveX /= abs(moveX);
	}
	else {
		moveX /= abs(moveZ);
		moveZ /= abs(moveZ);
	}
	camX += moveX * direction * MOVEMENT_SPEED;
	camZ += moveZ * direction * MOVEMENT_SPEED;
}

void special(int key, int x, int y) {
	switch (key){
	case GLUT_KEY_LEFT:
		rotateCamera(1);
		break;
	case GLUT_KEY_RIGHT:
		rotateCamera(-1);
		break;
	case GLUT_KEY_UP:
		zoomCamera(1);
		break;
	case GLUT_KEY_DOWN:
		zoomCamera(-1);
		break;
	}
	glutPostRedisplay();
}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(800, 800);
	glutCreateWindow("Skeletal Animation");

	initialise();
	glutDisplayFunc(display);
	glutTimerFunc(timeStep, update, 0);
	glutSpecialFunc(special);
	glutMainLoop();

	aiReleaseImport(scene);
}

