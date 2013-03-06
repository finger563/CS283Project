#ifndef Matrix_H
#define Matrix_H

#include <math.h>
#include "point.h"

class Matrix
{
public:
    float data[4][4];
    Matrix() { Clear(); }
    Matrix(const Matrix& m);
    ~Matrix(){}

    void Print(int x, int y);
    
    void SetIdentity();
    void Clear();    
    Matrix Transpose(void) const;
    
    Matrix operator -() const;
    Matrix& operator= (const Matrix& rhs);    
    Matrix operator* (const float rhs) const;    
    Matrix operator/ (const float rhs) const;
    Matrix operator* (const Matrix& rhs) const;
    Matrix operator+ (const Matrix& rhs) const;
    Matrix operator- (const Matrix& rhs) const;
    
    Vector operator* (const Vector rhs) const;
};

#endif