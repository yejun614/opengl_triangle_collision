/*
 * [선형대수 과제] 3차원 공간에서 발사된 총알과 삼각형의 충돌 시뮬레이션
 * 
 * 20193219 정예준
 * 20193205 조상연
 *
 * [참고자료]
 *   평면과 선의 교차점 구하기: http://www.gisdeveloper.co.kr/?p=792
 *   다각형 안에 점의 존재여부 판단하기: http://egloos.zum.com/lsujang/v/845586
 *   삼각형 안에 점이 있는지 확인하는 방법: https://hwanggoon.tistory.com/114
 *   평면상의 세 점으로부터 평면의 방정식 구하기 http://www.gisdeveloper.co.kr/?p=801
 */

#include <GL/glut.h>
#include <OpenGL/gl.h>
#include <cstdio>
#include <cmath>

// x 제곱을 구하는 매크로 함수
#define POW(x) ((x) * (x))

// PI 상수
const double PI = 3.141592653589793;

// 카메라 확대
float cameraScale = 2;
// 카메라 회전 속도
float cameraMoveSpeed = 0.1f;
// 카메라 회전 각도
float cameraRadians = 0;
// 카메라가 움지이는 반경 (원의 반지름)
float cameraRadius = 5;
// 직선 회전 속도
float lineMoveSpeed = 0.005f;

// 직선이 시작되는 지점 (총알이 발사되는 좌표)
GLfloat bulletPos[] = { 0, 0, 2 };
// 직선의 방향 (총알이 움직이는 벡터)
GLfloat bulletVector[] = { 0, 0, -1 };
// 삼각형의 좌표
GLfloat triangleVertex[3][3] = {
  { -0.5f, -0.5f,  0 },
  {  0.5f, -0.5f,  0 },
  {     0,  0.5f,  0 }
};

// 직선과 평면의 교점
GLfloat bulletMiddlePoint[3];
// 직선의 끝점 좌표
GLfloat bulletEndPoint[3];

// 충돌 여부를 저장
bool isCollision = false;

// 좌표 데이터를 입력받는 함수
void InputData() {
  float x, y, z;
  
  printf("(데이터 입력)\n");
  printf("좌료를 입력할 때는 ,(콤마)와 띄어쓰기에 조심해 주세요\n");
  printf("예시: 1, 2, 3\n\n");

  printf("삼각형 좌표 입력\n");

  printf("A (x1, y1, z1) : ");
  scanf("%f, %f, %f", &x, &y, &z);
  triangleVertex[0][0] = x;
  triangleVertex[0][1] = y;
  triangleVertex[0][2] = z;

  printf("B (x2, y2, z2) : ");
  scanf("%f, %f, %f", &x, &y, &z);
  triangleVertex[1][0] = x;
  triangleVertex[1][1] = y;
  triangleVertex[1][2] = z;

  printf("C (x3, y3, z3) : ");
  scanf("%f, %f, %f", &x, &y, &z);
  triangleVertex[2][0] = x;
  triangleVertex[2][1] = y;
  triangleVertex[2][2] = z;

  printf("\n총알 발사 좌표 입력\n");
  printf("v (v1, v2, v3) : ");
  scanf("%f, %f, %f", &x, &y, &z);
  bulletPos[0] = x;
  bulletPos[1] = y;
  bulletPos[2] = z;

  printf("\n총알 방향 벡터 입력\n");
  printf("u (u1, u2, u3) : ");
  scanf("%f, %f, %f", &x, &y, &z);
  bulletVector[0] = x;
  bulletVector[1] = y;
  bulletVector[2] = z;

  printf("\n데이터 입력 완료!\n");
}

// 벡터의 내적을 계산한 결과를 반환하는 함수
float VectorInnerProduct(GLfloat *A, GLfloat *B) {
  return (A[0] * B[0]) + (A[1] * B[1]) + (A[2] * B[2]);
}

// 벡터의 외적을 계산한 결과를 result에 저장하는 함수
void VectorCrossProduct(GLfloat *A, GLfloat *B, GLfloat *result) {
  result[0] = (A[1] * B[2]) - (A[2] * B[1]);
  result[1] = (A[2] * B[0]) - (A[0] * B[2]);
  result[2] = (A[0] * B[1]) - (A[1] * B[0]);
}

