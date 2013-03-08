#include "triangle.h"
#include "../main.h"

extern float z_buffer[SIZE_X];
extern short display_buffer[SIZE_X*SIZE_Y];

// Used for rotating the polygons in an object
void Triangle::Rotate( const Matrix& m ) {
    a = m*a;
    b = m*b;
    c = m*c;
    normal = m*normal;
}

void Triangle::Translate( const Vector3D& v) {
    a = a+v;
    b = b+v;
    c = c+v;
}

// Pass in the View to screen space transform
void Triangle::DrawWireframe ( const Matrix& m ) {
    if ( normal.z > 0 )
        return;
    //DRAW_Line ( s1.x, s1.y, s2.x, s2.y, color );
    //DRAW_Line ( s2.x, s2.y, s3.x, s3.y, color );
    //DRAW_Line ( s3.x, s3.y, s1.x, s1.y, color );
}

// Pass in the view to screen space transform
void Triangle::TransformToScreen ( const Matrix& m ) {
    s1 = a;
    s1.x = (s1.x)/s1.z; // d should be contained in m
    s1.y = (s1.y)/s1.z;
    s2 = b;
    s2.x = (s2.x)/s2.z;
    s2.y = (s2.y)/s2.z;
    s3 = c;
    s3.x = (s3.x)/s3.z;
    s3.y = (s3.y)/s3.z;

    // sort by y
    Point3D t1 = s1, t2 = s2, t3 = s3;
	Point2D tex1 = u, tex2 = v, tex3 = w;
	Point3D tmp;
	Point2D tex;
    if ( t1.y < s2.y ) {
        t1 = s2;
        t2 = s1;
		tex1 = v;
		tex2 = u;
    }
    if ( t1.y < s3.y ) {
        t3 = t1;
        t1 = s3;
		tex3 = tex1;
		tex1 = w;
    }
    if ( t2.y < t3.y ) {
        tmp = t2;
        t2 = t3;
        t3 = tmp;
		tex = tex2;
		tex2 = tex3;
		tex3 = tex;
    }
    s1 = t1;
    s2 = t2;
    s3 = t3;
	u1 = tex1;
	u2 = tex2;
	u3 = tex3;		// Sort the texture points along with polygon points

    s1 = m*s1;		// PERFORM THE TRANSFORMATION
    s2 = m*s2;
    s3 = m*s3;
    m12 = (s2.x-s1.x)/(s2.y-s1.y);   // inverse slope between t1 & t2
    m13 = (s3.x-s1.x)/(s3.y-s1.y);   // inverse slope between t1 & t3
    m23 = (s3.x-s2.x)/(s3.y-s2.y);   // inverse slope between t2 & t3

    s4.y = s2.y;
    s4.x = s1.x + (s2.y-s1.y)*m13;
	s4.z = 1/(-(s3.y*s3.z - s1.y*s1.z) + (s3.z - s1.z)*(s1.y + (s2.y - s1.y))) * ((s3.z-s1.z)*s1.y*s1.z - (s3.z*s3.y - s1.y*s1.z)*s1.z);

	dz12 = (s2.z-s1.z) / ((s2.z-s1.z)*s1.y*s1.z - (s2.y*s2.z-s1.y*s1.z)*s1.z);
	dz13 = (s3.z-s1.z) / ((s3.z-s1.z)*s1.y*s1.z - (s3.y*s3.z-s1.y*s1.z)*s1.z);
	dz14 = (s4.z-s1.z) / ((s4.z-s1.z)*s1.y*s1.z - (s4.y*s4.z-s1.y*s1.z)*s1.z);
	dz23 = (s3.z-s2.z) / ((s3.z-s2.z)*s2.y*s2.z - (s3.y*s3.z-s2.y*s2.z)*s2.z);
	dz43 = (s3.z-s4.z) / ((s3.z-s4.z)*s4.y*s4.z - (s3.y*s3.z-s4.y*s4.z)*s4.z);

    if ( abs(s1.y - s2.y) < EPSILON ) {   // Flat top
        if ( s1.x < s2.x ) {
            type = FLAT_TOP_RIGHT;
			sx = s1.x;
			ex = s2.x;
			sz = 1/s1.z;
			ez = 1/s2.z;
        }
        else {
			sx = s2.x;
			ex = s1.x;
			sz = 1/s2.z;
			ez = 1/s1.z;
            type = FLAT_TOP_LEFT;
        }
    }
    else {
		sx = s1.x;
		ex = s1.x;
		sz = 1/s1.z;
		ez = 1/s1.z;
        if ( abs(s2.y - s3.y) < EPSILON ) {   // Flat Bottom
            if ( s3.x < s2.x ) {
                type = FLAT_BOTTOM_RIGHT;
            }
            else {
                type = FLAT_BOTTOM_LEFT;
            }
        }
        else {      // Normal triangle
            if ( s2.x > s4.x ) {
                type = NORMAL_RIGHT;
            }
            else {
                type = NORMAL_LEFT;
            }
        }
    }
}

