/*
* Copyright 2006 Milan Digital Audio LLC
* Copyright 2009-2021 GrandOrgue contributors (see AUTHORS)
* License GPL-2.0 or later (https://www.gnu.org/licenses/old-licenses/gpl-2.0.html).
*/

#include "GOMidiSender.h"

#include "config/GOConfigReader.h"
#include "config/GOConfigWriter.h"
#include "midi/GOMidiEvent.h"
#include "midi/GOMidiMap.h"
#include "GODefinitionFile.h"
#include <wx/intl.h>

GOMidiSender::GOMidiSender(GODefinitionFile* organfile, MIDI_SENDER_TYPE type) :
	GOMidiSenderData(type),
	m_organfile(organfile),
	m_ElementID(-1)
{
}

GOMidiSender::~GOMidiSender()
{
}

const struct IniFileEnumEntry GOMidiSender::m_MidiTypes[] = {
	{ wxT("Note"), MIDI_S_NOTE },
	{ wxT("NoteNoVelocity"), MIDI_S_NOTE_NO_VELOCITY },
	{ wxT("ControlChange"), MIDI_S_CTRL },
	{ wxT("RPN"), MIDI_S_RPN },
	{ wxT("NRPN"), MIDI_S_NRPN },
	{ wxT("ProgramOn"), MIDI_S_PGM_ON },
	{ wxT("ProgramOff"), MIDI_S_PGM_OFF },
	{ wxT("ProgramRange"), MIDI_S_PGM_RANGE },
	{ wxT("RPNRange"), MIDI_S_RPN_RANGE },
	{ wxT("NRPNRange"), MIDI_S_NRPN_RANGE },
	{ wxT("NoteOn"), MIDI_S_NOTE_ON },
	{ wxT("NoteOff"), MIDI_S_NOTE_OFF },
	{ wxT("ControlOn"), MIDI_S_CTRL_ON },
	{ wxT("ControlOff"), MIDI_S_CTRL_OFF },
	{ wxT("RPNOn"), MIDI_S_RPN_ON },
	{ wxT("RPNOff"), MIDI_S_RPN_OFF },
	{ wxT("NRPNOn"), MIDI_S_NRPN_ON },
	{ wxT("NRPNOff"), MIDI_S_NRPN_OFF },
	{ wxT("HWNameLCD"), MIDI_S_HW_NAME_LCD },
	{ wxT("HWNameString"), MIDI_S_HW_NAME_STRING },
	{ wxT("HWLCD"), MIDI_S_HW_LCD },
	{ wxT("HWString"), MIDI_S_HW_STRING },
	{ wxT("RodgersStopChange"), MIDI_S_RODGERS_STOP_CHANGE },
};

void GOMidiSender::SetElementID(int id)
{
	m_ElementID = id;
}

void GOMidiSender::Load(GOConfigReader& cfg, wxString group, GOMidiMap& map)
{
	m_events.resize(0);

	int event_cnt = cfg.ReadInteger(CMBSetting, group, wxT("NumberOfMIDISendEvents"), 0, 255, false);

	m_events.resize(event_cnt);
	for(unsigned i = 0; i < m_events.size(); i++)
	{
		m_events[i].device = map.GetDeviceByString(cfg.ReadString(CMBSetting, group, wxString::Format(wxT("MIDISendDevice%03d"), i + 1), false));
		m_events[i].type = (midi_send_message_type)cfg.ReadEnum(CMBSetting, group, wxString::Format(wxT("MIDISendEventType%03d"), i + 1), m_MidiTypes, sizeof(m_MidiTypes)/sizeof(m_MidiTypes[0]));
		if (HasChannel(m_events[i].type))
			m_events[i].channel = cfg.ReadInteger(CMBSetting, group, wxString::Format(wxT("MIDISendChannel%03d"), i + 1), 1, 16);
		if (HasKey(m_events[i].type))
			m_events[i].key = cfg.ReadInteger(CMBSetting, group, wxString::Format(wxT("MIDISendKey%03d"), i + 1), 0, 0x200000);

		if (HasLowValue(m_events[i].type))
			m_events[i].low_value = cfg.ReadInteger(CMBSetting, group, wxString::Format(wxT("MIDISendLowValue%03d"), i + 1), 0, 0x3fff, false, 0);

		if (HasHighValue(m_events[i].type))
			m_events[i].high_value = cfg.ReadInteger(CMBSetting, group, wxString::Format(wxT("MIDISendHighValue%03d"), i + 1), 0, 0x3fff, false, 0x7f);

		if (HasStart(m_events[i].type))
			m_events[i].start = cfg.ReadInteger(CMBSetting, group, wxString::Format(wxT("MIDISendStart%03d"), i + 1), 0, 0x1f, false, 0);
		if (HasLength(m_events[i].type))
			m_events[i].length = cfg.ReadInteger(CMBSetting, group, wxString::Format(wxT("MIDISendLength%03d"), i + 1), 0, 0x1f, false, LengthLimit(m_events[i].type));
	}
}

