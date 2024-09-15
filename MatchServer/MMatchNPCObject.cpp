#include "stdafx.h"
#include "MMatchNPCObject.h"
#include "MMatchStage.h"
#include "MMatchObject.h"
#include "MMatchServer.h"
#include "MSharedCommandTable.h"
#include "MMath.h"
#include "MMatchRuleQuest.h"
#include "MQuestPlayer.h"

MMatchNPCObject::MMatchNPCObject(MUID& uid, MQUEST_NPC nType, u32 nFlags)
					: m_UID(uid), m_nType(nType), m_uidController(MUID(0,0)), m_Pos(0.0f,0.0f,0.0f), m_nFlags(nFlags)
{

}

void MMatchNPCObject::AssignControl(MUID& uidPlayer)
{
	m_uidController = uidPlayer;
}

void MMatchNPCObject::ReleaseControl()
{
	m_uidController = MUID(0,0);
}

void MMatchNPCObject::SetDropItem(MQuestDropItem* pDropItem)
{
	m_DropItem.Assign(pDropItem);

	// monster bible.
	//m_DropItem.nMonsetBibleIndex	= pDropItem->nMonsetBibleIndex;
}

////////////////////////////////////////////////////////////////////////////
MMatchNPCManager::MMatchNPCManager() : m_pStage(NULL), m_nLastSpawnTime(0), m_pPlayerManager(0), m_nBossCount(0)
{

}

MMatchNPCManager::~MMatchNPCManager()
{

}

bool MMatchNPCManager::AssignControl(MUID& uidNPC, MUID& uidPlayer)
{
	MMatchObject* pObject = MMatchServer::GetInstance()->GetObject(uidPlayer);
	if (!pObject) return false;
	MMatchNPCObject* pNPCObject = GetNPCObject(uidNPC);
	if (!pNPCObject) return false;

	// ControllerInfo ����
	SetNPCObjectToControllerInfo(uidPlayer, pNPCObject);

	// route cmd
	MCommand* pCmd = MMatchServer::GetInstance()->CreateCommand(MC_QUEST_ENTRUST_NPC_CONTROL, uidPlayer);
	pCmd->AddParameter(new MCmdParamUID(uidPlayer));
	pCmd->AddParameter(new MCmdParamUID(uidNPC));
	MMatchServer::GetInstance()->RouteToBattle(m_pStage->GetUID(), pCmd);

	#ifdef _DEBUG
	char text[256];
	sprintf_safe(text, "AssignControl(%u:%u) - (%u:%u)\n", uidNPC.High, uidNPC.Low, uidPlayer.High, uidPlayer.Low);
	OutputDebugString(text);
	#endif


	return true;
}

bool MMatchNPCManager::Spawn(MUID& uidNPC, MUID& uidController, unsigned char nSpawnPositionIndex)
{
	MMatchObject* pObject = MMatchServer::GetInstance()->GetObject(uidController);
	if ((pObject) && (m_pStage))
	{
		MMatchNPCObject* pNPCObject = GetNPCObject(uidNPC);
		if (pNPCObject)
		{
			SetNPCObjectToControllerInfo(uidController, pNPCObject);

			// route cmd
			MCommand* pCmd = MMatchServer::GetInstance()->CreateCommand(MC_QUEST_NPC_SPAWN, uidController);
			pCmd->AddParameter(new MCmdParamUID(uidController));
			pCmd->AddParameter(new MCmdParamUID(uidNPC));
			pCmd->AddParameter(new MCmdParamUChar((unsigned char)pNPCObject->GetType()));
			pCmd->AddParameter(new MCmdParamUChar((unsigned char)nSpawnPositionIndex));
			MMatchServer::GetInstance()->RouteToBattle(m_pStage->GetUID(), pCmd);


//			#ifdef _DEBUG
//			char text[256];
//			sprintf_safe(text, "Spawn(%u:%u) - (%u:%u)\n", uidNPC.High, uidNPC.Low, uidController.High, uidController.Low);
//			OutputDebugString(text);
//			#endif

			return true;
		}
	}
	return false;
}

