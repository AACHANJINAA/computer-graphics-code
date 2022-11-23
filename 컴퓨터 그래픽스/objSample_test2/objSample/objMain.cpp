// Reading Obj file
#define  _CRT_SECURE_NO_WARNINGS

#include "shader.h"
#include "objRead.h"
// 16 17 18 19 20 21
#define PROB 5

using namespace std;

GLuint g_window_w = 800;
GLuint g_window_h = 600;

#if PROB == 1
GLuint VAO[11];
GLuint VBO[2];

const int num_vertices = 3;
const int num_triangles = 1;
static int draw_type = 2;

glm::mat4 TR, PR, VR;

BOOL draw_change = false;
BOOL button_p = false, button_y = false, button_t = false, button_1 = false, button_2 = false;
BOOL button_f1 = false, button_f2 = false, button_o1 = false, button_o2 = false;

glm::vec3 cameraPos = glm::vec3(0.0f, 1.0f, -5.0f);
glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

void Display();
void Reshape(int w, int h);
void Keyboard(unsigned char key, int x, int y);
void InitBuffer();
void reset_timer();
void TimerFunction(int value);

objRead objReader[11];
GLint s1[11] = {
	objReader[0].loadObj_normalize_center("face1.TXT"),
	objReader[1].loadObj_normalize_center("face2.TXT"),
	objReader[2].loadObj_normalize_center("face3.TXT"),
	objReader[3].loadObj_normalize_center("face4.TXT"),
	objReader[4].loadObj_normalize_center("face5.TXT"),
	objReader[5].loadObj_normalize_center("face6.TXT"),

	objReader[6].loadObj_normalize_center("face7.TXT"),
	objReader[7].loadObj_normalize_center("face8.TXT"),
	objReader[8].loadObj_normalize_center("face9.TXT"),
	objReader[9].loadObj_normalize_center("face10.TXT"),
	objReader[10].loadObj_normalize_center("face11.TXT")
};
GLfloat colors[6][12][3][3];
GLfloat ang = 0.0f, t_ang = 0.0f, f_ang = 0.0f, o_ang = 0.0f;
GLfloat front_move = 0.0f, side_move = 0.0f, o_move = 0.0f;

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

	vShader[0] = MakeVertexShader("vertex.glsl", 0);			// Sun
	fShader[0] = MakeFragmentShader("fragment.glsl", 0);

	// shader Program
	s_program[0] = glCreateProgram();
	glAttachShader(s_program[0], vShader[0]);
	glAttachShader(s_program[0], fShader[0]);
	glLinkProgram(s_program[0]);
	checkCompileErrors(s_program[0], "PROGRAM");


	for (int a = 0; a < 6; a++) {
		for (int i = 0; i < 12; i++) {
			for (int j = 0; j < 3; j++)
				for (int k = 0; k < 3; k++) {
					if (a == 0) {
						if (k == 0)
							colors[a][i][j][k] = 0.0f;
						else
							colors[a][i][j][k] = 1.0f;
					}
					else if (a == 1) {
						if (k == 1)
							colors[a][i][j][k] = 0.0f;
						else
							colors[a][i][j][k] = 1.0f;
					}
					else if (a == 2) {
						if (k == 2)
							colors[a][i][j][k] = 0.0f;
						else
							colors[a][i][j][k] = 1.0f;
					}
					else if (a == 3) {
						if (k != 1)
							colors[a][i][j][k] = 0.0f;
						else
							colors[a][i][j][k] = 1.0f;
					}
					else if (a == 4) {
						if (k != 2)
							colors[a][i][j][k] = 0.0f;
						else
							colors[a][i][j][k] = 1.0f;
					}
					else if (a == 4) {
						if (k != 0)
							colors[a][i][j][k] = 0.0f;
						else
							colors[a][i][j][k] = 1.0f;
					}
				}
		}
	}
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

	for (int i = 0; i < 11; i++) {
		glGenVertexArrays(1, &VAO[i]);
		glBindVertexArray(VAO[i]);
		glGenBuffers(2, VBO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, objReader[i].outvertex.size() * sizeof(glm::vec3), &objReader[i].outvertex[0], GL_STATIC_DRAW);
		pAttribute = glGetAttribLocation(s_program[0], "aPos");
		glVertexAttribPointer(pAttribute, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(pAttribute);

		glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
		glBufferData(GL_ARRAY_BUFFER, objReader[i].outvertex.size() * sizeof(glm::vec3), colors, GL_STATIC_DRAW);
		nAttribute = glGetAttribLocation(s_program[0], "in_Color");
		glVertexAttribPointer(nAttribute, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(nAttribute);
	}
}
void Display()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (draw_type == 1)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else if (draw_type == 2)
		glPolygonMode(GL_FRONT_AND_BACK, GL_TRIANGLES);

	unsigned int modelLocation = glGetUniformLocation(s_program[0], "m");
	unsigned int viewLocation = glGetUniformLocation(s_program[0], "v");
	unsigned int projectionLocation = glGetUniformLocation(s_program[0], "p");

	glUseProgram(s_program[0]);

	if(!draw_change)
		for (int i = 0; i < 6; i++) {
			TR = glm::mat4(1.0f);
			PR = glm::mat4(1.0f);
			VR = glm::mat4(1.0f);

			VR = glm::lookAt(cameraPos, cameraDirection, cameraUp);
			glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &VR[0][0]);

			TR = glm::rotate(TR, glm::radians(-30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	
			if (i == 0) {
				TR = glm::translate(TR, glm::vec3(0.0f, 0.0f, front_move));
				TR = glm::rotate(TR, glm::radians(-f_ang), glm::vec3(1.0f, 0.0f, 0.0f));
			}

			if (i == 1 || i == 3)
				TR = glm::translate(TR, glm::vec3(0.0f, side_move, 0.0f));

			if(button_y)
				TR = glm::rotate(TR, glm::radians(ang), glm::vec3(0.0f, 1.0f, 0.0f));

			if (i == 4) 
				TR = glm::rotate(TR, glm::radians(t_ang), glm::vec3(0.0f, 1.0f, 0.0f));

	
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));

			if (!button_p) {
				PR = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
				PR = glm::translate(PR, glm::vec3(0.0, 0.0, -2.0));
			}
			else {
				PR = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, -2.0f, 2.0f);
			}

			glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &PR[0][0]);

			glBindVertexArray(VAO[i]);
			glDrawArrays(GL_TRIANGLES, 0, s1[i]);
		}
	else if (draw_change)
		for (int i = 6; i < 11; i++) {
			TR = glm::mat4(1.0f);
			PR = glm::mat4(1.0f);
			VR = glm::mat4(1.0f);

			VR = glm::lookAt(cameraPos, cameraDirection, cameraUp);
			glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &VR[0][0]);

			if (i == 10) {
				TR = glm::translate(TR, glm::vec3(0.0f, 0.0f, o_move));
				TR = glm::rotate(TR, glm::radians(-o_ang), glm::vec3(1.0f, 0.0f, 0.0f));
			}
			

			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));

			if (!button_p) {
				PR = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
				PR = glm::translate(PR, glm::vec3(0.0, 0.0, -2.0));
			}
			else {
				PR = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, -2.0f, 2.0f);
			}

			glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &PR[0][0]);

			glBindVertexArray(VAO[i]);
			glDrawArrays(GL_TRIANGLES, 0, s1[i]);
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
	case 's': case'S':
		reset_timer();
		break;
		// 큐브/피라미드 g,G
		{
	case 'g':
		draw_change = true;
		break;
	case 'G':
		draw_change = false;
		break;
		}
		// 은면제거 복구 h,H
		{
	case 'h':
		glEnable(GL_CULL_FACE);
		break;
	case 'H':
		glDisable(GL_CULL_FACE);
		break;
		}
		// 전체 y축 회전 y,Y
		{
	case 'y':
		button_y = true;
		glutTimerFunc(100, TimerFunction, 1);
		break;
	case 'Y':
		button_y = false;
		break;
		}
		// 윗면 y축 회전 t,T
		{
	case 't':
		button_t = true;
		glutTimerFunc(100, TimerFunction, 1);
		break;
	case 'T':
		button_t = false;
		break;
		}
		// 앞면 open / close f,F
		{
	case 'f':
		button_f1 = true;
		button_f2 = false;
		glutTimerFunc(100, TimerFunction, 1);
		break;
	case 'F':
		button_f1 = false;
		button_f2 = true;
		glutTimerFunc(100, TimerFunction, 1);
		break;
		}
		// 양 옆면 open / close 1,2
		{
	case '1':
		button_1 = true;
		button_2 = false;
		glutTimerFunc(100, TimerFunction, 1);
		break;
	case '2':
		button_1 = false;
		button_2 = true;
		glutTimerFunc(100, TimerFunction, 1);
		break;
		}
		// 피라미드 전체 움직임 o,O
		{
	case 'o':
		button_o1 = true;
		button_o2 = false;
		glutTimerFunc(100, TimerFunction, 1);
		break;
	case 'O':
		button_o1 = false;
		button_o2 = true;
		glutTimerFunc(100, TimerFunction, 1);
		break;
		}
		// 직각, 원근 투영 p,P
		{
	case 'p':
		button_p = true;
		break;
	case 'P':
		button_p = false;
		break;
		}
	case 'q': case 'Q':
		for (int i = 0; i < 11; i++) {
			glDeleteVertexArrays(1, &VAO[i]);
			glDeleteBuffers(2, VBO);
		}
		exit(0);
		break;
	}
	InitBuffer();
	glutPostRedisplay();
}
void reset_timer() {
	button_p = false;
}
void TimerFunction(int value)
{
	GLfloat speed = 2.0f, ms = 0.05f;
	if (button_y)
		ang += speed;
	else if (button_t)
		t_ang += speed;
	else if (button_f1) {
		if (f_ang >= 90.0f) {
			button_f1 = false;
		}
		if (f_ang >= 6.0f)
			front_move -= ms;
		f_ang += speed;
	}
	else if (button_f2) {
		if (f_ang <= 2.0f) {
			button_f2 = false;
		}
		if (f_ang <= 86.0f)
			front_move += ms;
		f_ang -= speed;
	}
	else if (button_1) {
		if (side_move >= 1.9f)
			button_1 = false;
		side_move += ms;
	}
	else if (button_2) {
		if (side_move <= 0.05f)
			button_2 = false;
		side_move -= ms;
	}
	else if (button_o1) {
		if (o_ang >= 180.0f) {
			button_o1 = false;
		}
		if (o_ang <= 90.0f)
			o_move -= ms;
		else if (o_ang > 90.0f && o_ang <= 180.0f)
			o_move -= ms;
		o_ang += speed;
	}
	else if (button_o2) {
		if (o_ang <= 2.0f) {
			button_o2 = false;
		}
		if (o_ang <= 86.0f)
			o_move += ms;
		o_ang -= speed;
	}
		
	InitBuffer();
	glutPostRedisplay();
	if(button_y || button_t || button_1 || button_2 || button_f1 || button_f2 || button_o1 || button_o2)
		glutTimerFunc(100, TimerFunction, 1);
}
#endif
#if PROB == 2

