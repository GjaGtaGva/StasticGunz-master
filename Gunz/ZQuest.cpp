#include "stdafx.h"
#include "ZQuest.h"
#include "ZGameClient.h"
#include "ZEnemy.h"
#include "ZActor.h"
#include "ZObjectManager.h"
#include "Physics.h"
#include "ZGlobal.h"
#include "ZMyCharacter.h"
#include "ZGame.h"
#include "ZScreenEffectManager.h"
#include "ZMapDesc.h"
#include "MQuestConst.h"
#include "ZWorldItem.h"
#include "ZCharacter.h"
#include "ZCharacterManager.h"
#include "ZModule_QuestStatus.h"
#include "ZWorldManager.h"
#include "ZCommandTable.h"
#include "ZModule_Skills.h"
#include "RNavigationMesh.h"
#include "RNavigationNode.h"
#include "ZEquipmentListBox.h"
#include "RBspObject.h"
#include "reinterpret.h"

ZQuest::ZQuest() : MBaseQuest(), m_bLoaded(false), m_bCreatedOnce(false)
{
	memset(&m_Cheet, 0, sizeof(m_Cheet));

	m_bIsQuestComplete = false;

	m_QuestCombatState = MQUEST_COMBAT_NONE;

	m_nRewardXP = 0;
	m_nRewardBP = 0;
}

ZQuest::~ZQuest()
{
	OnDestroyOnce();
}

#ifdef _QUEST

bool ZQuest::OnCreate()
{
	memset(&m_Cheet, 0, sizeof(m_Cheet));
	m_bIsRoundClear = false;
	ZGetQuest()->GetGameInfo()->ClearNPCKilled();
	m_fLastWeightTime = 0.0f;

	m_Map.Init();

	if (!ZGetGameTypeManager()->IsExplorationDerived(ZGetGameClient()->GetMatchStageSetting()->GetGameType())) {
		/// For Exploration - load all resources
		LoadNPCMeshesAll();
		LoadNPCSoundsAll();
	}
	else {
		/// For normal quests - load by scenarios, like og
		LoadNPCMeshes();
		LoadNPCSounds();
	}

	ZGetScreenEffectManager()->CreateQuestRes();

	return true;
}

void ZQuest::OnDestroy()
{
	m_Map.Final();

	ZGetNpcMeshMgr()->UnLoadAll();
	m_GameInfo.Final();

	ZGetScreenEffectManager()->DestroyQuestRes();
}

bool ZQuest::OnCreateOnce()
{
	if (m_bCreatedOnce) return true;

	m_bCreatedOnce = true;
	return Load();
}

void ZQuest::OnDestroyOnce()
{
	m_bCreatedOnce = false;
}

bool ZQuest::Load()
{
	if (m_bLoaded) return true;

	if (!m_DropTable.ReadXml(ZApplication::GetFileSystem(), FILENAME_DROPTABLE))
	{
		mlog("Error while Read m_DropTable %s", FILENAME_ZNPC_DESC);
		return false;
	}
	
	if (!m_NPCCatalogue.ReadXml(ZApplication::GetFileSystem(), FILENAME_ZNPC_DESC))
	{
		mlog("Error while Read Item Descriptor %s", FILENAME_ZNPC_DESC);
		return false;
	}

	ProcessNPCDropTableMatching();

	if (!m_MapCatalogue.ReadXml(ZApplication::GetFileSystem(), FILENAME_QUESTMAP))
	{
		mlog("Read Questmap Catalogue Failed");
		return false;
	}

	if (!ZGetAniEventMgr()->ReadXml(ZApplication::GetFileSystem(), "System/AnimationEvent.xml"))
	{
		mlog("Read Animation Event Failed");
		return false;
	}
	AniFrameInfo::m_pEventFunc = [](const RAniEventInfo& Info, v3 Pos) {
		ZGetSoundEngine()->PlaySound(Info.Filename, Pos, false);
	};

	m_bLoaded = true;
	return true;
}

void ZQuest::Reload()
{
	m_bLoaded = false;

#ifdef _DEBUG
	ZApplication::GetSkillManager()->Destroy();
	ZApplication::GetSkillManager()->Create();
#endif

	m_NPCCatalogue.Clear();
	m_MapCatalogue.Clear();
	m_DropTable.Clear();

	Load();
}

void ZQuest::OnGameCreate()
{
	if (!ZGetGameTypeManager()->IsQuestDerivedEX(ZGetGameClient()->GetMatchStageSetting()->GetGameType())) return;

	OnCreateOnce();
	Create();

}

void ZQuest::OnGameDestroy()
{
	if (!ZGetGameTypeManager()->IsQuestDerivedEX(ZGetGameClient()->GetMatchStageSetting()->GetGameType())) return;

	Destroy();

}


void ZQuest::OnGameUpdate(float fElapsed)
{
	if (!ZGetGameTypeManager()->IsQuestDerivedEX(ZGetGameClient()->GetMatchStageSetting()->GetGameType())) return;

	UpdateNavMeshWeight(fElapsed);

}


#else

	void ZQuest::OnGameCreate() {}
	void ZQuest::OnGameDestroy() {} 
	void ZQuest::OnGameUpdate(float fElapsed) {} 
	bool ZQuest::OnCreate(){return true;}
	void ZQuest::OnDestroy() {}
#endif

void ZQuest::UpdateNavMeshWeight(float fDelta)
{
	if ((ZGetGame()->GetTime() - m_fLastWeightTime) >= 1.0f)
	{
		RNavigationMesh* pNavMesh = ZGetGame()->GetWorld()->GetBsp()->GetNavigationMesh();		
		if (pNavMesh != NULL)
		{
			pNavMesh->ClearAllNodeWeight();
			ZNPCObjectMap* pNPCObjectMap = ZGetObjectManager()->GetNPCObjectMap();
			for(ZNPCObjectMap::iterator i = pNPCObjectMap->begin();i!=pNPCObjectMap->end();i++)
			{
				ZObject* pNPCObject = i->second;
				RNavigationNode* pNode = pNavMesh->FindClosestNode(pNPCObject->GetPosition());
				if (pNode)
				{
					float fWeight = pNode->GetWeight() + 1.0f;
					pNode->SetWeight(fWeight);
				}
			}
		}
		m_fLastWeightTime = ZGetGame()->GetTime();
	}

}

bool ZQuest::OnCommand(MCommand* pCommand)
{
	switch (pCommand->GetID())
	{
		HANDLE_COMMAND(MC_QUEST_GAME_INFO					,OnQuestGameInfo);
		HANDLE_COMMAND(MC_MATCH_RESPONSE_MONSTER_BIBLE_INFO ,OnSetMonsterBibleInfo);
	}

	return false;
}