void GOMidiSender::Save(GOConfigWriter& cfg, wxString group, GOMidiMap& map)
{
	cfg.WriteInteger(group, wxT("NumberOfMIDISendEvents"), m_events.size());
	for(unsigned i = 0; i < m_events.size(); i++)
	{
		cfg.WriteString(group, wxString::Format(wxT("MIDISendDevice%03d"), i + 1), map.GetDeviceByID(m_events[i].device));
		cfg.WriteEnum(group, wxString::Format(wxT("MIDISendEventType%03d"), i + 1), m_events[i].type, m_MidiTypes, sizeof(m_MidiTypes)/sizeof(m_MidiTypes[0]));
		if (HasChannel(m_events[i].type))
			cfg.WriteInteger(group, wxString::Format(wxT("MIDISendChannel%03d"), i + 1), m_events[i].channel);
		if (HasKey(m_events[i].type))
			cfg.WriteInteger(group, wxString::Format(wxT("MIDISendKey%03d"), i + 1), m_events[i].key);

		if (HasLowValue(m_events[i].type))
			cfg.WriteInteger(group, wxString::Format(wxT("MIDISendLowValue%03d"), i + 1), m_events[i].low_value);

		if (HasHighValue(m_events[i].type))
			cfg.WriteInteger(group, wxString::Format(wxT("MIDISendHighValue%03d"), i + 1), m_events[i].high_value);

		if (HasStart(m_events[i].type))
			cfg.WriteInteger(group, wxString::Format(wxT("MIDISendStart%03d"), i + 1), m_events[i].start);
		if (HasLength(m_events[i].type))
			cfg.WriteInteger(group, wxString::Format(wxT("MIDISendLength%03d"), i + 1), m_events[i].length);
	}
}

bool GOMidiSender::HasChannel(midi_send_message_type type)
{
	if (type == MIDI_S_NOTE ||
	    type == MIDI_S_NOTE_NO_VELOCITY ||
	    type == MIDI_S_CTRL ||
	    type == MIDI_S_RPN ||
	    type == MIDI_S_NRPN ||
	    type == MIDI_S_PGM_ON ||
	    type == MIDI_S_PGM_OFF ||
	    type == MIDI_S_PGM_RANGE ||
	    type == MIDI_S_RPN_RANGE ||
	    type == MIDI_S_NRPN_RANGE ||
	    type == MIDI_S_NOTE_ON ||
	    type == MIDI_S_NOTE_OFF ||
	    type == MIDI_S_CTRL_ON ||
	    type == MIDI_S_CTRL_OFF ||
	    type == MIDI_S_RPN_ON ||
	    type == MIDI_S_RPN_OFF ||
	    type == MIDI_S_NRPN_ON ||
	    type == MIDI_S_NRPN_OFF)
		return true;

	return false;
}

bool GOMidiSender::HasKey(midi_send_message_type type)
{
	if (m_type == MIDI_SEND_MANUAL)
		return false;

	if (type == MIDI_S_NOTE ||
	    type == MIDI_S_NOTE_NO_VELOCITY ||
	    type == MIDI_S_CTRL ||
	    type == MIDI_S_RPN ||
	    type == MIDI_S_NRPN ||
	    type == MIDI_S_RPN_RANGE ||
	    type == MIDI_S_NRPN_RANGE ||
	    type == MIDI_S_PGM_ON ||
	    type == MIDI_S_PGM_OFF ||
	    type == MIDI_S_NOTE_ON ||
	    type == MIDI_S_NOTE_OFF ||
	    type == MIDI_S_CTRL_ON ||
	    type == MIDI_S_CTRL_OFF ||
	    type == MIDI_S_RPN_ON ||
	    type == MIDI_S_RPN_OFF ||
	    type == MIDI_S_NRPN_ON ||
	    type == MIDI_S_NRPN_OFF ||
	    type == MIDI_S_RODGERS_STOP_CHANGE ||
	    type == MIDI_S_HW_NAME_STRING ||
	    type == MIDI_S_HW_NAME_LCD ||
	    type == MIDI_S_HW_STRING ||
	    type == MIDI_S_HW_LCD)
		return true;

	return false;
}

