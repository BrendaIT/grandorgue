/*
* Copyright 2006 Milan Digital Audio LLC
* Copyright 2009-2021 GrandOrgue contributors (see AUTHORS)
* License GPL-2.0 or later (https://www.gnu.org/licenses/old-licenses/gpl-2.0.html).
*/

#include "GOMidiOutPort.h"

#include "midi/GOMidi.h"
#include "midi/GOMidiEvent.h"
#include "midi/GOMidiMap.h"


GOMidiOutPort::GOMidiOutPort(GOMidi* midi, wxString prefix, wxString name) :
	GOMidiPort(midi, prefix, name),
	m_merger()
{
}

GOMidiOutPort::~GOMidiOutPort()
{
}

bool GOMidiOutPort::Open()
{
	m_merger.Clear();
	return m_IsActive;
}

void GOMidiOutPort::Send(const GOMidiEvent& e)
{
	if (!IsActive())
		return;
	if (GetID() == e.GetDevice() || e.GetDevice() == 0)
	{
		GOMidiEvent e1 = e;
		if (!m_merger.Process(e1))
			return;
		std::vector<std::vector<unsigned char>> msg;
		e1.ToMidi(msg, m_midi->GetMidiMap());
		for(unsigned i = 0; i < msg.size(); i++)
			SendData(msg[i]);
	}
}

const wxString GOMidiOutPort::GetPortName() const
{
	return wxT("GrandOrgue Output");
}
