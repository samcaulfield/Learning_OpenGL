#include <GL/glew.h>
#include <GL/freeglut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

GLuint fshaderid, vshaderid, programid, vao, vbo, ibo;

float angle;

float rotX[] = {
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
};

float rotY[] = {
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
};

const GLchar *fshader = {
"#version 400\n"\

"in vec4 ex_Color;\n"\
"out vec4 out_Color;\n"\

"void main(void)\n"\
"{\n"\
"	out_Color = ex_Color;\n"\
"}\n"
};

const GLchar *vshader = {
"#version 400\n"\

"layout(location=0) in vec4 in_Position;\n"\
"layout(location=1) in vec4 in_Color;\n"\
"out vec4 ex_Color;\n"\

"uniform mat4 rotX;\n"\
"uniform mat4 rotY;\n"\

"void main(void)\n"\
"{\n"\
"	gl_Position = (rotX * rotY) * in_Position;\n"\
"	ex_Color = in_Color;\n"\
"}\n"
};

#define TBUF_SZ 64
char tbuf[TBUF_SZ];

uint16_t winw = 800, winh = 800, frame = 0;

void createshaders(void);
void createvbo(void);
void display(void);
void idle(void);
void resize(int, int);
void timer(int);

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitContextVersion(4, 0);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE,
		GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutInitWindowSize(winw, winh);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	if (glutCreateWindow("Waiting for framerate data...") < 1) {
		fprintf(stderr, "ERROR: Could not create a window.\n");
		return EXIT_FAILURE;
	}
	GLenum result = glewInit();
	if (result != GLEW_OK) {
		fprintf(stderr, "ERROR: %s\n", glewGetErrorString(result));
		return EXIT_FAILURE;
	}
	createshaders();
	createvbo();
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutReshapeFunc(resize);
	glutTimerFunc(0, timer, 0);
	fprintf(stdout, "INFO: OpenGL Version: %s\n", glGetString(GL_VERSION));
	glutMainLoop();
	return EXIT_SUCCESS;
}

void createshaders(void)
{
	vshaderid = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vshaderid, 1, &vshader, NULL);
	glCompileShader(vshaderid);
	fshaderid = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fshaderid, 1, &fshader, NULL);
	glCompileShader(fshaderid);
	programid = glCreateProgram();
	glAttachShader(programid, vshaderid);
	glAttachShader(programid, fshaderid);
	glLinkProgram(programid);
	glUseProgram(programid);
}

void createvbo(void)
{
	const float vertices[64] = {
		-.5f, -.5f, .5f, 1,  0, 0, 1, 1,
		-.5f, .5f, .5f, 1,   1, 0, 0, 1,
		.5f, .5f, .5f, 1,    0, 1, 0, 1,
		.5f, -.5f, .5f, 1,   1, 1, 0, 1,
		-.5f, -.5f, -.5f, 1, 1, 1, 1, 1,
		-.5f, .5f, -.5f, 1,  1, 0, 0, 1,
		.5f, .5f, -.5f, 1,   1, 0, 1, 1,
		.5f, -.5f, -.5f, 1,  0, 0, 1, 1  
	};
	const GLuint indices[36] = {
		0,2,1, 0,3,2,
		4,3,0, 4,7,3,
		4,1,5, 4,0,1,
		3,6,2, 3,7,6,
		1,6,5, 1,2,6,
		7,5,6, 7,4,5
	};
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof vertices, vertices,
		GL_STATIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT),
		0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT),
		(GLvoid *) (4 * sizeof(GL_FLOAT)));
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof indices, indices,
		GL_STATIC_DRAW);
}

void display(void)
{
	frame++;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	angle += 0.001;
	if (angle > 180)
		angle = 0;
	rotX[5] = (float) cos(angle);
	rotX[6] = (float) -sin(angle);
	rotX[9] = (float) sin(angle);
	rotX[10] = (float) cos(angle);
	rotY[0] = (float) cos(angle);
	rotY[8] = (float) sin(angle);
	rotY[2] = (float) -sin(angle);
	rotY[10] = (float) cos(angle);
	glUniformMatrix4fv(glGetUniformLocation(programid, "rotX"), 1, GL_FALSE,
		rotX);
	glUniformMatrix4fv(glGetUniformLocation(programid, "rotY"), 1, GL_FALSE,
		rotY);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glutSwapBuffers();
	glutPostRedisplay();
}

void idle(void)
{
	glutPostRedisplay();
}

void resize(int neww, int newh)
{
	winw = neww;
	winh = newh;
	glViewport(0, 0, winw, winh);
}

void timer(int x)
{
	if (x) {
		snprintf(tbuf, TBUF_SZ, "%d FPS @ %d x %d", frame, winw, winh);
		glutSetWindowTitle(tbuf);
	}
	frame = 0;
	glutTimerFunc(1000, timer, 1);
}