bool GOMidiSender::HasLowValue(midi_send_message_type type)
{
	if (type == MIDI_S_NOTE_OFF ||
	    type == MIDI_S_CTRL_OFF ||
	    type == MIDI_S_RPN_OFF ||
	    type == MIDI_S_NRPN_OFF ||
	    type == MIDI_S_PGM_RANGE ||
	    type == MIDI_S_RPN_RANGE ||
	    type == MIDI_S_NRPN_RANGE ||
	    type == MIDI_S_NOTE ||
	    type == MIDI_S_NOTE_NO_VELOCITY ||
	    type == MIDI_S_RPN ||
	    type == MIDI_S_NRPN ||
	    type == MIDI_S_CTRL ||
	    type == MIDI_S_RODGERS_STOP_CHANGE ||
	    type == MIDI_S_HW_NAME_LCD ||
	    type == MIDI_S_HW_LCD)
		return true;
	return false;
}

bool GOMidiSender::HasHighValue(midi_send_message_type type)
{
	if (type == MIDI_S_NOTE_ON ||
	    type == MIDI_S_CTRL_ON ||
	    type == MIDI_S_RPN_ON ||
	    type == MIDI_S_NRPN_ON ||
	    type == MIDI_S_PGM_RANGE ||
	    type == MIDI_S_RPN_RANGE ||
	    type == MIDI_S_NRPN_RANGE ||
	    type == MIDI_S_NOTE ||
	    type == MIDI_S_NOTE_NO_VELOCITY ||
	    type == MIDI_S_RPN ||
	    type == MIDI_S_NRPN ||
	    type == MIDI_S_CTRL)
		return true;
	return false;
}

bool GOMidiSender::HasStart(midi_send_message_type type)
{
	if (type == MIDI_S_HW_NAME_STRING ||
	    type == MIDI_S_HW_NAME_LCD ||
	    type == MIDI_S_HW_STRING ||
	    type == MIDI_S_HW_LCD)
		return true;
	return false;
}

bool GOMidiSender::HasLength(midi_send_message_type type)
{
	if (type == MIDI_S_HW_NAME_STRING ||
	    type == MIDI_S_HW_NAME_LCD ||
	    type == MIDI_S_HW_STRING ||
	    type == MIDI_S_HW_LCD)
		return true;
	return false;
}

unsigned GOMidiSender::KeyLimit(midi_send_message_type type)
{
	if (type == MIDI_S_PGM_ON ||
	    type == MIDI_S_PGM_OFF)
		return 0x200000;

	if (type == MIDI_S_RPN ||
	    type == MIDI_S_NRPN ||
	    type == MIDI_S_RPN_ON ||
	    type == MIDI_S_RPN_OFF ||
	    type == MIDI_S_NRPN_ON ||
	    type == MIDI_S_NRPN_OFF ||
	    type == MIDI_S_HW_NAME_LCD ||
	    type == MIDI_S_HW_LCD)
		return 0x3fff;

	return 0x7f;
}

unsigned GOMidiSender::LowValueLimit(midi_send_message_type type)
{
	if (type == MIDI_S_PGM_RANGE)
		return 0x200000;
	if(type == MIDI_S_RPN_RANGE ||
	   type == MIDI_S_NRPN_RANGE)
		return 0x3fff;
	if (type == MIDI_S_RODGERS_STOP_CHANGE)
		return 35 * 7;
	return 0x7f;
}

unsigned GOMidiSender::HighValueLimit(midi_send_message_type type)
{
	if (type == MIDI_S_PGM_RANGE)
		return 0x200000;
	if(type == MIDI_S_RPN_RANGE ||
	   type == MIDI_S_NRPN_RANGE)
		return 0x3fff;
	return 0x7f;
}

unsigned GOMidiSender::StartLimit(midi_send_message_type type)
{
	if (type == MIDI_S_HW_NAME_STRING ||
	    type == MIDI_S_HW_STRING)
		return 15;
	if (type == MIDI_S_HW_NAME_LCD ||
	    type == MIDI_S_HW_LCD)
		return 31;
	return 0x00;
}