void Triangle::DrawFilled ( void ) {
    if ( normal.z > 0 )
        return;
	float zi;
    switch ( type ) {
    case FLAT_TOP_RIGHT:
        for (int y=s1.y;y>=s3.y;y--) {
			dzx = (ez-sz)/(ex - sx);
			zi = sz;
			for (int x = sx;x<=ex;x++) {
				if ( zi > z_buffer[ x + y*SIZE_X ] ) {
					z_buffer[x + y*SIZE_X] = zi;
					display_buffer[ x + y*SIZE_X ] = color;
				}
				zi += dzx;
            }
			sz -= dz13;
			ez -= dz23;
            sx -= m13;
            ex -= m23;
        }
        break;
    case FLAT_TOP_LEFT:
        for (int y=s1.y;y>=s3.y;y--) {
			dzx = (ez-sz)/(ex - sx);
			zi = sz;
			for (int x = sx;x<=ex;x++) {
				if ( zi > z_buffer[ x + y*SIZE_X ] ) {
					z_buffer[x + y*SIZE_X] = zi;
					display_buffer[ x + y*SIZE_X ] = color;
				}
				zi += dzx;
            }
			sz -= dz23;
			ez -= dz13;
            sx -= m23;
            ex -= m13;
        }
        break;
    case FLAT_BOTTOM_RIGHT:
        for (int y=s1.y;y>=s3.y;y--) {
			dzx = (ez-sz)/(ex - sx);
			zi = sz;
			for (int x = sx;x<=ex;x++) {
				if ( zi > z_buffer[ x + y*SIZE_X ] ) {
					z_buffer[x + y*SIZE_X] = zi;
					display_buffer[ x + y*SIZE_X ] = color;
				}
				zi += dzx;
            }
			sz -= dz13;
			ez -= dz12;
            sx -= m13;
            ex -= m12;
        }
        break;
    case FLAT_BOTTOM_LEFT:
        for (int y=s1.y;y>=s3.y;y--) {
			dzx = (ez-sz)/(ex - sx);
			zi = sz;
			for (int x = sx;x<=ex;x++) {
				if ( zi > z_buffer[ x + y*SIZE_X ] ) {
					z_buffer[x + y*SIZE_X] = zi;
					display_buffer[ x + y*SIZE_X ] = color;
				}
				zi += dzx;
            }
			sz -= dz12;
			ez -= dz13;
            sx -= m12;
            ex -= m13;
        }
        break;
    case NORMAL_RIGHT:
        for (int y=s1.y;y>=s2.y;y--) {
			dzx = (ez-sz)/(ex - sx);
			zi = sz;
			for (int x = sx;x<=ex;x++) {
				if ( zi > z_buffer[ x + y*SIZE_X ] ) {
					z_buffer[x + y*SIZE_X] = zi;
					display_buffer[ x + y*SIZE_X ] = color;
				}
				zi += dzx;
            }
			sz -= dz13;
			ez -= dz12;
            sx -= m13;
            ex -= m12;
        }
        sx = s4.x;
        ex = s2.x;
        for (int y=s4.y;y>=s3.y;y--) {
			dzx = (ez-sz)/(ex - sx);
			zi = sz;
			for (int x = sx;x<=ex;x++) {
				if ( zi > z_buffer[ x + y*SIZE_X ] ) {
					z_buffer[x + y*SIZE_X] = zi;
					display_buffer[ x + y*SIZE_X ] = color;
				}
				zi += dzx;
            }
			sz -= dz13;
			ez -= dz23;
            sx -= m13;
            ex -= m23;
        }
        break;
    case NORMAL_LEFT:
        for (int y=s1.y;y>=s2.y;y--) {
			dzx = (ez-sz)/(ex - sx);
			zi = sz;
			for (int x = sx;x<=ex;x++) {
				if ( zi > z_buffer[ x + y*SIZE_X ] ) {
					z_buffer[x + y*SIZE_X] = zi;
					display_buffer[ x + y*SIZE_X ] = color;
				}
				zi += dzx;
            }
			sz -= dz12;
			ez -= dz13;
            sx -= m12;
            ex -= m13;
        }
        sx = s2.x;
        ex = s4.x;
        for (int y=s4.y;y>=s3.y;y--) {
			dzx = (ez-sz)/(ex - sx);
			zi = sz;
			for (int x = sx;x<=ex;x++) {
				if ( zi > z_buffer[ x + y*SIZE_X ] ) {
					z_buffer[x + y*SIZE_X] = zi;
					display_buffer[ x + y*SIZE_X ] = color;
				}
				zi += dzx;
            }
			sz -= dz23;
			ez -= dz13;
            sx -= m23;
            ex -= m13;
        }
        break;
    default:
        break;
    }
}

