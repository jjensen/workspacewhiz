///////////////////////////////////////////////////////////////////////////////
// $Workfile: Timer.h $
// $Archive: /WorkspaceWhiz/Src/Shared/Timer.h $
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

class CTimer
{
public:
	CTimer(void);

	void Reset(void);
	void Start(void);
	void Stop(void);
	double GetDuration(void) const;
	double GetMillisecs(void) const;

protected:
	LARGE_INTEGER m_startTime;
	LONGLONG m_totalTime;

	LONGLONG m_frequency;
};

inline CTimer::CTimer(void) :
	m_totalTime(0)
{
	LARGE_INTEGER liFrequency;

	QueryPerformanceFrequency(&liFrequency);
	m_frequency = liFrequency.QuadPart;

	m_totalTime = 0;
}

inline void CTimer::Reset()
{
	m_totalTime = 0;
}
	
inline void CTimer::Start(void)
{
	QueryPerformanceCounter(&m_startTime);
}

inline void CTimer::Stop(void)
{
	LARGE_INTEGER stopTime;
	QueryPerformanceCounter(&stopTime);
	m_totalTime += (stopTime.QuadPart - m_startTime.QuadPart);
}

inline double CTimer::GetDuration(void) const
{
	return (double)m_totalTime * 1000000.0 / m_frequency;
}

inline double CTimer::GetMillisecs(void) const
{
	return (double)m_totalTime * 1000000.0 / m_frequency / 1000.0;
}