unsigned GOMidiSender::LengthLimit(midi_send_message_type type)
{
	if (type == MIDI_S_HW_NAME_STRING ||
	    type == MIDI_S_HW_STRING)
		return 15;
	if (type == MIDI_S_HW_NAME_LCD ||
	    type == MIDI_S_HW_LCD)
		return 31;
	return 0x00;
}

void GOMidiSender::SetDisplay(bool state)
{
	if (m_ElementID != -1)
	{
		GOMidiEvent e;
		e.SetMidiType(MIDI_NRPN);
		e.SetDevice(m_ElementID);
		e.SetValue(state ? 0x7F : 0x00);
		m_organfile->SendMidiRecorderMessage(e);
	}

	for(unsigned i = 0; i < m_events.size(); i++)
	{
		if (m_events[i].type == MIDI_S_NOTE)
		{
			GOMidiEvent e;
			e.SetDevice(m_events[i].device);
			e.SetMidiType(MIDI_NOTE);
			e.SetChannel(m_events[i].channel);
			e.SetKey(m_events[i].key);
			e.SetValue(state ? m_events[i].high_value : m_events[i].low_value);
			m_organfile->SendMidiMessage(e);
		}
		if (m_events[i].type == MIDI_S_CTRL)
		{
			GOMidiEvent e;
			e.SetDevice(m_events[i].device);
			e.SetMidiType(MIDI_CTRL_CHANGE);
			e.SetChannel(m_events[i].channel);
			e.SetKey(m_events[i].key);
			e.SetValue(state ? m_events[i].high_value : m_events[i].low_value);
			m_organfile->SendMidiMessage(e);
		}
		if (m_events[i].type == MIDI_S_RPN)
		{
			GOMidiEvent e;
			e.SetDevice(m_events[i].device);
			e.SetMidiType(MIDI_RPN);
			e.SetChannel(m_events[i].channel);
			e.SetKey(m_events[i].key);
			e.SetValue(state ? m_events[i].high_value : m_events[i].low_value);
			m_organfile->SendMidiMessage(e);
		}
		if (m_events[i].type == MIDI_S_NRPN)
		{
			GOMidiEvent e;
			e.SetDevice(m_events[i].device);
			e.SetMidiType(MIDI_NRPN);
			e.SetChannel(m_events[i].channel);
			e.SetKey(m_events[i].key);
			e.SetValue(state ? m_events[i].high_value : m_events[i].low_value);
			m_organfile->SendMidiMessage(e);
		}
		if (m_events[i].type == MIDI_S_PGM_RANGE)
		{
			GOMidiEvent e;
			e.SetDevice(m_events[i].device);
			e.SetMidiType(MIDI_PGM_CHANGE);
			e.SetChannel(m_events[i].channel);
			e.SetKey(state ? m_events[i].high_value : m_events[i].low_value);
			m_organfile->SendMidiMessage(e);
		}
		if (m_events[i].type == MIDI_S_RPN_RANGE)
		{
			GOMidiEvent e;
			e.SetDevice(m_events[i].device);
			e.SetMidiType(MIDI_RPN);
			e.SetChannel(m_events[i].channel);
			e.SetValue(m_events[i].key);
			e.SetKey(state ? m_events[i].high_value : m_events[i].low_value);
			m_organfile->SendMidiMessage(e);
		}
		if (m_events[i].type == MIDI_S_NRPN_RANGE)
		{
			GOMidiEvent e;
			e.SetDevice(m_events[i].device);
			e.SetMidiType(MIDI_NRPN);
			e.SetChannel(m_events[i].channel);
			e.SetValue(m_events[i].key);
			e.SetKey(state ? m_events[i].high_value : m_events[i].low_value);
			m_organfile->SendMidiMessage(e);
		}
		if (m_events[i].type == MIDI_S_PGM_ON && state)
		{
			GOMidiEvent e;
			e.SetDevice(m_events[i].device);
			e.SetMidiType(MIDI_PGM_CHANGE);
			e.SetChannel(m_events[i].channel);
			e.SetKey(m_events[i].key);
			m_organfile->SendMidiMessage(e);
		}
		if (m_events[i].type == MIDI_S_PGM_OFF && !state)
		{
			GOMidiEvent e;
			e.SetDevice(m_events[i].device);
			e.SetMidiType(MIDI_PGM_CHANGE);
			e.SetChannel(m_events[i].channel);
			e.SetKey(m_events[i].key);
			m_organfile->SendMidiMessage(e);
		}
		if (m_events[i].type == MIDI_S_NOTE_ON && state)
		{
			GOMidiEvent e;
			e.SetDevice(m_events[i].device);
			e.SetMidiType(MIDI_NOTE);
			e.SetChannel(m_events[i].channel);
			e.SetKey(m_events[i].key);
			e.SetValue(m_events[i].high_value);
			m_organfile->SendMidiMessage(e);
		}
		if (m_events[i].type == MIDI_S_NOTE_OFF && !state)
		{
			GOMidiEvent e;
			e.SetDevice(m_events[i].device);
			e.SetMidiType(MIDI_NOTE);
			e.SetChannel(m_events[i].channel);
			e.SetKey(m_events[i].key);
			e.SetValue(m_events[i].low_value);
			m_organfile->SendMidiMessage(e);
		}
		if (m_events[i].type == MIDI_S_CTRL_ON && state)
		{
			GOMidiEvent e;
			e.SetDevice(m_events[i].device);
			e.SetMidiType(MIDI_CTRL_CHANGE);
			e.SetChannel(m_events[i].channel);
			e.SetKey(m_events[i].key);
			e.SetValue(m_events[i].high_value);
			m_organfile->SendMidiMessage(e);
		}
		if (m_events[i].type == MIDI_S_CTRL_OFF && !state)
		{
			GOMidiEvent e;
			e.SetDevice(m_events[i].device);
			e.SetMidiType(MIDI_CTRL_CHANGE);
			e.SetChannel(m_events[i].channel);
			e.SetKey(m_events[i].key);
			e.SetValue(m_events[i].low_value);
			m_organfile->SendMidiMessage(e);
		}
		if (m_events[i].type == MIDI_S_RPN_ON && state)
		{
			GOMidiEvent e;
			e.SetDevice(m_events[i].device);
			e.SetMidiType(MIDI_RPN);
			e.SetChannel(m_events[i].channel);
			e.SetKey(m_events[i].key);
			e.SetValue(m_events[i].high_value);
			m_organfile->SendMidiMessage(e);
		}
		if (m_events[i].type == MIDI_S_RPN_OFF && !state)
		{
			GOMidiEvent e;
			e.SetDevice(m_events[i].device);
			e.SetMidiType(MIDI_RPN);
			e.SetChannel(m_events[i].channel);
			e.SetKey(m_events[i].key);
			e.SetValue(m_events[i].low_value);
			m_organfile->SendMidiMessage(e);
		}
		if (m_events[i].type == MIDI_S_NRPN_ON && state)
		{
			GOMidiEvent e;
			e.SetDevice(m_events[i].device);
			e.SetMidiType(MIDI_NRPN);
			e.SetChannel(m_events[i].channel);
			e.SetKey(m_events[i].key);
			e.SetValue(m_events[i].high_value);
			m_organfile->SendMidiMessage(e);
		}
		if (m_events[i].type == MIDI_S_NRPN_OFF && !state)
		{
			GOMidiEvent e;
			e.SetDevice(m_events[i].device);
			e.SetMidiType(MIDI_NRPN);
			e.SetChannel(m_events[i].channel);
			e.SetKey(m_events[i].key);
			e.SetValue(m_events[i].low_value);
			m_organfile->SendMidiMessage(e);
		}
		if (m_events[i].type == MIDI_S_HW_LCD)
		{
			GOMidiEvent e;
			e.SetDevice(m_events[i].device);
			e.SetMidiType(MIDI_SYSEX_HW_LCD);
			e.SetChannel(m_events[i].low_value);
			e.SetKey(m_events[i].key);
			e.SetValue(m_events[i].start);
			e.SetString(state ? _("ON") : _("OFF"), m_events[i].length);
			m_organfile->SendMidiMessage(e);
		}
		if (m_events[i].type == MIDI_S_HW_STRING)
		{
			GOMidiEvent e;
			e.SetDevice(m_events[i].device);
			e.SetMidiType(MIDI_SYSEX_HW_STRING);
			e.SetKey(m_events[i].key);
			e.SetValue(m_events[i].start);
			e.SetString(state ? _("ON") : _("OFF"), m_events[i].length);
			m_organfile->SendMidiMessage(e);
		}
		if (m_events[i].type == MIDI_S_RODGERS_STOP_CHANGE)
		{
			GOMidiEvent e;
			e.SetDevice(m_events[i].device);
			e.SetMidiType(MIDI_SYSEX_RODGERS_STOP_CHANGE);
			e.SetChannel(m_events[i].key);
			e.SetKey(m_events[i].low_value);
			e.SetValue(state);
			m_organfile->SendMidiMessage(e);
		}
	}
}

