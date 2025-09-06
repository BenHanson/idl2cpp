#include "stdafx.h"

#include "output.h"
#include "structs.h"

#include <iostream>

void output_events_header(const data_t& data)
{
	std::cout << "#pragma once\n\n" << idl2cpp_comment() <<
		"namespace " << data._namespace.back() << "\n{";

	for (auto& [iface, functions] : data._interfaces)
	{
		if (iface._level > 1)
			continue;

		if (!data._filter.empty() && iface._name != data._filter)
			continue;

		if (data._events.contains(std::make_pair(iface._namespace, iface._name)))
		{
			std::cout << "\nconst IID IID_" << iface._name << " =\n" <<
				"{ 0x" << iface._uuid.substr(0, 8) <<
				", 0x" << iface._uuid.substr(9, 4) <<
				", 0x" << iface._uuid.substr(14, 4) <<
				", { 0x" << iface._uuid.substr(19, 2) <<
				", 0x" << iface._uuid.substr(21, 2) <<
				", 0x" << iface._uuid.substr(24, 2) <<
				", 0x" << iface._uuid.substr(26, 2) <<
				", 0x" << iface._uuid.substr(28, 2) <<
				", 0x" << iface._uuid.substr(30, 2) <<
				", 0x" << iface._uuid.substr(32, 2) <<
				", 0x" << iface._uuid.substr(34, 2) <<
				" } };\n" <<
				"\nclass C" << iface._name << "Listener : public IDispatch\n{\n"
				"public:\n"
				"\t// IUnknown Methods\n"
				"\tvirtual STDMETHODIMP QueryInterface(REFIID riid, "
				"void** ppvObj) override;\n"
				"\tvirtual STDMETHODIMP_(ULONG) AddRef() override;\n"
				"\tvirtual STDMETHODIMP_(ULONG) Release() override;\n"
				"\n\t// IDispatch Methods\n"
				"\tvirtual STDMETHODIMP GetTypeInfoCount(UINT* iTInfo) override;\n"
				"\tvirtual STDMETHODIMP GetTypeInfo(UINT iTInfo, LCID lcid, "
				"ITypeInfo** ppTInfo) override;\n"
				"\tvirtual STDMETHODIMP GetIDsOfNames(REFIID riid, "
				"OLECHAR** rgszNames, UINT cNames,\n"
				"\t\tLCID lcid, DISPID* rgDispId) override;\n"
				"\tvirtual STDMETHODIMP Invoke(DISPID dispIdMember, "
				"REFIID riid, LCID lcid,\n"
				"\t\tWORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult,\n"
				"\t\tEXCEPINFO* pExcepInfo, UINT* puArgErr) override;\n"
				"\n\t// Attach/Detach from event source\n"
				"\tSTDMETHODIMP AttachToSource(IUnknown* pEventSource);\n"
				"\tSTDMETHODIMP DetachFromSource();\n"
				"\nprotected:\n"
				"\tlong m_lRefCount = 0;\n"
				"\tIConnectionPoint* m_pConnectionPoint = nullptr;\n"
				"\tDWORD m_dwConnection = 0;\n"
				"};\n";
		}
	}

	std::cout << "}\n";
}