bool ZQuest::OnGameCommand(MCommand* pCommand)
{
	switch (pCommand->GetID())
	{
		HANDLE_COMMAND(MC_QUEST_NPC_LOCAL_SPAWN						,OnNPCSpawn);
		HANDLE_COMMAND(MC_QUEST_NPC_SPAWN							,OnNPCSpawn);
		HANDLE_COMMAND(MC_QUEST_NPC_SPAWN_POS						,OnNPCSpawnPos);
		HANDLE_COMMAND(MC_QUEST_NPC_DEAD							,OnNPCDead);
		HANDLE_COMMAND(MC_QUEST_PEER_NPC_DEAD						,OnPeerNPCDead);
		HANDLE_COMMAND(MC_QUEST_ENTRUST_NPC_CONTROL					,OnEntrustNPCControl);
		HANDLE_COMMAND(MC_QUEST_PEER_NPC_BASICINFO					,OnPeerNPCBasicInfo);
		HANDLE_COMMAND(MC_QUEST_PEER_NPC_HPINFO						,OnPeerNPCHPInfo);
		HANDLE_COMMAND(MC_QUEST_PEER_NPC_ATTACK_MELEE				,OnPeerNPCAttackMelee);
		HANDLE_COMMAND(MC_QUEST_PEER_NPC_ATTACK_RANGE				,OnPeerNPCAttackRange);
		HANDLE_COMMAND(MC_QUEST_PEER_NPC_SKILL_START				,OnPeerNPCSkillStart);
		HANDLE_COMMAND(MC_QUEST_PEER_NPC_SKILL_EXECUTE				,OnPeerNPCSkillExecute);

		HANDLE_COMMAND(MC_QUEST_REFRESH_PLAYER_STATUS				,OnRefreshPlayerStatus);
		HANDLE_COMMAND(MC_QUEST_NPC_ALL_CLEAR						,OnClearAllNPC);
		HANDLE_COMMAND(MC_QUEST_ROUND_START							,OnQuestRoundStart);
		HANDLE_COMMAND(MC_MATCH_QUEST_PLAYER_DEAD					,OnQuestPlayerDead);
		HANDLE_COMMAND(MC_QUEST_COMBAT_STATE						,OnQuestCombatState);
		HANDLE_COMMAND(MC_QUEST_MOVETO_PORTAL						,OnMovetoPortal);
		HANDLE_COMMAND(MC_QUEST_READYTO_NEWSECTOR					,OnReadyToNewSector);
		HANDLE_COMMAND(MC_QUEST_SECTOR_START						,OnSectorStart);
		HANDLE_COMMAND(MC_QUEST_OBTAIN_QUESTITEM					,OnObtainQuestItem);
		HANDLE_COMMAND(MC_QUEST_OBTAIN_ZITEM						,OnObtainZItem);
		HANDLE_COMMAND(MC_QUEST_SECTOR_BONUS						,OnSectorBonus);

		HANDLE_COMMAND(MC_QUEST_COMPLETED							,OnQuestCompleted);
		HANDLE_COMMAND(MC_QUEST_FAILED								,OnQuestFailed);
	
		HANDLE_COMMAND(MC_QUEST_PING								,OnQuestPing);

#ifdef _QUEST_ITEM
		HANDLE_COMMAND(MC_MATCH_USER_REWARD_QUEST					,OnRewardQuest);
		HANDLE_COMMAND(MC_MATCH_NEW_MONSTER_INFO					,OnNewMonsterInfo );
#endif
		
		
	}


	return false;
}

bool ZQuest::OnNPCSpawn(MCommand* pCommand)
{
	if (g_pGame == NULL) return false;

	MUID uidChar, uidNPC;
	unsigned char nNPCType, nPositionIndex;

	pCommand->GetParameter(&uidChar,			0, MPT_UID);
	pCommand->GetParameter(&uidNPC,				1, MPT_UID);
	pCommand->GetParameter(&nNPCType,			2, MPT_UCHAR);
	pCommand->GetParameter(&nPositionIndex,		3, MPT_UCHAR);


	MQUEST_NPC NPCType = MQUEST_NPC(nNPCType);

	ZMapSpawnType nSpawnType = ZMST_NPC_MELEE;

	ZMapSpawnManager* pMSM = g_pGame->GetMapDesc()->GetSpawnManager();
	MQuestNPCInfo* pNPCInfo = GetNPCInfo(NPCType);
	if (pNPCInfo == NULL) return false;

	switch (pNPCInfo->GetSpawnType())
	{
	case MNST_MELEE: nSpawnType = ZMST_NPC_MELEE; break;
	case MNST_RANGE: nSpawnType = ZMST_NPC_RANGE; break;
	case MNST_BOSS: nSpawnType = ZMST_NPC_BOSS; break;
	default: _ASSERT(0);
	};

	ZMapSpawnData* pSpawnData = pMSM->GetSpawnData(nSpawnType, nPositionIndex);

	rvector NPCPos = rvector(0,0,0);
	rvector NPCDir = rvector(1,0,0);

	if (pSpawnData)
	{
		NPCPos = pSpawnData->m_Pos;
		NPCDir = pSpawnData->m_Dir;
	}
	
	

	// ���� ���ҽ� �ε��� �������� �ε� - �̷����� �׽�Ʈ���� ������Ѵ�.
//	if (ZIsLaunchDevelop())
	{
		RMesh* pNPCMesh = ZGetNpcMeshMgr()->Get(pNPCInfo->szMeshName);
		if (pNPCMesh)
		{
			if (!pNPCMesh->m_isMeshLoaded)
			{
				ZGetNpcMeshMgr()->Load(pNPCInfo->szMeshName);
				ZGetNpcMeshMgr()->ReloadAllAnimation();
			}
		}
	}

	float fTC = m_GameInfo.GetNPC_TC();
	ZActor* pNewActor = ZActor::CreateActor(NPCType, fTC, m_GameInfo.GetQuestLevel());
	if (pNewActor)
	{
		pNewActor->SetUID(uidNPC);
		pNewActor->SetPosition(NPCPos);
		pNewActor->SetDirection(NPCDir);
		bool bMyControl = (uidChar == ZGetGameClient()->GetPlayerUID());
		pNewActor->SetMyControl(bMyControl);
		
		ZCharacter *pOwner = ZGetCharacterManager()->Find(uidChar);
		if(pOwner)
			pNewActor->SetOwner(pOwner->GetUserName());

		if(pNewActor->m_pVMesh) {
		
			D3DCOLORVALUE color;

			color.r = pNPCInfo->vColor.x;
			color.g = pNPCInfo->vColor.y;
			color.b = pNPCInfo->vColor.z;
			color.a = 1.f;

			pNewActor->m_pVMesh->SetNPCBlendColor(color);//���� ������ ���..
		}

		ZGetObjectManager()->Add(pNewActor);
		ZGetEffectManager()->AddReBirthEffect(NPCPos);


		// ���� ������ NPC�� �����ϸ� �ڵ������� boss ���
		if ((pNPCInfo->nGrade == NPC_GRADE_BOSS) || (pNPCInfo->nGrade == NPC_GRADE_LEGENDARY))
		{
			m_GameInfo.GetBosses().push_back(uidNPC);
		}
	}

	
	


	return true;
}

