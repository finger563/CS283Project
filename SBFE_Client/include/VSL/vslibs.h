/** ----------------------------------------------------------
 * Very Simple Libraries
 *
 * Lighthouse3D
 *
 *
 * Full documentation at 
 * http://www.lighthouse3d.com/very-simple-libs
 *
 *
 * These libs requires:
 *	GLEW (http://glew.sourceforge.net/)
 *	Assimp (http://assimp.sourceforge.net/)
 *	Devil (http://openil.sourceforge.net/)
 *	TinyXML (http://www.grinninglizard.com/tinyxml/)
 *
 ---------------------------------------------------------------*/



#ifdef _WIN32

#ifdef _DEBUG
#pragma comment(lib,"vsld.lib")
#else
#pragma comment(lib,"vsl.lib")
#endif

#pragma comment(lib,"glew32.lib")
#endif

#include "vsShaderLib.h"
#include "vsMathLib.h"
#include "vsFontLib.h"
#include "vsLogLib.h"
#include "vsGLInfoLib.h"
#include "vsResourceLib.h"
#include "vsResModelLib.h"
