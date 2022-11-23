// Reading Obj file
#define  _CRT_SECURE_NO_WARNINGS

#include "shader.h"
#include "objRead.h"
// 13 14 15 16
#define PROB 2

using namespace std;

GLuint g_window_w = 600;
GLuint g_window_h = 600;

#if PROB == 1

GLuint VAO[3];
GLuint VBO_position[3];
GLuint VBO_normal[3];
GLuint VBO_color[3];

int polygon_mode = 2;

const int num_vertices = 3;
const int num_triangles = 1;

glm::mat4 TR;
static BOOL k_up = false, k_down = false, k_left = false, k_right = false;
static BOOL draw_cube = false, draw_pyramid = false;
static BOOL turn_x_left = false, turn_x_right = false, turn_y_left = false, turn_y_right = false;

GLfloat change_angle[4] = { 30.0f, -30.0f, 30.0f, -30.0f }, change_pos[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

void Display();
void Reshape(int w, int h);
void Keyboard(unsigned char key, int x, int y);
void InitBuffer();
void reset_angle();
void reset_All();
void TimerFunction(int value);
//--- load obj related variabales
objRead objReader[2];
GLint Cube = objReader[0].loadObj_normalize_center("cube.obj");
GLint Pyramid = objReader[1].loadObj_normalize_center("pyramid.obj");

int main(int argc, char** argv)
{
	// create window using freeglut
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(g_window_w, g_window_h);
	glutInitWindowPosition(0, 0);

	glutCreateWindow("Computer Graphics");

	//////////////////////////////////////////////////////////////////////////////////////
	//// initialize GLEW
	//////////////////////////////////////////////////////////////////////////////////////
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Unable to initialize GLEW ... exiting" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
	{
		std::cout << "GLEW OK\n";
	}

	//////////////////////////////////////////////////////////////////////////////////////
	//// Create shader program an register the shader
	//////////////////////////////////////////////////////////////////////////////////////

	GLuint vShader[4];
	GLuint fShader[4];

	vShader[0]= MakeVertexShader("vertex.glsl", 0);			// Sun
	fShader[0] = MakeFragmentShader("fragment.glsl", 0);

	// shader Program
	s_program[0] = glCreateProgram();
	glAttachShader(s_program[0], vShader[0]);
	glAttachShader(s_program[0], fShader[0]);
	glLinkProgram(s_program[0]);
	checkCompileErrors(s_program[0], "PROGRAM");

	InitBuffer();

	// callback functions
	glutKeyboardFunc(Keyboard);
	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


	// freeglut 윈도우 이벤트 처리 시작. 윈도우가 닫힐때까지 후한루프 실행.
	glutMainLoop();

	return 0;
}
void InitBuffer()
{
	//// 5.1. VAO 객체 생성 및 바인딩
	GLint pAttribute, nAttribute;
	// 2 triangles for quad floor
	glUseProgram(s_program[0]);

	for (int i = 0; i < 2; i++) {
		glGenVertexArrays(1, &VAO[i]);
		glBindVertexArray(VAO[i]);
		glGenBuffers(3, VBO_position);
		
		glBindBuffer(GL_ARRAY_BUFFER, VBO_position[0]);
		glBufferData(GL_ARRAY_BUFFER, objReader[i].outvertex.size() * sizeof(glm::vec3), &objReader[i].outvertex[0], GL_STATIC_DRAW);
		pAttribute = glGetAttribLocation(s_program[0], "aPos");
		glVertexAttribPointer(pAttribute, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(pAttribute);
	}
	glEnable(GL_DEPTH_TEST);
}
void Display()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (polygon_mode == 1)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else if (polygon_mode == 2)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glUseProgram(s_program[0]);
	TR = glm::mat4(1.0f);

	TR = glm::rotate(TR, glm::radians(30.0f), glm::vec3(1.0, 0.0, 0.0));
	TR = glm::rotate(TR, glm::radians(-30.0f), glm::vec3(0.0, 1.0, 0.0));
	TR = glm::scale(TR, glm::vec3(0.5, 0.5, 0.5));

	if (turn_x_right)
		TR = glm::rotate(TR, glm::radians(change_angle[0]), glm::vec3(1.0, 0.0, 0.0));
	else if (turn_x_left)
		TR = glm::rotate(TR, glm::radians(-change_angle[1]), glm::vec3(1.0, 0.0, 0.0));
	else if (turn_y_right)
		TR = glm::rotate(TR, glm::radians(change_angle[2]), glm::vec3(0.0, 1.0, 0.0));
	else if (turn_y_left)
		TR = glm::rotate(TR, glm::radians(-change_angle[3]), glm::vec3(0.0, 1.0, 0.0));

	if (k_up) {
		TR = glm::translate(TR, glm::vec3(0.0, change_pos[0], 0.0));
		TR = glm::translate(TR, glm::vec3(0.0, -change_pos[1], 0.0));
		TR = glm::translate(TR, glm::vec3(-change_pos[2], 0.0, 0.0));
		TR = glm::translate(TR, glm::vec3(change_pos[3], 0.0, 0.0));
		change_pos[0] += 0.01f;
	}
	else if (k_down) {
		TR = glm::translate(TR, glm::vec3(0.0, change_pos[0], 0.0));
		TR = glm::translate(TR, glm::vec3(0.0, -change_pos[1], 0.0));
		TR = glm::translate(TR, glm::vec3(-change_pos[2], 0.0, 0.0));
		TR = glm::translate(TR, glm::vec3(change_pos[3], 0.0, 0.0));
		change_pos[1] += 0.01f;
	}
	else if (k_left) {
		TR = glm::translate(TR, glm::vec3(0.0, change_pos[0], 0.0));
		TR = glm::translate(TR, glm::vec3(0.0, -change_pos[1], 0.0));
		TR = glm::translate(TR, glm::vec3(-change_pos[2], 0.0, 0.0));
		TR = glm::translate(TR, glm::vec3(change_pos[3], 0.0, 0.0));
		change_pos[2] += 0.01f;
	}
	else if (k_right) {
		TR = glm::translate(TR, glm::vec3(0.0, change_pos[0], 0.0));
		TR = glm::translate(TR, glm::vec3(0.0, -change_pos[1], 0.0));
		TR = glm::translate(TR, glm::vec3(-change_pos[2], 0.0, 0.0));
		TR = glm::translate(TR, glm::vec3(change_pos[3], 0.0, 0.0));
		change_pos[3] += 0.01f;
	}

	unsigned int modelLocation = glGetUniformLocation(s_program[0], "m");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));

	if (draw_cube) {
		glBindVertexArray(VAO[0]);
		glDrawArrays(GL_TRIANGLES, 0, Cube);
	}
	else if (draw_pyramid) {
		glBindVertexArray(VAO[1]);
		glDrawArrays(GL_TRIANGLES, 0, Pyramid);
	}
	glutSwapBuffers();
}
void Reshape(int w, int h)
{
	g_window_w = w;
	g_window_h = h;
	glViewport(0, 0, w, h);
}
void Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'c':
		draw_cube = true;
		draw_pyramid = false;
		break;
	case 'p':
		draw_cube = false;
		draw_pyramid = true;
		break;
	case 'x':
		k_left = false;
		k_right = false;
		k_up = false;
		k_down = false;
		reset_angle();
		turn_x_right = true;
		turn_x_left = false;
		turn_y_right = false;
		turn_y_left = false;
		glutTimerFunc(100, TimerFunction, 1);
		break;
	case 'z':
		k_left = false;
		k_right = false;
		k_up = false;
		k_down = false;
		reset_angle();
		turn_x_right = false;
		turn_x_left = true;
		turn_y_right = false;
		turn_y_left = false;
		glutTimerFunc(100, TimerFunction, 1);
		break;
	case 'y':
		k_left = false;
		k_right = false;
		k_up = false;
		k_down = false;
		reset_angle();
		turn_x_right = false;
		turn_x_left = false;
		turn_y_right = true;
		turn_y_left = false;
		glutTimerFunc(100, TimerFunction, 1);
		break;
	case 'u':
		k_left = false;
		k_right = false;
		k_up = false;
		k_down = false;
		reset_angle();
		turn_x_right = false;
		turn_x_left = false;
		turn_y_right = false;
		turn_y_left = true;
		glutTimerFunc(100, TimerFunction, 1);
		break;
	case 'h':
		glEnable(GL_DEPTH_TEST);
		break;
	case 'g':
		glDisable(GL_DEPTH_TEST);
		break;
	case '1':
		polygon_mode = 1;
		break;
	case '2':
		polygon_mode = 2;
		break;
	case 'a':
		k_left = true;
		k_right = false;
		k_up = false;
		k_down = false;
		break;
	case 'd':
		k_right = true;
		k_left = false;
		k_up = false;
		k_down = false;
		break;
	case 'w':
		k_up = true;
		k_left = false;
		k_right = false;
		k_down = false;
		break;
	case 's':
		k_down = true;
		k_left = false;
		k_right = false;
		k_up = false;
		break;
	case 't':
		reset_All();
		break;
	case 'q': case 'Q':
		exit(0);
	}
	InitBuffer();
	glutPostRedisplay();
}
void reset_angle() {
	for (int i = 0; i < 4; i++)
		change_angle[i] = 0.25f;
}
void reset_All() {
	for (int i = 0; i < 4; i++)
		change_angle[i] = 0.25f;
	for (int i = 0; i < 4; i++) 
		change_pos[i] = 0.1f;
	turn_x_right = false;
	turn_x_left = false;
	turn_y_right = false;
	turn_y_left = false;
	k_down = false;
	k_left = false;
	k_right = false;
	k_up = false;
}
void TimerFunction(int value)
{
	GLfloat speed = 2.0f;
	if(turn_x_right)
		change_angle[0] += speed;
	else if (turn_x_left)
		change_angle[1] += speed;
	else if (turn_y_right)
		change_angle[2] += speed;
	else if (turn_y_left)
		change_angle[3] += speed;

	InitBuffer();
	glutPostRedisplay();
	glutTimerFunc(100, TimerFunction, 1);
}