/// Like OnNPCSpawn, but with precise position coordinates given instead of spawn index
bool ZQuest::OnNPCSpawnPos(MCommand* pCommand)
{
	if (g_pGame == NULL) return false;

	MUID uidChar, uidNPC;
	unsigned char nNPCType;
	MVector pos;

	pCommand->GetParameter(&uidChar,			0, MPT_UID);
	pCommand->GetParameter(&uidNPC,				1, MPT_UID);
	pCommand->GetParameter(&nNPCType,			2, MPT_UCHAR);
	pCommand->GetParameter(&pos,				3, MPT_VECTOR);


	MQUEST_NPC NPCType = MQUEST_NPC(nNPCType);

	ZMapSpawnType nSpawnType = ZMST_NPC_MELEE;

	ZMapSpawnManager* pMSM = g_pGame->GetMapDesc()->GetSpawnManager();
	MQuestNPCInfo* pNPCInfo = GetNPCInfo(NPCType);
	if (pNPCInfo == NULL) return false;

	switch (pNPCInfo->GetSpawnType())
	{
		case MNST_MELEE: nSpawnType = ZMST_NPC_MELEE; break;
		case MNST_RANGE: nSpawnType = ZMST_NPC_RANGE; break;
		case MNST_BOSS: nSpawnType = ZMST_NPC_BOSS; break;
		default: _ASSERT(0);
	};

	//ZMapSpawnData* pSpawnData = pMSM->GetSpawnData(nSpawnType, nPositionIndex);

	rvector NPCPos = rvector(pos.x, pos.y, pos.z);
	/// TODO add direction to the params?
	rvector NPCDir = rvector(1,0,0);

	RMesh* pNPCMesh = ZGetNpcMeshMgr()->Get(pNPCInfo->szMeshName);
	if (pNPCMesh)
	{
		if (!pNPCMesh->m_isMeshLoaded)
		{
			ZGetNpcMeshMgr()->Load(pNPCInfo->szMeshName);
			ZGetNpcMeshMgr()->ReloadAllAnimation();
		}
	}

	float fTC = m_GameInfo.GetNPC_TC();
	ZActor* pNewActor = ZActor::CreateActor(NPCType, fTC, m_GameInfo.GetQuestLevel());
	if (pNewActor)
	{
		pNewActor->SetUID(uidNPC);
		pNewActor->SetPosition(NPCPos);
		pNewActor->SetDirection(NPCDir);
		bool bMyControl = (uidChar == ZGetGameClient()->GetPlayerUID());
		pNewActor->SetMyControl(bMyControl);
		
		ZCharacter *pOwner = ZGetCharacterManager()->Find(uidChar);
		if(pOwner)
			pNewActor->SetOwner(pOwner->GetUserName());

		if(pNewActor->m_pVMesh) {
		
			D3DCOLORVALUE color;

			color.r = pNPCInfo->vColor.x;
			color.g = pNPCInfo->vColor.y;
			color.b = pNPCInfo->vColor.z;
			color.a = 1.f;

			pNewActor->m_pVMesh->SetNPCBlendColor(color);
		}

		ZGetObjectManager()->Add(pNewActor);
		ZGetEffectManager()->AddReBirthEffect(NPCPos);


		if ((pNPCInfo->nGrade == NPC_GRADE_BOSS) || (pNPCInfo->nGrade == NPC_GRADE_LEGENDARY))
		{
			m_GameInfo.GetBosses().push_back(uidNPC);
		}
	}

	return true;
}

bool ZQuest::OnNPCDead(MCommand* pCommand)
{
	MUID uidPlayer, uidNPC;

	pCommand->GetParameter(&uidPlayer,	0, MPT_UID);
	pCommand->GetParameter(&uidNPC,		1, MPT_UID);

	ZActor* pActor = ZGetObjectManager()->GetNPCObject(uidNPC);
	if (pActor)
	{
		pActor->OnDie();
		ZGetObjectManager()->Delete(pActor);

		m_GameInfo.IncreaseNPCKilled();

		ZCharacter* pCharacter = ZGetCharacterManager()->Find(uidPlayer);
		if (pCharacter)
		{
			ZModule_QuestStatus* pMod = (ZModule_QuestStatus*)pCharacter->GetModule(ZMID_QUESTSTATUS);
			if (pMod)
			{
				pMod->AddKills();
			}
		}
	}

	return true;
}

/*
bool ZQuest::OnQuestGroupLoad(MCommand* pCommand)
{
	if(!pCommand) return false;

	int nGroupID = 0;

	pCommand->GetParameter(&nGroupID, 0, MPT_INT);

	MNPCGroup* pGroup = MGetNPCGroupMgr()->GetGroup( nGroupID );

	ZGetNpcMeshMgr()->CheckUnUsed();// ��� �����ϴ°ɷ� üũ�ϰ�

	if( pGroup ) {

		MNPCList::iterator it;

		for( it = pGroup->m_NpcList.begin();it!=pGroup->m_NpcList.end();++it) {

			ZGetNpcMeshMgr()->Load( (char*)(*it).c_str() );
		}
	}

	ZGetNpcMeshMgr()->ReloadAllAnimation();// ���� ���� ���ϸ��̼��� �ִٸ� �ε�

	ZGetNpcMeshMgr()->UnLoadChecked();// �����ϴ� �� ����

	return true;
}

bool ZQuest::OnQuestGroupClear(MCommand* pCommand)
{
	ZGetNpcMeshMgr()->UnLoadAll();

	return true;
}
*/

bool ZQuest::OnEntrustNPCControl(MCommand* pCommand)
{
	MUID uidChar, uidNPC;
	pCommand->GetParameter(&uidChar,	0, MPT_UID);
	pCommand->GetParameter(&uidNPC,		1, MPT_UID);

	ZActor* pNPC = ZGetObjectManager()->GetNPCObject(uidNPC);
	if (pNPC)
	{
		// uidChar�� ���÷��̾� UID�̸� �ش� NPC�� ���� �����ϴ� ���̴�.
		bool bMyControl = (uidChar == ZGetGameClient()->GetPlayerUID());
		pNPC->SetMyControl(bMyControl);

		ZCharacter *pOwner = ZGetCharacterManager()->Find(uidChar);
		if(pOwner)
			pNPC->SetOwner(pOwner->GetUserName());
	}

	return true;
}