// Pass in the Global to screen space transform
void Triangle::DrawFilledZbuffer ( const int y ) {
    if ( normal.z > 0 || 
		s1.y < y || s3.y > y )
        return;

    float dy = y - s1.y;	// dy is negative!
    float zi;
	
    switch ( type ) {
    case FLAT_TOP_RIGHT:
		//sz = 1/s1.z + (dy) * dz13;
		//ez = 1/s2.z + (dy) * dz23;
		sz -= dz13;			// because dy < 0, we decrement!
		ez -= dz23;

        sx = s1.x + (dy)*m13;
        ex = s2.x + (dy)*m23;
        break;
    case FLAT_TOP_LEFT:
		//sz = 1/s2.z + (dy) * dz23;
		//ez = 1/s1.z + (dy) * dz13;
		sz -= dz23;
		ez -= dz13;

        sx = s2.x + (dy)*m23;
        ex = s1.x + (dy)*m13;
        break;
    case FLAT_BOTTOM_RIGHT:
		//sz = 1/s1.z + (dy) * dz13;
		//ez = 1/s1.z + (dy) * dz12;
		sz -= dz13;
		ez -= dz12;

        sx = s1.x + (dy)*m13;
        ex = s1.x + (dy)*m12;
        break;
    case FLAT_BOTTOM_LEFT:
		//sz = 1/s1.z + (dy) * dz12;
		//ez = 1/s1.z + (dy) * dz13;
		sz -= dz12;
		ez -= dz13;

        sx = s1.x + (dy)*m12;
        ex = s1.x + (dy)*m13;
        break;
    case NORMAL_RIGHT:
        if ( s4.y <= y ) { 
			//sz = 1/s1.z + (dy) * dz14;
			//ez = 1/s1.z + (dy) * dz12;
			sz -= dz14;
			ez -= dz12;

            sx = s1.x + (dy)*m13;
            ex = s1.x + (dy)*m12;
        }
        else {
            dy = y - s4.y;
			//sz = 1/s4.z + (dy) * dz43;
			//ez = 1/s2.z + (dy) * dz23;
			sz -= dz43;
			ez -= dz23;

            sx = s4.x + (dy)*m13;
            ex = s2.x + (dy)*m23;
        }
        break;
    case NORMAL_LEFT:
        if ( s4.y <= y ) {
			//sz = 1/s1.z + (dy) * dz12;
			//ez = 1/s1.z + (dy) * dz14;
			sz -= dz12;
			ez -= dz14;

            sx = s1.x + (dy)*m12;
            ex = s1.x + (dy)*m13;
        }
        else {
            dy = y - s2.y;
			//sz = 1/s2.z + (dy) * dz23;
			//ez = 1/s4.z + (dy) * dz43;
			sz -= dz23;
			ez -= dz43;

            sx = s2.x + (dy)*m23;
            ex = s4.x + (dy)*m13;
        }
        break;
    default:
        break;
    }

	dzx =  (1/ez - 1/sz) / ((1/ez-1/sz)*ex/ez - (ex/ez - sx/sz)/ez);
	zi = sz;

    for (int x = sx;x<=ex;x++) {
        if ( zi > z_buffer[x] ) {
            z_buffer[x] = zi;
            display_buffer[ x + y*SIZE_X ] = color;
        }
        zi += dzx;
    }
}

