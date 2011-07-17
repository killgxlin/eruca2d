#ifndef CFG_FILE
#define CFG_FILE

class CfgFile
{
	#define MAX_PARTS 10
	#define LEN_PARTS 60
	#define MAX_LINE 600
public:
	CfgFile()
		:_fp(0)
	{
	}
	~CfgFile()
	{
	}

	bool	LoadFile(const char* name_)
	{
		_fp = fopen(name_,"r");
		if(0 == _fp) return false;

		return true;
	}

	bool	LoadLine()
	{
		char *str;
		while( (str = fgets(_line, MAX_LINE, _fp)) && str != 0 && *str == ';' );
		
		if( 0 == str || *str == EOF ) return false;

		int len = 0, i = 0;

		str = _line;
		
		str += strspn(str," \t");
		
		while (*str)
		{
			int len = (int)(strcspn(str," \t"));
			memcpy(_parts[i],str,len);
			_parts[i][len] = 0;
			str += len + strspn(str+len," \t");
			i++; 
		}

		/* strip trailing CR as on Unix ifs.getline() doesn't do that. */
		if (_parts[i-1][strlen(_parts[i-1])-1] == '\n')
			_parts[i-1][strlen(_parts[i-1])-1] = 0;
		for(; i<MAX_PARTS; ++i)
			_parts[i][0] = 0;
			
		return true;
	}

	const char* GetPart(int idx_)
	{
		if( idx_ >= MAX_PARTS || idx_ < 0 ) return 0;
		if( _parts[idx_][0] == 0 ) return 0;

		return _parts[idx_];
	}

private:

	char	_parts[MAX_PARTS][LEN_PARTS];
	char	_line[MAX_LINE];
	FILE*	_fp;
};


//CfgFile file("filename");
//while( file.LoadLine() )
//{
//	int n = file.TotalParts();
//	const char* pPart = file.GetPart(0);
//}



#endif