bool ZQuest::OnPeerNPCBasicInfo(MCommand* pCommand)
{
	MCommandParameter* pParam = pCommand->GetParameter(0);
	if(pParam->GetType()!=MPT_BLOB) return false;

	ZACTOR_BASICINFO* ppbi= (ZACTOR_BASICINFO*)pParam->GetPointer();
	
	ZBasicInfo bi;
	bi.position = rvector(ppbi->posx,ppbi->posy,ppbi->posz);
	bi.velocity = rvector(ppbi->velx,ppbi->vely,ppbi->velz);
	bi.direction = 1.f/32000.f * rvector(ppbi->dirx,ppbi->diry,ppbi->dirz);
	
	ZActor* pActor = ZGetObjectManager()->GetNPCObject(ppbi->uidNPC);
	if (pActor)
	{
		pActor->InputBasicInfo(&bi, ppbi->anistate);
	}


/*
	// ĳ������ ����ð��� ������Ʈ�Ѵ�
	// ĳ������ ����ð� ����ġ
	float fCurrentLocalTime = pActor->m_fTimeOffset + GetTime();

	// ĳ���Ͱ� ������ �ð��� ���� ������ �ð��� 3�� �̻� ���̰� ���� ���� �˰��ִ� �ð��� ��ģ��.
	float fTimeError = ppbi->fTime - fCurrentLocalTime;
	if(fabs(fTimeError)>3.f) {
		pCharacter->m_fTimeOffset = ppbi->fTime - GetTime();
		pCharacter->m_fAccumulatedTimeError = 0;
		pCharacter->m_nTimeErrorCount = 0;
	}else
	{
		// ���̰� 3�� �̳��̰� �����ð� ���ߴٰ� ����(������ ��)�� �����Ѵ�
		pCharacter->m_fAccumulatedTimeError += fTimeError;
		pCharacter->m_nTimeErrorCount ++;
		if(pCharacter->m_nTimeErrorCount > 10) {
			pCharacter->m_fTimeOffset += .5f*pCharacter->m_fAccumulatedTimeError/10.f;
			pCharacter->m_fAccumulatedTimeError = 0;
			pCharacter->m_nTimeErrorCount = 0;
		}
	}

	// ����ð��� ������ ������ �����ð����� ���.
	pCharacter->m_fLastReceivedTime = GetTime();
*/

/*
	// ���߿� ������ ���� histroy �� �����Ѵ�.
	if(bAddHistory)
	{
		ZBasicInfoItem *pitem=new ZBasicInfoItem;
		CopyMemory(&pitem->info, &bi, sizeof(ZBasicInfo));

		pitem->fReceivedTime=GetTime();

		pitem->fSendTime= ppbi->fTime - pCharacter->m_fTimeOffset;	// �� �������� ��ȯ
		
		pCharacter->m_BasicHistory.push_back(pitem);

		while(pCharacter->m_BasicHistory.size()>CHARACTER_HISTROY_COUNT)
		{
			delete *pCharacter->m_BasicHistory.begin();
			pCharacter->m_BasicHistory.erase(pCharacter->m_BasicHistory.begin());
		}
	}

	if(bUpdate)
	{
		// ���÷��̶��� �����ϰ� �� ĳ���ʹ� ����̳� �ӵ������ ������Ʈ �� �ʿ䰡 ����.
		if(!IsReplay() && pCharacter->IsHero()) return;


		((ZNetCharacter*)(pCharacter))->SetNetPosition(bi.position, bi.velocity, bi.direction);

		pCharacter->SetAnimationLower((ZC_STATE_LOWER)ppbi->lowerstate);
		pCharacter->SetAnimationUpper((ZC_STATE_UPPER)ppbi->upperstate);

		// ����ִ� ���Ⱑ �ٸ��� �ٲ��ش�
		if(pCharacter->GetItems()->GetSelectedWeaponParts()!=ppbi->selweapon) {
			pCharacter->ChangeWeapon((MMatchCharItemParts)ppbi->selweapon);
		}
	}
*/

	return true;
}

bool ZQuest::OnPeerNPCHPInfo(MCommand* pCommand)
{

	return true;
}

bool ZQuest::OnPrePeerNPCAttackMelee(MCommand* pCommand)	// ������ ó���ϴ°� ��Ÿ�̹� �ʴ�
{
	// TODO: �̶� �ִϸ��̼��� ����
	return true;
}

bool ZQuest::OnPeerNPCAttackMelee(MCommand* pCommand)
{
	MUID uidOwner;
	pCommand->GetParameter(&uidOwner,	0, MPT_UID);

	ZGetGame()->OnPeerShot_Melee(uidOwner,ZGetGame()->GetTime());

	return true;
}

bool ZQuest::OnPeerNPCAttackRange(MCommand* pCommand)
{
	MUID uidOwner;
	pCommand->GetParameter(&uidOwner,	0, MPT_UID);

	MCommandParameter* pParam = pCommand->GetParameter(1);
	if(pParam->GetType()!=MPT_BLOB) return false;	// ������ �ִ�

	ZPACKEDSHOTINFO *pinfo =(ZPACKEDSHOTINFO*)pParam->GetPointer();
	rvector pos = rvector(pinfo->posx,pinfo->posy,pinfo->posz);
	rvector to = rvector(pinfo->tox,pinfo->toy,pinfo->toz);


	// rocket �׽�Ʈ�� �־�ô�.
	ZObject* pOwner = ZGetGame()->m_ObjectManager.GetObject(uidOwner);
	MMatchItemDesc* pDesc = NULL;

	if(pOwner==NULL) return false; // ���� ġƮŰ�� �����...

	if( pOwner->GetItems() )
		if( pOwner->GetItems()->GetSelectedWeapon() )
			pDesc = pOwner->GetItems()->GetSelectedWeapon()->GetDesc();
	if (pDesc)
	{
		if (pDesc->m_nWeaponType == MWT_ROCKET)
		{
			rvector dir = to - pos;
			Normalize(dir);
			ZGetGame()->m_WeaponManager.AddRocket(pos,dir,pOwner);
			ZApplication::GetSoundEngine()->PlaySEFire(pDesc, pos.x, pos.y, pos.z, false);

			return true;
		}
		else if (pDesc->m_nWeaponType == MWT_SKILL)
		{
			rvector dir = to - pos;
			Normalize(dir);

			ZSkill skill;
			skill.Init(pDesc->m_nGadgetID, pOwner);

			ZApplication::GetSoundEngine()->PlaySEFire(pDesc, pos.x, pos.y, pos.z, false);

			ZGetGame()->m_WeaponManager.AddMagic(&skill, pos, dir, pOwner);
			return true;
		}

	}

	ZGetGame()->OnPeerShot_Range((MMatchCharItemParts)pinfo->sel_type,pOwner,
		ZGetGame()->GetTime(),pos,to, reinterpret<u32>(ZGetGame()->GetTime()));
	

	return true;
}

