// 5.ImageFiles.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
using namespace std;

const double PI = 3.141592653589793;
const string path = "x64/Debug/PPM/001.ppm";
const int width(300), height(300);

class Point {
public:
    uint8_t R, G, B;
    Point(int a, int b, int c) {
        if (a > 255) R = 255; else R = a;
        if (b > 255) G = 255; else G = b;
        if (c > 255) B = 255; else B = c;
    }
    Point() {
        R = 0;
        G = 0;
        B = 0;
    }
    string StrPoint() {
        string result;
        result += (char)R;
        result += (char)G;
        result += (char)B;
        return result;
    }
};


class vertex {
public:
    vertex() {
        x = 0;
        y = 0;
        z = 0;
    }
    vertex(float xx, float yy, float zz) {
        x = xx;
        y = yy;
        z = zz;
    }
   float x, y, z;
};

class vertex2d {
public:
    float x, y;
    vertex2d() {
        x = 0;
        y = 0;
    }
    vertex2d(float xx, float yy) {
        x = xx;
        y = yy;
    }
};

class vector3d {
public:
    float x, y, z;
    vector3d(vertex Normal) {
        x = Normal.x;
        y = Normal.y;
        z = Normal.z;
    }
    vector3d(float xx, float yy, float zz) {
        x = xx;
        y = yy;
        z = zz;
    }
    vector3d() {
        x = 0.f;
        y = 0.f;
        z = 0.f;
    }
};

class straight {
public:
    vertex M;
    vector3d P;
    straight(vertex MM, vector3d PP) {
        M = MM;
        P = PP;
    }
    straight(vertex MM, vertex MM1) {
        M = MM;
        P = vector3d(MM1.x - MM.x, MM1.y - MM.y, MM1.z - MM.z);
    }
};

class Edge {
public:
    float a, b, c;
    // Знак сравнения оппозитной точки с уравнением прямой
    bool IsOpp = false;
    Edge() {
        a = 0;
        b = 0;
        c = 0;
    }
    Edge(vertex2d p1, vertex2d p2) {    //определяем коэффициенты уравнения ax+by+c=0 для граней 1-2,2-3,3-1
        if ((p1.x - p2.x) == 0) {
            b = 0;
            a = 1;
            c = -p1.x;
        }
        else {
            b = -1;
            a = (static_cast<float>(p1.y - p2.y) / static_cast<float>(p1.x - p2.x));
            c = (-a * (p1.x + p2.x) - b * (p1.y + p2.y)) / 2;
        }
        cout << "a = " << a << " b = " << b << " c = " << c << "\n";
    }
    void IsPositive(vertex2d p) { 
        IsOpp = (p.x * a + p.y * b + c >= 0);
    }
};

// Поворот координат точки vert принадлежащей плоскости с нормалью vec1 относительно vec2 по оси X
void RotateVectorOnX(vertex& vert, vector3d vec1, vector3d vec2) {
    float alpha;
    alpha = acos((vec1.y * vec2.y + vec1.z * vec2.z) / (sqrt(vec1.y * vec1.y + vec1.z * vec1.z) * sqrt(vec2.y * vec2.y + vec2.z * vec2.z)));
    cout << alpha * 57.2958 << "\n";
    float tempv1;
    tempv1 = vert.y * cos(2 * PI + alpha) + vert.z * sin(2 * PI + alpha);
    vert.z = -vert.y * sin(2 * PI + alpha) + vert.z * cos(2 * PI + alpha);
    vert.y = tempv1;
}

// Поворот координат точки vert принадлежащей плоскости с нормалью vec1 относительно vec2 по оси Z
void RotateVectorOnZ(vertex& vert, vector3d vec1, vector3d vec2) {
    float alpha;
    alpha = acos((vec1.x * vec2.x + vec1.y * vec2.y) / (sqrt(vec1.x * vec1.x + vec1.y * vec1.y) * sqrt(vec2.x * vec2.x + vec2.y * vec2.y)));
    cout << alpha * 57.2958 << "\n";
    float tempv1;
    tempv1 = vert.x * cos(2 * PI + alpha) + vert.y * sin(2 * PI + alpha);
    vert.y = -vert.x * sin(2 * PI + alpha) + vert.y * cos(2 * PI + alpha);
    vert.x = tempv1;
}

