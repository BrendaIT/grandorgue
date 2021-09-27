/*
* Copyright 2006 Milan Digital Audio LLC
* Copyright 2009-2019 GrandOrgue contributors (see AUTHORS)
* License GPL-2.0 or later (https://www.gnu.org/licenses/old-licenses/gpl-2.0.html).
*/

#ifndef GOSOUNDSIMPLESAMPLERLIST_H
#define GOSOUNDSIMPLESAMPLERLIST_H

#include "GOSoundSampler.h"
#include "atomic.h"

class GOSoundSimpleSamplerList
{
private:
	atomic<GO_SAMPLER*> m_List;
public:
	GOSoundSimpleSamplerList()
	{
		Clear();
	}

	void Clear()
	{
		m_List = 0;
	}

	GO_SAMPLER* Get()
	{
		do
		{
			GO_SAMPLER* sampler = m_List;
			if (!sampler)
				return NULL;
			GO_SAMPLER* next = sampler->next;
			if (m_List.compare_exchange(sampler, next))
				return sampler;
		}
		while(true);
	}

	void Put(GO_SAMPLER* sampler)
	{
		do
		{
			GO_SAMPLER* current = m_List;
			sampler->next = current;
			if (m_List.compare_exchange(current, sampler))
				return;
		}
		while(true);
	}
};

#endif
