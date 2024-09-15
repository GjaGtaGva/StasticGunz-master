#pragma once

#include "MMatchRule.h"
#include "MMatchRuleBaseQuest.h"
#include "MMatchNPCObject.h"
#include "MMatchRuleQuest.h"
#include "MMatchQuestRound.h"
#include "MSacrificeQItemTable.h"
#include "MQuestItem.h"
#include "MMatchQuestGameLog.h"
#include "MQuestNPCSpawnTrigger.h"
#include "MQuestLevel.h"
#include "MMatchGameType.h"

typedef std::pair< MUID, u32 > SacrificeSlot;


class MMatchRuleExploration : public MMatchRuleBaseQuest {
private:
	struct MQuestStageGameInfo
	{
		int				nQL;
		int				nPlayerQL;
		int				nMapsetID;
		unsigned int	nScenarioID;
	};

	enum COMBAT_PLAY_RESULT_EX
	{
		CPR_PLAYING = 0,
		CPR_COMPLETE,
		CPR_FAILED
	};

	u64	m_nPrepareStartTime;
	u64	m_nCombatStartTime;
	u64	m_nQuestCompleteTime;

	MQuestSacrificeSlot				m_SacrificeSlot[ MAX_SACRIFICE_SLOT_COUNT ];
	int								m_nPlayerCount;
	MMatchQuestGameLogInfoManager	m_QuestGameLogInfoMgr;

	MQuestStageGameInfo				m_StageGameInfo;

	void ClearQuestLevel();
	void MakeStageGameInfo();
	void InitJacoSpawnTrigger();
	void MakeNPCnSpawn(MQUEST_NPC nNPCID, bool bAddQuestDropItem);
protected:
	MQuestLevel*			m_pQuestLevel;
	MQuestNPCSpawnTrigger	m_JacoSpawnTrigger;
	MQuestCombatState		m_nCombatState;

	virtual void ProcessNPCSpawn();
	virtual bool CheckNPCSpawnEnable();
	virtual void RouteGameInfo();
	virtual void RouteStageGameInfo();
	virtual void RouteCompleted();
	virtual void RouteFailed();
	virtual void OnCompleted();
	virtual void OnFailed();
	virtual void DistributeReward();
	
	void RouteMapSectorStart();
	void RouteMovetoPortal(const MUID& uidPlayer);
	void RouteReadyToNewSector(const MUID& uidPlayer);
	void RouteObtainQuestItem(u32 nQuestItemID);
	void RouteObtainZItem(u32 nItemID);
	void RouteSectorBonus(const MUID& uidPlayer, u32 nEXPValue);
	void RouteCombatState();
	bool MakeQuestLevel();
	void CombatProcess();
	void MoveToNextSector();
	void SetCombatState(MQuestCombatState nState);
	bool CheckReadytoNewSector();
	COMBAT_PLAY_RESULT_EX CheckCombatPlay();
	bool CheckQuestCompleted();
	bool CheckQuestCompleteDelayTime();
	void OnSectorCompleted();
	void ProcessCombatPlay();

	void OnBeginCombatState(MQuestCombatState nState);
	void OnEndCombatState(MQuestCombatState nState);

	void MakeRewardList();
	void DistributeXPnBP( MQuestPlayerInfo* pPlayerInfo, const int nRewardXP, const int nRewardBP, const int nScenarioQL );	
	bool DistributeQItem( MQuestPlayerInfo* pPlayerInfo, void** ppoutSimpleQuestItemBlob);
	bool DistributeZItem( MQuestPlayerInfo* pPlayerInfo, void** ppoutQuestRewardZItemBlob);
protected:
	virtual void OnBegin();
	virtual void OnEnd();
	virtual bool OnRun();
	virtual void OnCommand(MCommand* pCommand);
	virtual bool OnCheckRoundFinish();
public:
	MMatchRuleExploration(MMatchStage* pStage);
	virtual ~MMatchRuleExploration();

	void RefreshStageGameInfo();

	virtual void OnRequestPlayerDead(const MUID& uidVictim);
	virtual void OnObtainWorldItem(MMatchObject* pObj, int nItemID, int* pnExtraValues);

	void OnRequestTestSectorClear();
	void OnRequestTestFinish();

	void OnRequestMovetoPortal(const MUID& uidPlayer);
	void OnReadyToNewSector(const MUID& uidPlayer);

	virtual void OnRequestDropSacrificeItemOnSlot( const MUID& uidSender, const int nSlotIndex, const u32 nItemID );
	virtual void OnResponseDropSacrificeItemOnSlot( const MUID& uidSender, const int nSlotIndex, const u32 nItemID );
	virtual void OnRequestCallbackSacrificeItem( const MUID& uidSender, const int nSlotIndex, const u32 nItemID );
    virtual void OnResponseCallBackSacrificeItem( const MUID& uidSender, const int nSlotIndex, const u32 nItemID );
	virtual void OnRequestQL( const MUID& uidSender );
	virtual void OnResponseQL_ToStage( const MUID& uidStage );
	virtual void OnRequestSacrificeSlotInfo( const MUID& uidSender );
	virtual void OnResponseSacrificeSlotInfoToListener( const MUID& uidSender );
	virtual void OnResponseSacrificeSlotInfoToStage( const MUID& uidStage );
	virtual void OnChangeCondition();

	virtual bool							PrepareStart();
	virtual bool							IsSacrificeItemDuplicated( const MUID& uidSender, const int nSlotIndex, const u32 nItemID );
	virtual void							PreProcessLeaveStage( const MUID& uidLeaverUID );
	virtual void							DestroyAllSlot();
	virtual MMATCH_GAMETYPE GetGameType() { return MMATCH_GAMETYPE_QUEST; }


	void InsertNoParamQItemToPlayer( MMatchObject* pPlayer, MQuestItem* pQItem );
	
	void PostInsertQuestGameLogAsyncJob();

	void CollectStartingQuestGameLogInfo();
	void CollectEndQuestGameLogInfo();

	void RouteRewardCommandToStage( MMatchObject* pPlayer, const int nRewardXP, const int nRewardBP, void* pSimpleQuestItemBlob, void* pSimpleZItemBlob );
	
private :
	int CalcuOwnerQItemCount( const MUID& uidPlayer, const u32 nItemID );
};