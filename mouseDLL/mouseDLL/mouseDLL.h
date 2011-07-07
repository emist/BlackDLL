// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the MOUSEDLL_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// MOUSEDLL_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef MOUSEDLL_EXPORTS
#define MOUSEDLL_API __declspec(dllexport)
#else
#define MOUSEDLL_API __declspec(dllimport)
#endif

// This class is exported from the mouseDLL.dll
class MOUSEDLL_API CmouseDLL {
public:
	CmouseDLL(void);
	// TODO: add your methods here.
};

extern MOUSEDLL_API int nmouseDLL;

MOUSEDLL_API int fnmouseDLL(void);
