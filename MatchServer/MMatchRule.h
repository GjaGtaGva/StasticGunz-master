#ifndef _MMATCHRULE_H
#define _MMATCHRULE_H

#include "MMatchItem.h"
#include "MMatchTransDataType.h"
#include "MUID.h"
#include "MMatchGameType.h"
#include "MQuestPlayer.h"
#include "MMatchEventManager.h"

class MMatchObject;
class MMatchStage;

///////////////////////////////////////////////////////////////////////////////////////////////
/// ���� �� �⺻ ���̽� Ŭ����
class MMatchRule {
protected:
	MMatchGameTypeInfo*	m_pGameTypeInfo;							///< ���� Ÿ�� ����

	MMatchStage*		m_pStage;									///< �������� Ŭ����
	MMATCH_ROUNDSTATE	m_nRoundState;								///< ���� ���� ����
	int					m_nRoundCount;								///< ���� ��
	int					m_nRoundArg;								///< ������ �߰� ����
	u64					m_tmRoundStateTimer;
	int					m_nLastTimeLimitAnnounce;					// 60, 30, 10 �� �ϳ�

	MMatchEventManager m_OnBeginEventManager;						/// ������ �����Ҷ� �̺�Ʈ.
	MMatchEventManager m_OnGameEventManager;						/// ��Ʋ�� �̺�Ʈ.
	MMatchEventManager m_OnEndEventManager;							/// ������ ������ �̺�Ʈ.

protected:
	virtual bool RoundCount() { return false; }						///< ���� ī��Ʈ. ��� ���尡 ������ false�� ��ȯ�Ѵ�.
	virtual bool OnRun();											///< ����ƽ�� ȣ��
	virtual void OnBegin();											///< ��ü ���� ���۽� ȣ��
	virtual void OnEnd();											///< ��ü ���� ����� ȣ��
	virtual void OnRoundBegin();									///< ���� ������ �� ȣ��
	virtual void OnRoundEnd();										/// ���� ���� �� ȣ��
	virtual void OnRoundTimeOut();									///< ���尡 Ÿ�Ӿƿ����� ����� �� OnRoundEnd() ���̴�.

	virtual bool OnCheckRoundFinish() = 0;							///< ���尡 �������� üũ
	virtual bool OnCheckEnableBattleCondition() { return true; }	///< ���� �������� üũ
	virtual bool OnCheckBattleTimeOut(unsigned int tmTimeSpend);	///< ���� Ÿ�Ӿƿ����� üũ

	void SetRoundStateTimer(u64 tmTime)	{ m_tmRoundStateTimer = tmTime; }
	void InitRound();												///< ���ο� ���� �ʱ�ȭ
	void SetRoundState(MMATCH_ROUNDSTATE nState);					///< ���� ���� ����

	void InitOnBeginEventManager();
	void InitOnGameEventManager();
	void InitOnEndEventManager();

	void CheckOnBeginEvent();
	void CheckOnGameEvent();
	void CheckOnEndEvent();

	void RunOnBeginEvent();
	void RunOnGameEvent();
	void RunOnEndEvent();
public:
	MMatchRule()					{ _ASSERT(false); }				///< �� �����ڴ� ���� ������� �ʴ´�.
	MMatchRule(MMatchStage* pStage);								///< ������
	virtual ~MMatchRule()			{}								///< �Ҹ���
	MMatchStage* GetStage()			{ return m_pStage; }			///< �������� ��ȯ

	int GetRoundCount() const		{ return m_nRoundCount; }		///< �� ���� �� ��ȯ
	void SetRoundCount(int nRound)	{ m_nRoundCount = nRound; }		///< �� ���� �� ����
	int GetRoundArg() const			{ return m_nRoundArg; }			///< ���� ���� ��ȯ
	void SetRoundArg(int nArg)		{ m_nRoundArg = nArg; }			///< ���� ���� ����

	MMatchEventManager& GetOnBeginEventManager()	{ return m_OnBeginEventManager; }
	MMatchEventManager& GetOnGameEventManager()		{ return m_OnGameEventManager; }
	MMatchEventManager& GetOnEndEventManager()		{ return m_OnEndEventManager; }

	MMATCH_ROUNDSTATE GetRoundState()	{ return m_nRoundState; }				///< ���� ���� ��ȯ
	auto GetRoundStateTimer() const	{ return m_tmRoundStateTimer; }
	auto GetLastTimeLimitAnnounce() const	{ return m_nLastTimeLimitAnnounce; }
	void SetLastTimeLimitAnnounce(int nSeconds)	{ m_nLastTimeLimitAnnounce = nSeconds; }

	virtual void* CreateRuleInfoBlob()		{ return NULL; }

	/// �� ���ʽ� ���
	/// @param pAttacker		������
	/// @param pVictim			������
	/// @param nSrcExp			���� ����ġ
	/// @param poutAttackerExp	�����ڰ� ���� ����ġ
	/// @param poutTeamExp		���� ���� ����ġ
	virtual void CalcTeamBonus(MMatchObject* pAttacker,
		                       MMatchObject* pVictim,
							   int nSrcExp,
							   int* poutAttackerExp,
							   int* poutTeamExp);
	/// ������ �����Ҷ� ȣ��ȴ�.
	virtual void OnEnterBattle(MUID& uidChar) { }		
	// ������ �������� ȣ��ȴ�.
	virtual void OnLeaveBattle(MUID& uidChar) { }		
	/// �ش�꿡���� ����ϴ� Ŀ�ǵ�� ���� ó���Ѵ�.
	virtual void OnCommand(class MCommand* pCommand) { }		
	/// ��������� �Ծ��� ��� ȣ��ȴ�.
	virtual void OnObtainWorldItem(MMatchObject* pObj, int nItemID, int* pnExtraValues) { }
	/// Kill�� ȣ��
	virtual void OnGameKill(const MUID& uidAttacker, const MUID& uidVictim) { }
	
	virtual void OnTeam(const MUID &uidPlayer, enum MMatchTeam nTeam) { }

	
	bool Run();														///< ����ƽ
	void Begin();													///< ����
	void End();														///< ��

	void DebugTest();												///< ����� �׽�Ʈ

	virtual bool CheckPlayersAlive() { return true; }
	virtual void OnFailed() {}
	virtual MMATCH_GAMETYPE GetGameType() = 0;
};


///////////////////////////////////////////////////////////////////////////////////////////////








inline bool IsGameRuleDeathMatch(MMATCH_GAMETYPE nGameType)
{
	return (
		(nGameType == MMATCH_GAMETYPE_DEATHMATCH_SOLO) || 
		(nGameType == MMATCH_GAMETYPE_DEATHMATCH_TEAM) ||
		(nGameType == MMATCH_GAMETYPE_TRAINING) ||
		(nGameType == MMATCH_GAMETYPE_EXPLORATION) ||
		(nGameType == MMATCH_GAMETYPE_GUNGAME)
	);
}
inline bool IsGameRuleGladiator(MMATCH_GAMETYPE nGameType)
{
	return ((nGameType == MMATCH_GAMETYPE_GLADIATOR_SOLO) || 
			(nGameType == MMATCH_GAMETYPE_GLADIATOR_TEAM));
}

#endif