GLuint VAO[7];
GLuint VBO[2];

const int num_vertices = 3;
const int num_triangles = 1;
static int draw_type = 2;

glm::mat4 TR, PR, VR;

BOOL p1 = false, p2 = false, start1 = false, start2 = false;
BOOL up = false, m_left = false, down = false, m_right = false, front = false, back = false;

GLfloat dx1 = 0.0f, mx = 0.0f, my = 0.0f, mz = 0.0f;
GLfloat ly = 0.0f, d_ly = 0.0f;
GLfloat mv = 0.1f;

GLfloat colors[3][1124][3][3];

void Display();
void Reshape(int w, int h);
void Keyboard(unsigned char key, int x, int y);
void InitBuffer();
void reset_timer();
void TimerFunction(int value);
//--- load obj related variabales
objRead objReader[7];
GLint s1[7] = {
	objReader[0].loadObj_normalize_center("sphere.obj"),

	objReader[1].loadObj_normalize_center("sphere.obj"),
	objReader[2].loadObj_normalize_center("sphere.obj"),
	objReader[3].loadObj_normalize_center("sphere.obj"),

	objReader[4].loadObj_normalize_center("sphere.obj"),
	objReader[5].loadObj_normalize_center("sphere.obj"),
	objReader[6].loadObj_normalize_center("sphere.obj"),
};

int main(int argc, char** argv)
{
	// create window using freeglut
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(g_window_w, g_window_h);
	glutInitWindowPosition(0, 0);

	glutCreateWindow("Computer Graphics");

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

	for (int a = 0; a < 3; a++) {
		for (int i = 0; i < 1124; i++) {
			for (int j = 0; j < 3; j++)
				for (int k = 0; k < 3; k++) {
					if (a == 0) {
						if (k != 2)
							colors[a][i][j][k] = 0.7f;
						else
							colors[a][i][j][k] = 1.0f;
					}
					else if (a == 1) {
						if (k != 2)
							colors[a][i][j][k] = 0.5f;
						else
							colors[a][i][j][k] = 1.0f;
					}
					else if (a == 2) {
						if (k != 2)
							colors[a][i][j][k] = 0.3f;
						else
							colors[a][i][j][k] = 1.0f;
					}
				}
		}
	}

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

	for (int i = 0; i < 7; i++) {
		glGenVertexArrays(1, &VAO[i]);
		glBindVertexArray(VAO[i]);
		glGenBuffers(2, VBO);

		//도형이 다나오지않음
		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, objReader[i].outvertex.size() * sizeof(glm::vec3), &objReader[i].outvertex[0], GL_STATIC_DRAW);
		pAttribute = glGetAttribLocation(s_program[0], "aPos");
		glVertexAttribPointer(pAttribute, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(pAttribute);

		glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
		if(i == 0)
			glBufferData(GL_ARRAY_BUFFER, objReader[i].outvertex.size() * sizeof(glm::vec3), colors[0], GL_STATIC_DRAW);
		else if (i == 1 || i == 2 || i == 3)
			glBufferData(GL_ARRAY_BUFFER, objReader[i].outvertex.size() * sizeof(glm::vec3), colors[1], GL_STATIC_DRAW);
		else if (i == 4 || i == 5 || i == 6)
			glBufferData(GL_ARRAY_BUFFER, objReader[i].outvertex.size() * sizeof(glm::vec3), colors[2], GL_STATIC_DRAW);
		nAttribute = glGetAttribLocation(s_program[0], "in_Color");
		glVertexAttribPointer(nAttribute, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(nAttribute);

	}
	glEnable(GL_DEPTH_TEST);
}
void Display()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if(draw_type == 1)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else if(draw_type == 2) 
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glUseProgram(s_program[0]);

	glm::mat4 S_TR[3];

	TR = glm::mat4(1.0f);
	PR = glm::mat4(1.0f);
	VR = glm::mat4(1.0f);

	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	unsigned int modelLocation = glGetUniformLocation(s_program[0], "m");
	unsigned int projectionLocation = glGetUniformLocation(s_program[0], "p");
	unsigned int viewLocation = glGetUniformLocation(s_program[0], "v");

	TR = glm::translate(TR, glm::vec3(mx, my, mz));
	TR = glm::scale(TR, glm::vec3(0.2, 0.2, 0.2));

	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));

	GLfloat space_p = 2.0f;
	if (p1)
		PR = glm::ortho(space_p, -space_p, space_p, -space_p, space_p, -space_p);
	else if (p2) {
		PR = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 50.0f);
		PR = glm::translate(PR, glm::vec3(0.0f, 0.0f, -2.0f));
	}

	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &PR[0][0]);

	VR = glm::lookAt(cameraPos, cameraDirection, cameraUp);
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &VR[0][0]);

	glBindVertexArray(VAO[0]);
	glDrawArrays(GL_TRIANGLES, 0, s1[0]);

	// 두번째 원
	for (int i = 1; i < 4; i++) {
		TR = glm::mat4(1.0f);
		PR = glm::mat4(1.0f);
		// 중심과 거리 & 각도

		TR = glm::translate(TR, glm::vec3(mx, my, mz));
		if (i == 1)
			dx1 = 0.5f;
		else if (i == 2) {
			TR = glm::rotate(TR, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			dx1 = 0.5f;
		}
		else if (i == 3) {
			TR = glm::rotate(TR, glm::radians(-45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			dx1 = -0.5f;
		}
	
		// 속도
		if(i == 1)
			TR = glm::rotate(TR, glm::radians(d_ly), glm::vec3(0.0f, 1.0f, 0.0f));
		else 
			TR = glm::rotate(TR, glm::radians(ly), glm::vec3(0.0f, 1.0f, 0.0f));

		TR = glm::translate(TR, glm::vec3(dx1, 0.0f, 0.0f));
	
		TR = glm::scale(TR, glm::vec3(0.1, 0.1, 0.1));

		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));

		if (p1)
			PR = glm::ortho(space_p, -space_p, space_p, -space_p, space_p, -space_p);
		else if (p2) {
			PR = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 50.0f);
			PR = glm::translate(PR, glm::vec3(0.0f, 0.0f, -2.0f));
		}

		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &PR[0][0]);
		
		S_TR[i - 1] = TR;

		glBindVertexArray(VAO[i]);
		glDrawArrays(GL_TRIANGLES, 0, s1[i]);
	}
	// 세번째 원
	for (int i = 4; i < 7; i++) {
		TR = S_TR[i - 4];
		TR = glm::rotate(TR, glm::radians(d_ly), glm::vec3(0.0f, 1.0f, 0.0f));
		TR = glm::translate(TR, glm::vec3(1.7f, 0.0f, 0.0f));
		TR = glm::scale(TR, glm::vec3(0.5f, 0.5f, 0.5f));
		

		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));

		if (p1)
			PR = glm::ortho(space_p, -space_p, space_p, -space_p, space_p, -space_p);
		else if (p2) {
			PR = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 50.0f);
			PR = glm::translate(PR, glm::vec3(0.0f, 0.0f, -2.0f));
		}

		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &PR[0][0]);

		glBindVertexArray(VAO[i]);
		glDrawArrays(GL_TRIANGLES, 0, s1[i]);
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
	case 'r': case'R':
		reset_timer();
		break;
	case 'p': // 직각투영
		p1 = true;
		p2 = false;
		break;
	case 'P': // 원근투영
		p2 = true;
		p1 = false;
		break;
	case 'm': // 솔리드 모델
		draw_type = 1;
		break;
	case 'M': // 와이어 모델
		draw_type = 2;
		break;
	case 'w': case 'W':
		my += mv;
		break;
	case 'a': case 'A':
		mx -= mv;
		break;
	case 's': case 'S':
		my -= mv;
		break;
	case 'd': case 'D':
		mx += mv;
		break;
	case 'z': case 'Z':
		mz -= mv;
		break;
	case 'x': case'X':
		mz += mv;
		break;
	case 'y': // 공전
		start1 = true;
		start2 = false;
		glutTimerFunc(100, TimerFunction, 1);
		break;
	case 'Y': // 반대방향 공전
		start2 = true;
		start1 = false;
		glutTimerFunc(100, TimerFunction, 1);
		break;
	case 'q': case 'Q':
		for (int i = 0; i < 7; i++) {
			glDeleteVertexArrays(1, &VAO[i]);
			glDeleteBuffers(3, VBO);
		}
		exit(0);
		break;
	}
	InitBuffer();
	glutPostRedisplay();
}
void reset_timer() {
	start1 = false; start2 = false;
	p1 = false; p2 = false;
}
void TimerFunction(int value)
{
	GLfloat speed = 5.0f;

	if (start1) {
		ly += speed;
		d_ly += speed * 2;
	}
	else if (start2) {
		ly -= speed;
		d_ly -= speed * 2;
	}

	InitBuffer();
	glutPostRedisplay();
	if (start1 || start2)
		glutTimerFunc(100, TimerFunction, 1);
}

