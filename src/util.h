inline unsigned Hash(const char * key)
{
	int         len ;
	unsigned    hash ;
	int         i ;

	len = strlen(key);
	for (hash=0, i=0 ; i<len ; i++) {
		hash += (unsigned)key[i] ;
		hash += (hash<<10);
		hash ^= (hash>>6) ;
	}
	hash += (hash <<3);
	hash ^= (hash >>11);
	hash += (hash <<15);
	return hash ;
}

inline char* strtoupper(char* str_)
{
	char* tmp = str_;
	while( *str_ )
	{
		if( islower(*str_) )
		{
			*str_ = toupper(*str_);
		}
		++str_;
	}

	return tmp;
};

inline char* strtolower(char* str_)
{
	char* tmp = str_;

	while( *str_ )
	{
		if( isupper(*str_) )
		{
			*str_ = tolower(*str_);
		}
		++str_;
	}

	return tmp;
};