#endif

#if PROB == 2
#include <math.h>
GLuint VAO[4];
GLuint VBO_position[3];
GLuint VBO_normal[3];
GLuint VBO_color[3];

unsigned int modelLocation;

int polygon_mode = 2;

const int num_vertices = 3;
const int num_triangles = 1;

glm::mat4 TR;

void Display();
void Reshape(int w, int h);
void Keyboard(unsigned char key, int x, int y);
void InitBuffer();
void reset();
void all_reset();
void TimerFunction(int value);

BOOL alone_x_plus[2] = { false, false },
alone_x_minus[2] = { false, false },
alone_y_plus[2] = { false, false },
alone_y_minus[2] = { false, false };
BOOL every_plus = false, every_minus = false;
BOOL change_rect = false;

GLfloat speed = 10.0f, angle[2][4] = { {speed, speed, speed, speed}, {speed, speed, speed, speed} };
GLfloat rot = speed;

//--- load obj related variabales
objRead objReader[4];
GLint Object0 = objReader[0].loadObj_normalize_center("cube.obj");
GLint Object1 = objReader[1].loadObj_normalize_center("sphere.obj");
GLint Object2 = objReader[2].loadObj_normalize_center("pyramid.obj");
GLint Object3 = objReader[3].loadObj_normalize_center("cone.obj");