/// Override: spawn at exact coordinates
bool MMatchNPCManager::Spawn(MUID& uidNPC, MUID& uidController, MVector& pos)
{
	MMatchObject* pObject = MMatchServer::GetInstance()->GetObject(uidController);
	if ((pObject) && (m_pStage))
	{
		MMatchNPCObject* pNPCObject = GetNPCObject(uidNPC);
		if (pNPCObject)
		{
			SetNPCObjectToControllerInfo(uidController, pNPCObject);

			MCommand* pCmd = MMatchServer::GetInstance()->CreateCommand(MC_QUEST_NPC_SPAWN_POS, uidController);
			pCmd->AddParameter(new MCmdParamUID(uidController));
			pCmd->AddParameter(new MCmdParamUID(uidNPC));
			pCmd->AddParameter(new MCmdParamUChar((unsigned char)pNPCObject->GetType()));
			pCmd->AddParameter(new MCmdParamVector(pos.x, pos.y, pos.z));
			MMatchServer::GetInstance()->RouteToBattle(m_pStage->GetUID(), pCmd);

			return true;
		}
	}
	return false;
}

MMatchNPCObject* MMatchNPCManager::CreateNPCObject(MQUEST_NPC nType, unsigned char nSpawnPositionIndex)
{
	MQuestNPCInfo* pNPCInfo = MMatchServer::GetInstance()->GetQuest()->GetNPCInfo(nType);
	if (pNPCInfo == NULL) return NULL;

	MUID uidNPC = NewUID();
	u32 nNPCFlags=0;

	MMatchNPCObject* pNewNPC = new MMatchNPCObject(uidNPC, nType, nNPCFlags);

	m_NPCObjectMap.insert(MMatchNPCObjectMap::value_type(uidNPC, pNewNPC));

	// spawn�� NPC Count����
	MQuestNPCSpawnType nSpawnType = MNST_MELEE;
	
	if (pNPCInfo)
	{
		nSpawnType = pNPCInfo->GetSpawnType();

		// ���� �����̸� BossCount ����
		if ((pNPCInfo->nGrade == NPC_GRADE_BOSS) || (pNPCInfo->nGrade == NPC_GRADE_LEGENDARY))
		{
			m_nBossCount++;
		}
	}
	m_nNPCCount[nSpawnType]++;
	


	// ��Ʈ�ѷ� �Ҵ�
	MUID uidController = MUID(0,0);
	if (!FindSuitableController(uidController, NULL))
	{
		// ������ ����� ������ ����
		MQuestDropItem TempItem;
		DestroyNPCObject(uidNPC, TempItem);
		return NULL;
	}

	// ����
	if (!Spawn(uidNPC, uidController, nSpawnPositionIndex))
	{
		MQuestDropItem TempItem;
		DestroyNPCObject(uidNPC, TempItem);
		return NULL;
	}

	return pNewNPC;
}

/// Override: spawn at exact coordinates
MMatchNPCObject* MMatchNPCManager::CreateNPCObject(MQUEST_NPC nType, MVector& pos)
{
	MQuestNPCInfo* pNPCInfo = MMatchServer::GetInstance()->GetQuest()->GetNPCInfo(nType);
	if (pNPCInfo == NULL) return NULL;

	MUID uidNPC = NewUID();
	u32 nNPCFlags=0;

	MMatchNPCObject* pNewNPC = new MMatchNPCObject(uidNPC, nType, nNPCFlags);

	m_NPCObjectMap.insert(MMatchNPCObjectMap::value_type(uidNPC, pNewNPC));

	MQuestNPCSpawnType nSpawnType = MNST_MELEE;
	
	if (pNPCInfo)
	{
		nSpawnType = pNPCInfo->GetSpawnType();
		if ((pNPCInfo->nGrade == NPC_GRADE_BOSS) || (pNPCInfo->nGrade == NPC_GRADE_LEGENDARY))
		{
			m_nBossCount++;
		}
	}
	m_nNPCCount[nSpawnType]++;
	
	MUID uidController = MUID(0,0);
	if (!FindSuitableController(uidController, NULL))
	{
		mlog("CreateNPCObjectPos suitable controller not found\n");
		MQuestDropItem TempItem;
		DestroyNPCObject(uidNPC, TempItem);
		return NULL;
	}

	if (!Spawn(uidNPC, uidController, pos))
	{
		mlog("CreateNPCObjectPos SpawnPos got false\n");
		MQuestDropItem TempItem;
		DestroyNPCObject(uidNPC, TempItem);
		return NULL;
	}

	return pNewNPC;
}