void GOMidiSender::ResetKey()
{
	if (m_ElementID != -1)
	{
		GOMidiEvent e;
		e.SetMidiType(MIDI_CTRL_CHANGE);
		e.SetDevice(m_ElementID);
		e.SetKey(MIDI_CTRL_NOTES_OFF);
		e.SetValue(0);
		m_organfile->SendMidiRecorderMessage(e);
	}

	for(unsigned i = 0; i < m_events.size(); i++)
	{
		if (m_events[i].type == MIDI_S_NOTE || m_events[i].type == MIDI_S_NOTE_NO_VELOCITY)
		{
			GOMidiEvent e;
			e.SetDevice(m_events[i].device);
			e.SetMidiType(MIDI_CTRL_CHANGE);
			e.SetChannel(m_events[i].channel);
			e.SetKey(MIDI_CTRL_NOTES_OFF);
			e.SetValue(0);
			m_organfile->SendMidiMessage(e);
		}
	}
}

void GOMidiSender::SetKey(unsigned key, unsigned velocity)
{
	if (m_ElementID != -1)
	{
		GOMidiEvent e;
		e.SetMidiType(MIDI_NOTE);
		e.SetDevice(m_ElementID);
		e.SetKey(key & 0x7F);
		e.SetValue(velocity & 0x7F);
		m_organfile->SendMidiRecorderMessage(e);
	}

	for(unsigned i = 0; i < m_events.size(); i++)
	{
		if (m_events[i].type == MIDI_S_NOTE)
		{
			GOMidiEvent e;
			e.SetDevice(m_events[i].device);
			e.SetMidiType(MIDI_NOTE);
			e.SetChannel(m_events[i].channel);
			e.SetKey(key);
			e.SetValue(m_events[i].low_value + (velocity * (m_events[i].high_value - m_events[i].low_value)) / 0x7f);
			m_organfile->SendMidiMessage(e);
		}
		if (m_events[i].type == MIDI_S_NOTE_NO_VELOCITY)
		{
			GOMidiEvent e;
			e.SetDevice(m_events[i].device);
			e.SetMidiType(MIDI_NOTE);
			e.SetChannel(m_events[i].channel);
			e.SetKey(key);
			e.SetValue(velocity ? m_events[i].high_value : m_events[i].low_value);
			m_organfile->SendMidiMessage(e);
		}
	}
}