#endif
#if PROB == 3
#include <math.h>
GLuint VAO[5];
GLuint VBO[2];

const int num_vertices = 3;
const int num_triangles = 1;
static int draw_type = 2;

glm::mat4 TR, PR, VR;

BOOL move_1[2] = { false, false }, move_2[2] = { false, false }, move_3[2] = { false, false };
BOOL turn_standard_y[2] = { false, false }, turn_y[2] = { false , false };

GLfloat mx = 0.0f, my = 0.0f, mz[2] = { 0.0f, 0.0f };
GLfloat radians = 0.0f;
GLfloat q = 5.0f;
glm::vec3 cameraPos = glm::vec3(0.0f, 0.5f, -2.0f);
glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

void Display();
void Reshape(int w, int h);
void Keyboard(unsigned char key, int x, int y);
void InitBuffer();
void reset_timer();
void reset_pos();
void TimerFunction(int value);

objRead objReader[5];
GLint s1[5] = {
	objReader[0].loadObj_normalize_center("cube.obj"),
	objReader[1].loadObj_normalize_center("cube.obj"),
	objReader[2].loadObj_normalize_center("cube.obj"),
	objReader[3].loadObj_normalize_center("cube.obj"),
	objReader[4].loadObj_normalize_center("cube.obj")
};
GLfloat colors[5][12][3][3];

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

	vShader[0] = MakeVertexShader("vertex.glsl", 0);			// Sun
	fShader[0] = MakeFragmentShader("fragment.glsl", 0);

	// shader Program
	s_program[0] = glCreateProgram();
	glAttachShader(s_program[0], vShader[0]);
	glAttachShader(s_program[0], fShader[0]);
	glLinkProgram(s_program[0]);
	checkCompileErrors(s_program[0], "PROGRAM");


	for (int a = 0; a < 5; a++) {
		for (int i = 0; i < 12; i++) {
			for (int j = 0; j < 3; j++)
				for (int k = 0; k < 3; k++) {
					if (a == 0) {
						if (k != 2)
							colors[a][i][j][k] = 0.0f;
						else
							colors[a][i][j][k] = 1.0f;
					}
					else if (a == 1) {
						if (k != 1)
							colors[a][i][j][k] = 0.0f;
						else
							colors[a][i][j][k] = 1.0f;
					}
					else if (a == 2) {
						if (k != 0)
							colors[a][i][j][k] = 0.0f;
						else
							colors[a][i][j][k] = 1.0f;
					}
					else if (a == 3) {
						if (k == 2)
							colors[a][i][j][k] = 0.0f;
						else
							colors[a][i][j][k] = 1.0f;
					}
					else if (a == 4) {
						if (k == 1)
							colors[a][i][j][k] = 0.0f;
						else
							colors[a][i][j][k] = 1.0f;
					}
				}
		}
	}
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

	for (int i = 0; i < 5; i++) {
		glGenVertexArrays(1, &VAO[i]);
		glBindVertexArray(VAO[i]);
		glGenBuffers(2, VBO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, objReader[i].outvertex.size() * sizeof(glm::vec3), &objReader[i].outvertex[0], GL_STATIC_DRAW);
		pAttribute = glGetAttribLocation(s_program[0], "aPos");
		glVertexAttribPointer(pAttribute, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(pAttribute);

		glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
		glBufferData(GL_ARRAY_BUFFER, objReader[i].outvertex.size() * sizeof(glm::vec3), colors[i], GL_STATIC_DRAW);
		nAttribute = glGetAttribLocation(s_program[0], "in_Color");
		glVertexAttribPointer(nAttribute, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(nAttribute);
	}
	glEnable(GL_DEPTH_TEST);
}
void Display()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (draw_type == 1)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else if (draw_type == 2)
		glPolygonMode(GL_FRONT_AND_BACK, GL_TRIANGLES);

	unsigned int modelLocation = glGetUniformLocation(s_program[0], "m");
	unsigned int viewLocation = glGetUniformLocation(s_program[0], "v");
	unsigned int projectionLocation = glGetUniformLocation(s_program[0], "p");

	glUseProgram(s_program[0]);

	for (int i = 0; i < 5; i++) {
		TR = glm::mat4(1.0f);
		PR = glm::mat4(1.0f);
		VR = glm::mat4(1.0f);

		VR = glm::lookAt(cameraPos, cameraDirection, cameraUp);
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &VR[0][0]);

		if (i == 0)
			TR = glm::translate(TR, glm::vec3(mx, 0.0f, 0.0f));
		else if (i == 1)
			TR = glm::translate(TR, glm::vec3(mx, 0.1f, 0.0f));
		else if (i == 2) {
			TR = glm::translate(TR, glm::vec3(0.0f, 0.2f, 0.0f));
			TR = glm::translate(TR, glm::vec3(mx - 0.05f, 0.0f, 0.0f));
		}
		else if (i == 3) {
			TR = glm::translate(TR, glm::vec3(0.0f, 0.2f, 0.0f));
			TR = glm::translate(TR, glm::vec3(mx + 0.05f, 0.0f, 0.0f));
		}
		else if (i == 4) {
			TR = glm::translate(TR, glm::vec3(0.0f, 0.0f, 0.0f));
		}

		if (i == 1)
			TR = glm::rotate(TR, glm::radians(my), glm::vec3(0.0f, 1.0f, 0.0f));

		else if (i == 2) {
			TR = glm::rotate(TR, glm::radians(mz[0]), glm::vec3(1.0f, 0.0f, 0.0f));
			TR = glm::rotate(TR, glm::radians(my), glm::vec3(0.0f, 1.0f, 0.0f));
		}
		else if (i == 3) {
			TR = glm::rotate(TR, glm::radians(mz[1]), glm::vec3(1.0f, 0.0f, 0.0f));
			TR = glm::rotate(TR, glm::radians(my), glm::vec3(0.0f, 1.0f, 0.0f));
		}

		if (i == 0)
			TR = glm::scale(TR, glm::vec3(0.15f, 0.15f, 0.15f));
		else if (i == 1)
			TR = glm::scale(TR, glm::vec3(0.1f, 0.1f, 0.1f));
		else if (i == 2)
			TR = glm::scale(TR, glm::vec3(0.025f, 0.1f, 0.025f));
		else if (i == 3)
			TR = glm::scale(TR, glm::vec3(0.025f, 0.1f, 0.025f));
		else if (i == 4)
			TR = glm::scale(TR, glm::vec3(1.0f, 0.1f, 1.0f));

		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));

		PR = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
		PR = glm::translate(PR, glm::vec3(0.0, 0.0, -2.0));

		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &PR[0][0]);

		glBindVertexArray(VAO[i]);
		glDrawArrays(GL_TRIANGLES, 0, s1[i]);
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
	case 's': case'S':
		reset_timer();
		break;
	case 'c': case 'C':
		reset_pos();
		break;
		// 크레인 움직임 b,B / m,M / t,T
		{
	case 'b':
		move_1[0] = true;
		move_1[1] = false;
		glutTimerFunc(100, TimerFunction, 1);
		break;
	case 'B':
		move_1[1] = true;
		move_1[0] = false;
		glutTimerFunc(100, TimerFunction, 1);
		break;
	case 'm':
		move_2[0] = true;
		move_2[1] = false;
		glutTimerFunc(100, TimerFunction, 1);
		break;
	case 'M':
		move_2[1] = true;
		move_2[0] = false;
		glutTimerFunc(100, TimerFunction, 1);
		break;
	case 't':
		move_3[0] = true;
		move_3[1] = false;
		glutTimerFunc(100, TimerFunction, 1);
		break;
	case 'T':
		move_3[1] = true;
		move_3[0] = false;
		glutTimerFunc(100, TimerFunction, 1);
		break;
		}
		// 카메라 이동 x,X / z,Z
		{
	case 'x':
		cameraPos.x += 0.01f;
		cameraDirection.x += 0.01f;
		break;
	case 'X':
		cameraPos.x -= 0.01f;
		cameraDirection.x -= 0.01f;
		break;
	case 'z':
		cameraPos.z += 0.01f;
		cameraDirection.z += 0.01f;
		break;
	case 'Z':
		cameraPos.z -= 0.01f;
		cameraDirection.z -= 0.01f;
		break;
		}
		// 카메라 y축 회전 y,Y
		{
	case 'y':
		turn_y[0] = true;
		turn_y[1] = false;
		glutTimerFunc(100, TimerFunction, 1);
		break;
	case 'Y':
		turn_y[0] = false;
		turn_y[1] = true;
		glutTimerFunc(100, TimerFunction, 1);
		break;
		}
		// 화면 중심 y축 회전 r,R
		{
	case 'r':
		turn_standard_y[0] = true;
		turn_standard_y[1] = false;
		glutTimerFunc(100, TimerFunction, 1);
		break;
	case 'R':
		turn_standard_y[1] = true;
		turn_standard_y[0] = false;
		glutTimerFunc(100, TimerFunction, 1);
		break;
		}

	case 'q': case 'Q':
		for (int i = 0; i < 5; i++) {
			glDeleteVertexArrays(1, &VAO[i]);
			glDeleteBuffers(2, VBO);
		}
		exit(0);
		break;
	}
	InitBuffer();
	glutPostRedisplay();
}
void reset_timer() {
	move_1[0] = false;
	move_1[1] = false;
	move_2[0] = false;
	move_2[1] = false;
	move_3[0] = false;
	move_3[1] = false;
	turn_standard_y[0] = false;
	turn_standard_y[1] = false;
	turn_y[0] = false;
	turn_y[1] = false;
}
void reset_pos() {

	mx = 0.0f, my = 0.0f; mz[0] = 0.0f; mz[1] = 0.0f;
	cameraPos = glm::vec3(0.0f, 0.5f, -2.0f);
	q = 5.0f;
	cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f);
	cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	radians = 0.0f;
}
void TimerFunction(int value)
{
	GLfloat mv = 0.05f, speed = 5.0f;
	if (move_1[0]) {
		mx += mv;
		if (mx > 0.60f)
			move_1[0] = false;
	}
	else if (move_1[1]) {
		mx -= mv;
		if (mx < -0.60f)
			move_1[1] = false;
	}
	else if (move_2[0])
		my += speed;
	else if (move_2[1])
		my -= speed;
	else if (move_3[0]) {
		if (mz[0] >= 90.0f || mz[0] <= -90.0f) {
			q *= -1;
		}
		mz[0] += q;
		mz[1] -= q;
	}
	else if (move_3[1]) {
		if (mz[0] >= 90.0f || mz[0] <= -90.0f) {
			q *= -1;
		}
		mz[0] -= q;
		mz[1] += q;
	}

	if (turn_standard_y[0]) {
		radians += speed;
		cameraPos.x = sin(glm::radians(radians));
		cameraPos.z = -cos(glm::radians(radians));
	}
	else if (turn_standard_y[1]) {
		radians -= speed;
		cameraPos.x = sin(glm::radians(radians));
		cameraPos.z = -cos(glm::radians(radians));
	}

	if (turn_y[0]) {
		radians += speed;
		cameraDirection.x = -sin(glm::radians(radians)) * speed / 2.0f;
		cameraDirection.z = cos(glm::radians(radians)) * speed / 2.0f;
	}
	else if (turn_y[1]) {
		radians -= speed;
		cameraDirection.x = -sin(glm::radians(radians)) * speed / 2.0f;
		cameraDirection.z = cos(glm::radians(radians)) * speed / 2.0f;
	}

	InitBuffer();
	glutPostRedisplay();
	if (move_1[0] || move_1[1] || move_2[0] || move_2[1] || move_3[0] || move_3[1] || turn_standard_y[0] || turn_standard_y[1] || turn_y[0] || turn_y[1])
		glutTimerFunc(100, TimerFunction, 1);
}
#endif
#if PROB == 4
#include <math.h>
GLuint VAO[5];
GLuint VBO[2];