bool MMatchNPCManager::DestroyNPCObject(MUID& uidNPC, MQuestDropItem& outItem)
{
	MMatchNPCObjectMap::iterator itor = m_NPCObjectMap.find(uidNPC);
	if (itor != m_NPCObjectMap.end())
	{
		MMatchNPCObject* pNPCObject = (*itor).second;

		outItem.Assign(pNPCObject->GetDropItem());
#ifdef _MONSTER_BIBLE
		// outItem.nMonsetBibleIndex	= pNPCObject->GetDropItem()->nMonsetBibleIndex;
#ifdef _DEBUG
		// mlog( "MMatchNPCManager::DestroyNPCObject - Destroy npc's drop item monster bible index:%d\n", outItem.nMonsetBibleIndex );
#endif
#endif

		// Controller�� �־����� Controller������ �����ش�.
		MUID uidController = pNPCObject->GetController();
		if (uidController != MUID(0,0))
		{
			DelNPCObjectToControllerInfo(uidController, pNPCObject);
		}

		// spawn�� NPC Count����
		MQuestNPCSpawnType nSpawnType = MNST_MELEE;
		MQuestNPCInfo* pNPCInfo = MMatchServer::GetInstance()->GetQuest()->GetNPCInfo(pNPCObject->GetType());
		if (pNPCInfo)
		{
			nSpawnType = pNPCInfo->GetSpawnType();

			// ���� �����̸� BossCount ����
			if ((pNPCInfo->nGrade == NPC_GRADE_BOSS) || (pNPCInfo->nGrade == NPC_GRADE_LEGENDARY))
			{
				m_nBossCount--;
				if (m_nBossCount <= 0)
					BossDead = true;
			}
		}
		m_nNPCCount[nSpawnType]--;



		delete pNPCObject;
		m_NPCObjectMap.erase(itor);
		return true;
	}

	return false;
}

MUID MMatchNPCManager::NewUID()
{
	// MMatchObject uid�� ���� �׷� ���
	return MMatchServer::GetInstance()->UseUID();
}

void MMatchNPCManager::Create(MMatchStage* pStage, MQuestPlayerManager* pPlayerManager)
{
	m_pStage = pStage;
	m_pPlayerManager = pPlayerManager;
	m_nBossCount = 0;
	BossDead = false;

	memset(m_nNPCCount, 0, sizeof(m_nNPCCount));
}

void MMatchNPCManager::Destroy()
{
	Clear();
}

MMatchNPCObject* MMatchNPCManager::GetNPCObject(MUID& uidNPC)
{
	MMatchNPCObjectMap::iterator itor = m_NPCObjectMap.find(uidNPC);
	if (itor != m_NPCObjectMap.end())
	{
		return (*itor).second;
	}
	return NULL;
}

void MMatchNPCManager::Clear()
{
	ClearNPC();
}

void MMatchNPCManager::OnDelPlayer(const MUID& uidPlayer)
{
	MQuestPlayerInfo* pDelPlayerInfo = m_pPlayerManager->GetPlayerInfo(uidPlayer);

	if (pDelPlayerInfo)
	{
		pDelPlayerInfo->bEnableNPCControl = false;

//		for (MMatchNPCObjectMap::iterator itorNPC = pDelPlayerInfo->NPCObjects.begin(); 
//			itorNPC != pDelPlayerInfo->NPCObjects.end(); ++itorNPC)

		// ���� ���� ���̻ڴ�. ���߿� �̻ڰ� ���ľ��ҵ� - bird
		while (!pDelPlayerInfo->NPCObjects.empty())
		{
			MMatchNPCObjectMap::iterator itorNPC = pDelPlayerInfo->NPCObjects.begin();

			MMatchNPCObject* pNPCObject = (*itorNPC).second;
			MUID uidNPC = pNPCObject->GetUID();		// ���⼭ ������ - bird

			MUID uidController = MUID(0,0);
			if (FindSuitableController(uidController, pDelPlayerInfo))
			{
				// ���� �÷��̾�� �Ҵ�� NPC�� �ٸ� �÷��̾�� �Ű��ش�.
				AssignControl(uidNPC, uidController);
			}
			else
			{
				// ������ ����� ������ NPC ����
				MQuestDropItem TempItem;
				DestroyNPCObject(uidNPC, TempItem);	// ���⼭ NPCObjects.erase ��
			}
		}
	}
}


