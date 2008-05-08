///////////////////////////////////////////////////////////////////////////////
// $Workfile: Auto.h $
// $Archive: /WorkspaceWhiz/Src/Shared/Auto.h $
// $Date: 2003/01/05 $ $Revision: #4 $ $Author: Joshua $
///////////////////////////////////////////////////////////////////////////////
// This source file is part of the Workspace Whiz source distribution and
// is Copyright 1997-2003 by Joshua C. Jensen.  (http://workspacewhiz.com/)
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes so long as due credit is given and
// this header is left intact.
///////////////////////////////////////////////////////////////////////////////
#pragma once

template <class T>
class AutoBase
{
public:
    operator T*()									{  return m_autoObject;  }
    operator const T*() const						{  return m_autoObject;  }
    operator T*() const								{  return m_autoObject;  }
    T& operator*()									{  return *m_autoObject; }
    const T& operator*() const						{  return *m_autoObject; }
	T* Get() const									{  return m_autoObject;  }

protected:
    AutoBase() : m_autoObject(NULL) {}
    explicit AutoBase(T* newObj) : m_autoObject(newObj) {}

    T* m_autoObject;

private:
    AutoBase(const AutoBase<T>&);					// Not implemented.
    AutoBase<T>& operator=(const AutoBase&);		// Not implemented.
};


template <class T>
class AutoBaseClass : public AutoBase<T>
{
public:
    T* operator->()									{  return m_autoObject;  }
    const T* operator->() const						{  return m_autoObject;  }

protected:
	AutoBaseClass() { }
	explicit AutoBaseClass(T* newObj) : AutoBase<T>(newObj) { }

private:
    AutoBaseClass(const AutoBaseClass<T>&);					// Not implemented.
    AutoBaseClass<T>& operator=(const AutoBaseClass&);		// Not implemented.
};


template <class T>
class Auto : public AutoBaseClass<T>
{
public:
    Auto() : AutoBaseClass<T>() {}
    explicit Auto(T* newObj) : AutoBaseClass<T>(newObj) {}
    ~Auto()											{  delete m_autoObject;  }
	Auto<T>& operator=(const Auto<T>& newObj)		{  Attach(newObj);  return *this;  }
	Auto<T>& operator=(T* newObj)					{  Attach(newObj);  return *this;  }

    T* Detach()
	{
		T* retObj = m_autoObject;
		m_autoObject = NULL;
		return retObj;
	}

protected:
    void Attach(T* newObj)
	{
		if (newObj == m_autoObject)
			return;
		delete m_autoObject;
		m_autoObject = newObj;
	}
};


template <class T>
class AutoBasic : public AutoBase<T>
{
public:
    AutoBasic() : AutoBase<T>(NULL) {}
    explicit AutoBasic(T* newObj) : AutoBase<T>(newObj) {}
    ~AutoBasic()									{  delete m_autoObject;  }
	AutoBasic<T>& operator=(const AutoBasic<T>& newObj){  Attach(newObj);  return *this;  }
	AutoBasic<T>& operator=(T* newObj)				{  Attach(newObj);  return *this;  }

    T* Detach()
	{
		T* retObj = m_autoObject;
		m_autoObject = NULL;
		return retObj;
	}

protected:
    void Attach(T* newObj)
	{
		if (newObj == m_autoObject)
			return;
		delete m_autoObject;
		m_autoObject = newObj;
	}
};

