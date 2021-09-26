/*
* Copyright 2006 Milan Digital Audio LLC
* Copyright 2009-2019 GrandOrgue contributors (see AUTHORS)
* License GPL-2.0 (https://www.gnu.org/licenses/old-licenses/gpl-2.0.html).
*/

#ifndef MIDIEVENTKEYDIALOG_H_
#define MIDIEVENTKEYDIALOG_H_

#include "GOrgueKeyReceiver.h"
#include <wx/panel.h>

class wxChoice;
class wxToggleButton;

class MIDIEventKeyDialog : public wxPanel
{

private:
	GOrgueKeyReceiver* m_original;
	GOrgueKeyReceiverData m_key;
	wxChoice *m_keyselect;
	wxToggleButton* m_listen;
	wxChoice *m_keyminusselect;
	wxToggleButton* m_minuslisten;

	void OnKeyDown(wxKeyEvent& event);
	void OnListenClick(wxCommandEvent& event);
	void OnMinusListenClick(wxCommandEvent& event);

	void Listen(bool enable);
	void FillKeylist(wxChoice* select, unsigned shortcut);

protected:
	enum {
		ID_KEY_SELECT = 200,
		ID_MINUS_KEY_SELECT,
		ID_LISTEN,
		ID_LISTEN_MINUS,
	};
public:
	MIDIEventKeyDialog (wxWindow* parent, GOrgueKeyReceiver* event);
	~MIDIEventKeyDialog();

	void DoApply();

	DECLARE_EVENT_TABLE()
};

#endif