void GOMidiSender::SetValue(unsigned value)
{
	if (m_ElementID != -1)
	{
		GOMidiEvent e;
		e.SetMidiType(MIDI_NRPN);
		e.SetDevice(m_ElementID);
		e.SetValue(value & 0x7F);
		m_organfile->SendMidiRecorderMessage(e);
	}

	for(unsigned i = 0; i < m_events.size(); i++)
	{
		if (m_events[i].type == MIDI_S_CTRL)
		{
			GOMidiEvent e;
			e.SetDevice(m_events[i].device);
			e.SetMidiType(MIDI_CTRL_CHANGE);
			e.SetChannel(m_events[i].channel);
			e.SetKey(m_events[i].key);
			unsigned val = m_events[i].low_value + ((m_events[i].high_value - m_events[i].low_value) * value) / 0x7f;
			if (val < 0)
				val = 0;
			if (val > 0x7f)
				val = 0x7f;
			e.SetValue(val);
			m_organfile->SendMidiMessage(e);
		}
		if (m_events[i].type == MIDI_S_RPN)
		{
			GOMidiEvent e;
			e.SetDevice(m_events[i].device);
			e.SetMidiType(MIDI_RPN);
			e.SetChannel(m_events[i].channel);
			e.SetKey(m_events[i].key);
			unsigned val = m_events[i].low_value + ((m_events[i].high_value - m_events[i].low_value) * value) / 0x7f;
			if (val < 0)
				val = 0;
			if (val > 0x7f)
				val = 0x7f;
			e.SetValue(val);
			m_organfile->SendMidiMessage(e);
		}
		if (m_events[i].type == MIDI_S_NRPN)
		{
			GOMidiEvent e;
			e.SetDevice(m_events[i].device);
			e.SetMidiType(MIDI_NRPN);
			e.SetChannel(m_events[i].channel);
			e.SetKey(m_events[i].key);
			unsigned val = m_events[i].low_value + ((m_events[i].high_value - m_events[i].low_value) * value) / 0x7f;
			if (val < 0)
				val = 0;
			if (val > 0x7f)
				val = 0x7f;
			e.SetValue(val);
			m_organfile->SendMidiMessage(e);
		}
		if (m_events[i].type == MIDI_S_PGM_RANGE)
		{
			GOMidiEvent e;
			e.SetDevice(m_events[i].device);
			e.SetMidiType(MIDI_PGM_CHANGE);
			e.SetChannel(m_events[i].channel);
			unsigned val = m_events[i].low_value + ((m_events[i].high_value - m_events[i].low_value) * value) / 0x7f;
			if (val < 0)
				val = 0;
			if (val > 0x7f)
				val = 0x7f;
			e.SetKey(val);
			m_organfile->SendMidiMessage(e);
		}
		if (m_events[i].type == MIDI_S_HW_LCD)
		{
			GOMidiEvent e;
			e.SetDevice(m_events[i].device);
			e.SetMidiType(MIDI_SYSEX_HW_LCD);
			e.SetChannel(m_events[i].low_value);
			e.SetKey(m_events[i].key);
			e.SetValue(m_events[i].start);
			e.SetString(wxString::Format(_("%d %%"), value * 100 / 127), m_events[i].length);
			m_organfile->SendMidiMessage(e);
		}
		if (m_events[i].type == MIDI_S_HW_STRING)
		{
			GOMidiEvent e;
			e.SetDevice(m_events[i].device);
			e.SetMidiType(MIDI_SYSEX_HW_STRING);
			e.SetKey(m_events[i].key);
			e.SetValue(m_events[i].start);
			e.SetString(wxString::Format(_("%d %%"), value * 100 / 127), m_events[i].length);
			m_organfile->SendMidiMessage(e);
		}
	}
}