int main(int argc, char** argv)
{
	// create window using freeglut
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(g_window_w, g_window_h);
	glutInitWindowPosition(300, 0);

	glutCreateWindow("Computer Graphics");

	//////////////////////////////////////////////////////////////////////////////////////
	//// initialize GLEW
	//////////////////////////////////////////////////////////////////////////////////////
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Unable to initialize GLEW ... exiting" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
	{
		std::cout << "GLEW OK\n";
	}

	//////////////////////////////////////////////////////////////////////////////////////
	//// Create shader program an register the shader
	//////////////////////////////////////////////////////////////////////////////////////

	GLuint vShader[4];
	GLuint fShader[4];

	vShader[0] = MakeVertexShader("vertex.glsl", 0);			// Sun
	fShader[0] = MakeFragmentShader("fragment.glsl", 0);

	// shader Program
	s_program[0] = glCreateProgram();
	glAttachShader(s_program[0], vShader[0]);
	glAttachShader(s_program[0], fShader[0]);
	glLinkProgram(s_program[0]);
	checkCompileErrors(s_program[0], "PROGRAM");

	InitBuffer();

	// callback functions
	glutKeyboardFunc(Keyboard);
	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


	// freeglut 윈도우 이벤트 처리 시작. 윈도우가 닫힐때까지 후한루프 실행.
	glutMainLoop();

	return 0;
}
void InitBuffer()
{
	GLint nAttribute;
	GLint pAttribute;

	// 2 triangles for quad floor
	for (int i = 0; i < 4; i++) {
		glGenVertexArrays(1, &VAO[i]);
		glBindVertexArray(VAO[i]);
		glGenBuffers(3, VBO_position);

		glBindBuffer(GL_ARRAY_BUFFER, VBO_position[0]);
		glBufferData(GL_ARRAY_BUFFER, objReader[i].outvertex.size() * sizeof(glm::vec3), &objReader[i].outvertex[0], GL_STATIC_DRAW);
		pAttribute = glGetAttribLocation(s_program[0], "aPos");
		glVertexAttribPointer(pAttribute, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(pAttribute);
	}

	glEnable(GL_DEPTH_TEST);
}
void Display()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//cube
	glUseProgram(s_program[0]);

	TR = glm::mat4(1.0f);
	TR = glm::rotate(TR, glm::radians(30.0f), glm::vec3(1.0, 0.0, 0.0));
	TR = glm::rotate(TR, glm::radians(-30.0f), glm::vec3(0.0, 1.0, 0.0));
	TR = glm::scale(TR, glm::vec3(0.2, 0.2, 0.2));

	if (every_plus)
		TR = glm::rotate(TR, glm::radians(rot), glm::vec3(0.0, 1.0, 0.0));
	else if(every_minus)
		TR = glm::rotate(TR, glm::radians(-rot), glm::vec3(0.0, 1.0, 0.0));

	TR = glm::translate(TR, glm::vec3(-4.0, 0.0, 0.0));

	if (alone_x_plus[0])
		TR = glm::rotate(TR, glm::radians(angle[0][0]), glm::vec3(1.0, 0.0, 0.0));
	else if (alone_x_minus[0])
		TR = glm::rotate(TR, glm::radians(-angle[0][1]), glm::vec3(1.0, 0.0, 0.0));
	else if (alone_y_plus[0])
		TR = glm::rotate(TR, glm::radians(angle[0][2]), glm::vec3(0.0, 1.0, 0.0));
	else if (alone_y_minus[0])
		TR = glm::rotate(TR, glm::radians(-angle[0][3]), glm::vec3(0.0, 1.0, 0.0));

	modelLocation = glGetUniformLocation(s_program[0], "m");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));

	if (!change_rect) {
		glBindVertexArray(VAO[0]);
		glDrawArrays(GL_TRIANGLES, 0, Object0);
	}
	else {
		glBindVertexArray(VAO[2]);
		glDrawArrays(GL_TRIANGLES, 0, Object2);
	}
	//sphere
	TR = glm::mat4(1.0f);
	TR = glm::rotate(TR, glm::radians(30.0f), glm::vec3(1.0, 0.0, 0.0));
	TR = glm::rotate(TR, glm::radians(-30.0f), glm::vec3(0.0, 1.0, 0.0));
	TR = glm::scale(TR, glm::vec3(0.2, 0.2, 0.2));

	if (every_plus)
		TR = glm::rotate(TR, glm::radians(rot), glm::vec3(0.0, 1.0, 0.0));
	else if (every_minus)
		TR = glm::rotate(TR, glm::radians(-rot), glm::vec3(0.0, 1.0, 0.0));

	TR = glm::translate(TR, glm::vec3(4.0, 0.0, 0.0));

	if (alone_x_plus[1])
		TR = glm::rotate(TR, glm::radians(angle[1][0]), glm::vec3(1.0, 0.0, 0.0));
	else if (alone_x_minus[1])
		TR = glm::rotate(TR, glm::radians(-angle[1][1]), glm::vec3(1.0, 0.0, 0.0));
	else if (alone_y_plus[1])
		TR = glm::rotate(TR, glm::radians(angle[1][2]), glm::vec3(0.0, 1.0, 0.0));
	else if (alone_y_minus[1])
		TR = glm::rotate(TR, glm::radians(-angle[1][3]), glm::vec3(0.0, 1.0, 0.0));

	modelLocation = glGetUniformLocation(s_program[0], "m");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));

	if (!change_rect) {
		glBindVertexArray(VAO[1]);
		glDrawArrays(GL_TRIANGLES, 0, Object1);
	}
	else {
		glBindVertexArray(VAO[3]);
		glDrawArrays(GL_TRIANGLES, 0, Object3);
	}

	glutSwapBuffers();
}
void Reshape(int w, int h)
{
	g_window_w = w;
	g_window_h = h;
	glViewport(0, 0, w, h);
}
void Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 's': case 'S':
		all_reset();
		break;
	case 'e':
		change_rect = true;
		break;
	case 'r':
		reset();
		every_plus = true;
		every_minus = false;
		glutTimerFunc(100, TimerFunction, 1);
		break;
	case 'R':
		reset();
		every_plus = false;
		every_minus = true;
		glutTimerFunc(100, TimerFunction, 1);
		break;
	case 'a':
		alone_x_plus[0] = true; alone_x_minus[0] = false;
		alone_y_plus[0] = false; alone_y_minus[0] = false;
		glutTimerFunc(100, TimerFunction, 1);
		break;
	case 'A':
		alone_x_plus[0] = false; alone_x_minus[0] = true;
		alone_y_plus[0] = false; alone_y_minus[0] = false;
		glutTimerFunc(100, TimerFunction, 1);
		break;
	case 'b':
		alone_x_plus[0] = false; alone_x_minus[0] = false;
		alone_y_plus[0] = true; alone_y_minus[0] = false;
		glutTimerFunc(100, TimerFunction, 1);
		break;
	case 'B':
		alone_x_plus[0] = false; alone_x_minus[0] = false;
		alone_y_plus[0] = false; alone_y_minus[0] = true;
		glutTimerFunc(100, TimerFunction, 1);
		break;
	case 'c':
		alone_x_plus[1] = true; alone_x_minus[1] = false;
		alone_y_plus[1] = false; alone_y_minus[1] = false;
		glutTimerFunc(100, TimerFunction, 1);
		break;
	case 'C':
		alone_x_plus[1] = false; alone_x_minus[1] = true;
		alone_y_plus[1] = false; alone_y_minus[1] = false;
		glutTimerFunc(100, TimerFunction, 1);
		break;
	case 'd':
		alone_x_plus[1] = false; alone_x_minus[1] = false;
		alone_y_plus[1] = true; alone_y_minus[1] = false;
		glutTimerFunc(100, TimerFunction, 1);
		break;
	case 'D':
		alone_x_plus[1] = false; alone_x_minus[1] = false;
		alone_y_plus[1] = false; alone_y_minus[1] = true;
		glutTimerFunc(100, TimerFunction, 1);
		break;
	case 'q': case 'Q':
		exit(0);
		break;
	}
	InitBuffer();
	glutPostRedisplay();
}
void all_reset() {
	change_rect = false;
	every_plus = false; every_minus = false;
	alone_x_plus[0] = false; alone_x_minus[0] = false;
	alone_y_plus[0] = false; alone_y_minus[0] = false;
	alone_x_plus[1] = false; alone_x_minus[1] = false;
	alone_y_plus[1] = false; alone_y_minus[1] = false;
}
void reset() {
	alone_x_plus[0] = false; alone_x_minus[0] = false;
	alone_y_plus[0] = false; alone_y_minus[0] = false;
	alone_x_plus[1] = false; alone_x_minus[1] = false;
	alone_y_plus[1] = false; alone_y_minus[1] = false;
}
void TimerFunction(int value)
{
	if (every_plus || every_minus)
		rot += speed;

	for (int i = 0; i < 2; i++) {
		if (alone_x_plus[i])
			angle[i][0] += speed;
		else if (alone_x_minus[i])
			angle[i][1] += speed;
		else if (alone_y_plus[i])
			angle[i][2] += speed;
		else if (alone_y_minus[i])
			angle[i][3] += speed;
	}
	InitBuffer();
	glutPostRedisplay();
	glutTimerFunc(500, TimerFunction, 1);
}

