/*
* Copyright 2006 Milan Digital Audio LLC
* Copyright 2009-2021 GrandOrgue contributors (see AUTHORS)
* License GPL-2.0 or later (https://www.gnu.org/licenses/old-licenses/gpl-2.0.html).
*/

#ifndef GOMIDIRECORDER_H
#define GOMIDIRECORDER_H

#include "GOElementCreator.h"
#include "GOLabel.h"
#include "GOTime.h"
#include "GOTimerCallback.h"
#include "ptrvector.h"
#include <wx/file.h>
#include <wx/string.h>
#include <vector>

class GOMidiEvent;
class GOMidiMap;
class GODefinitionFile;

class GOMidiRecorder : public GOElementCreator, private GOTimerCallback
{
	typedef struct
	{
		unsigned elementID;
		unsigned channel;
		unsigned key;
	} midi_map;
private:
	GODefinitionFile* m_organfile;
	GOMidiMap& m_Map;
	GOLabel m_RecordingTime;
	unsigned m_RecordSeconds;
	unsigned m_NextChannel;
	unsigned m_NextNRPN;
	std::vector<midi_map> m_Mappings;
	std::vector<midi_map> m_Preconfig;
	unsigned m_OutputDevice;
	wxFile m_file;
	wxString m_Filename;
	bool m_DoRename;
	char m_Buffer[2000];
	unsigned m_BufferPos;
	unsigned m_FileLength;
	GOTime m_Last;

	static const struct ElementListEntry m_element_types[];
	const struct ElementListEntry* GetButtonList();

	void ButtonChanged(int id);

	void UpdateDisplay();
	void HandleTimer();

	void Ensure(unsigned length);
	void Flush();
	void EncodeLength(unsigned len);
	void Write(const void* data, unsigned len);
	void WriteEvent(GOMidiEvent& e);
	void SendEvent(GOMidiEvent& e);
	bool SetupMapping(unsigned element, bool isNRPN);

public:
	GOMidiRecorder(GODefinitionFile* organfile);
	~GOMidiRecorder();

	void SetOutputDevice(const wxString& device_id);
	void PreconfigureMapping(const wxString& element, bool isNRPN);
	void PreconfigureMapping(const wxString& element, bool isNRPN, const wxString& reference);
	void SetSamplesetId(unsigned id1, unsigned id2);

	void SendMidiRecorderMessage(GOMidiEvent& e);

	void Clear();

	void StartRecording(bool rename);
	bool IsRecording();
	void StopRecording();

	void Load(GOConfigReader& cfg);
	GOEnclosure* GetEnclosure(const wxString& name, bool is_panel);
	GOLabel* GetLabel(const wxString& name, bool is_panel);
};

#endif