const int num_vertices = 3;
const int num_triangles = 1;
static int draw_type = 2;

glm::mat4 TR, PR, VR;

BOOL move_1[2] = { false, false }, move_2[2] = { false, false }, move_3[2] = { false, false };
BOOL turn_standard_y[2] = { false, false }, turn_y[2] = { false , false};

GLfloat mx = 0.0f, my = 0.0f, mz[2] = {0.0f, 0.0f};
GLfloat radians = 0.0f;
GLfloat q = 5.0f;
glm::vec3 cameraPos, cameraDirection, cameraUp;
glm::vec3 cameraPos1 = glm::vec3(0.0f, 0.5f, -2.0f);
glm::vec3 cameraDirection1 = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraUp1 = glm::vec3(0.0f, 1.0f, 0.0f);

glm::vec3 cameraPos2 = glm::vec3(0.0f, 0.0f, -2.0f);
glm::vec3 cameraDirection2 = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraUp2 = glm::vec3(0.0f, 1.0f, 0.0f);

glm::vec3 cameraPos3 = glm::vec3(0.0f, 0.5f, -0.1f);
glm::vec3 cameraDirection3 = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraUp3 = glm::vec3(0.0f, 1.0f, 0.0f);

void Display();
void DrawScene(int a);
void Reshape(int w, int h);
void Keyboard(unsigned char key, int x, int y);
void InitBuffer();
void reset_timer();
void reset_pos();
void TimerFunction(int value);