bool ZQuest::OnRefreshPlayerStatus(MCommand* pCommand)
{
	// ��� hide�� ����
	bool bAdminHide = false;
	if (ZGetMyInfo()->IsAdminGrade()) 
	{
		MMatchObjCache* pCache = ZGetGameClient()->FindObjCache(ZGetMyUID());
		if (pCache && pCache->CheckFlag(MTD_PlayerFlags_AdminHide))
			bAdminHide = true;
	}

	if (!bAdminHide)
	{
		// �������̰ų� ������ ������¸� Ǭ��.
		ZGetGame()->ReleaseObserver();

		// �׾������� ������
		if (ZGetGame()->m_pMyCharacter->IsDie())
		{
			ZGetGame()->GetMatch()->RespawnSolo();
		}
	}


	// �ǿ� �Ѿ��� ä���
	for(ZCharacterManager::iterator i = ZGetCharacterManager()->begin();i!=ZGetCharacterManager()->end();i++)
	{
		ZCharacter* pCharacter = i->second;
		if (!pCharacter->IsAdminHide())	pCharacter->InitStatus();
	}



	ZGetGame()->CancelSuicide();

	return true;
}

bool ZQuest::OnClearAllNPC(MCommand* pCommand)
{
	ZGetObjectManager()->ClearNPC();

	return true;
}

bool ZQuest::OnQuestRoundStart(MCommand* pCommand)
{
	unsigned char nRound;
	pCommand->GetParameter(&nRound,		0, MPT_UCHAR);

	ZGetScreenEffectManager()->AddRoundStart(int(nRound));

	// ��������� �ʱ�ȭ
	ZGetWorldItemManager()->Reset();

	return true;
}

bool ZQuest::OnQuestPlayerDead(MCommand* pCommand)
{
	MUID uidVictim;
	pCommand->GetParameter(&uidVictim,		0, MPT_UID);

	ZCharacter* pVictim = ZGetCharacterManager()->Find(uidVictim);

	if(pVictim)
	{
		if (pVictim != ZGetGame()->m_pMyCharacter)
		{
			pVictim->Die();		// ���⼭ ������ �״´�
		}

		ZCharacterStatus* pVictimCS = NULL;
		pVictimCS = pVictim->GetStatus();
		pVictimCS->AddDeaths();
		if (pVictimCS->nLife > 0) pVictimCS->nLife--;
	}


	//ZGetGame()->OnPeerDieMessage(pVictim, pAttacker);
	

	return true;
}


bool ZQuest::OnQuestGameInfo(MCommand* pCommand)
{
	MCommandParameter* pParam = pCommand->GetParameter(0);
	if(pParam->GetType()!=MPT_BLOB) return false;
	void* pBlob = pParam->GetPointer();

	MTD_QuestGameInfo* pQuestGameInfo= (MTD_QuestGameInfo*)MGetBlobArrayElement(pBlob, 0);
	
	m_GameInfo.Init(pQuestGameInfo);


	return true;
}

bool ZQuest::OnQuestCombatState(MCommand* pCommand)
{
	char nState;
	pCommand->GetParameter(&nState,		0, MPT_CHAR);

	MQuestCombatState nCombatState = MQuestCombatState(nState);

	m_QuestCombatState = nCombatState; // ����..

	switch (nCombatState)
	{
	case MQUEST_COMBAT_PREPARE:
		{
		}
		break;
	case MQUEST_COMBAT_PLAY:
		{
		}
		break;
	case MQUEST_COMBAT_COMPLETED:
		{
			// ������ ���ʹ� ���� ��ũ�� ����.
			if (!m_GameInfo.IsCurrSectorLastSector())
			{
				if (g_pGame)
				{
					rvector portal_pos;
					int nCurrSectorIndex = m_GameInfo.GetCurrSectorIndex();
					int nLinkIndex = m_GameInfo.GetMapSectorLink(nCurrSectorIndex);
					portal_pos = g_pGame->GetMapDesc()->GetQuestSectorLink(nLinkIndex);
					ZGetWorldItemManager()->AddQuestPortal(portal_pos);
					m_GameInfo.SetPortalPos(portal_pos);

					ZChatOutput(MCOLOR(ZCOLOR_CHAT_SYSTEM_GAME), ZMsg(MSG_GAME_OPEN_PORTAL));

					m_tRemainedTime = GetGlobalTimeMS() + PORTAL_MOVING_TIME;
					m_bIsRoundClear = true;
				}
			}
		}
		break;
	};

	return true;
}


bool ZQuest::OnMovetoPortal(MCommand* pCommand)
{
	char nCurrSectorIndex;
	MUID uidPlayer;

	pCommand->GetParameter(&nCurrSectorIndex,		0, MPT_CHAR);
	pCommand->GetParameter(&uidPlayer,				1, MPT_UID);

	// ��Ż�� �̵��� ����� �ڽ��̸� ���⼭ ������ ���� ���ͷ� �̵�
	if (uidPlayer == ZGetGameClient()->GetPlayerUID())
	{
		m_bIsRoundClear = false;
		ZGetQuest()->GetGameInfo()->ClearNPCKilled();

		// ���⼭ ���ο� ���ͷ� �̵�
		m_GameInfo.OnMovetoNewSector((int)(nCurrSectorIndex));

		// �� ���ο� ���ͷ� �Դٰ� �޽����� ������.
		ZPostQuestReadyToNewSector(ZGetGameClient()->GetPlayerUID());
	}
	else
	{
		// �ش� �÷��̾� �̵�
		ZCharacter *pChar = ZGetCharacterManager()->Find(uidPlayer);
		if(pChar && m_CharactersGone.find(ZGetGameClient()->GetPlayerUID())==m_CharactersGone.end()) {
			// ���� ���� �̵����� ���� ��� �ش��÷��̾ �Ⱥ��̰� �����
			pChar->SetVisible(false);
			ZGetEffectManager()->AddReBirthEffect(pChar->GetPosition());
		}
	}


	return true;
}

