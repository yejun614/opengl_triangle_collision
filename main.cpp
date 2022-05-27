#include <GL/glut.h>
#include <OpenGL/gl.h>
#include <cstdio>
#include <cmath>

#define POW(x) ((x) * (x))

const double PI = 3.141592653589793;

float cameraMoveSpeed = 0.1f;
float lineMoveSpeed = 0.001f;

float cameraRadians = 0;
float cameraRadius = 5;

GLfloat bulletPos[] = { 0, 0, 10 };
GLfloat bulletVector[] = { 0, 0, -1 };
GLfloat triangleVertex[3][3] = {
  { -0.5f, -0.5f,  0 },
  {  0.5f, -0.5f,  0 },
  {     0,  0.5f,  0 }
};

void LineEndPoint(GLfloat *point, GLfloat *vector, float length, GLfloat *result) {
  result[0] = point[0] + (length * vector[0]);
  result[1] = point[1] + (length * vector[1]);
  result[2] = point[2] + (length * vector[2]);
}

void VectorFromPoints(GLfloat *A, GLfloat *B, GLfloat *result) {
  // 정점 A와 정점 B에 대해서 AB벡터의 길이를 result에 반환
  result[0] = B[0] - A[0];
  result[1] = B[1] - A[1];
  result[2] = B[2] - A[2];
}

bool InnerTrianglePointCheck(GLfloat *point) {
  // http://egloos.zum.com/lsujang/v/845586
  // https://sausagetaste.github.io/2020/10/08/what_is_rtx_1.html
  // https://hwanggoon.tistory.com/114

  GLfloat ap[3], bp[3], cp[3];
  GLfloat ab[3], bc[3], ca[3];

  VectorFromPoints(triangleVertex[0], point, ap);
  VectorFromPoints(triangleVertex[1], point, bp);
  VectorFromPoints(triangleVertex[2], point, cp);

  VectorFromPoints(triangleVertex[0], triangleVertex[1], ab);
  VectorFromPoints(triangleVertex[1], triangleVertex[2], bc);
  VectorFromPoints(triangleVertex[2], triangleVertex[0], ca);

  float chk1, chk2, chk3;

  chk1 = (ab[0] * ap[1]) - (ab[1] * ap[0]);
  chk2 = (bc[0] * bp[1]) - (bc[1] * bp[0]);
  chk3 = (ca[0] * cp[1]) - (ca[1] * cp[0]);

  // printf("CHK: %.1f %.1f %.1f\n", chk1, chk2, chk3);

  if (chk1 > 0 && chk2 > 0 && chk3 > 0) return true;
  if (chk1 < 0 && chk2 < 0 && chk3 < 0) return true;

  return false;
}

void init() {
  glClearColor(0, 0, 0, 0);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_ALWAYS);
}

void drawGrid(float width, float height) {
  glLineWidth(1);
  glColor3f(0.3f, 0.3f, 0.3f);

  for (float x = -1; x <= 1.1f; x += 0.1f) {
    glBegin(GL_LINES);
      glVertex3f(x, -width, 0);
      glVertex3f(x, width, 0);
    glEnd();
  }

  for (float y = -1; y <= 1.1f; y += 0.1f) {
    glBegin(GL_LINES);
      glVertex3f(-height, y, 0);
      glVertex3f(height, y, 0);
    glEnd();
  }
}

void display() {
  // [평면과 선의 교차점 구하기] http://www.gisdeveloper.co.kr/?p=792

  GLfloat bulletMiddlePoint[3], bulletEndPoint[3];

  // bulletMiddlePoint: 평면과 직선이 만나는 교점
  LineEndPoint(bulletPos, bulletVector, 10, bulletMiddlePoint);

  // bulletEndPoint: 직선의 끝점
  LineEndPoint(bulletMiddlePoint, bulletVector, 10, bulletEndPoint);

  bool isCollision = InnerTrianglePointCheck(bulletMiddlePoint);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  // gluLookAt(5, 5, 5, 0, 0, 0, 0, 0, 1);
  gluLookAt(
    (float)(cameraRadius * cos(cameraRadians)),
    (float)(cameraRadius * sin(cameraRadians)),
    5,
    0, 0, 0,
    0, 0, 1
  );

  drawGrid(1, 1);

  glLineWidth(3);
  glBegin(GL_LINES);
    glColor3f(1, 0, 0);
    glVertex3fv(bulletMiddlePoint);
    glVertex3fv(bulletEndPoint);
  glEnd();

  glBegin(GL_TRIANGLES);
    glPushMatrix();
    
    if (isCollision) glColor3f(0, 1, 0);
    else glColor3f(1, 1, 1);

    glVertex3fv(triangleVertex[0]);
    glVertex3fv(triangleVertex[1]);
    glVertex3fv(triangleVertex[2]);
    
    glPopMatrix();
  glEnd();

  glBegin(GL_LINES);
    glColor3f(1, 0, 0);
    glVertex3fv(bulletPos);
    glVertex3fv(bulletMiddlePoint);
  glEnd();

  glPointSize(5);
  glBegin(GL_POINTS);
    glColor3f(0, 0, 1);
    glVertex3fv(bulletMiddlePoint);
  glEnd();

  glFlush();
  glutSwapBuffers();
}

void reshape(GLsizei width, GLsizei height) {
	glViewport(0, 0, width, height);
  glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, width / height, 1.0, 50.0);
}

void keyboard(unsigned char key, int x, int y) {
  if (key == 'w') cameraRadius -= cameraMoveSpeed * 2;
  else if (key == 's') cameraRadius += cameraMoveSpeed * 2;
  else if (key == 'a') cameraRadians -= cameraMoveSpeed;
  else if (key == 'd') cameraRadians += cameraMoveSpeed;

  if (cameraRadius <= 0) cameraRadius = 0.1f;
  else if (cameraRadius > 10) cameraRadius = 10;
}

void specialKeyboard(int key, int x, int y) {
  if (key == GLUT_KEY_UP) bulletVector[1] += lineMoveSpeed;
  else if (key == GLUT_KEY_DOWN) bulletVector[1] -= lineMoveSpeed;
  else if (key == GLUT_KEY_LEFT) bulletVector[0] -= lineMoveSpeed;
  else if (key == GLUT_KEY_RIGHT) bulletVector[0] += lineMoveSpeed;
}

void update() {
  display();
}

int main(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
  glutInitWindowPosition(100, 100);
  glutInitWindowSize(500, 500);
  glutCreateWindow("Triangle Raycasting Example");
  
  init();
	
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutSpecialFunc(specialKeyboard);
  glutIdleFunc(update);

  glutMainLoop();

  return 0;
}
