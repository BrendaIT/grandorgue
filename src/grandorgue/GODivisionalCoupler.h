/*
* Copyright 2006 Milan Digital Audio LLC
* Copyright 2009-2021 GrandOrgue contributors (see AUTHORS)
* License GPL-2.0 or later (https://www.gnu.org/licenses/old-licenses/gpl-2.0.html).
*/

#ifndef GODIVISIONALCOUPLER_H
#define GODIVISIONALCOUPLER_H

#include "GODrawStop.h"
#include <vector>

class GODivisionalCoupler : public GODrawstop
{
private:
 	bool m_BiDirectionalCoupling;
	std::vector<unsigned> m_manuals;

	void ChangeState(bool on);
	void SetupCombinationState();

public:
	GODivisionalCoupler(GODefinitionFile* organfile);
	void Load(GOConfigReader& cfg, wxString group);

	unsigned GetNumberOfManuals();
	unsigned GetManual(unsigned index);
	bool IsBidirectional();

	wxString GetMidiType();
};

#endif /* GODIVISIONALCOUPLER_H */