bool ZQuest::OnReadyToNewSector(MCommand* pCommand)
{
	MUID uidPlayer;
	pCommand->GetParameter(&uidPlayer,	0, MPT_UID);

	m_CharactersGone.insert(uidPlayer);

	ZCharacter *pChar = ZGetCharacterManager()->Find(uidPlayer);

	// ���� ������ �ϰ� �ִ� ĳ���Ͱ� �̵��ϸ� �ٸ� ĳ���ͷ� �ٲ۴�
	if(ZGetCombatInterface()->GetObserver()->GetTargetCharacter()==pChar) {
		ZGetCombatInterface()->GetObserver()->ChangeToNextTarget();
	}

	if (uidPlayer == ZGetGameClient()->GetPlayerUID()) {
		MoveToNextSector();
	}
	else
	{
		// �ش� �÷��̾� �̵�

		ZCharacter *pChar = ZGetCharacterManager()->Find(uidPlayer);
		if(pChar && m_CharactersGone.find(ZGetGameClient()->GetPlayerUID())!=m_CharactersGone.end()) {

			// ���� �̹� �̵��� ��� �ش��÷��̾ ���̰� �����
//			pChar->SetVisible(true);

			// �̹��� �̵��� ĳ������ ��ġ
			int nPosIndex = ZGetCharacterManager()->GetCharacterIndex(pChar->GetUID(), false);
			if (nPosIndex < 0) nPosIndex=0;
			else if (nPosIndex >= MAX_QUSET_PLAYER_COUNT) nPosIndex = MAX_QUSET_PLAYER_COUNT-1;
			ZMapSpawnData* pSpawnData = ZGetWorld()->GetDesc()->GetSpawnManager()->GetSoloData(nPosIndex);
			if(pSpawnData) {
				pChar->SetPosition(pSpawnData->m_Pos);
				pChar->SetDirection(pSpawnData->m_Dir);
				ZGetEffectManager()->AddReBirthEffect(pSpawnData->m_Pos);
			}
			else
			{
				_ASSERT(0);
			}
		}

	}

	return true;
}

bool ZQuest::OnSectorStart(MCommand* pCommand)
{
	char nSectorIndex;
	pCommand->GetParameter(&nSectorIndex,	0, MPT_CHAR);

	m_bIsRoundClear = false;
	ZGetQuest()->GetGameInfo()->ClearNPCKilled();

	// ���� ���Ͱ� Ʋ���� ������ �̵��Ѵ�.
	if (m_GameInfo.GetCurrSectorIndex() != nSectorIndex)
	{
		// ���ο� ���ͷ� �̵�
		m_GameInfo.OnMovetoNewSector((int)nSectorIndex);
		
		MoveToNextSector();
	}

	// ��� ������� �����ش�.
	for(ZCharacterManager::iterator i = ZGetCharacterManager()->begin();i!=ZGetCharacterManager()->end();i++)
	{
		i->second->SetVisible(true);
	}

	ZGetWorldItemManager()->Reset();
	m_CharactersGone.clear();

	// admin hide �̸� �ٽ� �������� Ȱ��ȭ
	MMatchObjCache* pObjCache = ZGetGameClient()->FindObjCache(ZGetMyUID());
	if (pObjCache && pObjCache->CheckFlag(MTD_PlayerFlags_AdminHide)) {
		ZGetGameInterface()->GetCombatInterface()->SetObserverMode(true);
	}

	return true;
}

bool ZQuest::OnQuestCompleted(MCommand* pCommand)
{
	m_bIsQuestComplete = true;

	MCommandParameter* pParam = pCommand->GetParameter(0);
	if(pParam->GetType()!=MPT_BLOB) return false;

	void* pBlob = pParam->GetPointer();
	int nBlobSize = MGetBlobArrayCount(pBlob);

	for (int i = 0; i < nBlobSize; i++)
	{
		MTD_QuestReward* pQuestRewardNode = (MTD_QuestReward*)MGetBlobArrayElement(pBlob, i);


		// ���⼭ ���� ������ �� ���� �����ؼ� ȭ�鿡 �����ָ� �˴ϴ�. - bird
	}


	// ����������� ��� ���ش�.
	//ZGetWorldItemManager()->Reset(true);

	mlog("Quest Completed\n");
	return true;
}

bool ZQuest::OnQuestFailed(MCommand* pCommand)
{
	mlog("Quest Failed\n");

#ifdef _QUEST_ITEM
	m_bIsQuestComplete = false;
#endif

	return true;
}

bool ZQuest::OnObtainQuestItem(MCommand* pCommand)
{
	u32 nQuestItemID;
	pCommand->GetParameter(&nQuestItemID,	0, MPT_UINT);

	m_GameInfo.IncreaseObtainQuestItem();

#ifdef _QUEST_ITEM
	MQuestItemDesc* pQuestItemDesc = GetQuestItemDescMgr().FindQItemDesc((int)nQuestItemID);
	if (pQuestItemDesc)
	{
		char szMsg[ 128];
		ZTransMsg( szMsg, MSG_GAME_GET_QUEST_ITEM, 1, pQuestItemDesc->m_szQuestItemName);
		ZChatOutput( MCOLOR(ZCOLOR_CHAT_SYSTEM_GAME), szMsg);
	}
#endif


	return true;
}