// 총알이 발사된 정점과 평면간의 거리를 반환하는 함수
float DistancePointAndPlane() {
  float A = triangleVertex[0][1] * (triangleVertex[1][2] - triangleVertex[2][2]);
  A += triangleVertex[1][1] * (triangleVertex[2][2] - triangleVertex[0][2]);
  A += triangleVertex[2][1] * (triangleVertex[0][2] - triangleVertex[1][2]);

  float B = triangleVertex[0][2] * (triangleVertex[1][0] - triangleVertex[2][0]);
  B += triangleVertex[1][2] * (triangleVertex[2][0] - triangleVertex[0][0]);
  B += triangleVertex[2][2] * (triangleVertex[0][0] - triangleVertex[1][0]);

  float C = triangleVertex[0][0] * (triangleVertex[1][1] - triangleVertex[2][1]);
  C += triangleVertex[1][0] * (triangleVertex[2][1] - triangleVertex[0][1]);
  C += triangleVertex[2][0] * (triangleVertex[0][1] - triangleVertex[1][1]);

  float D = triangleVertex[0][0] * ((triangleVertex[1][1] * triangleVertex[2][2]) - (triangleVertex[2][1] * triangleVertex[1][2]));
  D += triangleVertex[1][0] * ((triangleVertex[2][1] * triangleVertex[0][2]) - (triangleVertex[0][1] * triangleVertex[2][2]));
  D += triangleVertex[2][0] * ((triangleVertex[0][1] * triangleVertex[1][2]) - (triangleVertex[1][1] * triangleVertex[0][2]));
  D *= -1;

  float t = (A * bulletPos[0]) + (B * bulletPos[1]) + (C * bulletPos[2]) + D;
  t /= (A * bulletVector[0]) + (B * bulletVector[1]) + (C * bulletVector[2]);
  t *= -1;

  return t;
}

// 정점 point에서 vector 방향으로 length 만큼 이동한 좌표를 result에 저장하는 함수
void LineEndPoint(GLfloat *point, GLfloat *vector, float length, GLfloat *result) {
  result[0] = point[0] + (length * vector[0]);
  result[1] = point[1] + (length * vector[1]);
  result[2] = point[2] + (length * vector[2]);
}

// 정점 A와 정점 B에 대해서 AB벡터의 길이를 result에 저장하는 함수
void VectorLength(GLfloat *A, GLfloat *B, GLfloat *result) {
  result[0] = B[0] - A[0];
  result[1] = B[1] - A[1];
  result[2] = B[2] - A[2];
}

// 정점 point와 삼각형의 충돌여부를 계산하는 함수
bool CollisionCheck(GLfloat *point) {
  // 벡터의 길이를 저장하는 배열
  GLfloat ap[3], bp[3], cp[3];
  GLfloat ab[3], bc[3], ca[3];

  // 벡터의 길이를 계산
  VectorLength(triangleVertex[0], point, ap);
  VectorLength(triangleVertex[1], point, bp);
  VectorLength(triangleVertex[2], point, cp);

  VectorLength(triangleVertex[0], triangleVertex[1], ab);
  VectorLength(triangleVertex[1], triangleVertex[2], bc);
  VectorLength(triangleVertex[2], triangleVertex[0], ca);

  // 공식에 따라 삼각형의 정점과 point간의 관계를 계산한다.
  float chk1 = (ab[0] * ap[1]) - (ab[1] * ap[0]);
  float chk2 = (bc[0] * bp[1]) - (bc[1] * bp[0]);
  float chk3 = (ca[0] * cp[1]) - (ca[1] * cp[0]);

  // 계산된 수식의 부호가 모두 양수이거나 혹은 음수하면 충돌
  if (chk1 > 0 && chk2 > 0 && chk3 > 0) return true;
  if (chk1 < 0 && chk2 < 0 && chk3 < 0) return true;

  // 계산된 수식의 부호가 서로 다르다면 충돌하지 않는 것
  return false;
}

// OpenGL 초기화 함수
void init() {
  // 배경 색상을 검정색으로 지정
  glClearColor(0, 0, 0, 0);
  // 깊이 테스트를 활성화
  glEnable(GL_DEPTH_TEST);
  // 깊이 함수를 GL_ALWAYS로 설정
  glDepthFunc(GL_ALWAYS);
}

// 바닥에 Grid를 그려주는 함수
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

// OpenGL 화면 그려주는 함수
void display() {
  // 화면 지우기
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // 카메라 설정
  gluLookAt(
    (float)(cameraRadius * cos(cameraRadians)),
    (float)(cameraRadius * sin(cameraRadians)),
    5,
    0, 0, 0,
    0, 0, 1
  );

  // 확대
  glScalef(cameraScale, cameraScale, cameraScale);

  // 바닥에 Grid 그리기
  drawGrid(1, 1);

  // 직선 (하단부)
  glLineWidth(3);
  glBegin(GL_LINES);
    glColor3f(1, 0, 0);
    glVertex3fv(bulletMiddlePoint);
    glVertex3fv(bulletEndPoint);
  glEnd();

  // 삼각형
  glBegin(GL_TRIANGLES);
    glPushMatrix();
    
    // 만약 삼각형과 총알이 충돌했다면 색상을 초록색으로 변경
    // 충돌하지 않았다면 하얀색으로 변경
    if (isCollision) glColor3f(0, 1, 0);
    else glColor3f(1, 1, 1);

    glVertex3fv(triangleVertex[0]);
    glVertex3fv(triangleVertex[1]);
    glVertex3fv(triangleVertex[2]);
    
    glPopMatrix();
  glEnd();

  // 직선 상단부
  glBegin(GL_LINES);
    glColor3f(1, 0, 0);
    glVertex3fv(bulletPos);
    glVertex3fv(bulletMiddlePoint);
  glEnd();

  if (isCollision) {
    // 직선과 평면의 교점
    glPointSize(5);
    glBegin(GL_POINTS);
      glColor3f(0, 0, 1);
      glVertex3fv(bulletMiddlePoint);
    glEnd(); 
  }

  // 화면 그리기
  glFlush();
  glutSwapBuffers();
}

