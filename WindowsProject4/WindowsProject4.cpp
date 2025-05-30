// WindowsProject4.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "WindowsProject4.h"
#include "math.h"
#include "timeapi.h"
#include <cmath>
#include <limits>

#define MAX_LOADSTRING 100
HWND hWnd;

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

struct Blocks {
    int xcenter;
    int ycenter;
    int width;
    int height;
    bool show;
};

struct Ball {
    int xcenter;
    int ycenter;
    int R;
    bool show;
    int vx; //Скорость
    int vy;
    float vmod; // Модуль вектора скорости
    float vs[2];
};

struct Racket {
    int xcenter;
    int ycenter;
    int width;
    int height;
    int speed;
};

int sign(float x) {
    return (x > 0) ? 1 : ((x < 0) ? -1 : 0);
}

void DrawLINE(HDC hdc, int x1, int y1, int x2, int y2, int s, int r, int g, int b) { //Рисование линии
    // Создаем перо (толщина 2, красный цвет)
    HPEN hPen = CreatePen(PS_SOLID, s, RGB(r, g, b));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen); // Выбираем перо в контекст

    // Рисуем линию
    MoveToEx(hdc, x1, y1, NULL); // Начальная точка (x1, y1)
    LineTo(hdc, x2, y2);         // Конечная точка (x2, y2)

    // Восстанавливаем старое перо и удаляем созданное
    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);
}

void VertexCollision(Ball ball, Blocks block, float* xMin, float* yMin, int* ACx, int* ACy, float* distmin) {
    int xmin = INT_MAX;
    int ymin = INT_MAX;
    float dC;

    float xC;
    float yC;


    int Ax;
    int Ay;

    int ax;
    int ay;
    long double sD;

    float tempx, tempy;

    for (int i = -1; i <= 1; i+=2) {
        for (int j = -1; j <= 1; j += 2) {
            Ax = block.xcenter + i*block.width / 2;
            Ay = block.ycenter + j*block.height / 2;

            ax = Ax - ball.xcenter;
            ay = Ay - ball.ycenter;
            sD = sqrt(pow((2.0 * ball.vy * ball.vx * ay - 2.0 * pow(ball.vy, 2) * ax), 2) - 4.0 * (pow(ball.vx, 2) + pow(ball.vy, 2)) * (pow(ball.vy, 2) * pow(ax, 2) + pow(ball.vx, 2) * pow(ay, 2) - 2 * ball.vx * ball.vy * ax * ay - pow(ball.R * ball.vx, 2))); //Дискриминант

            if (sD >= 0) {
                xC = ((2.0 * pow(ball.vy, 2) * ax - 2.0 * ball.vy * ball.vx * ay) + sign(ball.vx) * sD) / (2.0 * (pow(ball.vx, 2) + pow(ball.vy, 2)));
                yC = ((ball.vy*(xC-ax)+ay*ball.vx>=0)? 1 : -1) * ((ball.vx >= 0) ? 1 : -1) * sqrt(pow(ball.R, 2) - pow(xC, 2));

                tempx = xC; 
                tempy = yC;

                xC += ball.xcenter;
                yC += ball.ycenter;

                if (Ay == 274) {
                    Ay = Ay;
                }

                if ((sign(Ax - xC) == sign(ball.vx) || sign(Ax - xC) == 0) && (sign(Ay - yC) == sign(ball.vy) || sign(Ay - yC) == 0)) {
                    dC = pow(xC - Ax, 2) + pow(yC - Ay, 2);
                    if (*distmin > dC) {
                        *distmin = dC;
                        *xMin = xC;
                        *yMin = yC;
                        *ACx = Ax;
                        *ACy = Ay;
                    }
                }
            }
        }
    }

}

void VertexCollisionRacket(Ball ball, Racket racket, float* xMin, float* yMin, int* ACx, int* ACy, float* distmin) {
    int xmin = INT_MAX;
    int ymin = INT_MAX;
    float dC;

    float xC;
    float yC;


    int Ax;
    int Ay;

    int ax;
    int ay;
    long double sD;

    float tempx, tempy;

    for (int i = -1; i <= 1; i += 2) {
        for (int j = -1; j <= 1; j += 2) {
            Ax = racket.xcenter + i * racket.width / 2;
            Ay = racket.ycenter + j * racket.height / 2;

            ax = Ax - ball.xcenter;
            ay = Ay - ball.ycenter;
            sD = sqrt(pow((2.0 * ball.vy * ball.vx * ay - 2.0 * pow(ball.vy, 2) * ax), 2) - 4.0 * (pow(ball.vx, 2) + pow(ball.vy, 2)) * (pow(ball.vy, 2) * pow(ax, 2) + pow(ball.vx, 2) * pow(ay, 2) - 2 * ball.vx * ball.vy * ax * ay - pow(ball.R * ball.vx, 2))); //Дискриминант

            if (sD >= 0) {
                xC = ((2.0 * pow(ball.vy, 2) * ax - 2.0 * ball.vy * ball.vx * ay) + sign(ball.vx) * sD) / (2.0 * (pow(ball.vx, 2) + pow(ball.vy, 2)));
                yC = ((ball.vy * (xC - ax) + ay * ball.vx >= 0) ? 1 : -1) * ((ball.vx >= 0) ? 1 : -1) * sqrt(pow(ball.R, 2) - pow(xC, 2));

                tempx = xC;
                tempy = yC;

                xC += ball.xcenter;
                yC += ball.ycenter;

                if (Ay == 274) {
                    Ay = Ay;
                }

                if ((sign(Ax - xC) == sign(ball.vx) || sign(Ax - xC) == 0) && (sign(Ay - yC) == sign(ball.vy) || sign(Ay - yC) == 0)) {
                    dC = pow(xC - Ax, 2) + pow(yC - Ay, 2);
                    if (*distmin > dC) {
                        *distmin = dC;
                        *xMin = xC;
                        *yMin = yC;
                        *ACx = Ax;
                        *ACy = Ay;
                    }
                }
            }
        }
    }

}