objRead objReader[10];
GLint s1[5] = {
	objReader[0].loadObj_normalize_center("cube.obj"),
	objReader[1].loadObj_normalize_center("cube.obj"),
	objReader[2].loadObj_normalize_center("cube.obj"),
	objReader[3].loadObj_normalize_center("cube.obj"),
	objReader[4].loadObj_normalize_center("cube.obj"),
};
GLfloat colors[5][12][3][3]; 

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

	vShader[0] = MakeVertexShader("vertex.glsl", 0);			// Sun
	fShader[0] = MakeFragmentShader("fragment.glsl", 0);

	// shader Program
	s_program[0] = glCreateProgram();
	glAttachShader(s_program[0], vShader[0]);
	glAttachShader(s_program[0], fShader[0]);
	glLinkProgram(s_program[0]);
	checkCompileErrors(s_program[0], "PROGRAM");


	for (int a = 0; a < 5; a++) {
		for (int i = 0; i < 12; i++) {
			for (int j = 0; j < 3; j++)
				for (int k = 0; k < 3; k++) {
					if (a == 0) {
						if (k != 2)
							colors[a][i][j][k] = 0.0f;
						else
							colors[a][i][j][k] = 1.0f;
					}
					else if (a == 1) {
						if (k != 1)
							colors[a][i][j][k] = 0.0f;
						else
							colors[a][i][j][k] = 1.0f;
					}
					else if (a == 2) {
						if (k != 0)
							colors[a][i][j][k] = 0.0f;
						else
							colors[a][i][j][k] = 1.0f;
					}
					else if (a == 3) {
						if (k == 2)
							colors[a][i][j][k] = 0.0f;
						else
							colors[a][i][j][k] = 1.0f;
					}
					else if (a == 4) {
						if (k == 1)
							colors[a][i][j][k] = 0.0f;
						else
							colors[a][i][j][k] = 1.0f;
					}
				}
		}
	}
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

	for (int i = 0; i < 5; i++) {
		glGenVertexArrays(1, &VAO[i]);
		glBindVertexArray(VAO[i]);
		glGenBuffers(2, VBO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, objReader[i].outvertex.size() * sizeof(glm::vec3), &objReader[i].outvertex[0], GL_STATIC_DRAW);
		pAttribute = glGetAttribLocation(s_program[0], "aPos");
		glVertexAttribPointer(pAttribute, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(pAttribute);

		glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
		glBufferData(GL_ARRAY_BUFFER, objReader[i].outvertex.size() * sizeof(glm::vec3), colors[i % 5], GL_STATIC_DRAW);
		nAttribute = glGetAttribLocation(s_program[0], "in_Color");
		glVertexAttribPointer(nAttribute, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(nAttribute);
	}
	glEnable(GL_CULL_FACE);
}
void Display()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (draw_type == 1)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else if (draw_type == 2)
		glPolygonMode(GL_FRONT_AND_BACK, GL_TRIANGLES);
	glUseProgram(s_program[0]);

	glViewport(80, 0, 600, 300);
	DrawScene(1);
	glViewport(400, 300, 300, 225);
	DrawScene(2);
	glViewport(0, 300, 300, 225);
	DrawScene(3);
	//glViewport(100, 300, 300, 225);
	//DrawScene3();

	glutSwapBuffers();
}
void DrawScene(int a) {
	unsigned int modelLocation = glGetUniformLocation(s_program[0], "m");
	unsigned int viewLocation = glGetUniformLocation(s_program[0], "v");
	unsigned int projectionLocation = glGetUniformLocation(s_program[0], "p");

	if (a == 1) {
		cameraPos = cameraPos1;
		cameraDirection = cameraDirection1;
		cameraUp = cameraUp1;
	}
	else if(a == 2){
		cameraPos = cameraPos2;
		cameraDirection = cameraDirection2;
		cameraUp = cameraUp2;
	}
	else if (a == 3) {
		cameraPos = cameraPos3;
		cameraDirection = cameraDirection3;
		cameraUp = cameraUp3;
	}
	for (int i = 0; i < 5; i++) {
		TR = glm::mat4(1.0f);
		PR = glm::mat4(1.0f);
		VR = glm::mat4(1.0f);

		VR = glm::lookAt(cameraPos, cameraDirection, cameraUp);
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &VR[0][0]);

		// b or B
		if (i == 0)
			TR = glm::translate(TR, glm::vec3(mx, 0.1f, 0.0f));
		else if (i == 1)
			TR = glm::translate(TR, glm::vec3(mx, 0.2f, 0.0f));
		else if (i == 2) {
			TR = glm::translate(TR, glm::vec3(0.0f, 0.3f, 0.0f));
			TR = glm::translate(TR, glm::vec3(mx - 0.05f, 0.0f, 0.0f));
		}
		else if (i == 3) {
			TR = glm::translate(TR, glm::vec3(0.0f, 0.3f, 0.0f));
			TR = glm::translate(TR, glm::vec3(mx + 0.05f, 0.0f, 0.0f));
		}
		else if (i == 4) {
			TR = glm::translate(TR, glm::vec3(0.0f, 0.0f, 0.0f));
		}

		if (i == 1)
			TR = glm::rotate(TR, glm::radians(my), glm::vec3(0.0f, 1.0f, 0.0f));

		else if (i == 2) {
			TR = glm::rotate(TR, glm::radians(mz[0]), glm::vec3(1.0f, 0.0f, 0.0f));
			TR = glm::rotate(TR, glm::radians(my), glm::vec3(0.0f, 1.0f, 0.0f));
		}
		else if (i == 3) {
			TR = glm::rotate(TR, glm::radians(mz[1]), glm::vec3(1.0f, 0.0f, 0.0f));
			TR = glm::rotate(TR, glm::radians(my), glm::vec3(0.0f, 1.0f, 0.0f));
		}

		if (i == 0)
			TR = glm::scale(TR, glm::vec3(0.15f, 0.15f, 0.15f));
		else if (i == 1)
			TR = glm::scale(TR, glm::vec3(0.1f, 0.1f, 0.1f));
		else if (i == 2)
			TR = glm::scale(TR, glm::vec3(0.025f, 0.1f, 0.025f));
		else if (i == 3)
			TR = glm::scale(TR, glm::vec3(0.025f, 0.1f, 0.025f));
		else if (i == 4)
			TR = glm::scale(TR, glm::vec3(1.0f, 0.1f, 1.0f));

		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));

		if (a == 1) {
			PR = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
			PR = glm::translate(PR, glm::vec3(0.0, 0.0, -2.0));
		}
		else if (a == 2 || a == 3)
			PR = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, -2.0f, 2.0f);
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &PR[0][0]);

		glBindVertexArray(VAO[i]);
		glDrawArrays(GL_TRIANGLES, 0, s1[i]);
	}
}
void Reshape(int w, int h)
{
	g_window_w = w;
	g_window_h = h;
	//glViewport(w / 10, 0, 3 * w / 4, h / 2);
}
void Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
		case 's': case'S':
			reset_timer();
			break;
		case 'c': case 'C':
			reset_pos();
			break;
			// 크레인 움직임 b,B / m,M / t,T
			{
		case 'b':
			move_1[0] = true;
			move_1[1] = false;
			glutTimerFunc(100, TimerFunction, 1);
			break;
		case 'B':
			move_1[1] = true;
			move_1[0] = false;
			glutTimerFunc(100, TimerFunction, 1);
			break;
		case 'm':
			move_2[0] = true;
			move_2[1] = false;
			glutTimerFunc(100, TimerFunction, 1);
			break;
		case 'M':
			move_2[1] = true;
			move_2[0] = false;
			glutTimerFunc(100, TimerFunction, 1);
			break;
		case 't':
			move_3[0] = true;
			move_3[1] = false;
			glutTimerFunc(100, TimerFunction, 1);
			break;
		case 'T':
			move_3[1] = true;
			move_3[0] = false;
			glutTimerFunc(100, TimerFunction, 1);
			break;
			}
			// 카메라 이동 x,X / z,Z
			{
		case 'x':
			cameraPos.x += 0.01f;
			cameraDirection.x += 0.01f;
			break;
		case 'X':
			cameraPos.x -= 0.01f;
			cameraDirection.x -= 0.01f;
			break;
		case 'z':
			cameraPos.z += 0.01f;
			cameraDirection.z += 0.01f;
			break;
		case 'Z':
			cameraPos.z -= 0.01f;
			cameraDirection.z -= 0.01f;
			break;
			}
			// 카메라 y축 회전 y,Y
			{
		case 'y':
			turn_y[0] = true;
			turn_y[1] = false;
			glutTimerFunc(100, TimerFunction, 1);
			break;
		case 'Y':
			turn_y[0] = false;
			turn_y[1] = true;
			glutTimerFunc(100, TimerFunction, 1);
			break;
			}
			// 화면 중심 y축 회전 r,R
			{
		case 'r':
			turn_standard_y[0] = true;
			turn_standard_y[1] = false;
			glutTimerFunc(100, TimerFunction, 1);
			break;
		case 'R':
			turn_standard_y[1] = true;
			turn_standard_y[0] = false;
			glutTimerFunc(100, TimerFunction, 1);
			break;
			}
			
		case 'q': case 'Q':
			for (int i = 0; i < 5; i++) {
				glDeleteVertexArrays(1, &VAO[i]);
				glDeleteBuffers(2, VBO);
			}
			exit(0);
			break;
	}
	InitBuffer();
	glutPostRedisplay();
}
void reset_timer() {
	move_1[0] = false;
	move_1[1] = false;
	move_2[0] = false;
	move_2[1] = false;
	move_3[0] = false;
	move_3[1] = false;
	turn_standard_y[0] = false;
	turn_standard_y[1] = false;
	turn_y[0] = false;
	turn_y[1] = false;
}
void reset_pos() {
	mx = 0.0f, my = 0.0f; mz[0] = 0.0f; mz[1] = 0.0f;
	cameraPos = glm::vec3(0.0f, 0.5f, -2.0f);
	q = 5.0f;
	cameraPos1 = glm::vec3(0.0f, 0.5f, -2.0f);
	cameraDirection1 = glm::vec3(0.0f, 0.0f, 0.0f);
	cameraPos2 = glm::vec3(0.0f, 0.0f, -2.0f);
	cameraDirection2 = glm::vec3(0.0f, 0.0f, 0.0f);
	cameraPos3 = glm::vec3(0.0f, 0.5f, -0.1f);	
	cameraDirection3 = glm::vec3(0.0f, 0.0f, 0.0f);
	radians = 0.0f;
}
void TimerFunction(int value)
{
	GLfloat mv = 0.05f, speed = 5.0f;
	if (move_1[0]) {
		mx += mv;
		if (mx > 0.60f)
			move_1[0] = false;
	}
	else if (move_1[1]) {
		mx -= mv;
		if (mx < -0.60f)
			move_1[1] = false;
	}
	else if (move_2[0])
		my += speed;
	else if (move_2[1])
		my -= speed;
	else if (move_3[0]) {
		if (mz[0] >= 90.0f || mz[0] <= -90.0f) {
			q *= -1;
		}
		mz[0] += q;
		mz[1] -= q;
	}
	else if (move_3[1]) {
		if (mz[0] >= 90.0f || mz[0] <= -90.0f) {
			q *= -1;
		}
		mz[0] -= q;
		mz[1] += q;
	}

	if (turn_standard_y[0]) {
		radians += speed;
		cameraPos1.x = sin(glm::radians(radians));
		cameraPos1.z = -cos(glm::radians(radians));
		cameraPos2.x = sin(glm::radians(radians));
		cameraPos2.z = -cos(glm::radians(radians));
		cameraPos3.x = sin(glm::radians(radians));
		cameraPos3.z = -cos(glm::radians(radians));
	}
	else if (turn_standard_y[1]) {
		radians -= speed;
		cameraPos1.x = sin(glm::radians(radians));
		cameraPos1.z = -cos(glm::radians(radians));
		cameraPos2.x = sin(glm::radians(radians));
		cameraPos2.z = -cos(glm::radians(radians));
		cameraPos3.x = sin(glm::radians(radians));
		cameraPos3.z = -cos(glm::radians(radians));
	}

	if (turn_y[0]) {
		radians += speed;
		cameraDirection1.x = sin(glm::radians(radians)) * speed * 2;
		cameraDirection1.z = -cos(glm::radians(radians)) * speed * 2;
		cameraDirection2.x = sin(glm::radians(radians)) * speed * 2;
		cameraDirection2.z = -cos(glm::radians(radians)) * speed * 2;
		cameraDirection3.x = sin(glm::radians(radians)) * speed * 2;
		cameraDirection3.z = -cos(glm::radians(radians)) * speed * 2;
	}
	else if (turn_y[1]) {
		radians -= speed;
		cameraDirection1.x = sin(glm::radians(radians)) * speed / 2.0f;
		cameraDirection1.z = -cos(glm::radians(radians)) * speed / 2.0f;
		cameraDirection2.x = sin(glm::radians(radians)) * speed / 2.0f;
		cameraDirection2.z = -cos(glm::radians(radians)) * speed / 2.0f;
		cameraDirection3.x = sin(glm::radians(radians)) * speed * 2;
		cameraDirection3.z = -cos(glm::radians(radians)) * speed * 2;
	}

	InitBuffer();
	glutPostRedisplay();
	if (move_1[0] || move_1[1] || move_2[0] || move_2[1] || move_3[0] || move_3[1] || turn_standard_y[0] || turn_standard_y[1] || turn_y[0] || turn_y[1])
		glutTimerFunc(100, TimerFunction, 1);
}
#endif
#if PROB == 5
GLuint VAO[14];
GLuint VBO[2];