// 윈도우 창 크기가 변경되었을때 호출되는 함수
void reshape(GLsizei width, GLsizei height) {
	glViewport(0, 0, width, height);
  glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, width / height, 1.0, 50.0);
}

// 키보드 입력 함수 (일반키)
void keyboard(unsigned char key, int x, int y) {
  // 카메라 조작 (회전)
  if (key == 'w') cameraRadius -= cameraMoveSpeed * 2;
  else if (key == 's') cameraRadius += cameraMoveSpeed * 2;
  else if (key == 'a') cameraRadians -= cameraMoveSpeed;
  else if (key == 'd') cameraRadians += cameraMoveSpeed;
  // 카메라 조작 (확대)
  else if (key == 'n') cameraScale += cameraMoveSpeed;
  else if (key == 'm') cameraScale -= cameraMoveSpeed;
  // 데이터 입력
  else if (key == 'c') InputData();
  // 프로그램 종료
  else if (key == 'q') exit(0);

  // 회전 각도 제한
  if (cameraRadius <= 0) cameraRadius = 0.1f;
  else if (cameraRadius > 20) cameraRadius = 20;
}

// 키보드 입력 함수 (특수키)
void specialKeyboard(int key, int x, int y) {
  // 총알의 벡터 조작 (직선의 기울기를 변경)
  if (key == GLUT_KEY_UP) bulletVector[1] += lineMoveSpeed;
  else if (key == GLUT_KEY_DOWN) bulletVector[1] -= lineMoveSpeed;
  else if (key == GLUT_KEY_LEFT) bulletVector[0] -= lineMoveSpeed;
  else if (key == GLUT_KEY_RIGHT) bulletVector[0] += lineMoveSpeed;
}

// 업데이트 함수
void update() {
  // 평면의 법선벡터를 계산
  GLfloat planeVectorA[3], planeVectorB[3], planeNormalVector[3];
  VectorLength(triangleVertex[0], triangleVertex[1], planeVectorA);
  VectorLength(triangleVertex[0], triangleVertex[2], planeVectorB);
  VectorCrossProduct(planeVectorA, planeVectorB, planeNormalVector);

  // 직선과 평면의 법선벡터를 내적
  float innerProduct = VectorInnerProduct(planeNormalVector, bulletVector);

  // 총알과 평면 사이의 거리를 계산
  // (직선과 평면이 평행한 경우 길이를 3으로 고정)
  float distance = innerProduct == 0 ? 3 : DistancePointAndPlane();

  // bulletMiddlePoint: 평면과 직선이 만나는 교점
  LineEndPoint(bulletPos, bulletVector, distance, bulletMiddlePoint);
  // bulletEndPoint: 직선의 끝점
  LineEndPoint(bulletPos, bulletVector, distance * 2.0f, bulletEndPoint);

  // 충돌 여부를 저장하는 변수
  isCollision = false;

  // 직선과 평면이 평행하거나 총알 벡터가 평면과 반대인 경우 충돌하지 않는다.
  if (innerProduct != 0 && distance >= 0) {
    // 직선과 평면의 교점 좌표가 삼각형 내부에 있는지 판단
    isCollision = CollisionCheck(bulletMiddlePoint);
  }

  // 화면을 그린다.
  display();
}

// 프로그램 시작 지점
int main(int argc, char **argv) {
  // 프로그램 정보
  printf("3차원 공간에서 발사된 총알과 삼각형의 충돌 시뮬레이션\n");
  printf(" * 20193219 정예준 \n * 20193205 조상연\n\n");

  // 사용 방법 출력
  printf("(사용 방법)\n");
  printf("W A S D  : 카메라 이동\n");
  printf("N M      : 카메라 확대, 축소\n");
  printf("방향키   : 벡터 값 조정\n");
  printf("C        : 데이터 입력\n");
  printf("Q        : 프로그램 종료\n\n");

  // GLUT 초기화
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
  glutInitWindowPosition(100, 100);
  glutInitWindowSize(500, 500);
  glutCreateWindow("Triangle Raycasting Example");
  
  // OpenGL 초기화
  init();
	
  // OpenGL 함수 설정
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutSpecialFunc(specialKeyboard);
  glutIdleFunc(update);

  // 메인 루프 시작
  glutMainLoop();

  // 프로그램 종료
  return 0;
}