bool Collision(Ball* ball, Racket racket, Blocks* blocks, size_t blsize, int width, int height, HDC hdc = 0) {
    int q1, xC1, yC1, q2, xC2, yC2, q3, xC3, yC3, q4, xC4, yC4; //ПЕРЕДЕЛАТЬ
    float dC1, dC2, dC3, dC4;

    float xC, xCR; //Для углов
    float yC, yCR;

    int xmin = ball->xcenter + ball->vx;
    int ymin = ball->ycenter + ball->vy;
    float distmin = FLT_MAX;

    int ballpoint[2]; // Точка коллизии на шаре

    bool flag[3] = {0,0,0}; //Бока, верх-низ и углы

    int imin = -1; // Блок, который должен уйти

    int IntersectionPoint[2] = { ball->xcenter , ball->ycenter }; //Точка пересечения на шаре
    if (ball->vx >= 0 && ball->vy < 0) { //ПРАВО-НИЗ (для пользователя это ПРАВО ВЕРХ)
        for (size_t i = 0; i < blsize; i++) { // ПОСТАВИЛ "НЕ"
            if (blocks[i].show && !(blocks[i].xcenter + blocks[i].width / 2 < ball->xcenter - ball->R || 
                blocks[i].xcenter - blocks[i].width / 2 > ball->xcenter + ball->R + ball->vx ||
                blocks[i].ycenter + blocks[i].height / 2 < ball->ycenter - ball->R + ball->vy ||
                blocks[i].ycenter - blocks[i].height / 2 > ball->ycenter + ball->R)) {

                //blocks[i].show = 0;

                //НИЖНЯЯ КОЛЛИЗИЯ (для пользователя это ВЕРХ)
                ballpoint[0] = ball->xcenter;
                ballpoint[1] = ball->ycenter - ball->R;

                q4 = blocks[i].ycenter + blocks[i].height / 2 - ball->ycenter;
                xC4 = (q4 + ball->R)*(ball->vx / ball->vy) + ball->xcenter;
                if (xC4 >= blocks[i].xcenter - blocks[i].width / 2 && xC4 <= blocks[i].xcenter + blocks[i].width / 2 && xC4 >= ball->xcenter) { //Такое условие только для верха-низа. Для лева-права другое, на y
                    yC4 = q4 + ball->ycenter;
                    dC4 = pow((xC4 - ballpoint[0]), 2) + pow((yC4 - ballpoint[1]), 2);
                    if (distmin > dC4) {
                        distmin = dC4;
                        xmin = xC4;
                        ymin = yC4;

                        //TEMP
                        IntersectionPoint[0] = ballpoint[0];
                        IntersectionPoint[1] = ballpoint[1];

                        flag[0] = 0;
                        flag[1] = 1;
                        flag[2] = 0;

                        imin = i;
                    }
                }

                //ПРАВАЯ КОЛЛИЗИЯ
                if (ball->vx) {
                    ballpoint[0] = ball->xcenter + ball->R;
                    ballpoint[1] = ball->ycenter;

                    q2 = blocks[i].xcenter - blocks[i].width / 2 - ball->xcenter;
                    yC2 = (q2 - ball->R) * (ball->vy / ball->vx) + ball->ycenter;

                    //DrawLINE(hdc, ballpoint[0], ballpoint[1], xC2, yC2, 2, 255, 0, 0);

                    if (yC2 >= blocks[i].ycenter - blocks[i].height / 2 && yC2 <= blocks[i].ycenter + blocks[i].height / 2 && yC2 <= ball->ycenter) {
                        xC2 = q2 + ball->xcenter;
                        dC2 = pow((xC2 - ballpoint[0]), 2) + pow((yC2 - ballpoint[1]), 2);
                        if (distmin > dC2) {
                            distmin = dC2;
                            xmin = xC2;
                            ymin = yC2;

                            //TEMP
                            IntersectionPoint[0] = ballpoint[0];
                            IntersectionPoint[1] = ballpoint[1];

                            flag[0] = 1;
                            flag[1] = 0;
                            flag[2] = 0;

                            imin = i;
                        }
                    }
                }

                //УГЛЫ
                xC = -1;
                yC = -1;
                VertexCollision(*ball, blocks[i], &xC, &yC, &xmin, &ymin, &distmin);
                if (xC > 0) {
                    ballpoint[0] = xC;
                    ballpoint[1] = yC;

                    //TEMP
                    IntersectionPoint[0] = ballpoint[0];
                    IntersectionPoint[1] = ballpoint[1];

                    flag[0] = 0;
                    flag[1] = 0;
                    flag[2] = 1;

                    imin = i;
                }

            }
        }
        if (ball->xcenter + ball->R + ball->vx > width) {
            //ПРАВАЯ КОЛЛИЗИЯ
            if (ball->vx) {
                ballpoint[0] = ball->xcenter + ball->R;
                ballpoint[1] = ball->ycenter;

                q2 = width - ball->xcenter;
                yC2 = (q2 - ball->R) * (ball->vy / ball->vx) + ball->ycenter;

                //DrawLINE(hdc, ballpoint[0], ballpoint[1], xC2, yC2, 2, 255, 0, 0);

                xC2 = q2 + ball->xcenter;
                dC2 = pow((xC2 - ballpoint[0]), 2) + pow((yC2 - ballpoint[1]), 2);
                if (distmin > dC2) {
                    distmin = dC2;
                    xmin = xC2;
                    ymin = yC2;

                    //TEMP
                    IntersectionPoint[0] = ballpoint[0];
                    IntersectionPoint[1] = ballpoint[1];

                    flag[0] = 1;
                    flag[1] = 0;
                    flag[2] = 0;
                }
            }
        }
        if (ball->ycenter - ball->R + ball->vy < 0) {
            //НИЖНЯЯ КОЛЛИЗИЯ (для пользователя это ВЕРХ)
            ballpoint[0] = ball->xcenter;
            ballpoint[1] = ball->ycenter - ball->R;

            q4 = 0 - ball->ycenter;
            xC4 = (q4 + ball->R) * (ball->vx / ball->vy) + ball->xcenter;
            yC4 = q4 + ball->ycenter;
            dC4 = pow((xC4 - ballpoint[0]), 2) + pow((yC4 - ballpoint[1]), 2);
            if (distmin > dC4) {
                distmin = dC4;
                xmin = xC4;
                ymin = yC4;

                //TEMP
                IntersectionPoint[0] = ballpoint[0];
                IntersectionPoint[1] = ballpoint[1];

                flag[0] = 0;
                flag[1] = 1;
                flag[2] = 0;
            }
        }
        if (!(racket.xcenter + racket.width / 2 < ball->xcenter - ball->R ||
            racket.xcenter - racket.width / 2 > ball->xcenter + ball->R + ball->vx ||
            racket.ycenter + racket.height / 2 < ball->ycenter - ball->R + ball->vy ||
            racket.ycenter - racket.height / 2 > ball->ycenter + ball->R)) {

            //blocks[i].show = 0;

            //НИЖНЯЯ КОЛЛИЗИЯ (для пользователя это ВЕРХ)
            ballpoint[0] = ball->xcenter;
            ballpoint[1] = ball->ycenter - ball->R;

            q4 = racket.ycenter + racket.height / 2 - ball->ycenter;
            xC4 = (q4 + ball->R) * (ball->vx / ball->vy) + ball->xcenter;
            if (xC4 >= racket.xcenter - racket.width / 2 && xC4 <= racket.xcenter + racket.width / 2 && xC4 >= ball->xcenter) { //Такое условие только для верха-низа. Для лева-права другое, на y
                yC4 = q4 + ball->ycenter;
                dC4 = pow((xC4 - ballpoint[0]), 2) + pow((yC4 - ballpoint[1]), 2);
                if (distmin > dC4) {
                    distmin = dC4;
                    xmin = xC4;
                    ymin = yC4;

                    //TEMP
                    IntersectionPoint[0] = ballpoint[0];
                    IntersectionPoint[1] = ballpoint[1];

                    flag[0] = 0;
                    flag[1] = 1;
                    flag[2] = 0;
                }
            }

            //ПРАВАЯ КОЛЛИЗИЯ
            if (ball->vx) {
                ballpoint[0] = ball->xcenter + ball->R;
                ballpoint[1] = ball->ycenter;

                q2 = racket.xcenter - racket.width / 2 - ball->xcenter;
                yC2 = (q2 - ball->R) * (ball->vy / ball->vx) + ball->ycenter;

                //DrawLINE(hdc, ballpoint[0], ballpoint[1], xC2, yC2, 2, 255, 0, 0);

                if (yC2 >= racket.ycenter - racket.height / 2 && yC2 <= racket.ycenter + racket.height / 2 && yC2 <= ball->ycenter) {
                    xC2 = q2 + ball->xcenter;
                    dC2 = pow((xC2 - ballpoint[0]), 2) + pow((yC2 - ballpoint[1]), 2);
                    if (distmin > dC2) {
                        distmin = dC2;
                        xmin = xC2;
                        ymin = yC2;

                        //TEMP
                        IntersectionPoint[0] = ballpoint[0];
                        IntersectionPoint[1] = ballpoint[1];

                        flag[0] = 1;
                        flag[1] = 0;
                        flag[2] = 0;
                    }
                }
            }

            //УГЛЫ
            xCR = -1;
            yCR = -1;
            VertexCollisionRacket(*ball, racket, &xCR, &yCR, &xmin, &ymin, &distmin);
            if (xCR > 0) {
                ballpoint[0] = xCR;
                ballpoint[1] = yCR;

                //TEMP
                IntersectionPoint[0] = ballpoint[0];
                IntersectionPoint[1] = ballpoint[1];

                flag[0] = 0;
                flag[1] = 0;
                flag[2] = 1;
            }

        }

    }
    if (ball->vx < 0 && ball->vy < 0) { //ЛЕВО-НИЗ (для пользователя это ЛЕВО ВЕРХ)
        for (size_t i = 0; i < blsize; i++) { // ПОСТАВИЛ "НЕ"
            if (blocks[i].show && !(blocks[i].xcenter - blocks[i].width / 2 > ball->xcenter + ball->R ||
                blocks[i].xcenter + blocks[i].width / 2 < ball->xcenter - ball->R + ball->vx ||
                blocks[i].ycenter + blocks[i].height / 2 < ball->ycenter - ball->R + ball->vy ||
                blocks[i].ycenter - blocks[i].height / 2 > ball->ycenter + ball->R)) {

                //blocks[i].show = 0;

                //НИЖНЯЯ КОЛЛИЗИЯ (для пользователя это ВЕРХ)
                ballpoint[0] = ball->xcenter;
                ballpoint[1] = ball->ycenter - ball->R;

                q4 = blocks[i].ycenter + blocks[i].height / 2 - ball->ycenter;
                xC4 = (q4 + ball->R) * (ball->vx / ball->vy) + ball->xcenter;
                if (xC4 >= blocks[i].xcenter - blocks[i].width / 2 && xC4 <= blocks[i].xcenter + blocks[i].width / 2 && xC4 <= ball->xcenter) { //Такое условие только для верха-низа. Для лева-права другое, на y
                    yC4 = q4 + ball->ycenter;
                    dC4 = pow((xC4 - ballpoint[0]), 2) + pow((yC4 - ballpoint[1]), 2);
                    if (distmin > dC4) {
                        distmin = dC4;
                        xmin = xC4;
                        ymin = yC4;

                        //TEMP
                        IntersectionPoint[0] = ballpoint[0];
                        IntersectionPoint[1] = ballpoint[1];

                        flag[0] = 0;
                        flag[1] = 1;
                        flag[2] = 0;

                        imin = i;
                    }
                }

                //ЛЕВАЯ КОЛЛИЗИЯ
                if (ball->vx) {
                    ballpoint[0] = ball->xcenter - ball->R;
                    ballpoint[1] = ball->ycenter;

                    q3 = blocks[i].xcenter + blocks[i].width / 2 - ball->xcenter;
                    yC3 = (q3 + ball->R) * (ball->vy / ball->vx) + ball->ycenter;

                    //DrawLINE(hdc, ballpoint[0], ballpoint[1], xC2, yC2, 2, 255, 0, 0);

                    if (yC3 >= blocks[i].ycenter - blocks[i].height / 2 && yC3 <= blocks[i].ycenter + blocks[i].height / 2 && yC3 <= ball->ycenter) {
                        xC3 = q3 + ball->xcenter;
                        dC3 = pow((xC3 - ballpoint[0]), 2) + pow((yC3 - ballpoint[1]), 2);
                        if (distmin > dC3) {
                            distmin = dC3;
                            xmin = xC3;
                            ymin = yC3;

                            //TEMP
                            IntersectionPoint[0] = ballpoint[0];
                            IntersectionPoint[1] = ballpoint[1];

                            flag[0] = 1;
                            flag[1] = 0;
                            flag[2] = 0;

                            imin = i;
                        }
                    }
                }

                //УГЛЫ
                xC = -1;
                yC = -1;
                VertexCollision(*ball, blocks[i], &xC, &yC, &xmin, &ymin, &distmin);
                if (xC > 0) {
                    ballpoint[0] = xC;
                    ballpoint[1] = yC;

                    //TEMP
                    IntersectionPoint[0] = ballpoint[0];
                    IntersectionPoint[1] = ballpoint[1];

                    flag[0] = 0;
                    flag[1] = 0;
                    flag[2] = 1;

                    imin = i;
                }

            }
        }
        if (ball->xcenter - ball->R + ball->vx < 0) {
            //ЛЕВАЯ КОЛЛИЗИЯ
            if (ball->vx) {
                ballpoint[0] = ball->xcenter - ball->R;
                ballpoint[1] = ball->ycenter;

                q3 = 0 - ball->xcenter;
                yC3 = (q3 + ball->R) * (ball->vy / ball->vx) + ball->ycenter;

                //DrawLINE(hdc, ballpoint[0], ballpoint[1], xC2, yC2, 2, 255, 0, 0);

                xC3 = q3 + ball->xcenter;
                dC3 = pow((xC3 - ballpoint[0]), 2) + pow((yC3 - ballpoint[1]), 2);
                if (distmin > dC3) {
                    distmin = dC3;
                    xmin = xC3;
                    ymin = yC3;

                    //TEMP
                    IntersectionPoint[0] = ballpoint[0];
                    IntersectionPoint[1] = ballpoint[1];

                    flag[0] = 1;
                    flag[1] = 0;
                    flag[2] = 0;
                }
            }
        }
        if (ball->ycenter - ball->R + ball->vy < 0) {
            //НИЖНЯЯ КОЛЛИЗИЯ (для пользователя это ВЕРХ)
            ballpoint[0] = ball->xcenter;
            ballpoint[1] = ball->ycenter - ball->R;

            q4 = 0 - ball->ycenter;
            xC4 = (q4 + ball->R) * (ball->vx / ball->vy) + ball->xcenter;
            yC4 = q4 + ball->ycenter;
            dC4 = pow((xC4 - ballpoint[0]), 2) + pow((yC4 - ballpoint[1]), 2);
            if (distmin > dC4) {
                distmin = dC4;
                xmin = xC4;
                ymin = yC4;

                //TEMP
                IntersectionPoint[0] = ballpoint[0];
                IntersectionPoint[1] = ballpoint[1];

                flag[0] = 0;
                flag[1] = 1;
                flag[2] = 0;
            }
        }
        if (!(racket.xcenter - racket.width / 2 > ball->xcenter + ball->R ||
            racket.xcenter + racket.width / 2 < ball->xcenter - ball->R + ball->vx ||
            racket.ycenter + racket.height / 2 < ball->ycenter - ball->R + ball->vy ||
            racket.ycenter - racket.height / 2 > ball->ycenter + ball->R)) {

            //racket.show = 0;

            //НИЖНЯЯ КОЛЛИЗИЯ (для пользователя это ВЕРХ)
            ballpoint[0] = ball->xcenter;
            ballpoint[1] = ball->ycenter - ball->R;

            q4 = racket.ycenter + racket.height / 2 - ball->ycenter;
            xC4 = (q4 + ball->R) * (ball->vx / ball->vy) + ball->xcenter;
            if (xC4 >= racket.xcenter - racket.width / 2 && xC4 <= racket.xcenter + racket.width / 2 && xC4 <= ball->xcenter) { //Такое условие только для верха-низа. Для лева-права другое, на y
                yC4 = q4 + ball->ycenter;
                dC4 = pow((xC4 - ballpoint[0]), 2) + pow((yC4 - ballpoint[1]), 2);
                if (distmin > dC4) {
                    distmin = dC4;
                    xmin = xC4;
                    ymin = yC4;

                    //TEMP
                    IntersectionPoint[0] = ballpoint[0];
                    IntersectionPoint[1] = ballpoint[1];

                    flag[0] = 0;
                    flag[1] = 1;
                    flag[2] = 0;
                }
            }

            //ЛЕВАЯ КОЛЛИЗИЯ
            if (ball->vx) {
                ballpoint[0] = ball->xcenter - ball->R;
                ballpoint[1] = ball->ycenter;

                q3 = racket.xcenter + racket.width / 2 - ball->xcenter;
                yC3 = (q3 + ball->R) * (ball->vy / ball->vx) + ball->ycenter;

                //DrawLINE(hdc, ballpoint[0], ballpoint[1], xC2, yC2, 2, 255, 0, 0);

                if (yC3 >= racket.ycenter - racket.height / 2 && yC3 <= racket.ycenter + racket.height / 2 && yC3 <= ball->ycenter) {
                    xC3 = q3 + ball->xcenter;
                    dC3 = pow((xC3 - ballpoint[0]), 2) + pow((yC3 - ballpoint[1]), 2);
                    if (distmin > dC3) {
                        distmin = dC3;
                        xmin = xC3;
                        ymin = yC3;

                        //TEMP
                        IntersectionPoint[0] = ballpoint[0];
                        IntersectionPoint[1] = ballpoint[1];

                        flag[0] = 1;
                        flag[1] = 0;
                        flag[2] = 0;
                    }
                }
            }

            //УГЛЫ
            xCR = -1;
            yCR = -1;
            VertexCollisionRacket(*ball, racket, &xCR, &yCR, &xmin, &ymin, &distmin);
            if (xCR > 0) {
                ballpoint[0] = xCR;
                ballpoint[1] = yCR;

                //TEMP
                IntersectionPoint[0] = ballpoint[0];
                IntersectionPoint[1] = ballpoint[1];

                flag[0] = 0;
                flag[1] = 0;
                flag[2] = 1;
            }

        }
    }
    if (ball->vx < 0 && ball->vy >= 0) { //ЛЕВО-ВЕРХ (для пользователя это ЛЕВО НИЗ)
        for (size_t i = 0; i < blsize; i++) { // ПОСТАВИЛ "НЕ"
            if (blocks[i].show && !(blocks[i].xcenter - blocks[i].width / 2 > ball->xcenter + ball->R ||
                blocks[i].xcenter + blocks[i].width / 2 < ball->xcenter - ball->R + ball->vx ||
                blocks[i].ycenter - blocks[i].height / 2 > ball->ycenter + ball->R + ball->vy ||
                blocks[i].ycenter + blocks[i].height / 2 < ball->ycenter - ball->R)) {

                //blocks[i].show = 0;

                //ВЕРХНЯЯ КОЛЛИЗИЯ (для пользователя это НИЗ)
                if (ball->vy) {
                    ballpoint[0] = ball->xcenter;
                    ballpoint[1] = ball->ycenter + ball->R;

                    q1 = blocks[i].ycenter - blocks[i].height / 2 - ball->ycenter;
                    xC1 = (q1 - ball->R) * (ball->vx / ball->vy) + ball->xcenter;
                    if (xC1 >= blocks[i].xcenter - blocks[i].width / 2 && xC1 <= blocks[i].xcenter + blocks[i].width / 2 && xC1 <= ball->xcenter) { //Такое условие только для верха-низа. Для лева-права другое, на y
                        yC1 = q1 + ball->ycenter;
                        dC1 = pow((xC1 - ballpoint[0]), 2) + pow((yC1 - ballpoint[1]), 2);
                        if (distmin > dC1) {
                            distmin = dC1;
                            xmin = xC1;
                            ymin = yC1;

                            //TEMP
                            IntersectionPoint[0] = ballpoint[0];
                            IntersectionPoint[1] = ballpoint[1];

                            flag[0] = 0;
                            flag[1] = 1;
                            flag[2] = 0;

                            imin = i;
                        }
                    }
                }

                //ЛЕВАЯ КОЛЛИЗИЯ
                if (ball->vx) {
                    ballpoint[0] = ball->xcenter - ball->R;
                    ballpoint[1] = ball->ycenter;

                    q3 = blocks[i].xcenter + blocks[i].width / 2 - ball->xcenter;
                    yC3 = (q3 + ball->R) * (ball->vy / ball->vx) + ball->ycenter;

                    //DrawLINE(hdc, ballpoint[0], ballpoint[1], xC2, yC2, 2, 255, 0, 0);

                    if (yC3 >= blocks[i].ycenter - blocks[i].height / 2 && yC3 <= blocks[i].ycenter + blocks[i].height / 2 && yC3 >= ball->ycenter) {
                        xC3 = q3 + ball->xcenter;
                        dC3 = pow((xC3 - ballpoint[0]), 2) + pow((yC3 - ballpoint[1]), 2);
                        if (distmin > dC3) {
                            distmin = dC3;
                            xmin = xC3;
                            ymin = yC3;

                            //TEMP
                            IntersectionPoint[0] = ballpoint[0];
                            IntersectionPoint[1] = ballpoint[1];

                            flag[0] = 1;
                            flag[1] = 0;
                            flag[2] = 0;

                            imin = i;
                        }
                    }
                }

                //УГЛЫ
                xC = -1;
                yC = -1;
                VertexCollision(*ball, blocks[i], &xC, &yC, &xmin, &ymin, &distmin);
                if (xC > 0) {
                    ballpoint[0] = xC;
                    ballpoint[1] = yC;

                    //TEMP
                    IntersectionPoint[0] = ballpoint[0];
                    IntersectionPoint[1] = ballpoint[1];

                    flag[0] = 0;
                    flag[1] = 0;
                    flag[2] = 1;

                    imin = i;
                }

            }
        }
        if (ball->xcenter - ball->R + ball->vx < 0) {
            //ЛЕВАЯ КОЛЛИЗИЯ
            if (ball->vx) {
                ballpoint[0] = ball->xcenter - ball->R;
                ballpoint[1] = ball->ycenter;

                q3 = 0 - ball->xcenter;
                yC3 = (q3 + ball->R) * (ball->vy / ball->vx) + ball->ycenter;

                //DrawLINE(hdc, ballpoint[0], ballpoint[1], xC2, yC2, 2, 255, 0, 0);

                xC3 = q3 + ball->xcenter;
                dC3 = pow((xC3 - ballpoint[0]), 2) + pow((yC3 - ballpoint[1]), 2);
                if (distmin > dC3) {
                    distmin = dC3;
                    xmin = xC3;
                    ymin = yC3;

                    //TEMP
                    IntersectionPoint[0] = ballpoint[0];
                    IntersectionPoint[1] = ballpoint[1];

                    flag[0] = 1;
                    flag[1] = 0;
                    flag[2] = 0;
                }
            }
        }
        if (ball->ycenter + ball->R + ball->vy > height) {
            //ВЕРХНЯЯ КОЛЛИЗИЯ (для пользователя это НИЗ)
            if (ball->vy) {
                ballpoint[0] = ball->xcenter;
                ballpoint[1] = ball->ycenter + ball->R;

                q1 = height - ball->ycenter;
                xC1 = (q1 - ball->R) * (ball->vx / ball->vy) + ball->xcenter;
                yC1 = q1 + ball->ycenter;
                dC1 = pow((xC1 - ballpoint[0]), 2) + pow((yC1 - ballpoint[1]), 2);
                if (distmin > dC1) {
                    distmin = dC1;
                    xmin = xC1;
                    ymin = yC1;

                    //TEMP
                    IntersectionPoint[0] = ballpoint[0];
                    IntersectionPoint[1] = ballpoint[1];

                    flag[0] = 0;
                    flag[1] = 1;
                    flag[2] = 0;
                }
            }
        }
        if (!(racket.xcenter - racket.width / 2 > ball->xcenter + ball->R ||
            racket.xcenter + racket.width / 2 < ball->xcenter - ball->R + ball->vx ||
            racket.ycenter - racket.height / 2 > ball->ycenter + ball->R + ball->vy ||
            racket.ycenter + racket.height / 2 < ball->ycenter - ball->R)) {

            //racket.show = 0;

            //ВЕРХНЯЯ КОЛЛИЗИЯ (для пользователя это НИЗ)
            if (ball->vy) {
                ballpoint[0] = ball->xcenter;
                ballpoint[1] = ball->ycenter + ball->R;

                q1 = racket.ycenter - racket.height / 2 - ball->ycenter;
                xC1 = (q1 - ball->R) * (ball->vx / ball->vy) + ball->xcenter;
                if (xC1 >= racket.xcenter - racket.width / 2 && xC1 <= racket.xcenter + racket.width / 2 && xC1 <= ball->xcenter) { //Такое условие только для верха-низа. Для лева-права другое, на y
                    yC1 = q1 + ball->ycenter;
                    dC1 = pow((xC1 - ballpoint[0]), 2) + pow((yC1 - ballpoint[1]), 2);
                    if (distmin > dC1) {
                        distmin = dC1;
                        xmin = xC1;
                        ymin = yC1;

                        //TEMP
                        IntersectionPoint[0] = ballpoint[0];
                        IntersectionPoint[1] = ballpoint[1];

                        flag[0] = 0;
                        flag[1] = 1;
                        flag[2] = 0;

                    }
                }
            }

            //ЛЕВАЯ КОЛЛИЗИЯ
            if (ball->vx) {
                ballpoint[0] = ball->xcenter - ball->R;
                ballpoint[1] = ball->ycenter;

                q3 = racket.xcenter + racket.width / 2 - ball->xcenter;
                yC3 = (q3 + ball->R) * (ball->vy / ball->vx) + ball->ycenter;

                //DrawLINE(hdc, ballpoint[0], ballpoint[1], xC2, yC2, 2, 255, 0, 0);

                if (yC3 >= racket.ycenter - racket.height / 2 && yC3 <= racket.ycenter + racket.height / 2 && yC3 >= ball->ycenter) {
                    xC3 = q3 + ball->xcenter;
                    dC3 = pow((xC3 - ballpoint[0]), 2) + pow((yC3 - ballpoint[1]), 2);
                    if (distmin > dC3) {
                        distmin = dC3;
                        xmin = xC3;
                        ymin = yC3;

                        //TEMP
                        IntersectionPoint[0] = ballpoint[0];
                        IntersectionPoint[1] = ballpoint[1];

                        flag[0] = 1;
                        flag[1] = 0;
                        flag[2] = 0;

                    }
                }
            }

            //УГЛЫ
            xCR = -1;
            yCR = -1;
            VertexCollisionRacket(*ball, racket, &xCR, &yCR, &xmin, &ymin, &distmin);
            if (xCR > 0) {
                ballpoint[0] = xCR;
                ballpoint[1] = yCR;

                //TEMP
                IntersectionPoint[0] = ballpoint[0];
                IntersectionPoint[1] = ballpoint[1];

                flag[0] = 0;
                flag[1] = 0;
                flag[2] = 1;

            }

        }
    }
    if (ball->vx >= 0 && ball->vy >= 0) { //ПРАВО-ВЕРХ (для пользователя это ПРАВО НИЗ)
        for (size_t i = 0; i < blsize; i++) { // ПОСТАВИЛ "НЕ"
            if (blocks[i].show && !(blocks[i].xcenter + blocks[i].width / 2 < ball->xcenter - ball->R ||
                blocks[i].xcenter - blocks[i].width / 2 > ball->xcenter + ball->R + ball->vx ||
                blocks[i].ycenter - blocks[i].height / 2 > ball->ycenter + ball->R + ball->vy ||
                blocks[i].ycenter + blocks[i].height / 2 < ball->ycenter - ball->R)) {

                //blocks[i].show = 0;

                //ВЕРХНЯЯ КОЛЛИЗИЯ (для пользователя это НИЗ)
                if (ball->vy) {
                    ballpoint[0] = ball->xcenter;
                    ballpoint[1] = ball->ycenter + ball->R;

                    q1 = blocks[i].ycenter - blocks[i].height / 2 - ball->ycenter;
                    xC1 = (q1 - ball->R) * (ball->vx / ball->vy) + ball->xcenter;
                    if (xC1 >= blocks[i].xcenter - blocks[i].width / 2 && xC1 <= blocks[i].xcenter + blocks[i].width / 2 && xC1 >= ball->xcenter) { //Такое условие только для верха-низа. Для лева-права другое, на y
                        yC1 = q1 + ball->ycenter;
                        dC1 = pow((xC1 - ballpoint[0]), 2) + pow((yC1 - ballpoint[1]), 2);
                        if (distmin > dC1) {
                            distmin = dC1;
                            xmin = xC1;
                            ymin = yC1;

                            //TEMP
                            IntersectionPoint[0] = ballpoint[0];
                            IntersectionPoint[1] = ballpoint[1];

                            flag[0] = 0;
                            flag[1] = 1;
                            flag[2] = 0;

                            imin = i;
                        }
                    }
                }

                //ПРАВАЯ КОЛЛИЗИЯ
                if (ball->vx) {
                    ballpoint[0] = ball->xcenter + ball->R;
                    ballpoint[1] = ball->ycenter;

                    q2 = blocks[i].xcenter - blocks[i].width / 2 - ball->xcenter;
                    yC2 = (q2 - ball->R) * (ball->vy / ball->vx) + ball->ycenter;

                    //DrawLINE(hdc, ballpoint[0], ballpoint[1], xC2, yC2, 2, 255, 0, 0);

                    if (yC2 >= blocks[i].ycenter - blocks[i].height / 2 && yC2 <= blocks[i].ycenter + blocks[i].height / 2 && yC2 >= ball->ycenter) {
                        xC2 = q2 + ball->xcenter;
                        dC2 = pow((xC2 - ballpoint[0]), 2) + pow((yC2 - ballpoint[1]), 2);
                        if (distmin > dC2) {
                            distmin = dC2;
                            xmin = xC2;
                            ymin = yC2;

                            //TEMP
                            IntersectionPoint[0] = ballpoint[0];
                            IntersectionPoint[1] = ballpoint[1];

                            flag[0] = 1;
                            flag[1] = 0;
                            flag[2] = 0;

                            imin = i;
                        }
                    }
                }

                //УГЛЫ
                xC = -1;
                yC = -1;
                VertexCollision(*ball, blocks[i], &xC, &yC, &xmin, &ymin, &distmin);
                if (xC > 0) {
                    ballpoint[0] = xC;
                    ballpoint[1] = yC;

                    //TEMP
                    IntersectionPoint[0] = ballpoint[0];
                    IntersectionPoint[1] = ballpoint[1];

                    flag[0] = 0;
                    flag[1] = 0;
                    flag[2] = 1;

                    imin = i;
                }

            }
        }
        if (ball->ycenter + ball->R + ball->vy > height) {
            //ВЕРХНЯЯ КОЛЛИЗИЯ (для пользователя это НИЗ)
            if (ball->vy) {
                ballpoint[0] = ball->xcenter;
                ballpoint[1] = ball->ycenter + ball->R;

                q1 = height - ball->ycenter;
                xC1 = (q1 - ball->R) * (ball->vx / ball->vy) + ball->xcenter;
                yC1 = q1 + ball->ycenter;
                dC1 = pow((xC1 - ballpoint[0]), 2) + pow((yC1 - ballpoint[1]), 2);
                if (distmin > dC1) {
                    distmin = dC1;
                    xmin = xC1;
                    ymin = yC1;

                    //TEMP
                    IntersectionPoint[0] = ballpoint[0];
                    IntersectionPoint[1] = ballpoint[1];

                    flag[0] = 0;
                    flag[1] = 1;
                    flag[2] = 0;
                }
            }
        }
        if (ball->xcenter + ball->R + ball->vx > width) {
            //ПРАВАЯ КОЛЛИЗИЯ
            if (ball->vx) {
                ballpoint[0] = ball->xcenter + ball->R;
                ballpoint[1] = ball->ycenter;

                q2 = width - ball->xcenter;
                yC2 = (q2 - ball->R) * (ball->vy / ball->vx) + ball->ycenter;

                //DrawLINE(hdc, ballpoint[0], ballpoint[1], xC2, yC2, 2, 255, 0, 0);

                xC2 = q2 + ball->xcenter;
                dC2 = pow((xC2 - ballpoint[0]), 2) + pow((yC2 - ballpoint[1]), 2);
                if (distmin > dC2) {
                    distmin = dC2;
                    xmin = xC2;
                    ymin = yC2;

                    //TEMP
                    IntersectionPoint[0] = ballpoint[0];
                    IntersectionPoint[1] = ballpoint[1];

                    flag[0] = 1;
                    flag[1] = 0;
                    flag[2] = 0;
                }
            }
        }
        if (!(racket.xcenter + racket.width / 2 < ball->xcenter - ball->R ||
            racket.xcenter - racket.width / 2 > ball->xcenter + ball->R + ball->vx ||
            racket.ycenter - racket.height / 2 > ball->ycenter + ball->R + ball->vy ||
            racket.ycenter + racket.height / 2 < ball->ycenter - ball->R)) {

            //racket.show = 0;

            //ВЕРХНЯЯ КОЛЛИЗИЯ (для пользователя это НИЗ)
            if (ball->vy) {
                ballpoint[0] = ball->xcenter;
                ballpoint[1] = ball->ycenter + ball->R;

                q1 = racket.ycenter - racket.height / 2 - ball->ycenter;
                xC1 = (q1 - ball->R) * (ball->vx / ball->vy) + ball->xcenter;
                if (xC1 >= racket.xcenter - racket.width / 2 && xC1 <= racket.xcenter + racket.width / 2 && xC1 >= ball->xcenter) { //Такое условие только для верха-низа. Для лева-права другое, на y
                    yC1 = q1 + ball->ycenter;
                    dC1 = pow((xC1 - ballpoint[0]), 2) + pow((yC1 - ballpoint[1]), 2);
                    if (distmin > dC1) {
                        distmin = dC1;
                        xmin = xC1;
                        ymin = yC1;

                        //TEMP
                        IntersectionPoint[0] = ballpoint[0];
                        IntersectionPoint[1] = ballpoint[1];

                        flag[0] = 0;
                        flag[1] = 1;
                        flag[2] = 0;

                    }
                }
            }

            //ПРАВАЯ КОЛЛИЗИЯ
            if (ball->vx) {
                ballpoint[0] = ball->xcenter + ball->R;
                ballpoint[1] = ball->ycenter;

                q2 = racket.xcenter - racket.width / 2 - ball->xcenter;
                yC2 = (q2 - ball->R) * (ball->vy / ball->vx) + ball->ycenter;

                //DrawLINE(hdc, ballpoint[0], ballpoint[1], xC2, yC2, 2, 255, 0, 0);

                if (yC2 >= racket.ycenter - racket.height / 2 && yC2 <= racket.ycenter + racket.height / 2 && yC2 >= ball->ycenter) {
                    xC2 = q2 + ball->xcenter;
                    dC2 = pow((xC2 - ballpoint[0]), 2) + pow((yC2 - ballpoint[1]), 2);
                    if (distmin > dC2) {
                        distmin = dC2;
                        xmin = xC2;
                        ymin = yC2;

                        //TEMP
                        IntersectionPoint[0] = ballpoint[0];
                        IntersectionPoint[1] = ballpoint[1];

                        flag[0] = 1;
                        flag[1] = 0;
                        flag[2] = 0;

                    }
                }
            }

            //УГЛЫ
            xCR = -1;
            yCR = -1;
            VertexCollisionRacket(*ball, racket, &xCR, &yCR, &xmin, &ymin, &distmin);
            if (xCR > 0) {
                ballpoint[0] = xCR;
                ballpoint[1] = yCR;

                //TEMP
                IntersectionPoint[0] = ballpoint[0];
                IntersectionPoint[1] = ballpoint[1];

                flag[0] = 0;
                flag[1] = 0;
                flag[2] = 1;

            }

        }
    }

    //Sleep(100);
    //if (imin == 102) {
    //    imin = imin;
    //}

    if (hdc) DrawLINE(hdc, IntersectionPoint[0], IntersectionPoint[1], xmin, ymin, 2, 0, 255, 0);


    ball->xcenter += xmin - IntersectionPoint[0];
    ball->ycenter += ymin - IntersectionPoint[1];
    ball->vx -= xmin - IntersectionPoint[0];
    ball->vy -= ymin - IntersectionPoint[1];
    if (ball->vx == 0 && ball->vy == 0) {
        ball->vx = ball->vmod * ball->vs[0];
        ball->vy = ball->vmod * ball->vs[1];
    }

    blocks[imin].show = 0;

    if (hdc) {
        HBRUSH brush = CreateSolidBrush(RGB(24, 200, 0));
        SelectObject(hdc, brush); //Выбираем кисть
        Ellipse(hdc, ball->xcenter - ball->R, ball->ycenter - ball->R, ball->xcenter + ball->R, ball->ycenter + ball->R);
        DeleteObject(brush);
    }

    if (flag[0]) {
        ball->vx *= -1;
        ball->vs[0] *= -1;
        if (hdc) DrawLINE(hdc, ball->xcenter, ball->ycenter, ball->xcenter + ball->vx, ball->ycenter + ball->vy, 2, 255, 0, 255);
        return 1;
    }
    if (flag[1]) {
        ball->vy *= -1;
        ball->vs[1] *= -1;
        if (hdc) DrawLINE(hdc, ball->xcenter, ball->ycenter, ball->xcenter + ball->vx, ball->ycenter + ball->vy, 2, 255, 0, 255);
        return 1;
    }

    if (flag[2]) {
        float tempvmod = sqrt(ball->vx * ball->vx + ball->vy * ball->vy);
       /* float temp1 = (ball->vy + ymin - IntersectionPoint[1]);
        float temp2 = (ball->vx + xmin - IntersectionPoint[0]);
        float temp3 = xC - ball->xcenter + (xmin - IntersectionPoint[0]);*/
        float at1 = atan((ball->vy + ymin - IntersectionPoint[1]) / (ball->vx + xmin - IntersectionPoint[0]));
        float at2 = 2 * atan((xC - ball->xcenter + (xmin - IntersectionPoint[0])) / (yC - ball->ycenter + (ymin - IntersectionPoint[1])));
        float phinew = -(at1 + at2);

        int t1 = ((ball->vx * (ymin - ball->ycenter) >= 0) ? 1 : -1);
        float s1 = ball->vx * (ymin - ball->ycenter);
        int t2 = (((ymin - ball->ycenter) >= 0) ? 1 : -1);
        float t3 = sin(phinew);

        ball->vx = tempvmod * cos(phinew) * ((ball->vx * (ymin - ball->ycenter) >= 0) ? 1 : -1) * (((ymin - ball->ycenter) >= 0) ? 1 : -1);
        //ball->vy = tempvmod * sin(phinew) * ((ball->vx * (ymin - ball->ycenter) >= 0) ? 1 : -1) * (((ymin - ball->ycenter) >= 0) ? 1 : -1);
        ball->vy = tempvmod * sin(phinew) * t1 * t2;

        

        ball->vs[0] = (ball->vx)/ sqrt(ball->vx * ball->vx + ball->vy * ball->vy);
        ball->vs[1] = (ball->vy) / sqrt(ball->vx * ball->vx + ball->vy * ball->vy);
        if (hdc) DrawLINE(hdc, ball->xcenter, ball->ycenter, ball->xcenter + ball->vx, ball->ycenter + ball->vy, 2, 255, 0, 255);
        return 1;
    }

    return 0;

    
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWSPROJECT4, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT4));

    

    HDC device_context = GetDC(hWnd);
    RECT r;
    GetClientRect(hWnd, &r);
    auto width = r.right - r.left;//определяем размеры и сохраняем
    auto height = r.bottom - r.top;

    HDC context = CreateCompatibleDC(device_context);

    SelectObject(context, CreateCompatibleBitmap(device_context, width, height));//привязываем окно к контексту
    
    GetClientRect(hWnd, &r);

    MSG msg = { 0 };

    
    const int xcount = 7;
    const int ycount = 16;
    const int blsize = xcount*ycount;
    const int border = 2;
    const float yinterval = 0.06; //Высота блоков
    const float blocksheightcoef = 0.2;

    Blocks blocks[blsize];

    for (int i = 0; i < blsize; i++) {
        blocks[i].xcenter = i % xcount * width / xcount + width / (2*xcount);
        blocks[i].ycenter = i / xcount * width / (2 * xcount) + width * yinterval;
        blocks[i].width = width / xcount - border;
        blocks[i].height = width * yinterval;

        blocks[i].ycenter *= blocksheightcoef;
        blocks[i].height *= blocksheightcoef;

        blocks[i].show = bool(i%xcount - 3) + (i / xcount < 10);
    }

    Racket racket;
    racket.width = width / xcount - border;
    racket.height = 1.5 * width * yinterval * blocksheightcoef;
    racket.xcenter = width / 2;
    racket.ycenter = height - racket.height - border;
    racket.speed = 5;

    Ball ball;
    ball.R = 10;
    ball.xcenter = 0.7*width / 2;
    ball.ycenter = height - 5 * racket.height - ball.R - 2*border;
    ball.vx = 50;
    ball.vy = 50; //-10
    ball.vmod = sqrt(ball.vx * ball.vx + ball.vy * ball.vy);
    ball.vs[0] = ball.vx / ball.vmod;
    ball.vs[1] = ball.vy / ball.vmod;

    bool iffirst = 1;

    // Main message loop:
    while (msg.message != WM_QUIT)
    {
            while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
            {
                if (msg.message == WM_QUIT)
                {
                    break;
                }

                TranslateMessage(&msg);
                DispatchMessage(&msg);

            }

            POINT pnt;
            GetCursorPos(&pnt);
            ScreenToClient(GetForegroundWindow(), &pnt);
            racket.xcenter = max(pnt.x, width / (2 * xcount) + 2 * border);
            racket.xcenter = min(pnt.x, width - width / (2 * xcount) - 2 * border);

            //if (GetAsyncKeyState(VK_LEFT) && racket.xcenter > width / (2 * xcount) + 2 * border) racket.xcenter -= racket.speed;
            //if (GetAsyncKeyState(VK_RIGHT) && racket.xcenter < width - width / (2 * xcount) - 2 * border) racket.xcenter += racket.speed;


            ////**TEMP**
            //float coef = 1.0 / 3;
            //if (GetAsyncKeyState(VK_LEFT)) ball.xcenter -= racket.speed * coef;
            //if (GetAsyncKeyState(VK_RIGHT)) ball.xcenter += racket.speed * coef;
            //if (GetAsyncKeyState(VK_UP)) ball.ycenter -= racket.speed * coef;
            //if (GetAsyncKeyState(VK_DOWN)) ball.ycenter += racket.speed * coef;
            ////**TEMP**

            HBRUSH brush;

            brush = CreateHatchBrush(HS_BDIAGONAL, RGB(0, 255, 0)); // Создаём кисть определённого стиля и цвета

            RECT WinCoord = {}; //Массив координат окна 
            GetWindowRect(hWnd, &WinCoord); //Узнаём координаты

            brush = CreateSolidBrush(RGB(0, 0, 0));
            SelectObject(context, brush); //Выбираем кисть
            Rectangle(context, 0, 0, WinCoord.right, WinCoord.bottom); //фон
            DeleteObject(brush);

            brush = CreateSolidBrush(RGB(0, 0, 255)); //Создаём кисть определённого стиля и цвета
            SelectObject(context, brush); //Выбираем кисть

            ////**TEMP**
            //for (int i = 0; i < blsize; i++) {
            //    blocks[i].show = bool(i % xcount - 3) + (i / xcount < 10);
            //}
            //ball.vx = 400 * cos(timeGetTime() * .001);
            //ball.vy = 400 * sin(timeGetTime() * .001);
            ////**TEMP**

            for (int i = 0; i < blsize; i++) {
                if (blocks[i].show) {
                    Rectangle(context, blocks[i].xcenter - blocks[i].width/2, blocks[i].ycenter - blocks[i].height / 2, blocks[i].xcenter + blocks[i].width / 2, blocks[i].ycenter + blocks[i].height / 2);
                }
            }
            
            //Rectangle(context, WinCoord.right/3, WinCoord.bottom / 3, WinCoord.right / 3 + 100 * cos(timeGetTime() * .001), WinCoord.bottom / 3 + 100 * sin(timeGetTime() * .001)); //Рисуем новый прямоугольник, который будет небом
            DeleteObject(brush); //Очищаем память от созданной, но уже ненужной кисти

            brush = CreateSolidBrush(RGB(0, 255, 255));
            SelectObject(context, brush); //Выбираем кисть
            Rectangle(context, racket.xcenter - racket.width / 2, racket.ycenter - racket.height / 2, racket.xcenter + racket.width / 2, racket.ycenter + racket.height / 2);
            DeleteObject(brush);

            brush = CreateSolidBrush(RGB(240, 200, 0));
            SelectObject(context, brush); //Выбираем кисть
            Ellipse(context, ball.xcenter - ball.R, ball.ycenter - ball.R, ball.xcenter + ball.R, ball.ycenter + ball.R);
            DeleteObject(brush);

            if (!iffirst) {
                while (Collision(&ball, racket, blocks, sizeof(blocks) / sizeof(blocks[0]), width, height, context));
                ball.vx = ball.vs[0] * ball.vmod;
                ball.vy = ball.vs[1] * ball.vmod;
            }

            DrawLINE(context, ball.xcenter, ball.ycenter, ball.xcenter + ball.vx, ball.ycenter + ball.vy, 2, 255, 0, 255);

            BitBlt(device_context, 0, 0, WinCoord.right, WinCoord.bottom, context, 0, 0, SRCCOPY);

            //Sleep(10);

            // TODO: Add any drawing code that uses hdc here...
            //EndPaint(hWnd, &ps);
            iffirst = 0;
     }

    ExitProcess(0);


    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT4));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WINDOWSPROJECT4);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

    hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
