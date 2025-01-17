/*
* Copyright 2006 Milan Digital Audio LLC
* Copyright 2009-2021 GrandOrgue contributors (see AUTHORS)
* License GPL-2.0 or later (https://www.gnu.org/licenses/old-licenses/gpl-2.0.html).
*/

#ifndef GOTIMER_H
#define GOTIMER_H

#include "GOTime.h"
#include "threading/GOMutex.h"
#include <wx/timer.h>
#include <vector>

class GOTimerCallback;

class GOTimer : private wxTimer
{
	typedef struct
	{
		GOTime time;
		GOTimerCallback* callback;
	        unsigned interval;
	} GOTimerEntry;
private:
	std::vector<GOTimerEntry> m_Entries;
	GOMutex m_Lock;

 	void Schedule();
	void Notify();

public:
	GOTimer();
	~GOTimer();

	void SetTimer(GOTime time, GOTimerCallback* callback, unsigned interval = 0);
	void SetRelativeTimer(GOTime time, GOTimerCallback* callback, unsigned interval = 0);
	void UpdateInterval(GOTimerCallback* callback, unsigned interval);
	void DeleteTimer(GOTimerCallback* callback);
};

#endif
