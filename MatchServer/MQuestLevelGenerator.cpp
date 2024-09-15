#include "stdafx.h"
#include "MQuestLevelGenerator.h"
#include "MQuestLevel.h"
#include "MMath.h"
#include "MMatchServer.h"
//#include "MSacrificeQItemTable.h"

///////////////////////////////////////////////////////////////////////////////
MQuestLevelGenerator::MQuestLevelGenerator() : m_nPlayerQL(0), m_nMapsetID(-1), m_nScenarioID(-1)
{
	memset(m_nSacriQItemID, 0, sizeof(unsigned int) * MAX_SCENARIO_SACRI_ITEM);
}

MQuestLevelGenerator::~MQuestLevelGenerator()
{

}

void MQuestLevelGenerator::BuildPlayerQL(int nPlayerQL)
{
	m_nPlayerQL = nPlayerQL;
}

void MQuestLevelGenerator::BuildMapset(int nMapsetID)
{
	m_nMapsetID = nMapsetID;
}

void MQuestLevelGenerator::BuildSacriQItem(unsigned int nItemID)
{
	for (int i = 0; i < MAX_SACRIFICE_SLOT_COUNT; i++)
	{
		if ((m_nSacriQItemID[i] == 0) || (i == (MAX_SCENARIO_SACRI_ITEM-1)))
		{
			m_nSacriQItemID[i] = nItemID;
			break;
		}
	}
}

MQuestLevel* MQuestLevelGenerator::MakeLevel()
{
	// �ó����� ����
	m_nScenarioID = MakeScenarioID();


	// �ֻ��� ����
	int dice = (int)Dice(1, SCENARIO_STANDARD_DICE_SIDES, 0);

	MQuestLevel* pNewLevel = new MQuestLevel();
	pNewLevel->Init(m_nScenarioID, dice);

	return pNewLevel;
}

MQuestLevel* MQuestLevelGenerator::MakeExploration()
{
	m_nScenarioID = 100;

	int dice = (int)Dice(1, SCENARIO_STANDARD_DICE_SIDES, 0);

	MQuestLevel* pNewLevel = new MQuestLevel();
	pNewLevel->Init(m_nScenarioID, dice);

	return pNewLevel;
}

int MQuestLevelGenerator::MakeScenarioID()
{
	MMatchQuest* pQuest = MMatchServer::GetInstance()->GetQuest();
	int id = pQuest->GetScenarioCatalogue()->MakeScenarioID(m_nMapsetID, m_nPlayerQL, m_nSacriQItemID);

	if (!pQuest->GetScenarioCatalogue()->GetInfo(id))
	{
		id = pQuest->GetScenarioCatalogue()->GetDefaultStandardScenarioID();
		mlog("[Gva:MQuestLevelGenerator::MakeScenarioID] using default scenario id: %i\n", id);
	}
	else {
		mlog("[Gva:MQuestLevelGenerator::MakeScenarioID] generated scenario id: %i\n", id);
	}


	return id;
}

int MQuestLevelGenerator::ReturnScenarioID()
{
	return MakeScenarioID();
}




