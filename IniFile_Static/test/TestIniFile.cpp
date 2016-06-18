// TestCIniFile::cpp : Defines the entry point for the console application.
//
#include "../src/IniFile.h"
#include <iostream>

#include "vld.h"

using namespace std;


int Test()
{
	string FileName = "test2.ini";

	CIniFile inifile(FileName);

	inifile.SetValueInt("Port1", 11, "clzhan");
	inifile.SetValueInt("Port1", 11, "clzhan2");
	inifile.SetValueInt("Port1", 11, "clzhan3");
	inifile.SetValueInt("Port1", 11, "clzhan4");
	inifile.SetValueInt("Port1", 11, "clzhan5");
	//inifile.SetValueInt("Port2", 11, "clzhan");
	//inifile.SetValueInt("Port3", 11, "clzhan");
	//inifile.SetValueInt("Port4", 11, "clzhan");

	list<string> sections;

	sections = inifile.GetSectionNames();

	for (list<string>::iterator iter = sections.begin();
		iter != sections.end(); ++iter)									
	{
		printf("sections %s\n", iter->c_str());
	}

	inifile.CommentSection(CIniFile::SemiColon, "clzhan");

	printf("port1 = %d\n", inifile.GetValueInt("Port1", "clzhan1"));
	return 0;
}



int main(int argc, char* argv[])
{
    //string str_num = "555";
    //cout << convert<int>(str_num) << endl;

    //float f_num = 666.0f;
    //cout << convert<string>(f_num) << endl;

    //system("PAUSE");
    //system("cls");

	Test();

	return 0;
}