/*
* Copyright 2006 Milan Digital Audio LLC
* Copyright 2009-2019 GrandOrgue contributors (see AUTHORS)
* License GPL-2.0 (https://www.gnu.org/licenses/old-licenses/gpl-2.0.html).
*/

#ifndef GORGUESOUNDSCHEDULER_H
#define GORGUESOUNDSCHEDULER_H

#include "atomic.h"
#include "mutex.h"
#include <vector>

class GOSoundWorkItem;

class GOSoundScheduler
{
private:
	std::vector<GOSoundWorkItem*> m_Work;
	std::vector<GOSoundWorkItem**> m_WorkItems;
	atomic_uint m_NextItem;
	unsigned m_ItemCount;
	unsigned m_RepeatCount;
	GOMutex m_Mutex;

	void Lock();
	void Unlock();
	void Update();

	bool CompareItem(GOSoundWorkItem* a, GOSoundWorkItem* b);
	void SortList(std::vector<GOSoundWorkItem*>& list);
	void ResetList(std::vector<GOSoundWorkItem*>& list);
	void AddList(GOSoundWorkItem* item, std::vector<GOSoundWorkItem*>& list);
	void RemoveList(GOSoundWorkItem* item, std::vector<GOSoundWorkItem*>& list);
	void ExecList(std::vector<GOSoundWorkItem*>& list);

public:
	GOSoundScheduler();
	~GOSoundScheduler();

	void SetRepeatCount(unsigned count);

	void Clear();
	void Reset();
	void Exec();
	void Add(GOSoundWorkItem* item);
	void Remove(GOSoundWorkItem* item);

	GOSoundWorkItem* GetNextGroup();
};

#endif