#endif

#if PROB == 3
#include <math.h>
GLuint VAO[4];
GLuint VBO_position[3];
GLuint VBO_normal[3];
GLuint VBO_color[3];

unsigned int modelLocation;

int polygon_mode = 2;

const int num_vertices = 3;
const int num_triangles = 1;

glm::mat4 TR;

void Display();
void Reshape(int w, int h);
void Keyboard(unsigned char key, int x, int y);
void InitBuffer();
void reset();
void all_reset();
void TimerFunction(int value);

static char s = 'r';
static int a = -1, b = -1;

BOOL move_control[2] = {false, false};
BOOL scale_rect[2][2] = {{ false, false }, { false, false }};
BOOL change_rect = false;
BOOL stop_scale[2] = { false, false };
BOOL move_center = false, move_return = false;
BOOL move_change = false;
BOOL move_updownx[2] = { false , false }, move_updowny[2] = { false, false };
BOOL tornado = false;

GLfloat speed = 1.0f, mass[2] = { 0.01f , 0.01f };
GLfloat pos_x[2][2] = { { 4.0f, 4.0f}, {4.0f, 4.0f} },
pos_y[2][2] = { { 4.0f, 4.0f}, {4.0f, 4.0f} }, 
pos_z[2][2] = { { 4.0f, 4.0f}, {4.0f, 4.0f} };
GLfloat scale_size[2] = {-0.2f, -0.2f};
GLfloat p1 = -1.0f , p2 = -1.0f;
GLfloat move_c = 0.0f;
GLfloat s1 = 0.2f, s2 = 0.2f;
GLfloat r, e = 0.0f, p[3] = {0.0f, 0.0f, 0.0f};

