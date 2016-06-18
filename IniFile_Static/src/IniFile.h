#ifndef INI_FILE_H_H_
#define INI_FILE_H_H_


#include <string>
#include <list>
#include <sstream>
#pragma warning(disable:4786)


class CIniFile
{
public:
    struct Record
    {
        std::string Comments;
        char Commented;
        std::string Section;
        std::string Key;
        std::string Value;
    };

    enum CommentChar
    {
        Pound = '#',
        SemiColon = ';'
    };

	CIniFile(std::string FileName);
	~CIniFile(void);

	//获取所有的SectionName
    std::list<std::string> GetSectionNames();

    std::list<Record>      GetSection(std::string SectionName);
    std::list<Record>      GetRecord(std::string KeyName, std::string SectionName);
    std::string            GetValueString(std::string KeyName, std::string SectionName);
	int                    GetValueInt(std::string KeyName, std::string SectionName);
	int                    GetValueInt(std::string KeyName, std::string SectionName, int defaultValue);
    std::string            Content();

    bool SectionExists(std::string SectionName);
    bool RecordExists(std::string KeyName, std::string SectionName);

    bool AddSection(std::string SectionName);
	//string
    bool SetValueString(std::string KeyName, std::string Value, std::string SectionName);
	//int
	bool SetValueInt(std::string KeyName, int Value, std::string SectionName);

    bool DeleteSection(std::string SectionName);
    bool DeleteRecord(std::string KeyName, std::string SectionName);

    bool RenameSection(std::string OldSectionName, std::string NewSectionName);
    bool Sort(bool Descending);

    bool SetSectionComments(std::string Comments, std::string SectionName);
    bool SetRecordComments(std::string Comments, std::string KeyName, std::string SectionName);

	//注释整个作用域
    bool CommentSection(char CommentChar, std::string SectionName);
    bool CommentRecord(CommentChar cc, std::string KeyName, std::string SectionName);

    bool UnCommentSection(std::string SectionName);
    bool UnCommentRecord(std::string KeyName, std::string SectionName);

private:
	std::list<Record> GetSections();
	bool Load(std::string FileName, std::list<Record>& content);	
	bool Save(std::string FileName, std::list<Record>& content);
	bool CreateFile(std::string FileName);

	std::string _FileName;

	struct RecordSectionIs : std::unary_function<Record, bool>
	{
		std::string section_;

		RecordSectionIs(const std::string& section): section_(section){}

		bool operator()( const Record& rec ) const
		{
			return rec.Section == section_;
		}
	};

	struct RecordSectionKeyIs : std::unary_function<Record, bool>
	{
		std::string section_;
		std::string key_;

		RecordSectionKeyIs(const std::string& section, const std::string& key): section_(section),key_(key){}

		bool operator()( const Record& rec ) const
		{
			return ((rec.Section == section_)&&(rec.Key == key_));
		}
	};

	struct AscendingSectionSort
	{
		bool operator()(Record& Start, Record& End) const
		{
			return Start.Section < End.Section;
		}
	};

	struct DescendingSectionSort
	{
		bool operator()(Record& Start, Record& End) const
		{
			return Start.Section > End.Section;
		}
	};

	struct AscendingRecordSort
	{
		bool operator()(Record& Start, Record& End) const
		{
			return Start.Key < End.Key;
		}
	};

	struct DescendingRecordSort
	{
		bool operator()(Record& Start, Record& End) const
		{
			return Start.Key > End.Key;
		}
	};
};


#endif //INI_FILE_H_H_