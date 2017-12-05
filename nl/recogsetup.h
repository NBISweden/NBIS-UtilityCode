#ifndef _RECOGSETUP_H_
#define _RECOGSETUP_H_



//=============================================================
//=============================================================
//=============================================================
class CBackupDict
{
public:
	CBackupDict();
	~CBackupDict();

	void LoadFromTextFile(const CMString & fileName);
	void Load(const CMString & fileName);
	void Save(const CMString & fileName);


	bool AddPronunciations(CMStringDictionary & newDict,
					       CMStringDictionary & newProns,
					       const CMString & grapheme);

private:
    CMStringDictionary m_backupGraphemes; 
	CMStringDictionary m_backupProns;

};


typedef TMPtrList<CBackupDict> CBackupDictList;
//=========================================================
//=========================================================
//=========================================================
class CRecognizerSetup
{
public:
	CRecognizerSetup();
	~CRecognizerSetup();

	//List of pron sources...
	void AddBackupDict(const CMString & fileName);
	void AddBackupDictAscii(const CMString & fileName);

	void SetDataFileName(const CMString & fileName);
	void SetDictFileName(const CMString & fileName);

	bool DumpOutFiles(CMGrammarBigramExtract & bigrams, CMPtrStringList & dict);
private:
	CMString m_dataFileName;
	CMString m_dictFileName;

	CBackupDictList m_backupDicts;
};




#endif //_RECOGSETUP_H_
