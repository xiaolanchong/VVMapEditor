//======================= Copyright © 2004, Eugene V. Gorbachev =================
//
// Purpose: 
//
//=============================================================================

#ifndef CRTMALLOC_H
#define CRTMALLOC_H
#ifdef _WIN32
#pragma once
#endif

class CrtAlloc : public IMalloc
{
    STDMETHOD (QueryInterface)(REFIID riid, void **ppvObj)
	{
	    if (IsEqualIID(riid, IID_IMalloc)	|| 
			IsEqualIID(riid, IID_IUnknown) )
	    {
	        *ppvObj=this;
	        return S_OK;
	    }
		else
		{
			*ppvObj = NULL;
			return E_NOINTERFACE;
		}
	}
	
    STDMETHOD_(ULONG, AddRef)() { return 2;};
    STDMETHOD_(ULONG, Release)() { return 1;};
	virtual SIZE_T STDMETHODCALLTYPE GetSize(  void *pv)  { return 0;}
        
	virtual int STDMETHODCALLTYPE DidAlloc( void *pv) { return 0;}
        
	virtual void STDMETHODCALLTYPE HeapMinimize( void) {}

	
	virtual void* STDMETHODCALLTYPE	Alloc( ULONG uSize )	
	{
		return malloc( uSize );
	}
	virtual   void* STDMETHODCALLTYPE	Realloc( void* pOldBuffer, ULONG uNewSize)
	{
		return realloc( pOldBuffer, uNewSize);
	}
	virtual   void STDMETHODCALLTYPE	Free( void* pBuffer )
	{
		free(pBuffer);
	}
};


#endif // CRTMALLOC_H