//--- load obj related variabales
objRead objReader[4];
GLint Object0 = objReader[0].loadObj_normalize_center("cube.obj");
GLint Object1 = objReader[1].loadObj_normalize_center("sphere.obj");
GLint Object2 = objReader[2].loadObj_normalize_center("pyramid.obj");
GLint Object3 = objReader[3].loadObj_normalize_center("cone.obj");

int main(int argc, char** argv)
{
	// create window using freeglut
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(g_window_w, g_window_h);
	glutInitWindowPosition(300, 0);

	glutCreateWindow("Computer Graphics");

	//////////////////////////////////////////////////////////////////////////////////////
	//// initialize GLEW
	//////////////////////////////////////////////////////////////////////////////////////
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Unable to initialize GLEW ... exiting" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
	{
		std::cout << "GLEW OK\n";
	}

	//////////////////////////////////////////////////////////////////////////////////////
	//// Create shader program an register the shader
	//////////////////////////////////////////////////////////////////////////////////////

	GLuint vShader[4];
	GLuint fShader[4];

	vShader[0] = MakeVertexShader("vertex.glsl", 0);			// Sun
	fShader[0] = MakeFragmentShader("fragment.glsl", 0);

	// shader Program
	s_program[0] = glCreateProgram();
	glAttachShader(s_program[0], vShader[0]);
	glAttachShader(s_program[0], fShader[0]);
	glLinkProgram(s_program[0]);
	checkCompileErrors(s_program[0], "PROGRAM");

	InitBuffer();

	// callback functions
	glutKeyboardFunc(Keyboard);
	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


	// freeglut 윈도우 이벤트 처리 시작. 윈도우가 닫힐때까지 후한루프 실행.
	glutMainLoop();

	return 0;
}
void InitBuffer()
{
	GLint nAttribute;
	GLint pAttribute;

	// 2 triangles for quad floor
	for (int i = 0; i < 4; i++) {
		glGenVertexArrays(1, &VAO[i]);
		glBindVertexArray(VAO[i]);
		glGenBuffers(3, VBO_position);

		glBindBuffer(GL_ARRAY_BUFFER, VBO_position[0]);
		glBufferData(GL_ARRAY_BUFFER, objReader[i].outvertex.size() * sizeof(glm::vec3), &objReader[i].outvertex[0], GL_STATIC_DRAW);
		pAttribute = glGetAttribLocation(s_program[0], "aPos");
		glVertexAttribPointer(pAttribute, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(pAttribute);
	}

	glEnable(GL_DEPTH_TEST);
}
void Display()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//cube
	glUseProgram(s_program[0]);

	TR = glm::mat4(1.0f);
	if (move_control[0]) {
		if (s == 'x')
			pos_x[0][b] += speed;
		else if (s == 'y')
			pos_y[0][b] += speed;
		else if (s == 'z')
			pos_z[0][b] += speed;
		move_control[0] = false;
	}
	if (scale_rect[0][0]) {
		scale_size[0] -= mass[0];
		scale_rect[0][0] = false;
	}
	else if (scale_rect[0][1]) {
		scale_size[0] += mass[0];
		scale_rect[0][1] = false;
	}
	TR = glm::rotate(TR, glm::radians(30.0f), glm::vec3(1.0, 0.0, 0.0));
	TR = glm::rotate(TR, glm::radians(-30.0f), glm::vec3(0.0, 1.0, 0.0));

	if (tornado) {
		TR = glm::scale(TR, glm::vec3(scale_size[0], scale_size[0], scale_size[0]));
		TR = glm::translate(TR, glm::vec3());
	}
	else if (!stop_scale[0] && !tornado) {
		TR = glm::scale(TR, glm::vec3(scale_size[0], scale_size[0], scale_size[0]));
		TR = glm::translate(TR, glm::vec3(pos_x[0][0] - pos_x[0][1] + 4.0f - move_c, pos_y[0][0] - pos_y[0][1], pos_z[0][0] - pos_z[0][1]));
	}
	else {
		TR = glm::scale(TR, glm::vec3(scale_size[0] * p1, scale_size[0] * p1, scale_size[0] * p1));
		TR = glm::translate(TR, glm::vec3((pos_x[0][0] - pos_x[0][1] + 4.0f) / p1, pos_y[0][0] - pos_y[0][1], pos_z[0][0] - pos_z[0][1]));
	}

	modelLocation = glGetUniformLocation(s_program[0], "m");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));

	if (!change_rect) {
		glBindVertexArray(VAO[0]);
		glDrawArrays(GL_TRIANGLES, 0, Object0);
	}
	else {
		glBindVertexArray(VAO[2]);
		glDrawArrays(GL_TRIANGLES, 0, Object2);
	}
	//sphere
	TR = glm::mat4(1.0f);
	if (move_control[1]) {
		if (s == 'x')
			pos_x[1][b] += speed;
		else if (s == 'y')
			pos_y[1][b] += speed;
		else if (s == 'z')
			pos_z[1][b] += speed;

		move_control[1] = false;
	}
	if (scale_rect[1][0]) {
		scale_size[1] += mass[1];
		scale_rect[1][0] = false;
	}
	else if (scale_rect[1][1]) {
		scale_size[1] -= mass[1];
		scale_rect[1][1] = false;
	}
	TR = glm::rotate(TR, glm::radians(30.0f), glm::vec3(1.0, 0.0, 0.0));
	TR = glm::rotate(TR, glm::radians(-30.0f), glm::vec3(0.0, 1.0, 0.0));

	if (!stop_scale[1]) {
		TR = glm::scale(TR, glm::vec3(scale_size[1], scale_size[1], scale_size[1]));
		TR = glm::translate(TR, glm::vec3(pos_x[1][0] - pos_x[1][1] - 4.0f + move_c, pos_y[1][0] - pos_y[1][1], pos_z[1][0] - pos_z[1][1]));
	}
	else {	
		TR = glm::scale(TR, glm::vec3(scale_size[1] * p2, scale_size[1] * p2, scale_size[1] * p2));
		TR = glm::translate(TR, glm::vec3((pos_x[1][0] - pos_x[1][1] - 4.0f) / p2, pos_y[1][0] - pos_y[1][1], pos_z[1][0] - pos_z[1][1]));
	}


	modelLocation = glGetUniformLocation(s_program[0], "m");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));

	if (!change_rect) {
		glBindVertexArray(VAO[1]);
		glDrawArrays(GL_TRIANGLES, 0, Object1);
	}
	else {
		glBindVertexArray(VAO[3]);
		glDrawArrays(GL_TRIANGLES, 0, Object3);
	}

	glutSwapBuffers();
}
void Reshape(int w, int h)
{
	g_window_w = w;
	g_window_h = h;
	glViewport(0, 0, w, h);
}
void Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'r': case 'R': // 리셋
		all_reset();
		break;
	case 'v': case 'V':
		tornado = true;
		glutTimerFunc(500, TimerFunction, 1);
		break;
	case 'i':
		move_updownx[0] = true;
		glutTimerFunc(500, TimerFunction, 1);
		break;
	case 'I':
		move_updownx[1] = true;
		glutTimerFunc(500, TimerFunction, 1);
		break;
	case 'u':
		move_updowny[0] = true;
		glutTimerFunc(500, TimerFunction, 1);
		break;
	case 'U':
		move_updowny[1] = true;
		glutTimerFunc(500, TimerFunction, 1);
		break;
	case 's': //자리바꾸기
		all_reset();
		move_change = true;
		glutTimerFunc(500, TimerFunction, 1);
		break;
		// 갔다 돌아오기
	case 't': case'T':
		all_reset();
		move_center = true;
		glutTimerFunc(500, TimerFunction, 1);
		break;
		// 멈춰서 확대 축소 pP oO
		{
	case 'p':
		stop_scale[0] = true;
		p1 -= 0.02f;
		break;
	case 'P':
		stop_scale[0] = true;
		p1 += 0.02f;
		break;
	case 'o':
		stop_scale[1] = true;
		p2 -= 0.02f;
		break;
	case 'O':
		stop_scale[1] = true;
		p2 += 0.02f;
		break;
		}
		// rect1 xyz 이동 aAbBcC
		{
	case 'x':
		move_control[0] = true;
		s = 'x'; b = 0;
		break;
	case 'X':
		move_control[0] = true;
		s = 'x'; b = 1;
		break;
	case 'y':
		move_control[0] = true;
		s = 'y'; b = 0;
		break;
	case 'Y':
		move_control[0] = true;
		s = 'y'; b = 1;
		break;
	case 'z':
		move_control[0] = true;
		s = 'z'; b = 0;
		break;
	case 'Z':
		move_control[0] = true;
		s = 'z';b = 1;
		break;
	}
		// rect2 xyz 이동 xXyYzZ
		{
		case 'a':
			move_control[1] = true;
			s = 'x'; b = 0;
			break;
		case 'A':
			move_control[1] = true;
			s = 'x'; b = 1;
			break;
		case 'b':
			move_control[1] = true;
			s = 'y'; b = 0;
			break;
		case 'B':
			move_control[1] = true;
			s = 'y'; b = 1;
			break;
		case 'c':
			move_control[1] = true;
			s = 'z'; b = 0;
			break;
		case 'C':
			move_control[1] = true;
			s = 'z'; b = 1;
			break; }
		// 원점에 대해 확대 축소 mM nN
		{
		case 'm':
			scale_rect[0][0] = false;
			scale_rect[0][1] = true;
			scale_rect[1][0] = false;
			scale_rect[1][1] = false;
			break;
		case 'n':
			scale_rect[0][0] = true;
			scale_rect[0][1] = false;
			scale_rect[1][0] = false;
			scale_rect[1][1] = false;
			break;
		case 'M':
			scale_rect[0][0] = false;
			scale_rect[0][1] = false;
			scale_rect[1][0] = true;
			scale_rect[1][1] = false;
			break;
		case 'N':
			scale_rect[0][0] = false;
			scale_rect[0][1] = false;
			scale_rect[1][0] = false;
			scale_rect[1][1] = true;
			break;
		}
	case 'q': case 'Q':
		exit(0);
		break;
	}
	InitBuffer();
	glutPostRedisplay();
}
void reset() {

}
void all_reset() {
	s = 'r';
	a = -1, b = -1;
	speed = 1.0f;
	scale_size[0] = -0.2f;
	scale_size[1] = -0.2f;
	mass[0] = 0.01f;
	mass[1] = 0.01f;
	for (int i = 0; i < 2; i++) {
		for (int k = 0; i < 2; i++) {
			pos_x[i][k] = 4.0f;
			pos_y[i][k] = 4.0f;
			pos_z[i][k] = 4.0f;
		}
	}
	p1 = -1.0f, p2 = -1.0f;
	move_c = 0.0f;
	move_updownx[0] = false;
	move_updownx[1] = false;
	move_updowny[0] = false;
	move_updowny[1] = false;
	move_center = false;
	move_return = false;
	move_change = false;
	stop_scale[0] = false;
	stop_scale[1] = false;
	change_rect = false;
	move_control[0] = false;
	move_control[1] = false;
	scale_rect[0][0] = false;
	scale_rect[0][1] = false;
	scale_rect[1][0] = false;
	scale_rect[1][1] = false;
}
void TimerFunction(int value)
{	
	if (move_return) {
		move_c -= speed / 2.0;
		if (pos_x[1][0] - pos_x[1][1] - 4.0f + move_c <= -4.0f)
			move_return = false;
	}
	else if (move_center) {
		if (pos_x[1][0] - pos_x[1][1] - 4.0f + move_c < 0.0f)
			move_c += speed / 2.0;
		if (pos_x[1][0] - pos_x[1][1] - 4.0f + move_c >= 0.0f) {
			move_return = true;
			move_center = false;
		}
	}
	else if (move_change) {
		if (pos_x[1][0] - pos_x[1][1] - 4.0f + move_c < 4.0f)
			move_c += speed / 2.0;
		else 
			move_change = false;
	}
	else if (move_updownx[0]) {
		if (pos_z[0][0] - pos_z[1][0] > 4.0f) {
			s1 = -0.2f;
			s2 = -0.2f;
		}
		pos_z[0][0] += s1;
		pos_z[1][0] -= s2;
		
		if (pos_z[0][0] - pos_z[1][0] < 0) {
			move_updownx[0] = false;
			s1 = 0.2f;
			s2 = 0.2f;
		}
	}
	else if (move_updownx[1]) {
		if (pos_z[0][0] - pos_z[1][0] < -4.0f) {
			s1 = -0.2f;
			s2 = -0.2f;
		}
		pos_z[0][0] -= s1;
		pos_z[1][0] += s2;

		if (pos_z[0][0] - pos_z[1][0] > 0) {
			move_updownx[1] = false;
			s1 = 0.2f;
			s2 = 0.2f;
		}
	}
	else if (move_updowny[0]) {
		if (pos_x[0][0] - pos_x[1][0] > 4.0f) {
			s1 = -0.2f;
			s2 = -0.2f;
		}
		pos_x[0][0] += s1;
		pos_x[1][0] -= s2;

		if (pos_x[0][0] - pos_x[1][0] < 0) {
			move_updowny[0] = false;
			s1 = 0.2f;
			s2 = 0.2f;
		}
	}
	else if (move_updowny[1]) {
		if (pos_x[0][0] - pos_x[1][0] < -4.0f) {
			s1 = -0.2f;
			s2 = -0.2f;
		}
		pos_x[0][0] -= s1;
		pos_x[1][0] += s2;

		if (pos_x[0][0] - pos_x[1][0] > 0) {
			move_updowny[1] = false;
			s1 = 0.2f;
			s2 = 0.2f;
		}
	}
	else if (tornado) {
		p[0] = cos(r) * 2.0f;
		p[2] = sin(r) * 2.0f;

		p[0] = cos(r) * 2.0f;
		pos_z[1][0] = sin(r) * 2.0f;
		e += 10.0f;
		r = e * 3.14 / 180.0;		
	}
	
	InitBuffer();
	glutPostRedisplay();
	glutTimerFunc(250, TimerFunction, 1);
}
#endif

