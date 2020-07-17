
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 5.03.0280 */
/* at Thu Nov 29 17:39:00 2001
 */
/* Compiler settings for E:\prov\prov.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32 (32b run), ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __prov_h__
#define __prov_h__

/* Forward Declarations */ 

#ifndef __IConnectionSource_FWD_DEFINED__
#define __IConnectionSource_FWD_DEFINED__
typedef interface IConnectionSource IConnectionSource;
#endif 	/* __IConnectionSource_FWD_DEFINED__ */


#ifndef __IIsRowset_FWD_DEFINED__
#define __IIsRowset_FWD_DEFINED__
typedef interface IIsRowset IIsRowset;
#endif 	/* __IIsRowset_FWD_DEFINED__ */


#ifndef __MySql_FWD_DEFINED__
#define __MySql_FWD_DEFINED__

#ifdef __cplusplus
typedef class MySql MySql;
#else
typedef struct MySql MySql;
#endif /* __cplusplus */

#endif 	/* __MySql_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

#ifndef __IConnectionSource_INTERFACE_DEFINED__
#define __IConnectionSource_INTERFACE_DEFINED__

/* interface IConnectionSource */
/* [full][local][uuid][object] */ 


EXTERN_C const IID IID_IConnectionSource;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("736d0580-addd-11d5-af08-00c026effb81")
    IConnectionSource : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetConnection( 
            /* [out] */ void __RPC_FAR *__RPC_FAR *ppConnection) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ReleaseConnection( 
            /* [in] */ void __RPC_FAR *pConnection) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IConnectionSourceVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IConnectionSource __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IConnectionSource __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IConnectionSource __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetConnection )( 
            IConnectionSource __RPC_FAR * This,
            /* [out] */ void __RPC_FAR *__RPC_FAR *ppConnection);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ReleaseConnection )( 
            IConnectionSource __RPC_FAR * This,
            /* [in] */ void __RPC_FAR *pConnection);
        
        END_INTERFACE
    } IConnectionSourceVtbl;

    interface IConnectionSource
    {
        CONST_VTBL struct IConnectionSourceVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IConnectionSource_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IConnectionSource_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IConnectionSource_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IConnectionSource_GetConnection(This,ppConnection)	\
    (This)->lpVtbl -> GetConnection(This,ppConnection)

#define IConnectionSource_ReleaseConnection(This,pConnection)	\
    (This)->lpVtbl -> ReleaseConnection(This,pConnection)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IConnectionSource_GetConnection_Proxy( 
    IConnectionSource __RPC_FAR * This,
    /* [out] */ void __RPC_FAR *__RPC_FAR *ppConnection);


void __RPC_STUB IConnectionSource_GetConnection_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IConnectionSource_ReleaseConnection_Proxy( 
    IConnectionSource __RPC_FAR * This,
    /* [in] */ void __RPC_FAR *pConnection);


void __RPC_STUB IConnectionSource_ReleaseConnection_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IConnectionSource_INTERFACE_DEFINED__ */


#ifndef __IIsRowset_INTERFACE_DEFINED__
#define __IIsRowset_INTERFACE_DEFINED__

/* interface IIsRowset */
/* [full][local][uuid][object] */ 


EXTERN_C const IID IID_IIsRowset;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("736d0580-addd-11d5-af08-00c026effb92")
    IIsRowset : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE IsRowset( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IIsRowsetVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IIsRowset __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IIsRowset __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IIsRowset __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *IsRowset )( 
            IIsRowset __RPC_FAR * This);
        
        END_INTERFACE
    } IIsRowsetVtbl;

    interface IIsRowset
    {
        CONST_VTBL struct IIsRowsetVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IIsRowset_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IIsRowset_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IIsRowset_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IIsRowset_IsRowset(This)	\
    (This)->lpVtbl -> IsRowset(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IIsRowset_IsRowset_Proxy( 
    IIsRowset __RPC_FAR * This);


void __RPC_STUB IIsRowset_IsRowset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IIsRowset_INTERFACE_DEFINED__ */



#ifndef __PROVLib_LIBRARY_DEFINED__
#define __PROVLib_LIBRARY_DEFINED__

/* library PROVLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_PROVLib;

EXTERN_C const CLSID CLSID_MySql;

#ifdef __cplusplus

class DECLSPEC_UUID("30D3C022-A927-11D5-AF08-00C026EFFB81")
MySql;
#endif
#endif /* __PROVLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