const int num_vertices = 3;
const int num_triangles = 1;
static int draw_type = 2;

glm::mat4 TR, PR, VR;

void Display();
void Reshape(int w, int h);
void Keyboard(unsigned char key, int x, int y);
void InitBuffer();
void reset_timer();
void TimerFunction(int value);

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, -4.0f);
glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 0.1f, 0.0f);

BOOL turn_standard_y[2] = { false , false }, jump = false, button_f[2] = { false, false }, swing_move = false, move_up = false;
BOOL turn_w = false, turn_s = false, turn_a = false, turn_d = false;


GLfloat ang = 0.0f;
GLfloat f_ang = 0.0f, front_move = 0.0f;
GLfloat mx = 0.0f, my = -0.8f, mz = 0.0f;
GLfloat mv = 0.01f, swing[2] = { 0.0f, 0.0f };
GLfloat xpos = float(rand() % 14) / 10.0f - 1.0f;
GLfloat zpos = float(rand() % 10) / 10.0f * -1.0f;
GLfloat r = 5.0f, turn_ang = 0.0f;

objRead objReader[14];
GLint s1[14] = {
	objReader[0].loadObj_normalize_center("face1.TXT"),
	objReader[1].loadObj_normalize_center("face2.TXT"),
	objReader[2].loadObj_normalize_center("face3.TXT"),
	objReader[3].loadObj_normalize_center("face4.TXT"),
	objReader[4].loadObj_normalize_center("face5.TXT"),
	objReader[5].loadObj_normalize_center("face6.TXT"),

	objReader[6].loadObj_normalize_center("cube.obj"),
	objReader[7].loadObj_normalize_center("cube.obj"),
	objReader[8].loadObj_normalize_center("cube.obj"),
	objReader[9].loadObj_normalize_center("cube.obj"),
	objReader[10].loadObj_normalize_center("cube.obj"),
	objReader[11].loadObj_normalize_center("cube.obj"),
	objReader[12].loadObj_normalize_center("cube.obj"),
	objReader[13].loadObj_normalize_center("cube.obj")
};