// Поворот координат точки vert принадлежащей плоскости с нормалью vec1 относительно vec2 по оси Y
void RotateVectorOnY(vertex& vert, vector3d vec1, vector3d vec2) {
    float alpha;
    alpha = acos((vec1.x * vec2.x + vec1.z * vec2.z) / (sqrt(vec1.x * vec1.x + vec1.z * vec1.z) * sqrt(vec2.x * vec2.x + vec2.z * vec2.z)));
    cout << alpha * 57.2958 << "\n";
    float tempv1;
    tempv1 = vert.x * cos(2 * PI + alpha) + vert.z * sin(2 * PI + alpha);
    vert.z = -vert.x * sin(2 * PI + alpha) + vert.z * cos(2 * PI + alpha);
    vert.x = tempv1;
}

void RelativeCoords(vertex& v1, vertex& v2, vertex& v3) {
    float maxX, maxZ, minX, minZ;
    if ((v1.x >= v2.x) && (v1.x >= v3.x)) maxX = v1.x;
    if ((v2.x >= v3.x) && (v2.x >= v1.x)) maxX = v2.x;
    if ((v3.x >= v1.x) && (v3.x >= v2.x)) maxX = v3.x;

    if ((v1.x <= v2.x) && (v1.x <= v3.x)) minX = v1.x;
    if ((v2.x <= v3.x) && (v2.x <= v1.x)) minX = v2.x;
    if ((v3.x <= v1.x) && (v3.x <= v2.x)) minX = v3.x;

    if ((v1.z >= v2.z) && (v1.z >= v3.z)) maxZ = v1.z;
    if ((v2.z >= v3.z) && (v2.z >= v1.z)) maxZ = v2.z;
    if ((v3.z >= v1.z) && (v3.z >= v2.z)) maxZ = v3.z;

    if ((v1.z <= v2.z) && (v1.z <= v3.z)) minZ = v1.z;
    if ((v2.z <= v3.z) && (v2.z <= v1.z)) minZ = v2.z;
    if ((v3.z <= v1.z) && (v3.z <= v2.z)) minZ = v3.z;
    cout << "minmaxX = (" << minX << ":" << maxX << ") minmaxZ = (" << minZ << ":" << maxZ << ")\n";

    float temp;
    if ((maxX - minX) >= (maxZ - minZ)) temp = 1 / (maxX - minX); else temp = 1 / (maxZ - minZ);
    v1.x = v1.x * temp;
    v2.x = v2.x * temp;
    v3.x = v3.x * temp;
    v1.z = v1.z * temp;
    v2.z = v2.z * temp;
    v3.z = v3.z * temp;
}