void MMatchNPCManager::ClearNPC()
{
	for (MMatchNPCObjectMap::iterator itor = m_NPCObjectMap.begin(); itor != m_NPCObjectMap.end(); ++itor)
	{
		MMatchNPCObject* pNPCObject = (*itor).second;

		// Controller�� �־����� Controller������ �����ش�.
		MUID uidController = pNPCObject->GetController();
		if (uidController != MUID(0,0))
		{
			DelNPCObjectToControllerInfo(uidController, pNPCObject);
		}

		delete pNPCObject;
	}

	m_NPCObjectMap.clear();
	memset(m_nNPCCount, 0, sizeof(m_nNPCCount));
}


bool MMatchNPCManager::FindSuitableController(MUID& out, MQuestPlayerInfo* pSender)
{
	int nScore = 999999;
	MUID uidChar = MUID(0,0);
	bool bExist = false;

	for (MQuestPlayerManager::iterator itor = m_pPlayerManager->begin(); itor != m_pPlayerManager->end(); ++itor)
	{
		MQuestPlayerInfo* pControllerInfo = (*itor).second;

		if ((pSender != NULL) && (pControllerInfo == pSender)) continue;
		if (pControllerInfo->bEnableNPCControl == false) continue;

		int nControllerScore = pControllerInfo->GetNPCControlScore();
		if (nControllerScore < nScore)
		{
			bExist = true;
			nScore = nControllerScore;
			uidChar = (*itor).first;
		}
	}

	if (bExist)
	{
		out = uidChar;
		return true;
	}
	
	return false;
}



void MMatchNPCManager::SetNPCObjectToControllerInfo(MUID& uidChar, MMatchNPCObject* pNPCObject)
{
	MUID uidLaster = pNPCObject->GetController();
	if (uidLaster != MUID(0,0))
	{
		DelNPCObjectToControllerInfo(uidLaster, pNPCObject);
	}

	MQuestPlayerInfo* pControllerInfo = m_pPlayerManager->GetPlayerInfo(uidChar);
	if (pControllerInfo)
	{
		pNPCObject->AssignControl(uidChar);
		pControllerInfo->NPCObjects.insert(MMatchNPCObjectMap::value_type(pNPCObject->GetUID(), pNPCObject));
	}
}

void MMatchNPCManager::DelNPCObjectToControllerInfo(MUID& uidChar, MMatchNPCObject* pNPCObject)
{
	pNPCObject->ReleaseControl();

	MQuestPlayerInfo* pControllerInfo = m_pPlayerManager->GetPlayerInfo(uidChar);
	if (pControllerInfo)
	{
		MMatchNPCObjectMap::iterator itor = pControllerInfo->NPCObjects.find(pNPCObject->GetUID());
		if (itor != pControllerInfo->NPCObjects.end())
		{
			pControllerInfo->NPCObjects.erase(itor);
		}
	}
}

bool MMatchNPCManager::IsControllersNPC(MUID& uidChar, MUID& uidNPC)
{
	MMatchNPCObject* pNPCObject = GetNPCObject(uidNPC);
	if (pNPCObject)
	{
		if (pNPCObject->GetController() == uidChar) return true;
	}

	return false;
}


//////////////////////////////////////////////////////////////////////////
// �� �÷��̾��� ��Ʈ���� �ٸ�������� �ű��.

void MMatchNPCManager::RemovePlayerControl(const MUID& uidPlayer)
{
	MQuestPlayerInfo* pDelPlayerInfo = m_pPlayerManager->GetPlayerInfo(uidPlayer);

	if (pDelPlayerInfo)
	{
		pDelPlayerInfo->bEnableNPCControl = false;

		while (!pDelPlayerInfo->NPCObjects.empty())
		{
			MMatchNPCObjectMap::iterator itorNPC = pDelPlayerInfo->NPCObjects.begin();

			MMatchNPCObject* pNPCObject = (*itorNPC).second;
			MUID uidNPC = pNPCObject->GetUID();		// ���⼭ ������ - bird

			MUID uidController = MUID(0,0);
			if (FindSuitableController(uidController, pDelPlayerInfo))
			{
				// ���� �÷��̾�� �Ҵ�� NPC�� �ٸ� �÷��̾�� �Ű��ش�.
				AssignControl(uidNPC, uidController);
			}
			else
			{
				// ������ ����� ������ ���Ҵ� ����
				break;
			}
		}

		pDelPlayerInfo->bEnableNPCControl = true;
	}
}