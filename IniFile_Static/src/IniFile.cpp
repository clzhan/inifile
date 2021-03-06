﻿#include "IniFile.h"
#include <fstream>
#include <algorithm>
#include <sys/stat.h>

using namespace std;

template<class out_type, class in_value>
out_type convert(const in_value& t)
{
	out_type result;

	try
	{
		std::stringstream stream;
		stream << t;
		stream >> result;
		stream.clear();
	}
	catch (...) {}

	return result;
}

// replace all old_value with new_value in 'str'
string& replace_all(string& str, const string& old_value, const string& new_value)   
{   
	while (true)
	{   
		string::size_type pos(0);   
		if((pos = str.find(old_value)) != string::npos)   
			str.replace(pos,old_value.length(),new_value);   
		else
			break;   
	}   
	return str;   
}   

// A function to trim whitespace from both sides of a given string
void Trim(string& str, const string & ChrsToTrim = " \t\n\r", int TrimDir = 0)
{
    size_t startIndex = str.find_first_not_of(ChrsToTrim);
    if (startIndex == string::npos)
    {
        str.erase();
        return;
    }
	
    if (TrimDir < 2)
        str = str.substr(startIndex, str.size() - startIndex);
    if (TrimDir != 1)
        str = str.substr(0, str.find_last_not_of(ChrsToTrim) + 1);

	// Trim whitespace from both sides of '='
	if (str.at(0) != '#')
		str = replace_all(str, " ", "");
	if (str.at(0) != '#')
		str = replace_all(str, "\t", "");
}

//inline void TrimRight(string& str, const string & ChrsToTrim = " \t\n\r")
//{
//    Trim(str, ChrsToTrim, 2);
//}

//inline void TrimLeft(string& str, const string & ChrsToTrim = " \t\n\r")
//{
//    Trim(str, ChrsToTrim, 1);
//}

// A function to transform a string to uppercase if neccessary
//void UCase(string& str, bool ucase)
//{
//	if(ucase)
//		transform(str.begin(), str.end(), str.begin(), toupper);
//}

bool CIniFile::Load(string FileName, list<Record>& content)
{
    string s;																// Holds the current line from the ini file
    string CurrentSection;												    // Holds the current section name

    ifstream inFile(FileName.c_str());									    // Create an input filestream
    if (!inFile.is_open())
        return false;									                    // If the input file doesn't open, then return
    content.clear();														// Clear the content list

    string comments = "";													// A string to store comments in

    while (!getline(inFile, s).eof())										// Read until the end of the file
    {
        Trim(s);															// Trim whitespace from the ends
        if (!s.empty())														// Make sure its not a blank line
        {
            Record r;														// Define a new record

            if ((s[0] == '#') || (s[0] == ';'))								// Is this a commented line?
            {
                if ((s.find('[') == string::npos) 							// If there is no [ or =
                    && (s.find('=') == string::npos))						// Then it's a comment
                {
                    comments += s + '\n';									// Add the comment to the current comments string
                }
                else
                {
                    r.Commented = s[0];										// Save the comment character
                    s.erase(s.begin());										// Remove the comment for further processing
                    Trim(s);
                }// Remove any more whitespace
            }
            else
				r.Commented = ' ';										    // else mark it as not being a comment

            if (s.find('[') != string::npos)								// Is this line a section?
            {
                s.erase(s.begin());											// Erase the leading bracket
                s.erase(s.find(']'));										// Erase the trailing bracket
                r.Comments = comments;									    // Add the comments string (if any)
                comments = "";												// Clear the comments for re-use
                r.Section = s;												// Set the Section value
                r.Key = "";													// Set the Key value
                r.Value = "";												// Set the Value value
                CurrentSection = s;
            }

            if (s.find('=') != string::npos)								// Is this line a Key/Value?
            {
                r.Comments = comments;									    // Add the comments string (if any)
                comments = "";												// Clear the comments for re-use
                r.Section = CurrentSection;								    // Set the section to the current Section
                r.Key = s.substr(0, s.find('='));							// Set the Key value to everything before the = sign
                r.Value = s.substr(s.find('=') + 1);						// Set the Value to everything after the = sign
            }

            if (comments == "")											    // Don't add a record yet if its a comment line
                content.push_back(r);										// Add the record to content
        }
    }

    inFile.close();															// Close the file
    return true;
}


/*=========================================================================
CIniFile : The constructor
*========================================================================*/
CIniFile::CIniFile(std::string FileName)
{
	_FileName = FileName;

	struct stat stFileInfo;

	if (-1 == stat(_FileName.c_str(), &stFileInfo))
	{
		//文件不存在
		CreateFile(_FileName); //创建文件
	}

}

