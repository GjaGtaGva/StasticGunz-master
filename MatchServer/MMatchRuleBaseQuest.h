#ifndef _MMATCHRULE_BASEQUEST_H
#define _MMATCHRULE_BASEQUEST_H

#include "MMatchRule.h"
#include "MMatchNPCObject.h"
#include "MQuestPlayer.h"
#include "MMatchQuestRound.h"


class MMatchQuestGameLogInfoManager;

class MMatchRuleBaseQuest : public MMatchRule {
protected:
	MMatchNPCManager			m_NPCManager;
	MQuestPlayerManager			m_PlayerManager;
	bool						m_bQuestCompleted;

	u64							m_nLastNPCSpawnTime;
	int							m_nNPCSpawnCount;
	u64							m_nSpawnTime;
	int							m_nFirstPlayerCount;

	u64							m_nLastNPCAssignCheckTime;
	u64							m_nLastPingTime;

protected:
	virtual void OnBegin();
	virtual void OnEnd();
	virtual bool OnRun();
	virtual void OnRoundBegin();
	virtual void OnRoundEnd();
	virtual bool OnCheckRoundFinish();
	virtual void OnRoundTimeOut();
	virtual bool RoundCount();
	virtual bool OnCheckEnableBattleCondition();

	virtual void OnCommand(MCommand* pCommand);
protected:
	void CheckRewards(MUID& uidPlayer, MQuestDropItem* pDropItem, MVector& pos);
	bool CheckPlayersAlive();
	virtual void ProcessNPCSpawn() = 0;
	virtual bool CheckNPCSpawnEnable() = 0;
	virtual void RouteGameInfo() = 0;
	virtual void RouteStageGameInfo() = 0;

	MMatchNPCObject* SpawnNPC(MQUEST_NPC nNPC, int nPosIndex);
	MMatchNPCObject* SpawnNPC(MQUEST_NPC nNPC, MVector& pos);

	virtual void OnCompleted();
	virtual void OnFailed();
	virtual void RouteCompleted() = 0;
	virtual void RouteFailed() = 0;
	virtual void DistributeReward() = 0;

	void ReAssignNPC();
	void SendClientLatencyPing();
public:

	/// Spawn npc at exact coordinates
	virtual void OnRequestNPCSpawn(int nNPCType, MVector& pos);

	virtual void OnRequestNPCDead(MUID& uidSender, MUID& uidKiller, MUID& uidNPC, MVector& pos);

	virtual void OnRequestPlayerDead(const MUID& uidVictim);
	virtual void OnRequestTestNPCSpawn(int nNPCType, int nNPCCount);
	virtual void OnRequestTestClearNPC();

	void RefreshPlayerStatus();
	void ClearAllNPC();

	virtual void OnRequestDropSacrificeItemOnSlot( const MUID& uidSender, const int nSlotIndex, const u32 nItemID ) {}
	virtual void OnResponseDropSacrificeItemOnSlot( const MUID& uidSender, const int nSlotIndex, const u32 nItemID ) {}
	virtual void OnRequestCallbackSacrificeItem( const MUID& uidSender, const int nSlotIndex, const u32 nItemID ) {}
	virtual void OnResponseCallBackSacrificeItem( const MUID& uidSender, const int nSlotIndex, const u32 nItemID )	{}
	virtual void OnRequestQL( const MUID& uidSender ) {}
	virtual void OnResponseQL_ToStage( const MUID& uidStage )	{}
	virtual void OnRequestSacrificeSlotInfo( const MUID& uidSender ) {}
	virtual void OnResponseSacrificeSlotInfoToListener( const MUID& uidSender ) {}
	virtual void OnResponseSacrificeSlotInfoToStage( const MUID& uidStage )	{}
	virtual void OnChangeCondition() {}

	virtual bool							PrepareStart() { return true; }
	virtual void							PreProcessLeaveStage( const MUID& uidLeaverUID );


	virtual void CheckMonsterBible( const MUID& uidUser, const int nMonsterBibleIndex );
	virtual void PostNewMonsterInfo( const MUID& uidUser, const char nMonIndex );
	
public:
	MMatchRuleBaseQuest(MMatchStage* pStage);
	virtual ~MMatchRuleBaseQuest();
	virtual void OnEnterBattle(MUID& uidChar);
	virtual void OnLeaveBattle(MUID& uidChar);
};






#endif