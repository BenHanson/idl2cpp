const char g_events_template[] =
R"(
/***************************************************************
*  QueryInterface -- Determines if this component supports the
*  requested interface, places a pointer to that interface
*  in ppvObj if it is
*  available, and returns S_OK.  If not, sets ppvObj to nullptr
*  and returns E_NOINTERFACE.
***************************************************************/
STDMETHODIMP C$Name$Listener::QueryInterface(REFIID riid, void** ppvObj)
{
	if (riid == IID_IUnknown)
	{
		*ppvObj = static_cast<IUnknown*>(this);
	}
	else if (riid == IID_IDispatch)
	{
		*ppvObj = static_cast<IDispatch*>(this);
	}
	else if (riid == IID_$Name$)
	{
		*ppvObj = static_cast<IDispatch*>(this);
	}
	else
	{
		*ppvObj = nullptr;
		return E_NOINTERFACE;
	}

	static_cast<IUnknown*>(*ppvObj)->AddRef();
	return S_OK;
}

/*********************************************************
*  AddRef() -- In order to allow an object to delete itself
*  when it is no longer needed, it is necessary to maintain
*  a count of all references to this object. When a new
*  reference is created, this function
*  increments the count.
*********************************************************/
STDMETHODIMP_(ULONG) C$Name$Listener::AddRef()
{
	return InterlockedIncrement(&m_lRefCount);
}

/*****************************************************************
*  Release() -- When a reference to this object is removed, this
*  function decrements the reference count. If the
*  reference count is 0, then
*  this function deletes this object and returns 0.
*****************************************************************/
STDMETHODIMP_(ULONG) C$Name$Listener::Release()
{
	ULONG ul = InterlockedDecrement(&m_lRefCount);

	if (ul == 0)
		delete this;

	return ul;
}

/************************************************************
*   IDispatch Interface -- This interface allows this class
*   to be used as an automation server, allowing its functions
*   to be called by other COM objects.
************************************************************/

/**********************************************************
*   GetTypeInfoCount -- This function determines if the
*   class supports type
*   information interfaces or not. It places 1 in
*   iTInfo if the class supports
*   type information and 0 if it does not.
**********************************************************/
STDMETHODIMP C$Name$Listener::GetTypeInfoCount(UINT* iTInfo)
{
	*iTInfo = 0;
	return S_OK;
}

/******************************************************
*   GetTypeInfo -- Returns the type information for the
*   class. For classes
*   that do not support type information,
*   this function returns E_NOTIMPL;
******************************************************/
STDMETHODIMP C$Name$Listener::GetTypeInfo(UINT iTInfo, LCID lcid,
	ITypeInfo** ppTInfo)
{
	return E_NOTIMPL;
}

/*******************************************************
*   GetIDsOfNames -- Takes an array of strings and
*   returns an array of DISPIDs
*   that correspond to the methods or properties indicated.
*   If the name is not
*   recognized, returns DISP_E_UNKNOWNNAME.
*******************************************************/
STDMETHODIMP C$Name$Listener::GetIDsOfNames(REFIID riid,
	OLECHAR** rgszNames, UINT cNames, LCID lcid, DISPID* rgDispId)
{
	return E_NOTIMPL;
}

/**************************************************************
*   Invoke -- Takes a dispid and uses it to call
*   another of this class's
*   methods. Returns S_OK if the call was successful.
**************************************************************/
STDMETHODIMP C$Name$Listener::Invoke(DISPID dispIdMember, REFIID riid,
	LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult,
	EXCEPINFO* pExcepInfo, UINT* puArgErr)
{
	switch (dispIdMember)
	{
$Switch$}

	return S_OK;
}

/****************************************************************
*  AttachToSource -- This method attaches to an event source.
****************************************************************/
STDMETHODIMP C$Name$Listener::AttachToSource(IUnknown* pEventSource)
{
	HRESULT hr = S_OK;
	IConnectionPointContainer* pCPC = nullptr;

	hr = pEventSource->QueryInterface(IID_IConnectionPointContainer,
		(void**)&pCPC);

	if (SUCCEEDED(hr))
	{
		hr = pCPC->FindConnectionPoint(IID_$Name$,
			&m_pConnectionPoint);

		if (SUCCEEDED(hr))
		{
			hr = m_pConnectionPoint->Advise(this, &m_dwConnection);
		}

		pCPC->Release();
	}

	return hr;
}

/*****************************************************************
*  DetachFromSource -- This method detaches from an event source.
*****************************************************************/
STDMETHODIMP C$Name$Listener::DetachFromSource()
{
	HRESULT hr = S_OK;

	if (m_pConnectionPoint != nullptr)
	{
		m_pConnectionPoint->Unadvise(m_dwConnection);
		m_pConnectionPoint->Release();
		m_pConnectionPoint = nullptr;
	}

	return hr;
})";