/*=========================================================================
CIniFile : The destructor
*========================================================================*/
CIniFile::~CIniFile()
{

}

bool CIniFile::CreateFile(std::string FileName)
{
	ofstream outFile(FileName.c_str());									    // Create an output filestream
	if (!outFile.is_open())
		return false;
	outFile.close();														// Close the file
	return true;

}

bool CIniFile::Save(string FileName, list<Record>& content)
{
    ofstream outFile(FileName.c_str());									    // Create an output filestream
    if (!outFile.is_open())
        return false;									                    // If the output file doesn't open, then return
	
	for (list<Record>::iterator iter = content.begin();
		iter != content.end(); ++iter)										// Loop through each list
    {
        outFile << iter->Comments;									    	// Write out the comments
		if (iter->Commented != ' ')
			outFile << iter->Commented;

		if(iter->Key == "")													// Is this a section?			
			outFile<< "[" << iter->Section << "]" << endl;					// Then format the section
		else
			outFile << iter->Key << "=" << iter->Value << endl;				// Else format a key/value
    }

    outFile.close();														// Close the file
    return true;
}



list<string> CIniFile::GetSectionNames()
{
    list<string> data;														// Holds the return data
    list<Record> content;												    // Holds the current record

    if (Load(_FileName, content))											// Make sure the file is loaded
    {
		for (list<Record>::iterator iter = content.begin();
			iter != content.end(); ++iter)									// Loop through the content
        {
            if (iter->Key == "")											// If there is no key value, then its a section
                data.push_back(iter->Section);						    	// Add the section to the return data
        }
    }

    return data;															// Return the data
}

list<CIniFile::Record> CIniFile::GetSections()
{
    list<Record> data;														// Holds the return data
    list<Record> content;												    // Holds the current record

	if (Load(_FileName, content))											// Make sure the file is loaded
    {
		for (list<Record>::iterator iter = content.begin();
			iter != content.end(); ++iter)									// Loop through the content
        {
            if (iter->Key == "")											// If this is a section 
                data.push_back(*iter);								    	// Add the record to the return data
        }
    }

    return data;															// Return the data
}

list<CIniFile::Record> CIniFile::GetSection(string SectionName)
{
    list<Record> data;														// Holds the return data
    list<Record> content;												    // Holds the current record

    if (Load(_FileName, content))											// Make sure the file is loaded
    {
		for (list<Record>::iterator iter = content.begin();
			iter != content.end(); ++iter)									// Loop through the content
        {
            if ((iter->Section == SectionName)					        	// If this is the section name we want
                && (iter->Key != ""))										// but not the section name itself
                data.push_back(*iter);								    	// Add the record to the return data
        }
    }

    return data;															// Return the data
}

list<CIniFile::Record> CIniFile::GetRecord(string KeyName, string SectionName)
{
    list<Record> data;														// Holds the return data
    list<Record> content;												    // Holds the current record

    if (Load(_FileName, content))											// Make sure the file is loaded
    {
        list<Record>::iterator iter = find_if(
			content.begin(), content.end(),
            CIniFile::RecordSectionKeyIs(SectionName, KeyName));		    // Locate the Record

        if (iter == content.end())
            return data;								                    // The Record was not found

        data.push_back(*iter);											    // The Record was found
    }
    return data;															// Return the Record
}

string CIniFile::GetValueString(std::string KeyName, std::string SectionName, std::string defaultValue)
{
	list<Record> record = GetRecord(KeyName, SectionName);	    // Get the Record

	if (!record.empty())													                   // Make sure there is a value to return
		return record.front().Value;										              // And return the value

	return defaultValue;																// No value was found, return default
}

string CIniFile::GetValueString(string KeyName, string SectionName)
{
    list<Record> record = GetRecord(KeyName, SectionName);	    // Get the Record

    if (!record.empty())													// Make sure there is a value to return
        return record.front().Value;										// And return the value

    return "";																// No value was found
}
int CIniFile::GetValueInt(std::string KeyName, std::string SectionName)
{
	list<Record> record = GetRecord(KeyName, SectionName);	                // Get the Record

	int num = -1;

	if (!record.empty())												    // Make sure there is a value to return
	{
		num = convert<int>(record.front().Value);
		return num;                                                         // And return the value
	}
			
	return -1;																// No value was found
}
int CIniFile::GetValueInt(std::string KeyName, std::string SectionName,int defaultValue)
{
	list<Record> record = GetRecord(KeyName, SectionName);	                // Get the Record

	int num = -1;

	if (!record.empty())												    // Make sure there is a value to return
	{
		num = convert<int>(record.front().Value);
		return num;                                                         // And return the value
	}

	return defaultValue;																// No value was found
}
float   CIniFile::GetValuefloat(std::string KeyName, std::string SectionName, float defaultValue)
{
	list<Record> record = GetRecord(KeyName, SectionName);	                // Get the Record

	float num = -1;

	if (!record.empty())												    // Make sure there is a value to return
	{
		num = convert<float>(record.front().Value);
		return num;                                                         // And return the value
	}

	return defaultValue;
}

