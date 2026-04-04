/**
* Copyright (C) 2026 Elisha Riedlinger
*
* This software is  provided 'as-is', without any express  or implied  warranty. In no event will the
* authors be held liable for any damages arising from the use of this software.
* Permission  is granted  to anyone  to use  this software  for  any  purpose,  including  commercial
* applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*   1. The origin of this software must not be misrepresented; you must not claim that you  wrote the
*      original  software. If you use this  software  in a product, an  acknowledgment in the product
*      documentation would be appreciated but is not required.
*   2. Altered source versions must  be plainly  marked as such, and  must not be  misrepresented  as
*      being the original software.
*   3. This notice may not be removed or altered from any source distribution.
*/

#include "dinputto8.h"

IFACEMETHODIMP ClassFactoryBase::QueryInterface(REFIID riid, LPVOID FAR * ppvObj)
{
	if (ppvObj == nullptr)
	{
		return E_POINTER;
	}

	if (riid == IID_IUnknown || riid == IID_IClassFactory)
	{
		*ppvObj = static_cast<IClassFactory*>(this);
	}
	else
	{
		*ppvObj = nullptr;
		return E_NOINTERFACE;
	}

	AddRef();
	return S_OK;
}

IFACEMETHODIMP_(ULONG) ClassFactoryBase::AddRef()
{
	return _InterlockedIncrement(&m_refCount);
}

IFACEMETHODIMP_(ULONG) ClassFactoryBase::Release()
{
	const ULONG ref = _InterlockedDecrement(&m_refCount);
	if (ref == 0)
	{
		delete this;
	}
	return ref;
}

IFACEMETHODIMP ClassFactoryBase::LockServer(BOOL fLock)
{
	if (fLock)
	{
		ModuleObjectCount::IncrementObjectCount();
	}
	else
	{
		ModuleObjectCount::DecrementObjectCount();
	}
	return S_OK;
}