GLfloat colors[6][12][3][3];

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(g_window_w, g_window_h);
	glutInitWindowPosition(0, 0);

	glutCreateWindow("Computer Graphics");

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

	for (int a = 0; a < 6; a++) {
		for (int i = 0; i < 12; i++) {
			for (int j = 0; j < 3; j++)
				for (int k = 0; k < 3; k++) {
					if (a == 0) {
						if (k != 2)
							colors[a][i][j][k] = 0.0f;
						else
							colors[a][i][j][k] = 1.0f;
					}
					else if (a == 1) {
						if (k != 1)
							colors[a][i][j][k] = 0.0f;
						else
							colors[a][i][j][k] = 1.0f;
					}
					else if (a == 2) {
						if (k != 0)
							colors[a][i][j][k] = 0.0f;
						else
							colors[a][i][j][k] = 1.0f;
					}
					else if (a == 3) {
						if (k == 2)
							colors[a][i][j][k] = 0.0f;
						else
							colors[a][i][j][k] = 1.0f;
					}
					else if (a == 4) {
						if (k == 1)
							colors[a][i][j][k] = 0.0f;
						else
							colors[a][i][j][k] = 1.0f;
					}
					else if (a == 5) {
						if (k == 0)
							colors[a][i][j][k] = 0.0f;
						else
							colors[a][i][j][k] = 1.0f;
					}
				}
		}
	}
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

	for (int i = 0; i < 14; i++) {
		glGenVertexArrays(1, &VAO[i]);
		glBindVertexArray(VAO[i]);
		glGenBuffers(2, VBO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, objReader[i].outvertex.size() * sizeof(glm::vec3), &objReader[i].outvertex[0], GL_STATIC_DRAW);
		pAttribute = glGetAttribLocation(s_program[0], "aPos");
		glVertexAttribPointer(pAttribute, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(pAttribute);

		glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
		if (i != 12)
			glBufferData(GL_ARRAY_BUFFER, objReader[i].outvertex.size() * sizeof(glm::vec3), colors[i % 6], GL_STATIC_DRAW);
		else
			glBufferData(GL_ARRAY_BUFFER, objReader[i].outvertex.size() * sizeof(glm::vec3), colors[3], GL_STATIC_DRAW);
		nAttribute = glGetAttribLocation(s_program[0], "in_Color");
		glVertexAttribPointer(nAttribute, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(nAttribute);
	}
	glEnable(GL_DEPTH_TEST);
}
void Display()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (draw_type == 1)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else if (draw_type == 2)
		glPolygonMode(GL_FRONT_AND_BACK, GL_TRIANGLES);

	unsigned int modelLocation = glGetUniformLocation(s_program[0], "m");
	unsigned int viewLocation = glGetUniformLocation(s_program[0], "v");
	unsigned int projectionLocation = glGetUniformLocation(s_program[0], "p");

	glUseProgram(s_program[0]);

	for (int i = 0; i < 6; i++) {
		TR = glm::mat4(1.0f);
		PR = glm::mat4(1.0f);
		VR = glm::mat4(1.0f);

		VR = glm::lookAt(cameraPos, cameraDirection, cameraUp);
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &VR[0][0]);

		if (i == 0) {
			TR = glm::translate(TR, glm::vec3(0.0f, 0.0f, front_move));
			TR = glm::rotate(TR, glm::radians(-f_ang), glm::vec3(1.0f, 0.0f, 0.0f));
		}

		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));

		PR = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
		PR = glm::translate(PR, glm::vec3(0.0, 0.0, -2.0));

		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &PR[0][0]);

		glBindVertexArray(VAO[i]);
		glDrawArrays(GL_TRIANGLES, 0, s1[i]);
	}
	for (int i = 6; i < 13; i++) {
		TR = glm::mat4(1.0f);
		PR = glm::mat4(1.0f);
		VR = glm::mat4(1.0f);

		VR = glm::lookAt(cameraPos, cameraDirection, cameraUp);
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &VR[0][0]);

		if (i == 7)
			TR = glm::translate(TR, glm::vec3(0.0f, 0.08f, 0.0f));
		else if (i == 8)
			TR = glm::translate(TR, glm::vec3(-0.03f, -0.08f, 0.0f));
		else if (i == 9)
			TR = glm::translate(TR, glm::vec3(0.03f, -0.08f, 0.0f));
		else if (i == 10)
			TR = glm::translate(TR, glm::vec3(-0.05f, -0.03f, 0.0f));
		else if (i == 11)
			TR = glm::translate(TR, glm::vec3(0.05f, -0.03f, 0.0f));
		else if (i == 12)
			TR = glm::translate(TR, glm::vec3(0.0f, 0.075f, 0.035f));

		TR = glm::translate(TR, glm::vec3(mx, my, mz));

		if (i != 6 && i != 7) {
			TR = glm::rotate(TR, glm::radians(turn_ang), glm::vec3(0.0f, 1.0f, 0.0f));
			if (i == 12)
				TR = glm::translate(TR, glm::vec3(0.04f, 0.0f, 0.0f));
		}

		if (i == 8 || i == 11)
			TR = glm::rotate(TR, glm::radians(swing[0]), glm::vec3(1.0f, 0.0f, 0.0f));
		else if (i == 9 || i == 10)
			TR = glm::rotate(TR, glm::radians(swing[1]), glm::vec3(1.0f, 0.0f, 0.0f));

		if (i == 6)
			TR = glm::scale(TR, glm::vec3(0.05f, 0.05f, 0.05f));
		else if (i == 7)
			TR = glm::scale(TR, glm::vec3(0.03f, 0.03f, 0.03f));
		else if (i == 8 || i == 9 || i == 10 || i == 11)
			TR = glm::scale(TR, glm::vec3(0.01f, 0.025f, 0.01f));
		else if (i == 12)
			TR = glm::scale(TR, glm::vec3(0.01f, 0.01f, 0.01f));

		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));

		PR = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
		PR = glm::translate(PR, glm::vec3(0.0, 0.0, -2.0));

		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &PR[0][0]);

		glBindVertexArray(VAO[i]);
		glDrawArrays(GL_TRIANGLES, 0, s1[i]);
	}

	TR = glm::mat4(1.0f);
	PR = glm::mat4(1.0f);
	VR = glm::mat4(1.0f);

	VR = glm::lookAt(cameraPos, cameraDirection, cameraUp);
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &VR[0][0]);

	TR = glm::translate(TR, glm::vec3(xpos, -0.8f, zpos));

	TR = glm::scale(TR, glm::vec3(0.2f, 0.03f, 0.2f));

	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));

	PR = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
	PR = glm::translate(PR, glm::vec3(0.0, 0.0, -2.0));

	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &PR[0][0]);

	glBindVertexArray(VAO[13]);
	glDrawArrays(GL_TRIANGLES, 0, s1[13]);

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
	GLfloat m = 0.1f, move_robot = 0.01f;
	switch (key) {
	case 'r': case 'R':
		reset_timer();
		break;
	case 'e':
		swing_move = true;
		glutTimerFunc(100, TimerFunction, 1);
		break;
	case 'o':
		button_f[0] = true;
		button_f[1] = false;
		glutTimerFunc(100, TimerFunction, 1);
		break;
	case 'O':
		button_f[0] = false;
		button_f[1] = true;
		glutTimerFunc(100, TimerFunction, 1);
		break;
		// 카메라 이동 x,X / z,Z / y,Y
		{
	case 'z':
		cameraPos.z += m;
		break;
	case 'Z':
		cameraPos.z -= m;
		break;
	case 'x':
		cameraPos.x += m;
		break;
	case 'X':
		cameraPos.x -= m;
		break;
	case 'y':
		turn_standard_y[0] = true;
		turn_standard_y[1] = false;
		glutTimerFunc(100, TimerFunction, 1);
		break;
	case 'Y':
		turn_standard_y[1] = true;
		turn_standard_y[0] = false;
		glutTimerFunc(100, TimerFunction, 1);
		break;
		}
		// wasd j i
		{
	case 'w': case 'W':
		turn_w = true;
		turn_s = false;
		turn_a = false;
		turn_d = false;
		turn_ang = -90.0f;
		mz += move_robot;
		if (!move_up) {
			if (xpos + 0.25f > mx && mx > xpos - 0.25f && zpos + 0.25f > mz && mz > zpos - 0.25f)
				mz -= move_robot;
		}
		else
			if (xpos + 0.25f > mx && mx > xpos - 0.25f && zpos + 0.25f > mz && mz > zpos - 0.25f)
				my = -0.75f;
			else
				my = -0.8f;
		break;
	case 's': case 'S':
		turn_w = false;
		turn_s = true;
		turn_a = false;
		turn_d = false;
		turn_ang = 90.0f;
		mz -= move_robot;
		if (!move_up) {
			if (xpos + 0.25f > mx && mx > xpos - 0.25f && zpos + 0.25f > mz && mz > zpos - 0.25f)
				mz += move_robot;
		}
		else
			if (xpos + 0.25f > mx && mx > xpos - 0.25f && zpos + 0.25f > mz && mz > zpos - 0.25f)
				my = -0.75f;
			else
				my = -0.8f;
		break;
	case 'a': case 'A':
		turn_w = false;
		turn_s = false;
		turn_a = true;
		turn_d = false;
		turn_ang = 0.0f;
		mx += move_robot;
		if (mx > 0.7f)
			mx = -0.65f;
		if (!move_up) {
			if (xpos + 0.25f > mx && mx > xpos - 0.25f && zpos + 0.25f > mz && mz > zpos - 0.25f)
				mx -= move_robot;
		}
		else
			if (xpos + 0.25f > mx && mx > xpos - 0.25f && zpos + 0.25f > mz && mz > zpos - 0.25f)
				my = -0.75f;
			else
				my = -0.8f;
		break;
	case 'd': case 'D':
		turn_w = false;
		turn_s = false;
		turn_a = false;
		turn_d = true;
		turn_ang = -180.0f;
		mx -= move_robot;
		if (mx < -0.7f)
			mx = 0.65f;
		if (!move_up) {
			if (xpos + 0.25f > mx && mx > xpos - 0.25f && zpos + 0.25f > mz && mz > zpos - 0.25f)
				mx += move_robot;
		}
		else
			if (xpos + 0.25f > mx && mx > xpos - 0.25f && zpos + 0.25f > mz && mz > zpos - 0.25f)
				my = -0.75f;
			else
				my = -0.8f;
		break;
	case 'j': case 'J':
		move_up = true;
		jump = true;
		glutTimerFunc(100, TimerFunction, 1);
		
		break;
	case 'i': case 'I':
		mx = 0.0f; my = -0.8f; mz = 0.0f;
		cameraPos = glm::vec3(0.0f, 0.0f, -4.0f);
		cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f);
		cameraUp = glm::vec3(0.0f, 0.1f, 0.0f);
		break;
		}
	case 'q': case 'Q':
		for (int i = 0; i < 14; i++) {
			glDeleteVertexArrays(1, &VAO[i]);
			glDeleteBuffers(2, VBO);
		}
		exit(0);
		break;
	}
	InitBuffer();
	glutPostRedisplay();
}
void reset_timer() {
	turn_standard_y[0] = false;
	turn_standard_y[1] = false;
	button_f[0] = false;
	button_f[1] = false;
}
void TimerFunction(int value)
{
	GLfloat speed = 2.0f, m = 0.1f;
	if (turn_standard_y[0]) {
		ang += speed;
		cameraPos.x = -sin(glm::radians(ang));
		cameraPos.z = cos(glm::radians(ang));
	}
	else if (turn_standard_y[1]) {
		ang -= speed;
		cameraPos.x = -sin(glm::radians(ang));
		cameraPos.z = cos(glm::radians(ang));
	}

	if (jump) {
		my += mv;
		if (my >= -0.75f)
			if (xpos + 0.25f > mx && mx > xpos - 0.25f && zpos + 0.25f > mz && mz > zpos - 0.25f) {
				my = -0.75f;
				jump = false;
			}

		if (my < -0.8f) {
			jump = false;
			my = -0.8f;
			mv *= -1;
		}
	}

	if (button_f[0]) {
		if (f_ang >= 90.0f)
			button_f[0] = false;

		if (f_ang >= 6.0f)
			front_move -= mv * 5.0f;
		f_ang += speed;
	}
	else if (button_f[1]) {
		if (f_ang <= 2.0f)
			button_f[1] = false;

		if (f_ang <= 86.0f)
			front_move += mv * 5.0f;
		f_ang -= speed;
	}

	if (swing_move) {
		if (swing[0] >= 60.0f || swing[0] <= -60.0f) {
			r *= -1;
		}
		swing[0] += r;
		swing[1] -= r;
	}

	InitBuffer();
	glutPostRedisplay();
	if (turn_standard_y[0] || turn_standard_y[1] || jump || button_f[0] || button_f[1] || swing_move)
		glutTimerFunc(100, TimerFunction, 1);
}
#endif
#if PROB == 6
#include <time.h>
GLuint VAO[14];
GLuint VBO[2];

const int num_vertices = 3;
const int num_triangles = 1;
static int draw_type = 1;

glm::mat4 TR, PR, VR;

void Display();
void Reshape(int w, int h);
void Keyboard(unsigned char key, int x, int y);
void mouse(int button, int state, int x, int y);
void InitBuffer();
void reset();
void TimerFunction(int value);

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, -4.0f);
glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 0.1f, 0.0f);

objRead objReader[14];
GLint s1[14] = {
	objReader[0].loadObj_normalize_center("face1.TXT"),
	objReader[1].loadObj_normalize_center("face2.TXT"),
	objReader[2].loadObj_normalize_center("face3.TXT"),
	objReader[3].loadObj_normalize_center("face4.TXT"),
	objReader[4].loadObj_normalize_center("face5.TXT"),
	objReader[5].loadObj_normalize_center("face6.TXT"),

	objReader[6].loadObj_normalize_center("sphere.obj"),
	objReader[7].loadObj_normalize_center("sphere.obj"),
	objReader[8].loadObj_normalize_center("sphere.obj"),
	objReader[9].loadObj_normalize_center("sphere.obj"),
	objReader[10].loadObj_normalize_center("sphere.obj"),

	objReader[11].loadObj_normalize_center("cube.obj"),
	objReader[12].loadObj_normalize_center("cube.obj"),
	objReader[13].loadObj_normalize_center("cube.obj")
};

GLfloat colors[6][12][3][3], c[1][1124][3][3];

BOOL timer_start = false, turn_standard_y[2] = { false, false }, open_bottom = false, move_cube[2] = { false , false};