string CIniFile::Content()
{
    string s = "";															// Hold our return string
    list<Record> content;												    // Holds the current record

    if (Load(_FileName, content))											// Make sure the file loads
    {
		for (list<Record>::iterator iter = content.begin();
			iter != content.end(); ++iter)									// Loop through the content
        {
            if (iter->Comments != "")
				s += iter->Comments;										// Add the comments
            if (iter->Commented != ' ')
				s += iter->Commented;										// If this is commented, then add it
            if ((iter->Key == ""))											// Is this a section?
                s += '[' + iter->Section + ']';								// Add the section
            else
                s += iter->Key + '=' + iter->Value;			    			// Or the Key value to the return srting

            if (iter != content.end())
                s += '\n';								                    // If this is not the last line, add a CrLf
        }
        
		return s;															// Return the contents
    }

    return "";
}

bool CIniFile::SectionExists(string SectionName)
{
    list<Record> content;												    // Holds the current record

    if (Load(_FileName, content))											// Make sure the file is loaded
    {
        list<Record>::iterator iter = find_if(
			content.begin(), content.end(),
            CIniFile::RecordSectionIs(SectionName));					    // Locate the Section

        if (iter == content.end())
            return false;							                        // The Section was not found
    }
    else
        return false;

    return true;															// The Section was found
}

bool CIniFile::RecordExists(string KeyName, string SectionName)
{
    list<Record> content;												    // Holds the current record

    if (Load(_FileName, content))											// Make sure the file is loaded
    {
        list<Record>::iterator iter = find_if(
			content.begin(), content.end(),
            CIniFile::RecordSectionKeyIs(SectionName, KeyName));	        // Locate the Section/Key

        if (iter == content.end())
            return false;							                        // The Section/Key was not found
    }
    else
        return false;

    return true;															// The Section/Key was found
}

