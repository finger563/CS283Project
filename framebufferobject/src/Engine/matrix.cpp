#include "matrix.h"

Matrix::Matrix(const Matrix& m) {
    for (int row=0;row<4;row++) {
        for (int col=0;col<4;col++) {
            data[row][col] = m.data[row][col];
        }
    }
}

void Matrix::Print(int x, int y) {
}

void Matrix::SetIdentity() {
    Clear();
    for (int row=0;row<4;row++) {
        data[row][row] = 1;
    }
}

void Matrix::Clear() {
    for (int row=0;row<4;row++) {
        for (int col=0;col<4;col++) {
            data[row][col] = 0;
        }
    }
}
   
Matrix Matrix::Transpose(void) const {
    Matrix m;
    for (int row=0;row<4;row++) {
        for (int col=0;col<4;col++) {
            m.data[row][col] = data[col][row];
        }
    }
    return m;
}

Matrix Matrix::operator -() const {
    Matrix m;
    for (int row=0;row<4;row++) {
        for (int col=0;col<4;col++) {
            m.data[row][col] = - data[row][col];
        }
    }
    return m;
}

Matrix& Matrix::operator= (const Matrix& rhs) {
    for (int row=0;row<4;row++) {
        for (int col=0;col<4;col++) {
            data[row][col] = rhs.data[row][col];
        }
    }
    return (*this);  
}
   
Matrix Matrix::operator* (const float rhs) const {
    Matrix m(*this);
    for (int row=0;row<4;row++) {
        for (int col=0;col<4;col++) {
            m.data[row][col] *= rhs;
        }
    }
    return m;
}
    
Matrix Matrix::operator/ (const float rhs) const {
    Matrix m(*this);
    for (int row=0;row<4;row++) {
        for (int col=0;col<4;col++) {
            m.data[row][col] /= rhs;
        }
    }
    return m;
}

Matrix Matrix::operator* (const Matrix& rhs) const {
    Matrix m;
    for (int row=0;row<4;row++) {
        for (int col=0;col<4;col++) {
            for (int i=0;i<4;i++) {
                m.data[row][col] += data[row][i] * rhs.data[i][col];
            }
        }
    }
    return m;
}

Matrix Matrix::operator+ (const Matrix& rhs) const {
    Matrix m(*this);
    for (int row=0;row<4;row++) {
        for (int col=0;col<4;col++) {
            m.data[row][col] += rhs.data[row][col];
        }
    }
    return m;
}

Matrix Matrix::operator- (const Matrix& rhs) const {
    Matrix m(*this);
    for (int row=0;row<4;row++) {
        for (int col=0;col<4;col++) {
            m.data[row][col] -= rhs.data[row][col];
        }
    }
    return m;
}

Vector3D Matrix::operator* (const Vector3D rhs) const {
    Vector3D v;
    v.x = rhs.x*data[0][0] + rhs.y*data[0][1] + rhs.z*data[0][2] + data[0][3];
    v.y = rhs.x*data[1][0] + rhs.y*data[1][1] + rhs.z*data[1][2] + data[1][3];
    v.z = rhs.x*data[2][0] + rhs.y*data[2][1] + rhs.z*data[2][2] + data[2][3];
    //v.w = rhs.x*data[3][0] + rhs.y*data[3][1] + rhs.z*data[3][2] + rhs.w*data[3][3];
    return v;
}
