//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
USERES("prn2sword.res");
USEFORM("prn2swordfrm.cpp", Form2);
USEUNIT("..\src\keys\versekey.cpp");
USEUNIT("..\src\keys\strkey.cpp");
USEUNIT("..\src\keys\swkey.cpp");
USEUNIT("..\src\keys\listkey.cpp");
USEUNIT("..\src\utilfuns\utilstr.cpp");
USEUNIT("..\src\mgr\localemgr.cpp");
USEUNIT("..\src\mgr\swlocale.cpp");
USEUNIT("..\src\mgr\swconfig.cpp");
USEUNIT("..\src\mgr\swmgr.cpp");
USEUNIT("..\src\mgr\filemgr.cpp");
USEUNIT("..\src\utilfuns\roman.c");
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	try
	{
		Application->Initialize();
		Application->CreateForm(__classid(TForm2), &Form2);
                 Application->Run();
	}
	catch (Exception &exception)
	{
		Application->ShowException(&exception);
	}
	return 0;
}
//---------------------------------------------------------------------------
