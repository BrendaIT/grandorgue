/*
* Copyright 2006 Milan Digital Audio LLC
* Copyright 2009-2021 GrandOrgue contributors (see AUTHORS)
* License GPL-2.0 or later (https://www.gnu.org/licenses/old-licenses/gpl-2.0.html).
*/

#ifndef GOPIPECONFIGTREENODE_H
#define GOPIPECONFIGTREENODE_H

#include "GOPipeConfigNode.h"
#include <vector>

class GOPipeConfigTreeNode : public GOPipeConfigNode, private GOPipeUpdateCallback
{
private:
	std::vector<GOPipeConfigNode*> m_Childs;
	GOPipeUpdateCallback* m_Callback;

	void UpdateAmplitude();
	void UpdateTuning();
	void UpdateAudioGroup();

public:
	GOPipeConfigTreeNode(GOPipeConfigNode* parent, GODefinitionFile* organfile, GOPipeUpdateCallback* callback);

	void AddChild(GOPipeConfigNode* node);
	unsigned GetChildCount();
	GOPipeConfigNode* GetChild(unsigned index);
	GOSampleStatistic GetStatistic();
};

#endif
