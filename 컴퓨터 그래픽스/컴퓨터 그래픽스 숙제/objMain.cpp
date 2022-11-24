// Reading Obj file
#define  _CRT_SECURE_NO_WARNINGS

#include "shader.h"
#include "objRead.h"

using namespace std;

GLuint g_window_w = 600;
GLuint g_window_h = 600;

#include <time.h>
GLuint VAO[102];
GLuint VBO[2];

const int num_vertices = 3;
const int num_triangles = 1;
static int draw_type = 1;

glm::mat4 TR, PR, VR;

void DrawScene(unsigned int modelLocation, unsigned int viewLocation, unsigned int projectionLocation, int kind);
void Display();
void Keyboard(unsigned char key, int x, int y);
void InitBuffer();
void reset();
void dir_first_pointofview_pos(GLfloat mx, GLfloat my, GLfloat mz);
void collision_check(const char* kindofmove);
void TimerFunction(int value);

GLfloat move_z = 0.0f; // 미로 생성에 translate에 쓰이는 z값
GLfloat scale_y[10][10], store_scale_y[10][10], size_change[10][10]; // 미로 size 증가, 처음 미로의 y크기 저장, 감소에 쓰이는 scale/y값
int remove_maze[10][10]; // 미로생성할 때 지울 곳을 저장해주는 값
GLfloat object_speed = 0.01f; // 객체 움직임 속도 값
GLfloat mx = 0.4f, my = -0.15f, mz = 0.0f; // 객체 위치
GLfloat p_p_figures = 10.0f; // 직각 투영 수치
GLfloat turn_dir = 1, radians = 0.0f, speed = 10.0f; // 도는 방향, 각도, 각도의 타이머당 증가량
GLfloat arrive_pos[2]; // 도착 지점

BOOL but_O = false, but_P = true, m_maze_rect = false, object_appear = false;
BOOL turn_camera = false, first_view = false;

int row, col, f_row = 0, f_col = 1;

GLfloat colors[4][12][3][3];

glm::vec3 Top_View = glm::vec3(0.0f, 4.0f, -0.5f);
glm::vec3 Top_cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 Top_cameraUp = glm::vec3(0.0f, 0.5f, 0.0f);

int decide_first_pointofview = 0;
glm::vec3 first_pointofview_pos[4] = { 
	glm::vec3(mx, my + 0.0005f, mz - 0.0005f),
	glm::vec3(mx, my + 0.0005f, mz + 0.0005f),
	glm::vec3(mx - 0.0005f, my + 0.0005f, mz),
    glm::vec3(mx + 0.0005f, my + 0.0005f, mz)
};
glm::vec3 first_pointofview_dir = glm::vec3(mx, my, mz);

glm::vec3 third_pointofview_pos = glm::vec3(0.0f, 4.5f, -6.5f);
glm::vec3 third_pointofview_dir = glm::vec3(0.0f, 0.0f, 0.0f);

glm::vec3 cameraPos = third_pointofview_pos; // 쿼터뷰            위에서 내려봄 0.0f / 4.0f / -0.5f         정면 0.0f / 4.0f / 0.0f
glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 0.1f, 0.0f);

