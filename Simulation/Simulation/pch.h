#ifndef PCH_H
#define PCH_H
#define NOMINMAX

#include "framework.h"
#include <d3d9.h>
#include <d3dx9.h>
#include <dinput.h>

#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <functional>
#include <string>


#include "Define.h"
#include "Macro.h"
#include "Struct.h"

#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include "VectorHelper.h"
#include "MathHelper.h"
#include "DebugHelper.h"

#ifndef DBG_NEW 
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif
#endif

extern HWND g_hWnd;
extern HINSTANCE g_hInst;
using namespace std;

#endif //PCH_H
