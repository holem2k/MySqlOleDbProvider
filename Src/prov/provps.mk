
provps.dll: dlldata.obj prov_p.obj prov_i.obj
	link /dll /out:provps.dll /def:provps.def /entry:DllMain dlldata.obj prov_p.obj prov_i.obj \
		kernel32.lib rpcndr.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib \

.c.obj:
	cl /c /Ox /DWIN32 /D_WIN32_WINNT=0x0400 /DREGISTER_PROXY_DLL \
		$<

clean:
	@del provps.dll
	@del provps.lib
	@del provps.exp
	@del dlldata.obj
	@del prov_p.obj
	@del prov_i.obj