#if PROB == 4
#include <math.h>
GLuint VAO;
GLuint VBO_position[3];
GLuint VBO_normal[3];
GLuint VBO_color[3];

unsigned int modelLocation, viewLocation;

int polygon_mode = 2;

const int num_vertices = 3;
const int num_triangles = 1;

glm::mat4 TR;

void Display();
void Reshape(int w, int h);
void Keyboard(unsigned char key, int x, int y);
void InitBuffer();
void TimerFunction(int value);

//--- load obj related variabales
objRead objReader;
GLint Object = objReader.loadObj_normalize_center("cube.obj");

int main(int argc, char** argv)
{
	// create window using freeglut
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(g_window_w, g_window_h);
	glutInitWindowPosition(300, 0);

	glutCreateWindow("Computer Graphics");

	//////////////////////////////////////////////////////////////////////////////////////
	//// initialize GLEW
	//////////////////////////////////////////////////////////////////////////////////////
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Unable to initialize GLEW ... exiting" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
	{
		std::cout << "GLEW OK\n";
	}

	//////////////////////////////////////////////////////////////////////////////////////
	//// Create shader program an register the shader
	//////////////////////////////////////////////////////////////////////////////////////

	GLuint vShader[4];
	GLuint fShader[4];

	vShader[0] = MakeVertexShader("vertex.glsl", 0);			// Sun
	fShader[0] = MakeFragmentShader("fragment.glsl", 0);

	// shader Program
	s_program[0] = glCreateProgram();
	glAttachShader(s_program[0], vShader[0]);
	glAttachShader(s_program[0], fShader[0]);
	glLinkProgram(s_program[0]);
	checkCompileErrors(s_program[0], "PROGRAM");

	InitBuffer();

	// callback functions
	glutKeyboardFunc(Keyboard);
	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


	// freeglut 윈도우 이벤트 처리 시작. 윈도우가 닫힐때까지 후한루프 실행.
	glutMainLoop();

	return 0;
}
void InitBuffer()
{
	GLint nAttribute;
	GLint pAttribute;

	// 2 triangles for quad floor
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(3, VBO_position);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_position[0]);
	glBufferData(GL_ARRAY_BUFFER, objReader.outvertex.size() * sizeof(glm::vec3), &objReader.outvertex[0], GL_STATIC_DRAW);
	pAttribute = glGetAttribLocation(s_program[0], "aPos");
	glVertexAttribPointer(pAttribute, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(pAttribute);

	glDeleteBuffers(1, &VAO);
	glDeleteBuffers(3, VBO_position);

	glEnable(GL_DEPTH_TEST);
}
void Display()	
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glUseProgram(s_program[0]);
	//glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 1.0f); //--- 카메라 위치
	//glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f); //--- 카메라 바라보는 방향
	//glm::vec3 cameraUp = glm::vec3(1.0f, 0.0f, 0.0f); //--- 카메라 위쪽 방향

	//TR = glm::mat4(1.0f);
	//glm::mat4 view = glm::mat4(1.0f);
	//view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
	//unsigned int viewLocation = glGetUniformLocation(s_program[0], "v");
	//glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

	TR = glm::scale(TR, glm::vec3(0.5f, 0.5f, 0.5f));
	TR = glm::rotate(TR, glm::radians(30.0f), glm::vec3(1.0, 0.0, 0.0));
	TR = glm::rotate(TR, glm::radians(-30.0f), glm::vec3(0.0, 1.0, 0.0));

	modelLocation = glGetUniformLocation(s_program[0], "m");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, Object);
	glutSwapBuffers();
}
void Reshape(int w, int h)
{
	g_window_w = w;
	g_window_h = h;
	glViewport(0, 0, w, h);
}
void Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'q': case 'Q':
		exit(0);
		break;
	}
	InitBuffer();
	glutPostRedisplay();
}
void TimerFunction(int value)
{

	InitBuffer();
	glutPostRedisplay();
	glutTimerFunc(100, TimerFunction, 1);
}

#endif