bool CIniFile::AddSection(string SectionName)
{
    list<Record> content;												    // Holds the current record

    if (Load(_FileName, content))											// Make sure the file is loaded
    {
        if (!SectionExists(SectionName))
        {
 			Record s = {"", ' ', SectionName, "", ""};						// Define a new section
            content.push_back(s);											// Add the section
            
			return Save(_FileName, content);									// Save
        }
		else
        	return true;
    }

    return false;															// The file did not open
}
bool CIniFile::SetValuefloat(std::string KeyName, float Value, std::string SectionName)
{
	string str_num = convert<string>(Value);
	list<Record> content;												    // Holds the current record

	if (Load(_FileName, content))											// Make sure the file is loaded
	{
		if (!SectionExists(SectionName))						    // If the Section doesn't exist
		{
			Record s = { "", ' ', SectionName, "", "" };					// Define a new section
			Record r = { "", ' ', SectionName, KeyName, str_num };			// Define a new record

			content.push_back(s);											// Add the section
			content.push_back(r);											// Add the record

			return Save(_FileName, content);								    // Save
		}

		if (!RecordExists(KeyName, SectionName))				             // If the Key doesn't exist
		{
			Record r = { "", ' ', SectionName, KeyName, str_num };			// Define a new record

			list<Record>::reverse_iterator r_iter = find_if(
				content.rbegin(), content.rend(),
				CIniFile::RecordSectionIs(SectionName));					// Locate the Section
			list<Record>::iterator iter = r_iter.base();
			content.insert(iter, r);											// Add the record

			return Save(_FileName, content);								    // Save
		}

		list<Record>::iterator iter = find_if(
			content.begin(), content.end(),
			CIniFile::RecordSectionKeyIs(SectionName, KeyName));	        // Locate the Record

		iter->Value = str_num;												// Insert the correct value

		return Save(_FileName, content);									    // Save
	}

	return false;															// In the event the file does not load

}
bool CIniFile::SetValueInt(std::string KeyName, int Value, std::string SectionName)
{
	string str_num = convert<string>(Value);
	list<Record> content;												    // Holds the current record

	if (Load(_FileName, content))											// Make sure the file is loaded
	{
		if (!SectionExists(SectionName))						    // If the Section doesn't exist
		{
			Record s = { "", ' ', SectionName, "", "" };					// Define a new section
			Record r = { "", ' ', SectionName, KeyName, str_num };			// Define a new record

			content.push_back(s);											// Add the section
			content.push_back(r);											// Add the record

			return Save(_FileName, content);								    // Save
		}

		if (!RecordExists(KeyName, SectionName))				             // If the Key doesn't exist
		{
			Record r = { "", ' ', SectionName, KeyName, str_num };			// Define a new record

			list<Record>::reverse_iterator r_iter = find_if(
				content.rbegin(), content.rend(),
				CIniFile::RecordSectionIs(SectionName));					// Locate the Section
			list<Record>::iterator iter = r_iter.base();
			content.insert(iter, r);											// Add the record

			return Save(_FileName, content);								    // Save
		}

		list<Record>::iterator iter = find_if(
			content.begin(), content.end(),
			CIniFile::RecordSectionKeyIs(SectionName, KeyName));	        // Locate the Record

		iter->Value = str_num;												// Insert the correct value

		return Save(_FileName, content);									    // Save
	}

	return false;															// In the event the file does not load

}
bool CIniFile::SetValueString(string KeyName, string Value, string SectionName)
{
    list<Record> content;												    // Holds the current record


    if (Load(_FileName, content))											// Make sure the file is loaded
    {
        if (!SectionExists(SectionName))						    // If the Section doesn't exist
        {
            Record s = { "", ' ', SectionName, "", "" };					// Define a new section
            Record r = { "", ' ', SectionName, KeyName, Value };			// Define a new record

			content.push_back(s);											// Add the section
            content.push_back(r);											// Add the record
            
			return Save(_FileName, content);								    // Save
        }

        if (!RecordExists(KeyName, SectionName))				    // If the Key doesn't exist
        {
            Record r = { "", ' ', SectionName, KeyName, Value };			// Define a new record
           
			list<Record>::reverse_iterator r_iter = find_if(
				content.rbegin(), content.rend(),
                CIniFile::RecordSectionIs(SectionName));					// Locate the Section
			list<Record>::iterator iter =  r_iter.base();
            content.insert(iter,r);											// Add the record
			
			return Save(_FileName, content);								    // Save
        }

        list<Record>::iterator iter = find_if(
			content.begin(), content.end(),
            CIniFile::RecordSectionKeyIs(SectionName, KeyName));	        // Locate the Record

        iter->Value = Value;												// Insert the correct value
        
		return Save(_FileName, content);									    // Save
    }

    return false;															// In the event the file does not load
}

bool CIniFile::DeleteSection(string SectionName)
{
    list<Record> content;												    // Holds the current record

    if (Load(_FileName, content))											// Make sure the file is loaded
    {
		list<Record>::iterator new_end = remove_if(
			content.begin(), content.end(),
			CIniFile::RecordSectionIs(SectionName));
		content.erase(new_end, content.end());

		return Save(_FileName, content);									    // Save
    }

    return false;															// In the event the file does not load
}

bool CIniFile::DeleteRecord(string KeyName, string SectionName)
{
    list<Record> content;												    // Holds the current record

	if (Load(_FileName, content))											// Make sure the file is loaded
    {
        list<Record>::iterator iter = find_if(
			content.begin(), content.end(),
            CIniFile::RecordSectionKeyIs(SectionName, KeyName));	        // Locate the Section/Key

        if (iter == content.end())
            return false;							                        // The Section/Key was not found

        content.erase(iter);												// Remove the Record
        
		return Save(_FileName, content);									    // Save
    }

    return false;															// In the event the file does not load
}

bool CIniFile::RenameSection(string OldSectionName, string NewSectionName)
{
    list<Record> content;												    // Holds the current record

    if (!SectionExists(NewSectionName)                            // If the Section doesn't exist
        && Load(_FileName, content))                                         // Make sure the file is loaded
    {
        for (list<Record>::iterator iter = content.begin();
            iter != content.end(); ++iter)									// Loop through the records
        {
            if (iter->Section == OldSectionName)							// Is this the OldSectionName?
                iter->Section = NewSectionName;							    // Now its the NewSectionName
        }

		return Save(_FileName, content);									    // Save
    }

    return false;															// In the event the file does not load
}