// Pass in the Global to screen space transform
void Triangle::DrawTexturedZbuffer ( const int y ) {
    if ( normal.z > 0 || 
		s1.y < y || s3.y > y )
        return;

    float dy = y - s1.y;	// dy is negative!
	
    switch ( type ) {
    case FLAT_TOP_RIGHT:
		sz -= dz13;			// because dy < 0, we decrement
		ez -= dz23;

        sx = s1.x + (dy)*m13;
        ex = s2.x + (dy)*m23;

		stu = (u3.x - u1.x)*(y/sz - s1.y*s1.z)/(s3.y*s3.z - s1.y*s1.z) + u1.x;
		stv = (u3.y - u1.y)*(y/sz - s1.y*s1.z)/(s3.y*s3.z - s1.y*s1.z) + u1.y;
		etu = (u3.x - u2.x)*(y/ez - s2.y*s2.z)/(s3.y*s3.z - s2.y*s2.z) + u2.x;
		etv = (u3.y - u2.y)*(y/ez - s2.y*s2.z)/(s3.y*s3.z - s2.y*s2.z) + u2.y;
        break;
    case FLAT_TOP_LEFT:
		sz -= dz23;
		ez -= dz13;

        sx = s2.x + (dy)*m23;
        ex = s1.x + (dy)*m13;
		
		stu = (u3.x - u2.x)*(y/sz - s2.y*s2.z)/(s3.y*s3.z - s2.y*s2.z) + u2.x;
		stv = (u3.y - u2.y)*(y/sz - s2.y*s2.z)/(s3.y*s3.z - s2.y*s2.z) + u2.y;
		etu = (u3.x - u1.x)*(y/ez - s1.y*s1.z)/(s3.y*s3.z - s1.y*s1.z) + u1.x;
		etv = (u3.y - u1.y)*(y/ez - s1.y*s1.z)/(s3.y*s3.z - s1.y*s1.z) + u1.y;
        break;
    case FLAT_BOTTOM_RIGHT:
		sz -= dz13;
		ez -= dz12;

        sx = s1.x + (dy)*m13;
        ex = s1.x + (dy)*m12;

		stu = (u3.x - u1.x)*(y/sz - s1.y*s1.z)/(s3.y*s3.z - s1.y*s1.z) + u1.x;
		stv = (u3.y - u1.y)*(y/sz - s1.y*s1.z)/(s3.y*s3.z - s1.y*s1.z) + u1.y;
		etu = (u2.x - u1.x)*(y/ez - s1.y*s1.z)/(s2.y*s2.z - s1.y*s1.z) + u1.x;
		etv = (u2.y - u1.y)*(y/ez - s1.y*s1.z)/(s2.y*s2.z - s1.y*s1.z) + u1.y;
        break;
    case FLAT_BOTTOM_LEFT:
		sz -= dz12;
		ez -= dz13;

        sx = s1.x + (dy)*m12;
        ex = s1.x + (dy)*m13;
		
		stu = (u2.x - u1.x)*(y/sz - s1.y*s1.z)/(s2.y*s2.z - s1.y*s1.z) + u1.x;
		stv = (u2.y - u1.y)*(y/sz - s1.y*s1.z)/(s2.y*s2.z - s1.y*s1.z) + u1.y;
		etu = (u3.x - u1.x)*(y/ez - s1.y*s1.z)/(s3.y*s3.z - s1.y*s1.z) + u1.x;
		etv = (u3.y - u1.y)*(y/ez - s1.y*s1.z)/(s3.y*s3.z - s1.y*s1.z) + u1.y;
        break;
    case NORMAL_RIGHT:
        if ( s4.y <= y ) { 
			sz -= dz14;
			ez -= dz12;

            sx = s1.x + (dy)*m13;
            ex = s1.x + (dy)*m12;

			stu = (u3.x - u1.x)*(y/sz - s1.y*s1.z)/(s3.y*s3.z - s1.y*s1.z) + u1.x;
			stv = (u3.y - u1.y)*(y/sz - s1.y*s1.z)/(s3.y*s3.z - s1.y*s1.z) + u1.y;
			etu = (u2.x - u1.x)*(y/ez - s1.y*s1.z)/(s2.y*s2.z - s1.y*s1.z) + u1.x;
			etv = (u2.y - u1.y)*(y/ez - s1.y*s1.z)/(s2.y*s2.z - s1.y*s1.z) + u1.y;
        }
        else {
            dy = y - s4.y;
			sz -= dz43;
			ez -= dz23;

            sx = s4.x + (dy)*m13;
            ex = s2.x + (dy)*m23;

			stu = (u3.x - u1.x)*(y/sz - s1.y*s1.z)/(s3.y*s3.z - s1.y*s1.z) + u1.x;
			stv = (u3.y - u1.y)*(y/sz - s1.y*s1.z)/(s3.y*s3.z - s1.y*s1.z) + u1.y;
			etu = (u3.x - u2.x)*(y/ez - s2.y*s2.z)/(s3.y*s3.z - s2.y*s2.z) + u2.x;
			etv = (u3.y - u2.y)*(y/ez - s2.y*s2.z)/(s3.y*s3.z - s2.y*s2.z) + u2.y;
        }
        break;
    case NORMAL_LEFT:
        if ( s4.y <= y ) {
			sz -= dz12;
			ez -= dz14;

            sx = s1.x + (dy)*m12;
            ex = s1.x + (dy)*m13;
			
			stu = (u2.x - u1.x)*(y/sz - s1.y*s1.z)/(s2.y*s2.z - s1.y*s1.z) + u1.x;
			stv = (u2.y - u1.y)*(y/sz - s1.y*s1.z)/(s2.y*s2.z - s1.y*s1.z) + u1.y;
			etu = (u3.x - u1.x)*(y/ez - s1.y*s1.z)/(s3.y*s3.z - s1.y*s1.z) + u1.x;
			etv = (u3.y - u1.y)*(y/ez - s1.y*s1.z)/(s3.y*s3.z - s1.y*s1.z) + u1.y;
        }
        else {
            dy = y - s2.y;
			sz -= dz23;
			ez -= dz43;

            sx = s2.x + (dy)*m23;
            ex = s4.x + (dy)*m13;
			
			stu = (u3.x - u2.x)*(y/sz - s2.y*s2.z)/(s3.y*s3.z - s2.y*s2.z) + u2.x;
			stv = (u3.y - u2.y)*(y/sz - s2.y*s2.z)/(s3.y*s3.z - s2.y*s2.z) + u2.y;
			etu = (u3.x - u1.x)*(y/ez - s1.y*s1.z)/(s3.y*s3.z - s1.y*s1.z) + u1.x;
			etv = (u3.y - u1.y)*(y/ez - s1.y*s1.z)/(s3.y*s3.z - s1.y*s1.z) + u1.y;
        }
        break;
    default:
        break;
    }
	
	dzx =  (1/ez - 1/sz) / ((1/ez-1/sz)*ex/ez - (ex/ez - sx/sz)/ez);
    float zi = sz;

	float tx=stu,ty=stv;
	float uscale,vscale;
	uscale = (etu-stu)/(ex/ez - sx/sz);
	vscale = (etv-stv)/(ex/ez - sx/sz);

    for (int x = sx;x<=ex;x++) {
        if ( zi > z_buffer[x] ) {
            z_buffer[x] = zi;
			tx = (x/zi - sx/sz) * uscale + stu;
			ty = (x/zi - sx/sz) * vscale + stv;
			int index = (int)tx + texwidth*((int)ty);
			if ( index < 0 ) 
				index = 0;
			display_buffer[ x + y*SIZE_X ] = texture[index%(texwidth*texwidth)];
        }
        zi += dzx;		// because dx > 0, we increment
    }
}