objRead objReader;
GLint s1 = objReader.loadObj_normalize_center("cube.obj");

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(g_window_w, g_window_h);
	glutInitWindowPosition(400, 0);

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

	scanf_s("%d", &row); scanf_s("%d", &col);
	if (row > 10 || row < 5 || col > 10 || col < 5) {
		for (int i = 0; i < row * col; i++) {
			glDeleteVertexArrays(1, &VAO[i]);
			glDeleteBuffers(1, VBO);
		}
		exit(0);
	}
	srand(time(0));
	for (int i = 0; i < row; i++) {
		for (int k = 0; k < col; k++) {
			GLfloat rd = float(rand() % 6) / 10.0f + 0.2f;
			scale_y[i][k] = rd;
			store_scale_y[i][k] = rd;
			size_change[i][k] = 0.01f;
		}
	}
	for (int a = 0; a < 4; a++) {
		for (int i = 0; i < 12; i++) {
			for (int j = 0; j < 3; j++)
				for (int k = 0; k < 3; k++) {
					if (a == 0) {
						if (i < 4) {
							if (k == 0)
								colors[a][i][j][k] = 1.0f;
							else if (k == 1)
								colors[a][i][j][k] = 0.0f;
							else
								colors[a][i][j][k] = 0.5f;
						}
						else if (i >= 4 && i < 8) {
							if (k == 0)
								colors[a][i][j][k] = 0.0f;
							else if (k == 1)
								colors[a][i][j][k] = 1.0f;
							else
								colors[a][i][j][k] = 0.5f;
						}
						else if (i >= 8 && i < 12) {
							if (k == 0)
								colors[a][i][j][k] = 1.0f;
							else if (k == 1)
								colors[a][i][j][k] = 1.0f;
							else
								colors[a][i][j][k] = 0.0f;
						}
					}
					else if (a == 1) {
						if (k != 1)
							colors[a][i][j][k] = 0.0f;
						else
							colors[a][i][j][k] = 1.0f;
					}
					else if (a == 2) {
						if (k == 0)
							colors[a][i][j][k] = 0.8f;
						else if (k == 1)
							colors[a][i][j][k] = 0.8f;
						else
							colors[a][i][j][k] = 1.0f;
					}
					else if (a == 3) {
						if (k == 0)
							colors[a][i][j][k] = 1.0f;
						else if (k == 1)
							colors[a][i][j][k] = 0.8f;
						else
							colors[a][i][j][k] = 0.0f;
					}
				}
		}
	}

	InitBuffer();

	// callback functions
	glutKeyboardFunc(Keyboard);
	glutDisplayFunc(Display);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glutMainLoop();

	return 0;
}
void InitBuffer()
{
	GLint pAttribute, nAttribute;
	glUseProgram(s_program[0]);
	//미로
	for (int i = 0; i < row * col; i++) {
		glGenVertexArrays(1, &VAO[i]);
		glBindVertexArray(VAO[i]);
		glGenBuffers(2, VBO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, objReader.outvertex.size() * sizeof(glm::vec3), &objReader.outvertex[0], GL_STATIC_DRAW);
		pAttribute = glGetAttribLocation(s_program[0], "aPos");
		glVertexAttribPointer(pAttribute, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(pAttribute);

		glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
		glBufferData(GL_ARRAY_BUFFER, objReader.outvertex.size() * sizeof(glm::vec3), colors[0], GL_STATIC_DRAW);
		nAttribute = glGetAttribLocation(s_program[0], "in_Color");
		glVertexAttribPointer(nAttribute, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(nAttribute);
	}
	// 바닥
	{
		glGenVertexArrays(1, &VAO[row * col]);
		glBindVertexArray(VAO[row * col]);
		glGenBuffers(2, VBO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, objReader.outvertex.size() * sizeof(glm::vec3), &objReader.outvertex[0], GL_STATIC_DRAW);
		pAttribute = glGetAttribLocation(s_program[0], "aPos");
		glVertexAttribPointer(pAttribute, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(pAttribute);

		glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
		glBufferData(GL_ARRAY_BUFFER, objReader.outvertex.size() * sizeof(glm::vec3), colors[1], GL_STATIC_DRAW);
		nAttribute = glGetAttribLocation(s_program[0], "in_Color");
		glVertexAttribPointer(nAttribute, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(nAttribute);
	}
	// 객체
	{
		glGenVertexArrays(1, &VAO[row * col + 1]);
		glBindVertexArray(VAO[row * col + 1]);
		glGenBuffers(2, VBO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, objReader.outvertex.size() * sizeof(glm::vec3), &objReader.outvertex[0], GL_STATIC_DRAW);
		pAttribute = glGetAttribLocation(s_program[0], "aPos");
		glVertexAttribPointer(pAttribute, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(pAttribute);

		glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
		glBufferData(GL_ARRAY_BUFFER, objReader.outvertex.size() * sizeof(glm::vec3), colors[2], GL_STATIC_DRAW);
		nAttribute = glGetAttribLocation(s_program[0], "in_Color");
		glVertexAttribPointer(nAttribute, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(nAttribute);
	}
	// 도착지점
	{
		glGenVertexArrays(1, &VAO[row * col + 2]);
		glBindVertexArray(VAO[row * col + 2]);
		glGenBuffers(2, VBO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, objReader.outvertex.size() * sizeof(glm::vec3), &objReader.outvertex[0], GL_STATIC_DRAW);
		pAttribute = glGetAttribLocation(s_program[0], "aPos");
		glVertexAttribPointer(pAttribute, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(pAttribute);

		glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
		glBufferData(GL_ARRAY_BUFFER, objReader.outvertex.size() * sizeof(glm::vec3), colors[3], GL_STATIC_DRAW);
		nAttribute = glGetAttribLocation(s_program[0], "in_Color");
		glVertexAttribPointer(nAttribute, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(nAttribute);
	}
	glEnable(GL_DEPTH_TEST);
}
void DrawScene(unsigned int modelLocation, unsigned int viewLocation, unsigned int projectionLocation, int kind) {
	// 미로
	for (int i = 0; i < row; i++) {
		for (int k = 0; k < col; k++) {
			TR = glm::mat4(1.0f);
			PR = glm::mat4(1.0f);
			VR = glm::mat4(1.0f);

			move_z = float(i) * 0.4f;

			if(i > row / 2)
				TR = translate(TR, glm::vec3(float(k) * 0.4f, 0.0f, move_z));
			else if(i <= row / 2)
				TR = translate(TR, glm::vec3(float(k) * 0.4f, 0.0f, move_z));

			TR = scale(TR, glm::vec3(0.2f, scale_y[i][k], 0.2f));

			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));

			if (but_P) {
				PR = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
				PR = glm::translate(PR, glm::vec3(0.0, 0.0, -2.0));
			}
			else if (but_O)
				PR = glm::ortho(-p_p_figures, p_p_figures, -p_p_figures, p_p_figures, -p_p_figures, p_p_figures);

			glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &PR[0][0]);

			if (kind == 0)
				VR = glm::lookAt(cameraPos, cameraDirection, cameraUp);
			else 
				VR = glm::lookAt(Top_View, Top_cameraDirection, Top_cameraUp);
			glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &VR[0][0]);

			if (remove_maze[i][k] != -1) {
				glBindVertexArray(VAO[k]);
				glDrawArrays(GL_TRIANGLES, 0, s1);
			}
			else {
				arrive_pos[0] = i;
				arrive_pos[1] = k;
			}
		}
	}
	// 바닥
	{
		TR = glm::mat4(1.0f);
		PR = glm::mat4(1.0f);
		VR = glm::mat4(1.0f);

		TR = translate(TR, glm::vec3(float(row) * 0.15f, -0.5f, float(col) * 0.15f));

		TR = scale(TR, glm::vec3(2.5f, 0.3f, 2.5f));

		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));

		if (but_P) {
			PR = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
			PR = glm::translate(PR, glm::vec3(0.0, 0.0, -2.0));
		}
		else if (but_O)
			PR = glm::ortho(-p_p_figures, p_p_figures, -p_p_figures, p_p_figures, -p_p_figures, p_p_figures);

		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &PR[0][0]);

		if (kind == 0)
			VR = glm::lookAt(cameraPos, cameraDirection, cameraUp);
		else
			VR = glm::lookAt(Top_View, Top_cameraDirection, Top_cameraUp);		
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &VR[0][0]);

		glBindVertexArray(VAO[row * col]);
		glDrawArrays(GL_TRIANGLES, 0, s1);
	}
	// 객체
	if (object_appear)
	{
		TR = glm::mat4(1.0f);
		PR = glm::mat4(1.0f);
		VR = glm::mat4(1.0f);

		TR = translate(TR, glm::vec3(mx, my, mz));

		if (first_view) {
			dir_first_pointofview_pos(mx, my, mz);
			
			switch (decide_first_pointofview) {
			case 1: // 상
				cameraPos = first_pointofview_pos[0];
				break;
			case 2: // 하
				cameraPos = first_pointofview_pos[1];
				break;
			case 3: // 좌
				cameraPos = first_pointofview_pos[2];
				break;
			case 4: // 우
				cameraPos = first_pointofview_pos[3];
				break;
			}
			
			cameraDirection = first_pointofview_dir;
		}

		TR = scale(TR, glm::vec3(0.1f, 0.1f, 0.1f));

		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));

		if (but_P) {
			PR = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
			PR = glm::translate(PR, glm::vec3(0.0, 0.0, -2.0));
		}
		else if (but_O)
			PR = glm::ortho(-p_p_figures, p_p_figures, -p_p_figures, p_p_figures, -p_p_figures, p_p_figures);

		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &PR[0][0]);

		if (kind == 0)
			VR = glm::lookAt(cameraPos, cameraDirection, cameraUp);
		else
			VR = glm::lookAt(Top_View, Top_cameraDirection, Top_cameraUp);
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &VR[0][0]);

		glBindVertexArray(VAO[row * col + 1]);
		glDrawArrays(GL_TRIANGLES, 0, s1);
	}
	// 도착지점
	{
		TR = glm::mat4(1.0f);
		PR = glm::mat4(1.0f);
		VR = glm::mat4(1.0f);

		TR = translate(TR, glm::vec3(arrive_pos[1] * 0.4f, -0.1f, arrive_pos[0] * 0.4f));

		TR = scale(TR, glm::vec3(0.07f, 0.07f, 0.07f));

		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));

		if (but_P) {
			PR = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
			PR = glm::translate(PR, glm::vec3(0.0, 0.0, -2.0));
		}
		else if (but_O)
			PR = glm::ortho(-p_p_figures, p_p_figures, -p_p_figures, p_p_figures, -p_p_figures, p_p_figures);

		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &PR[0][0]);

		if (kind == 0)
			VR = glm::lookAt(cameraPos, cameraDirection, cameraUp);
		else
			VR = glm::lookAt(Top_View, Top_cameraDirection, Top_cameraUp);
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &VR[0][0]);

		glBindVertexArray(VAO[row * col + 2]);
		glDrawArrays(GL_TRIANGLES, 0, s1);
	}
}
void Display()
{
	glClearColor(1.0f, 0.5f, 0.5f, 0.5f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (draw_type == 1)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else if (draw_type == 2)
		glPolygonMode(GL_FRONT_AND_BACK, GL_TRIANGLES);

	unsigned int modelLocation = glGetUniformLocation(s_program[0], "m");
	unsigned int viewLocation = glGetUniformLocation(s_program[0], "v");
	unsigned int projectionLocation = glGetUniformLocation(s_program[0], "p");

	glUseProgram(s_program[0]);

	glViewport(0, 0, g_window_w, g_window_h);
	DrawScene(modelLocation, viewLocation, projectionLocation, 0);
	glViewport(g_window_w / 7 * 5 + 20, 0, g_window_w / 7 * 2, g_window_h / 7 * 2);
	DrawScene(modelLocation, viewLocation, projectionLocation, 5);
	
	glutSwapBuffers();
}
void Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
		{
	case 'x':
		cameraPos.x += 0.5f;
		break;
	case 'X':
		cameraPos.x -= 0.5f;
		break;
	case 'y':
		cameraPos.y += 0.5f;
		break;
	case 'Y':
		cameraPos.y -= 0.5f;
		printf("%f %f %f", cameraPos.x, cameraPos.y, cameraPos.z);
		break;
	case 'z':
		cameraPos.z += 0.5f;
		break;
	case 'Z':
		cameraPos.z -= 0.5f;
		break;
	case 'e': case'E':
		printf("%f %f %f", cameraPos.x, cameraPos.y, cameraPos.z);
		break;
	}
	case 'o': case 'O': // 직각 투영
		but_O = true;
		but_P = false;
		break;
		break;
	case 'p': case 'P': // 원근 투영
		but_O = false;
		but_P = true;
		break;
	case 'm': // 미로 움직임 on
		m_maze_rect = true;
		glutTimerFunc(100, TimerFunction, 1);
		break;
	case 'M': // 미로 움직임 off
		m_maze_rect = false;
		break;
	case 't':
		turn_camera = true;
		turn_dir = +1;
		glutTimerFunc(100, TimerFunction, 1);
		break;
	case 'T':
		turn_camera = true;
		turn_dir = -1;
		glutTimerFunc(100, TimerFunction, 1);
		break;
	case 'r': case 'R': // 미로 제작
		srand(time(0));
		while (1)
		{
			if (f_row == row - 1 || f_col == col - 1)
				break;
			int choice_pos = rand() % 2;
			if (choice_pos == 0) {
				if (f_row == row - 1)
					f_col++;
				else 
					f_row++;
			}
			else if (choice_pos == 1){
				if (f_col == col - 1) 
					f_row++;
				else 
					f_col++;
			}
			remove_maze[f_row][f_col] = -1;
		}
		remove_maze[0][1] = -1;
		break;
	case 'v':
		m_maze_rect = false;
		for (int i = 0; i < row; i++) {
			for (int k = 0; k < col; k++) {
				scale_y[i][k] = 0.08f;
				size_change[i][k] = 0.05f;
			}
		}
		break;
	case 'V':
		for (int i = 0; i < row; i++)
			for (int k = 0; k < col; k++)
				scale_y[i][k] = store_scale_y[i][k];
		m_maze_rect = true;
		break;
	case 'i': case 'I': // 객체 등장 원래 s/S 임
		object_appear = true;
		break;
		// 상하좌우 wasd
		{
	case 'w': case 'W':
		mz += object_speed;
		collision_check("move_z_plus");
		break;
	case 's': case 'S':
		mz -= object_speed;
		collision_check("move_z_minus");
		break;
	case 'a': case 'A':
		mx += object_speed;
		collision_check("move_x_plus");
		break;
	case 'd': case 'D':
		mx -= object_speed;
		collision_check("move_x_minus");
		break;
		}
	case '+': case '=': // 속도 UP
		object_speed += 0.005f;
		if (object_speed > 0.03f)
			object_speed -= 0.005f;
		break;
	case '-': case '_': // 속도 DOWN
		object_speed -= 0.005f;
		if (object_speed < 0.0f)
			object_speed += 0.005f;
		break;
	case '1': // 1인칭 시점
		if (object_appear) {
			decide_first_pointofview = 1;
			cameraPos = first_pointofview_pos[0];
			cameraDirection = first_pointofview_dir;
			first_view = true;
		}
;		break;
	case '3': // 3인칭 시점
		first_view = false;
		decide_first_pointofview = 0;
		cameraPos = third_pointofview_pos;
		cameraDirection = third_pointofview_dir;
		break;
		// 시점 변경 7 8 9 0 (상 하 좌 우)
		{
	case '7':
		decide_first_pointofview = 1;
		cameraPos = first_pointofview_pos[0];
		break;
	case '8':
		decide_first_pointofview = 2;
		cameraPos = first_pointofview_pos[1];
		break;
	case '9':
		decide_first_pointofview = 3;
		cameraPos = first_pointofview_pos[2];
		break;
	case '0':
		decide_first_pointofview = 4;
		cameraPos = first_pointofview_pos[3];
		break;
		}
	case 'c': case 'C': // 전체 위치, bool변수 초기화
		reset();
		break;
	case 'q': case 'Q':
		for (int i = 0; i <= row * col + 2; i++) {
			glDeleteVertexArrays(1, &VAO[i]);
			glDeleteBuffers(2, VBO);
		}
		exit(0);
		break;
	
	}
	printf("%c키 명령을 받았습니다\n", key);
	InitBuffer();
	glutPostRedisplay();
}
void reset() {
	// bool 변수
	but_O = false, but_P = true, m_maze_rect = false;
	object_appear = false; turn_camera = false;
	first_view = false;

	// 미로
	f_row = 0; f_col = 1;
	for (int a = 0; a < row; a++)
		for (int b = 0; b < col; b++)
			remove_maze[a][b] = 0;
	for (int i = 0; i < row; i++)
		for (int k = 0; k < col; k++)
			scale_y[i][k] = store_scale_y[i][k];
	
	// 카메라
	cameraPos = third_pointofview_pos;
	cameraDirection = third_pointofview_dir;
	turn_dir = 1, radians = 0.0f, speed = 5.0f;

	// 객체
	object_speed = 0.01f;
	mx = 0.4f, my = -0.15f, mz = 0.0f;
}
void dir_first_pointofview_pos(GLfloat mx, GLfloat my, GLfloat mz) {
	first_pointofview_pos[0] = glm::vec3(mx, my + 0.0005f, mz - 0.0005f);
	first_pointofview_pos[1] = glm::vec3(mx, my + 0.0005f, mz + 0.0005f);
	first_pointofview_pos[2] = glm::vec3(mx - 0.0005f, my + 0.0005f, mz);
	first_pointofview_pos[3] = glm::vec3(mx + 0.0005f, my + 0.0005f, mz);

	first_pointofview_dir = glm::vec3(mx, my, mz);
}
void collision_check(const char* kindofmove) {
	for (int i = 0; i < row; i++) {
		for (int k = 0; k < col; k++) {
			if (remove_maze[i][k] == -1) {
				if ((mz >= float(arrive_pos[0]) * 0.4f - 0.1f && mz <= float(arrive_pos[0]) * 0.4f + 0.1f)
					&& (mx >= float(arrive_pos[1]) * 0.4f - 0.1f && mx <= float(arrive_pos[1]) * 0.4f + 0.1f)) {
					printf("YOU REACHED ARRIVE POINT!!!!!!!!!!");
					exit(0);
				}
			}
			else if (remove_maze[i][k] == 0) {
				if ((mz >= float(i) * 0.4f - 0.3f && mz <= float(i) * 0.4f + 0.3f) && (mx >= float(k) * 0.4f - 0.3f && mx <= float(k) * 0.4f + 0.3f)) {
					if (kindofmove == "move_x_plus") 
						mx -= object_speed;
					else if (kindofmove == "move_x_minus") 
						mx += object_speed;
					else if (kindofmove == "move_z_plus") 
						mz -= object_speed;
					else if (kindofmove == "move_z_minus") 
						mz += object_speed;
				}
			}
		}
	}
}
void TimerFunction(int value)
{
	if (m_maze_rect) {
		for (int i = 0; i < row; i++) {
			for (int k = 0; k < col; k++) {
				if (k % 3 == 0)
					scale_y[i][k] += size_change[i][k];
				else if (k % 3 == 1)
					scale_y[i][k] += size_change[i][k] * 2.0f;
				else if (k % 3 == 2)
					scale_y[i][k] += size_change[i][k] * 4.0f;

				if (scale_y[i][k] >= 0.9f || scale_y[i][k] <= 0.2f)
					size_change[i][k] *= -1;
			}
		}
	}

	if (turn_camera) {
		radians += speed / 2.0f;
		cameraPos.x = -sin(glm::radians(radians)) * speed * turn_dir;
		cameraPos.z = cos(glm::radians(radians)) * speed * turn_dir;
	}

	InitBuffer();
	glutPostRedisplay();
	if (m_maze_rect || turn_camera)
		glutTimerFunc(100, TimerFunction, 1);
}