void GOMidiSender::SetLabel(const wxString& text)
{
	for(unsigned i = 0; i < m_events.size(); i++)
	{
		if (m_events[i].type == MIDI_S_HW_LCD)
		{
			GOMidiEvent e;
			e.SetDevice(m_events[i].device);
			e.SetMidiType(MIDI_SYSEX_HW_LCD);
			e.SetChannel(m_events[i].low_value);
			e.SetKey(m_events[i].key);
			e.SetValue(m_events[i].start);
			e.SetString(text, m_events[i].length);
			m_organfile->SendMidiMessage(e);
		}
		if (m_events[i].type == MIDI_S_HW_STRING)
		{
			GOMidiEvent e;
			e.SetDevice(m_events[i].device);
			e.SetMidiType(MIDI_SYSEX_HW_STRING);
			e.SetKey(m_events[i].key);
			e.SetValue(m_events[i].start);
			e.SetString(text, m_events[i].length);
			m_organfile->SendMidiMessage(e);
		}
	}
}

void GOMidiSender::SetName(const wxString& text)
{
	for(unsigned i = 0; i < m_events.size(); i++)
	{
		if (m_events[i].type == MIDI_S_HW_NAME_LCD)
		{
			GOMidiEvent e;
			e.SetDevice(m_events[i].device);
			e.SetMidiType(MIDI_SYSEX_HW_LCD);
			e.SetChannel(m_events[i].low_value);
			e.SetKey(m_events[i].key);
			e.SetValue(m_events[i].start);
			e.SetString(text, m_events[i].length);
			m_organfile->SendMidiMessage(e);
		}
		if (m_events[i].type == MIDI_S_HW_NAME_STRING)
		{
			GOMidiEvent e;
			e.SetDevice(m_events[i].device);
			e.SetMidiType(MIDI_SYSEX_HW_STRING);
			e.SetKey(m_events[i].key);
			e.SetValue(m_events[i].start);
			e.SetString(text, m_events[i].length);
			m_organfile->SendMidiMessage(e);
		}
	}
}

void GOMidiSender::Assign(const GOMidiSenderData& data)
{
	*(GOMidiSenderData*)this = data;
	if (m_organfile)
		m_organfile->Modified();
}