GLfloat bm_x[5] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f}, bm_y[5] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
GLfloat dir_x[5] = { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f}, dir_y[5] = { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat radians = 0.0, turn_cube = 0.0f, turn_speed = 2.0f, t_cube = 0.0f;
GLfloat turn_ang = 30.0f, move_t = turn_ang / 300.0f;
static int cnt = 0;

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(g_window_w, g_window_h);
	glutInitWindowPosition(0, 0);

	glutCreateWindow("Computer Graphics");

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

	for (int a = 0; a < 6; a++) {
		for (int i = 0; i < 12; i++) {
			for (int j = 0; j < 3; j++)
				for (int k = 0; k < 3; k++) {
					if (a == 0) {
						if (k != 2)
							colors[a][i][j][k] = 0.0f;
						else
							colors[a][i][j][k] = 1.0f;
					}
					else if (a == 1) {
						if (k != 1)
							colors[a][i][j][k] = 0.0f;
						else
							colors[a][i][j][k] = 1.0f;
					}
					else if (a == 2) {
						if (k != 0)
							colors[a][i][j][k] = 0.0f;
						else
							colors[a][i][j][k] = 1.0f;
					}
					else if (a == 3) {
						if (k == 2)
							colors[a][i][j][k] = 0.0f;
						else
							colors[a][i][j][k] = 1.0f;
					}
					else if (a == 4) {
						if (k == 1)
							colors[a][i][j][k] = 0.0f;
						else
							colors[a][i][j][k] = 1.0f;
					}
					else if (a == 5) {
						if (k == 0)
							colors[a][i][j][k] = 0.0f;
						else
							colors[a][i][j][k] = 1.0f;
					}
				}
		}
	}

	for (int a = 0; a < 1; a++) {
		for (int i = 0; i < 1124; i++) {
			for (int j = 0; j < 3; j++)
				for (int k = 0; k < 3; k++) {
					if (k == 2)
						c[a][i][j][k] = 0.7f;
					else 
						c[a][i][j][k] = 0.3f;
				}
		}
	}
	InitBuffer();

	// callback functions
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(mouse);
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

	for (int i = 0; i < 14; i++) {
		glGenVertexArrays(1, &VAO[i]);
		glBindVertexArray(VAO[i]);
		glGenBuffers(2, VBO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, objReader[i].outvertex.size() * sizeof(glm::vec3), &objReader[i].outvertex[0], GL_STATIC_DRAW);
		pAttribute = glGetAttribLocation(s_program[0], "aPos");
		glVertexAttribPointer(pAttribute, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(pAttribute);

		glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
		if(i <= 5)
			glBufferData(GL_ARRAY_BUFFER, objReader[i].outvertex.size() * sizeof(glm::vec3), colors[i % 6], GL_STATIC_DRAW);
		else if (i > 5 && i < 11)
			glBufferData(GL_ARRAY_BUFFER, objReader[i].outvertex.size() * sizeof(glm::vec3), c[0], GL_STATIC_DRAW);
		else 
			glBufferData(GL_ARRAY_BUFFER, objReader[i].outvertex.size() * sizeof(glm::vec3), colors[i % 3], GL_STATIC_DRAW);
		nAttribute = glGetAttribLocation(s_program[0], "in_Color");
		glVertexAttribPointer(nAttribute, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(nAttribute);
	}
	glEnable(GL_DEPTH_TEST);
}
void Display()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (draw_type == 1)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else if (draw_type == 2)
		glPolygonMode(GL_FRONT_AND_BACK, GL_TRIANGLES);

	unsigned int modelLocation = glGetUniformLocation(s_program[0], "m");
	unsigned int viewLocation = glGetUniformLocation(s_program[0], "v");
	unsigned int projectionLocation = glGetUniformLocation(s_program[0], "p");

	glUseProgram(s_program[0]);

	for (int i = 1; i < 6; i++) {
		TR = glm::mat4(1.0f);
		PR = glm::mat4(1.0f);
		VR = glm::mat4(1.0f);

		VR = glm::lookAt(cameraPos, cameraDirection, cameraUp);
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &VR[0][0]);

		TR = rotate(TR, glm::radians(turn_cube), glm::vec3(0.0f, 0.0f, 1.0f));

		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));

		PR = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
		PR = glm::translate(PR, glm::vec3(0.0, 0.0, -2.0));

		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &PR[0][0]);

		if (i == 5 && open_bottom)
			break;

		glBindVertexArray(VAO[i]);
		glDrawArrays(GL_TRIANGLES, 0, s1[i]);
	}

	for (int i = 6; i < 6 + cnt; i++) {
		TR = glm::mat4(1.0f);
		PR = glm::mat4(1.0f);
		VR = glm::mat4(1.0f);

		VR = glm::lookAt(cameraPos, cameraDirection, cameraUp);
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &VR[0][0]);

		TR = translate(TR, glm::vec3(bm_x[i - 6], bm_y[i - 6], 0.0f));

		TR = scale(TR, glm::vec3(0.1f, 0.1f, 0.1f));

		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));

		PR = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
		PR = glm::translate(PR, glm::vec3(0.0, 0.0, -2.0));

		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &PR[0][0]);

		glBindVertexArray(VAO[i]);
		glDrawArrays(GL_TRIANGLES, 0, s1[i]);
	}

	for (int i = 11; i < 14; i++) {
		TR = glm::mat4(1.0f);
		PR = glm::mat4(1.0f);
		VR = glm::mat4(1.0f);

		VR = glm::lookAt(cameraPos, cameraDirection, cameraUp);
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &VR[0][0]);
		
		/*if (move_cube[0] || move_cube[1])
			TR = translate(TR, glm::vec3(t_cube, 0.0f, 0.0f));*/

		if (i == 11) {
			TR = translate(TR, glm::vec3(0.0f, -0.8f, -1.0f));
			TR = scale(TR, glm::vec3(0.05f, 0.05f, 0.05f));
		}
		else if (i == 12) {
			TR = translate(TR, glm::vec3(0.0f, -0.8f, -0.7f));
			TR = scale(TR, glm::vec3(0.1f, 0.1f, 0.1f));
		}
		else if (i == 13) {
			TR = translate(TR, glm::vec3(0.0f, -0.8f, -0.4f));
			TR = scale(TR, glm::vec3(0.15f, 0.15f, 0.15f));
		}

		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));

		PR = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
		PR = glm::translate(PR, glm::vec3(0.0, 0.0, -2.0));

		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &PR[0][0]);

		glBindVertexArray(VAO[i]);
		glDrawArrays(GL_TRIANGLES, 0, s1[i]);
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
		timer_start = true;
		glutTimerFunc(100, TimerFunction, 1);
		break;
	case 'r': case 'R':
		reset();
		break;
		// 카메라 y,Y / z,Z
		{
	case 'z':
		cameraPos.z += 0.1f;
		break;
	case 'Z':
		cameraPos.z -= 0.1f;
		break;
	case 'y':
		turn_standard_y[0] = true;
		turn_standard_y[1] = false;
		break;
	case 'Y':
		turn_standard_y[0] = true;
		turn_standard_y[1] = false;
		break;
		}
	case 'o':
		open_bottom = true;
		break;
	case 'O':
		open_bottom = false;
		break;
	case 'b': case 'B':
		cnt++;
		if (cnt > 5)
			cnt -= 1;
		break;
	case 'q': case 'Q':
		for (int i = 0; i < 14; i++) {
			glDeleteVertexArrays(1, &VAO[i]);
			glDeleteBuffers(2, VBO);
		}
		exit(0);
		break;
	}
	InitBuffer();
	glutPostRedisplay();
}
void mouse(int button, int state, int x, int y) {

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && x < g_window_w / 2){
		move_cube[0] = true;
		move_cube[1] = false;
	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && x >= g_window_w / 2) {
		move_cube[0] = false;
		move_cube[1] = true;
	}
}
void reset() {
	timer_start = false;
	cnt = 0;
}
void TimerFunction(int value)
{
	GLfloat speed = 0.05f, wall_pos = 0.75f;

	if (turn_standard_y[0]) {
		radians += turn_speed;
		cameraPos.x = sin(glm::radians(radians));
		cameraPos.z = -cos(glm::radians(radians));
	}
	else if (turn_standard_y[1]) {
		radians -= turn_speed;
		cameraPos.x = sin(glm::radians(radians));
		cameraPos.z = -cos(glm::radians(radians));
	}

	if (move_cube[0]) {
		turn_cube += turn_speed;
		//t_cube += move_t;
		if (turn_cube > turn_ang || turn_cube < -turn_ang) {
			turn_speed *= -1;
			//move_t *= -1;
		}
	}
	else if (move_cube[1]) {
		turn_cube -= turn_speed;
		//t_cube += move_t;
		if (turn_cube > turn_ang || turn_cube < -turn_ang) {
			turn_speed *= -1;
			//move_t *= -1;
		}
	}

	if (timer_start) {
		for (int i = 0; i < cnt; i++) {
			bm_x[i] += dir_x[i] * speed;
			bm_y[i] += dir_y[i] * speed;

			if (bm_x[i] >= wall_pos) {
				dir_x[i] = float(rand() % 10) / 10.0f - 1.1f;
			}
			else if (bm_x[i] <= -wall_pos) {
				dir_x[i] = float(rand() % 10) / 10.0f + 0.1f;
			}
			else if (bm_y[i] >= wall_pos) {
				dir_y[i] = float(rand() % 10) / 10.0f - 1.1f;
			}
			else if (bm_y[i] <= -wall_pos) {
				if (!open_bottom)
					dir_y[i] = float(rand() % 10) / 10.0f + 0.1f;
			}
		}
	}
	InitBuffer();
	glutPostRedisplay();
	if (timer_start)
		glutTimerFunc(100, TimerFunction, 1);
}
#endif

