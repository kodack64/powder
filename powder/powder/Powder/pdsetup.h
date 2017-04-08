#ifndef _POWDER_H_
#define _POWDER_H_

#include <string>
#include <algorithm>
#include <cfloat>
#include <climits>
#include <ctime>
#include <cmath>
#include <complex>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <utility>
#include <vector>

#include "../GL/freeglut.h"

#ifdef _MSC_VER
#include <conio.h>
#include <windows.h>
#include <crtdbg.h>
#endif _MSC_VER

#define PI (2*acos(0.0))
#define EPS (10E-8)

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if (p) { delete (p);     (p)=NULL; } }
#endif    
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if (p) { delete[] (p);   (p)=NULL; } }
#endif    

using namespace std;

#endif _POWDER_H_