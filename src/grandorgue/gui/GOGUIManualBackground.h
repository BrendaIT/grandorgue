/*
* Copyright 2006 Milan Digital Audio LLC
* Copyright 2009-2021 GrandOrgue contributors (see AUTHORS)
* License GPL-2.0 or later (https://www.gnu.org/licenses/old-licenses/gpl-2.0.html).
*/

#ifndef GOGUIMANUALBACKGROUND_H
#define GOGUIMANUALBACKGROUND_H

#include "GOGUIControl.h"

#include "GOBitmap.h"

class GOGUIManualBackground : public GOGUIControl
{
private:
	unsigned m_ManualNumber;
	wxRect m_VRect;
	GOBitmap m_VBackground;
	wxRect m_HRect;
	GOBitmap m_HBackground;

public:
	GOGUIManualBackground(GOGUIPanel* panel, unsigned manual_number);

	void Init(GOConfigReader& cfg, wxString group);
	void Load(GOConfigReader& cfg, wxString group);
	void Layout();

	void PrepareDraw(double scale, GOBitmap* background);
	void Draw(GODC& dc);
};

#endif
