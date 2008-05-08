typedef bool (__stdcall *PFNCALLBACKFUNC)(DWORD, DWORD);
typedef void * (__stdcall *PFNALLOCFUNC)(DWORD);
typedef bool (__stdcall *PFNDEALLOCFUNC)(void *);

struct _JCALG1_Info
{
	DWORD majorVer;
	DWORD minorVer;
	DWORD nFastSize;
	DWORD nSmallSize;
};


extern "C" DWORD _stdcall JCALG1_Compress( 
	void *Source, 
	DWORD Length, 
	void *Destination, 
	DWORD WindowSize, 
	PFNALLOCFUNC, 
	PFNDEALLOCFUNC, 
	PFNCALLBACKFUNC); 

extern "C" DWORD _stdcall JCALG1_Decompress_Fast( 
	void *Source, 
	void *Destination);

extern "C" DWORD _stdcall JCALG1_Decompress_Small( 
	void *Source, 
	void *Destination);

extern "C" DWORD _stdcall JCALG1_GetNeededBufferSize( 
	DWORD nSize); 

extern "C" DWORD _stdcall JCALG1_GetInfo( 
	_JCALG1_Info *JCALG1_Info); 

extern "C" DWORD _stdcall JCALG1_GetUncompressedSizeOfCompressedBlock(
	void *pBlock);

extern "C" DWORD _stdcall JCALG1_GetNeededBufferSize( 
	DWORD nSize); 