bool CIniFile::Sort(bool Descending)
{
    list<CIniFile::Record> content;											// Used to hold the sorted content
	list<CIniFile::Record> sections = GetSections();			    // Get a list of Sections

    if (!sections.empty())													// Is there anything to process?
    {
        if (Descending)													    // Descending or Ascending?
            sections.sort(DescendingSectionSort());
        else																// Sort the Sections
            sections.sort(AscendingSectionSort());

        for (list<Record>::iterator iter = sections.begin();
			iter != sections.end(); ++iter)									// For each Section
        {
            content.push_back(*iter);										// Add the sorted Section to the content

			list<CIniFile::Record> records = GetSection(iter->Section); // Get a list of Records for this section

            if (Descending)												    // Descending or Ascending?
                records.sort(DescendingRecordSort());
            else															// Sort the Records
                records.sort(AscendingRecordSort());

            for (list<Record>::iterator it = records.begin();
				it != records.end(); ++it)									// For each Record
                content.push_back(*it);									    // Add the sorted Record to the content
        }
		return Save(_FileName, content);									    // Save
    }

    return false;															// There were no sections
}

bool CIniFile::SetSectionComments(string Comments, string SectionName)
{
    list<Record> content;												    // Holds the current record

    if (Load(_FileName, content))											// Make sure the file is loaded
    {
        for (list<Record>::iterator iter = content.begin();
		iter != content.end(); ++iter)										// Loop through the records
        {
            if ((iter->Section == SectionName) &&							// Is this the Section?
                (iter->Key == ""))											// And not a record
            {
                if (Comments.size() >= 2)									// Is there a comment?
                {
                    if (Comments.substr(Comments.size() - 2) != "\n")		// Does the string end in a newline?
                        Comments += "\n";									// If not, add one
                }

                iter->Comments = Comments;								    // Set the comments
                
				return Save(_FileName, content);							    // Save
            }
        }
    }

    return false;															// In the event the file does not load
}

bool CIniFile::SetRecordComments(string Comments, string KeyName, string SectionName)
{
    list<Record> content;												    // Holds the current record

	if (Load(_FileName, content))											// Make sure the file is loaded
    {
        list<Record>::iterator iter = find_if(
			content.begin(), content.end(),
            CIniFile::RecordSectionKeyIs(SectionName, KeyName));	        // Locate the Section/Key

        if (iter == content.end())
            return false;							                        // The Section/Key was not found

        if (Comments.size() >= 2)											// Is there a comment?
        {
            if (Comments.substr(Comments.size() - 2) != "\n")				// Does the string end in a newline?
                Comments += "\n";											// If not, add one
        }

        iter->Comments = Comments;										    // Set the comments
        
		return Save(_FileName, content);									    // Save
    }

    return false;															// In the event the file does not load
}

bool CIniFile::CommentSection(char CommentChar, string SectionName)
{
    list<Record> content;												    // Holds the current record

	if (Load(_FileName, content))											// Make sure the file is loaded
    {
        for (list<Record>::iterator iter = content.begin();
			iter != content.end(); ++iter)
        {
            if (iter->Section == SectionName)								// Is this the right section?
                iter->Commented = CommentChar;							    // Change the comment value
        }
		return Save(_FileName, content);									    // Save
    }

    return false;															// In the event the file does not load
}

bool CIniFile::CommentRecord(CommentChar cc, string KeyName, string SectionName)
{
    list<Record> content;												    // Holds the current record

	if (Load(_FileName, content))											// Make sure the file is loaded
    {
        list<Record>::iterator iter = find_if(
			content.begin(), content.end(),
            CIniFile::RecordSectionKeyIs(SectionName, KeyName));	        // Locate the Section/Key

        if (iter == content.end())
            return false;							                        // The Section/Key was not found

        iter->Commented = cc;										        // Change the Comment value
        
		return Save(_FileName, content);									    // Save
    }

    return false;															// In the event the file does not load
}

bool CIniFile::UnCommentSection(string SectionName)
{
    list<Record> content;												    // Holds the current record

	if (Load(_FileName, content))											// Make sure the file is loaded
    {
        for (list<Record>::iterator iter = content.begin();
			iter != content.end(); ++iter)
        {
            if (iter->Section == SectionName)								// Is this the right section?
                iter->Commented = ' ';										// Remove the comment value
        }
        
		return Save(_FileName, content);									    // Save
    }

    return false;															// In the event the file does not load
}

bool CIniFile::UnCommentRecord(string KeyName, string SectionName)
{
    list<Record> content;												    // Holds the current record

	if (Load(_FileName, content))											// Make sure the file is loaded
    {
        list<Record>::iterator iter = find_if(
			content.begin(), content.end(),
            CIniFile::RecordSectionKeyIs(SectionName, KeyName));	        // Locate the Section/Key

        if (iter == content.end())
            return false;							                        // The Section/Key was not found

        iter->Commented = ' ';												// Remove the Comment value
        
		return Save(_FileName, content);									    // Save
    }

    return false;															// In the event the file does not load
}