bool ZQuest::OnObtainZItem(MCommand* pCommand)
{
	u32 nItemID;
	pCommand->GetParameter(&nItemID,	0, MPT_UINT);

	m_GameInfo.IncreaseObtainQuestItem();

#ifdef _QUEST_ITEM
	MMatchItemDesc* pItemDesc = MGetMatchItemDescMgr()->GetItemDesc(nItemID);
	if (pItemDesc)
	{
		char szMsg[ 128];
		ZTransMsg( szMsg, MSG_GAME_GET_QUEST_ITEM, 1, pItemDesc->m_szName);
		ZChatOutput( MCOLOR(ZCOLOR_CHAT_SYSTEM_GAME), szMsg);
	}
#endif


	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
void ZQuest::LoadNPCMeshes()
{
#ifdef _DEBUG
	// AI
	if ((ZApplication::GetInstance()->GetLaunchMode() == ZApplication::ZLAUNCH_MODE_STANDALONE_QUEST) || 
		(ZApplication::GetInstance()->GetLaunchMode() == ZApplication::ZLAUNCH_MODE_STANDALONE_AI))
	{
		ZGetNpcMeshMgr()->LoadAll();

		ZGetNpcMeshMgr()->ReloadAllAnimation();
		return;
	}
#endif

	if (!m_GameInfo.IsInited()) 
	{
		mlog("not inialized Quest Game Info\n");
		_ASSERT(0);
		return;
	}

	for (int i = 0; i < m_GameInfo.GetNPCInfoCount(); i++)
	{
		MQUEST_NPC npc = m_GameInfo.GetNPCInfo(i);

		ZGetNpcMeshMgr()->Load(GetNPCInfo(npc)->szMeshName);
	}

	ZGetNpcMeshMgr()->ReloadAllAnimation();
}

void ZQuest::LoadNPCMeshesAll()
{
	ZGetNpcMeshMgr()->LoadAll();
	ZGetNpcMeshMgr()->ReloadAllAnimation();
}

void ZQuest::LoadNPCSounds()
{
	if (!m_GameInfo.IsInited())
	{
		mlog("ZQuest::LoadNPCSounds - not inialized Quest Game Info\n");
		return;
	}

	ZSoundEngine* pSE = ZApplication::GetSoundEngine();
	if (pSE == NULL) return;


	for (int i = 0; i < m_GameInfo.GetNPCInfoCount(); i++)
	{
		MQUEST_NPC npc = m_GameInfo.GetNPCInfo(i);
		if (!pSE->LoadNPCResource(npc))
		{
			mlog("failed npc sound\n");
		}
	}
}

void ZQuest::LoadNPCSoundsAll()
{
	if (!m_GameInfo.IsInited())
	{
		mlog("ZQuest::LoadNPCSoundsAll - not inialized Quest Game Info\n");
		return;
	}

	ZSoundEngine* pSE = ZApplication::GetSoundEngine();
	if (pSE == NULL) return;


	for (const auto npc : AllNpcs) {
		if (!pSE->LoadNPCResource(npc))
		{
			mlog("failed all npc sound\n");
		}
	}
}

void ZQuest::MoveToNextSector()
{
	ZCharacter *pMyChar = ZGetGame()->m_pMyCharacter;
	pMyChar->InitStatus();

	// ���ο� ����� �̵�!!
	ZGetWorldManager()->SetCurrent(m_GameInfo.GetCurrSectorIndex());
	// �̹��� �̵��� ĳ������ ��ġ
	int nPosIndex = ZGetCharacterManager()->GetCharacterIndex(pMyChar->GetUID(), false);
	if (nPosIndex < 0) nPosIndex=0;
	ZMapSpawnData* pSpawnData = ZGetWorld()->GetDesc()->GetSpawnManager()->GetSoloData(nPosIndex);
	// �� ��ǥ�� �̵�
	if (pSpawnData!=NULL && pMyChar!=NULL)
	{
		pMyChar->SetPosition(pSpawnData->m_Pos);
		pMyChar->SetDirection(pSpawnData->m_Dir);
		ZGetEffectManager()->AddReBirthEffect(pSpawnData->m_Pos);
	}

	// �ƹ��� �������� �ʴ´�.
	for(ZCharacterManager::iterator i = ZGetCharacterManager()->begin();i!=ZGetCharacterManager()->end();i++)
	{
		i->second->SetVisible(false);
	}

	// ko�� ����ȭ
	ZModule_QuestStatus* pMod = (ZModule_QuestStatus*)pMyChar->GetModule(ZMID_QUESTSTATUS);
	if (pMod)
	{
		int nKills = pMod->GetKills();
		ZGetScreenEffectManager()->SetKO(nKills);
	}
}

#ifdef _QUEST_ITEM
bool ZQuest::OnRewardQuest( MCommand* pCmd )
{
	if( 0 == pCmd )
		return false;
	
	int nXP, nBP;

	pCmd->GetParameter( &nXP, 0, MPT_INT );
	pCmd->GetParameter( &nBP, 1, MPT_INT );
	MCommandParameter* pParam1 = pCmd->GetParameter( 2 );
	MCommandParameter* pParam2 = pCmd->GetParameter( 3 );

	GetMyObtainQuestItemList( nXP, nBP, pParam1->GetPointer(), pParam2->GetPointer() );
	
	return true;
}

// ȹ�� ������ ����Ʈ �ڽ� ������Ʈ
class ObtainItemListBoxItem : public MListItem
{
protected:
	MBitmap*			m_pBitmap;
	char				m_szName[ 128];

public:
	ObtainItemListBoxItem( MBitmap* pBitmap, const char* szName)
	{
		m_pBitmap = pBitmap;
		strcpy_safe( m_szName, szName);
	}
	virtual const char* GetString( void)
	{
		return m_szName;
	}
	virtual const char* GetString( int i)
	{
		if ( i == 1)
			return m_szName;

		return NULL;
	}
	virtual MBitmap* GetBitmap( int i)
	{
		if ( i == 0)
			return m_pBitmap;

		return NULL;
	}
};
void ZQuest::GetMyObtainQuestItemList( int nRewardXP, int nRewardBP, void* pMyObtainQuestItemListBlob, void* pMyObtainZItemListBlob )
{
	m_nRewardXP = nRewardXP;
	m_nRewardBP = nRewardBP;

	if (( 0 == pMyObtainQuestItemListBlob ) || ( 0 == pMyObtainZItemListBlob )) return;

	int							i;
	int							nQuestItemCount;
	MTD_QuestItemNode*			pQuestItemNode;
	ZMyQuestItemNode*			pNewQuestItem;
	ZMyQuestItemMap::iterator	itQItem;
	ZMyInfo*					pMyInfo;

	pMyInfo = ZGetMyInfo();
	MListBox* pListBox = (MListBox*)ZGetGameInterface()->GetIDLResource()->FindWidget( "QuestResult_ItemListbox");
	if ( pListBox)
		pListBox->RemoveAll();

	// ����Ʈ ������ -----------------
	nQuestItemCount = MGetBlobArrayCount( pMyObtainQuestItemListBlob );

	for( i = 0; i < nQuestItemCount; ++i )
	{
		pQuestItemNode = reinterpret_cast< MTD_QuestItemNode* >( MGetBlobArrayElement(pMyObtainQuestItemListBlob, i) );

		// ����Ʈ �ڽ� ������Ʈ
		if ( pListBox && (pQuestItemNode->m_nCount > 0))
		{
			MQuestItemDesc* pQuestItemDesc = GetQuestItemDescMgr().FindQItemDesc( pQuestItemNode->m_nItemID);
			char szNum[ 10];
			sprintf_safe( szNum, "%d", pQuestItemNode->m_nCount);
			char szMsg[ 128];
			ZTransMsg( szMsg, MSG_GAME_GET_QUEST_ITEM2, 2, pQuestItemDesc->m_szQuestItemName, szNum);
			pListBox->Add( new ObtainItemListBoxItem( ZGetGameInterface()->GetQuestItemIcon( pQuestItemNode->m_nItemID, true), szMsg));
		}


		itQItem = pMyInfo->GetItemList()->GetQuestItemMap().find( pQuestItemNode->m_nItemID );
		if( pMyInfo->GetItemList()->GetQuestItemMap().end() != itQItem )
		{
			// �ɸ����� ������ �ִ� ����Ʈ �����ۿ� �߰�.
			if( 0 != pQuestItemNode->m_nCount )
			{
				// ������ �ִ�ġ�� �ʰ����� �ʾƼ� ���������� �߰��� �Ǿ��� ���.
				itQItem->second->Increase( pQuestItemNode->m_nCount );
			}
			else
			{
				// ������ �ִ�ġ�� �ʰ��Ͽ��� �߰��� ������ 0���� �������� ���.
				// �̶��� �߰������� Ŭ���̾�Ʈ�� �ʰ��� ������ �׿����� ó�������� ���� ��� ��.
			}
		}
		else
		{
			// ���� ��ϵǴ� �������� �ʱ�ȭ�� ���ְ� ����� ����� ��.

			pNewQuestItem = new ZMyQuestItemNode;
			if( 0 == pNewQuestItem )
			{
				continue;
			}

			pNewQuestItem->Create( pQuestItemNode->m_nItemID, 
								   pQuestItemNode->m_nCount, 
								   GetQuestItemDescMgr().FindQItemDesc(pQuestItemNode->m_nItemID) );

			pMyInfo->GetItemList()->GetQuestItemMap().Add( pNewQuestItem->GetItemID(), pNewQuestItem );
		}
	}

	// �Ϲ� ������ -----------------
	int nZItemCount = MGetBlobArrayCount( pMyObtainZItemListBlob );
	for (int i = 0; i < nZItemCount; i++)
	{
		MTD_QuestZItemNode* pZItemNode = (MTD_QuestZItemNode*)( MGetBlobArrayElement(pMyObtainZItemListBlob, i) );

		// ����Ʈ �ڽ� ������Ʈ
		if ( pListBox )
		{
			MMatchItemDesc* pItemDesc = MGetMatchItemDescMgr()->GetItemDesc(pZItemNode->m_nItemID);

			char szMsg[ 128];
			ZTransMsg( szMsg, MSG_GAME_GET_QUEST_ITEM2, 2, pItemDesc->m_szName, "1");
			pListBox->Add( new ObtainItemListBoxItem( GetItemIconBitmap( pItemDesc, true), szMsg));
		}
	}

}


bool ZQuest::OnNewMonsterInfo( MCommand* pCmd )
{
	char nMonIndex;

	if( 0 == pCmd )
		return false;

	pCmd->GetParameter( &nMonIndex, 0, MPT_CHAR );
	return true;
}

#endif

bool ZQuest::OnPeerNPCSkillStart(MCommand* pCommand)
{
	MUID uidOwner,uidTarget;
	int nSkill;
	rvector targetPos;
	pCommand->GetParameter(&uidOwner,	0, MPT_UID);
	pCommand->GetParameter(&nSkill,		1, MPT_INT);
	pCommand->GetParameter(&uidTarget,	2, MPT_UID);
	pCommand->GetParameter(&targetPos,	3, MPT_POS);

	ZActor* pActor = ZGetObjectManager()->GetNPCObject(uidOwner);
	if (pActor)
	{
		ZModule_Skills *pmod = (ZModule_Skills *)pActor->GetModule(ZMID_SKILLS);
		pmod->PreExcute(nSkill,uidTarget,targetPos);
	}

	return true;
}

bool ZQuest::OnPeerNPCSkillExecute(MCommand* pCommand)
{
	MUID uidOwner,uidTarget;
	int nSkill;
	rvector targetPos;
	pCommand->GetParameter(&uidOwner,	0, MPT_UID);
	pCommand->GetParameter(&nSkill,		1, MPT_INT);
	pCommand->GetParameter(&uidTarget,	2, MPT_UID);
	pCommand->GetParameter(&targetPos,	3, MPT_POS);

	ZActor* pActor = ZGetObjectManager()->GetNPCObject(uidOwner);
	if (pActor)
	{
		ZModule_Skills *pmod = (ZModule_Skills *)pActor->GetModule(ZMID_SKILLS);
		pmod->Excute(nSkill,uidTarget,targetPos);
	}

	return true;
}


bool ZQuest::OnSetMonsterBibleInfo( MCommand* pCmd )
{
	if( 0 == pCmd )
		return false;

	MUID				uid;
	MCommandParameter*	pParam;
	void*				pMonBibleInfoBlob;
	MQuestMonsterBible*	pMonBible;

	pCmd->GetParameter( &uid, 0, MPT_UID );

	if( uid != ZGetGameClient()->GetPlayerUID() )
	{
		return false;
	}

	pParam				= pCmd->GetParameter(1);
	pMonBibleInfoBlob	= pParam->GetPointer();
	pMonBible			= reinterpret_cast< MQuestMonsterBible* >( MGetBlobArrayElement(pMonBibleInfoBlob, 0) );

	return true;
}

bool ZQuest::OnPeerNPCDead(MCommand* pCommand)
{
	MUID uidKiller, uidNPC;

	pCommand->GetParameter(&uidKiller,	0, MPT_UID);
	pCommand->GetParameter(&uidNPC,		1, MPT_UID);

	ZActor* pActor = ZGetObjectManager()->GetNPCObject(uidNPC);
	if (pActor)
	{
		pActor->OnPeerDie(uidKiller);

		if (uidKiller == ZGetMyUID())
		{
			ZModule_QuestStatus* pMod = (ZModule_QuestStatus*)g_pGame->m_pMyCharacter->GetModule(ZMID_QUESTSTATUS);
			if (pMod)
			{
				ZGetScreenEffectManager()->AddKO();
			}
		}
	}

	return true;
}

bool ZQuest::OnSectorBonus(MCommand* pCommand)
{
	MUID uidPlayer;
	u32 nExpValue = 0;
	pCommand->GetParameter(&uidPlayer,	0, MPT_UID);
	pCommand->GetParameter(&nExpValue,	1, MPT_UINT);


	int nAddedXP = GetExpFromTransData(nExpValue);
	int nExpPercent = GetExpPercentFromTransData(nExpValue);


	if(ZGetCharacterManager()->Find(uidPlayer) == g_pGame->m_pMyCharacter)
	{
		ZGetScreenEffectManager()->AddExpEffect(nAddedXP);
		ZGetMyInfo()->SetLevelPercent(nExpPercent);
		ZGetScreenEffectManager()->SetGuageExpFromMyInfo();
	}

	return true;
}


bool ZQuest::OnQuestPing(MCommand* pCommand)
{
	u32 TimeStamp;
	pCommand->GetParameter(&TimeStamp,	0, MPT_UINT);
	ZPostQuestPong(TimeStamp);

	return true;
}