class Polygon {
public:
    Polygon() {
        v1 = vertex(0, 0, 0);
        v2 = vertex(100, 0, 0);
        v3 = vertex(0, 0, 100);
        A = B = C = D = 0.f;
    }
    Polygon(vertex inv1, vertex inv2, vertex inv3) {
        v1 = inv1;
        v2 = inv2;
        v3 = inv3;
        NormalVector();
    }
    string outstr() {
        string result;
        result += to_string(v1.x);
        result += ' ';
        result += to_string(v1.y);
        result += ' ';
        result += to_string(v1.z);
        result += '\n';
        result += to_string(v2.x);
        result += ' ';
        result += to_string(v2.y);
        result += ' ';
        result += to_string(v2.z);
        result += '\n';
        result += to_string(v3.x);
        result += ' ';
        result += to_string(v3.y);
        result += ' ';
        result += to_string(v3.z);
        result += '\n';
        return result;
    }
    string outstr2d() {
        string result;
        result += to_string(v2d1.x);
        result += ' ';
        result += to_string(v2d1.y);
        result += '\n';
        result += to_string(v2d2.x);
        result += ' ';
        result += to_string(v2d2.y);
        result += '\n';
        result += to_string(v2d3.x);
        result += ' ';
        result += to_string(v2d3.y);
        result += '\n';
        return result;
    }
    void SetVertex3d(vertex inv1, vertex inv2, vertex inv3) {
        v1 = inv1;
        v2 = inv2;
        v3 = inv3;
    }
    void SetVertex2d(vertex2d inv1, vertex2d inv2, vertex2d inv3) {
        v2d1 = inv1;
        v2d2 = inv2;
        v2d3 = inv3;
    }
    void SetExtremum() {
        if ((v2d1.x >= v2d2.x) && (v2d1.x >= v2d3.x)) {
            maxX = v2d1.x;
        }
        else
            if ((v2d2.x >= v2d3.x) && (v2d2.x >= v2d1.x)) {
                maxX = v2d2.x;
            }
            else maxX = v2d3.x;
        if ((v2d1.y >= v2d2.y) && (v2d1.y >= v2d3.y)) {
            maxY = v2d1.y;
        }
        else
            if ((v2d2.y >= v2d3.y) && (v2d2.y >= v2d1.y)) {
                maxY = v2d2.y;
            }
            else maxY = v2d3.y;
        if ((v2d1.x < v2d2.x) && (v2d1.x < v2d3.x)) {
            minX = v2d1.x;
        }
        else
            if ((v2d2.x < v2d3.x) && (v2d2.x < v2d1.x)) {
                minX = v2d2.x;
            }
            else minX = v2d3.x;
        if ((v2d1.y < v2d2.y) && (v2d1.y < v2d3.y)) {
            minY = v2d1.y;
        }
        else
            if ((v2d2.y < v2d3.y) && (v2d2.y < v2d1.y)) {
                minY = v2d2.y;
            }
            else minY = v2d3.y;
    }
    void NormalVector() {
        vector3d _A(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
        vector3d _B(v1.x - v3.x, v1.y - v3.y, v1.z - v3.z);
        float x = _A.y * _B.z - _A.z * _B.y;
        float y = _A.z * _B.x - _A.x * _B.z;
        float z = _A.x * _B.y - _A.y * _B.x;
        float length = sqrt(x * x + y * y + z * z);
        x = x / length;
        y = y / length;
        z = z / length;
        Normal = vector3d(x, y, z);
        A = x;
        B = y;
        C = z;
        D = -(A * v1.x + B * v1.y + C * v1.z);
    }
    void Normalize() {
        RotateVectorOnZ(v1, Normal, vector3d(0, -1, 0));
        RotateVectorOnZ(v2, Normal, vector3d(0, -1, 0));
        RotateVectorOnZ(v3, Normal, vector3d(0, -1, 0));
        cout << outstr();
        RotateVectorOnX(v1, Normal, vector3d(0, -1, 0));
        RotateVectorOnX(v2, Normal, vector3d(0, -1, 0));
        RotateVectorOnX(v3, Normal, vector3d(0, -1, 0));
        cout << outstr();
        RelativeCoords(v1, v2, v3);
        //NormalizeVertex(v1, Normal, vector3d(0, 1, 0));
        //cout << outstr();
        //NormalizeVertex(v2, Normal, vector3d(0, 1, 0));
        //cout << outstr();
        //NormalizeVertex(v3, Normal, vector3d(0, 1, 0));
        cout << outstr();
    }
    vertex v1, v2, v3;
    vertex2d v2d1, v2d2, v2d3;
    vector3d Normal;
    Edge edge1, edge2, edge3;
    int minX = 0, minY = 0, maxX = 0, maxY = 0;
    float A, B, C, D;
};

void TransPoly(Polygon& a) {
    a.SetVertex2d(vertex2d(a.v1.x+100, 200-a.v1.z), vertex2d(a.v2.x+100, 200-a.v2.z), vertex2d(a.v3.x+100, 200-a.v3.z));
    a.edge1 = Edge(a.v2d2, a.v2d3);
    a.edge1.IsPositive(a.v2d1);
    a.edge2 = Edge(a.v2d1, a.v2d3);
    a.edge2.IsPositive(a.v2d2);
    a.edge3 = Edge(a.v2d1, a.v2d2);
    a.edge3.IsPositive(a.v2d3);
    a.SetExtremum();
}

class Camera {
public:
    Camera() {
        CameraPos = vertex(0, 0, 0);
        CameraVector = vector3d(0, 1, 0);
    }
    Camera(float X, float Y, float Z) {
        CameraPos = vertex(X, Y, Z);
        CameraVector = vector3d(0, 1, 0);
    }
    Camera(vertex _CameraVertex, vector3d _CameraVector) {
        CameraPos = _CameraVertex;
        CameraVector = _CameraVector;
    }
    vertex CameraPos;
    vector3d CameraVector;
private:
    float fov = 90;
};

vertex Crossing(straight s, Polygon poly, bool& HaveCrossing) {
    float temp = poly.Normal.x * s.P.x + poly.Normal.y * s.P.y + poly.Normal.z * s.P.z;
    if (temp == 0) {
        HaveCrossing = false;
        return vertex(0, 0, 0);
    }
    temp = -((poly.D + poly.A * s.M.x + poly.B * s.M.y + poly.C * s.M.z) / temp);
    HaveCrossing = true;
    return vertex(s.P.x * temp + s.M.x, s.P.y * temp + s.M.y, s.P.z * temp + s.M.z);
}

class Screen {
public:
    Screen(int w, int h) {
        WxH = w * h;
        for (int i = 0; i < WxH; i++) {
            Data.push_back(Point());
        }
        _width = w;
        _height = h;
    }
    void Fill(int r, int g, int b) {
        Point TempPoint(r, g, b);
        for (int i = 0; i < WxH; i++) {
            Data[i] = TempPoint;
        }
    }
    Point& operator[](const int index) {
        return Data[index];
    }
    int width() {
        return _width;
    }
    int height() {
        return _height;
    }
    int capacity() {
        return WxH;
    }
    string Str(int index) {
        return  Data[index].StrPoint();
    }
    void RayTrace(Polygon Poly, Camera MainCam) {
        float tempX, tempY, tempZ;
        bool HaveCrossing;
        for (int i=0; i<_height;i++)
            for (int j = 0; j < _width; j++) {
                tempX = (float)MainCam.CameraPos.x - float(_width / 2) + float(j);
                tempY = (float)MainCam.CameraPos.y + float(_width / 2);
                tempZ = (float)MainCam.CameraPos.z + float(_height / 2) - float(i);
                straight s1(MainCam.CameraPos, vertex(tempX, tempY, tempZ));
                vertex PointCrossing(Crossing(s1, Poly, HaveCrossing));

            }
    }
    void add(Polygon a) {
        bool gr1 = false, gr2 = false, gr3 = false;
        float func = 0;
        cout << "Min =" << a.minX << "," << a.minY << " Max = " <<a.maxX << "," << a.maxY << "\n";
        for (int i = a.minX; i <= a.maxX; i++) {
            for (int j = a.minY; j <= a.maxY; j++) {
                func = (i * a.edge1.a + j * a.edge1.b + a.edge1.c);
                if ((a.edge1.IsOpp) && (func >= 0)) gr1 = true;
                if (!(a.edge1.IsOpp) && !(func >= 0)) gr1 = true;
                //cout << "Point " << i << "," << j << " gr1 = " << gr1 << "\n";

                func = (i * a.edge2.a + j * a.edge2.b + a.edge2.c);
                if ((a.edge2.IsOpp) && (func >= 0)) gr2 = true;
                if (!(a.edge2.IsOpp) && !(func >= 0)) gr2 = true;
                //cout << "Point " << i << "," << j << " gr2 = " << gr1 << "\n";

                func = (i * a.edge3.a + j * a.edge3.b + a.edge3.c);
                if ((a.edge3.IsOpp) && (func >= 0)) gr3 = true;
                if (!(a.edge3.IsOpp) && !(func >= 0)) gr3 = true;
                //cout << "Point " << i << "," << j << " gr3 = " << gr3 << "\n";
                if (gr1 && gr2 && gr3 && (i<_width) && (j < _height)) {
                    Data[i + j * _width] = Point(i, 0, j);
                //    cout << "Point " << i << "," << j << "\n";
                }
                gr1 = gr2 = gr3 = false;

            }
        }
    }
    int _width, _height, WxH;
private:
    vector<Point> Data;
};

void FillImage(string path, Screen a) {
    ofstream file(path, ios::binary);
    const char bin0a = (char)10;
    const char bin20 = (char)32;
    if (file) {
        file << "P6" << bin0a << to_string(a.width()) << bin20 << to_string(a.height()) << bin0a << to_string(255) << bin0a;
        for (int i = 0; i < a.capacity(); i++) {
            file << a.Str(i);
        }
    }
    else { cout << "Error"; }
    file.close();
}



int main()
{
    Screen Cadr(width, height);
    Cadr.Fill(255, 255, 255);
    Polygon p1(vertex(-100, 100, 0), vertex(0, 0, 100), vertex(0, 100, 0));
    TransPoly(p1);
    Cadr.add(p1);
    FillImage(path, Cadr);
    cout << dec << p1.outstr() << p1.outstr2d();
    Camera Cam(vertex(100, -100, 100), vector3d(0, 1, 0));
    cout << p1.Normal.x << " " << p1.Normal.y << " " << p1.Normal.z << "\n";
    p1.Normalize();
}