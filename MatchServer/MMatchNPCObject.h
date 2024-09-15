#ifndef _MMATCHNPCOBJECT_H
#define _MMATCHNPCOBJECT_H


#include "MVector3.h"
#include "MBaseQuest.h"
#include "MUID.h"
#include "MQuestDropTable.h"
#include <map>
using namespace std;

class MMatchStage;
struct MQuestPlayerInfo;

/// NPC ������Ʈ�� �÷��� - �������� ���ٸ��� ������� �ʴ´�.
enum NPCOBJECT_FLAG
{
	NOF_NONE			= 0,

};


/// NPC ������Ʈ
class MMatchNPCObject
{
private:
	MUID				m_UID;						///< ID
	MQUEST_NPC			m_nType;					///< NPCŸ��
	MUID				m_uidController;			///< ������
	MVector3			m_Pos;						///< ��ġ
	u32	m_nFlags;					///< �÷��� ����
	MQuestDropItem		m_DropItem;					///< ������ �ִ� ������ - ���� ��� nDropItemType�� QDIT_NA�̴�.

public:
	/// ������
	/// @param uid		NPC UID
	/// @param nType	NPC ����
	/// @param nFlags	�÷���
	MMatchNPCObject(MUID& uid, MQUEST_NPC nType, u32 nFlags=0);
	/// �Ҹ���
	~MMatchNPCObject() { }
	/// NPC ������ �÷��̾�� �Ҵ��Ѵ�.
	/// @param uidPlayer	�Ҵ��� �÷��̾� UID
	void AssignControl(MUID& uidPlayer);		
	/// NPC ������ ����
	void ReleaseControl();
	/// Drop�� �������� �����Ѵ�.
	/// @param pDropItem	������ ����
	void SetDropItem(MQuestDropItem* pDropItem);

	// gets
	MUID GetUID()					{ return m_UID; }				///< NPC UID ��ȯ
	MQUEST_NPC	GetType()			{ return m_nType; }				///< NPC ���� ��ȯ
	MUID& GetController()			{ return m_uidController; }		///< NPC ������(�÷��̾�) UID ��ȯ
	MQuestDropItem* GetDropItem()	{ return &m_DropItem; }			///< ��� ������ ���� ��ȯ

	inline void SetFlag(unsigned int nFlag, bool bValue);			///< �÷��� ����
	inline bool CheckFlag(unsigned int nFlag);						///< �÷��� üũ
	inline void SetFlags(unsigned int nFlags);						///< �÷��� ����
	inline u32 GetFlags();								///< �÷��� ��ȯ
	inline bool HasDropItem();										///< ��� �������� ������ �ִ��� ��ȯ

};

typedef map<MUID, MMatchNPCObject*>		MMatchNPCObjectMap;


class MQuestPlayerManager;

/// NPC ������Ʈ ������
class MMatchNPCManager
{
private:
	// var
	MMatchStage*					m_pStage;
	MQuestPlayerManager*			m_pPlayerManager;
	MMatchNPCObjectMap				m_NPCObjectMap;

	u32				m_nLastSpawnTime;		// for test

	int								m_nNPCCount[MNST_END];		// ����Ÿ�Ժ� ����ִ� NPC��
	int								m_nBossCount;				// ����ִ� ���� ��

	// func
	MUID NewUID();
	bool AssignControl(MUID& uidNPC, MUID& uidPlayer);
	bool Spawn(MUID& uidNPC, MUID& uidController, unsigned char nSpawnPositionIndex);
	bool Spawn(MUID& uidNPC, MUID& uidController, MVector& pos);
	void Clear();
	bool FindSuitableController(MUID& out, MQuestPlayerInfo* pSender);

	void SetNPCObjectToControllerInfo(MUID& uidChar, MMatchNPCObject* pNPCObject);
	void DelNPCObjectToControllerInfo(MUID& uidChar, MMatchNPCObject* pNPCObject);
public:
	bool BossDead{};

	/// ������
	MMatchNPCManager();
	/// �Ҹ���
	~MMatchNPCManager();
	/// �ʱ�ȭ
	/// @param pStage				�������� Ŭ����
	/// @param pPlayerManager		����Ʈ�꿡���� PlayerManager
	void Create(MMatchStage* pStage, MQuestPlayerManager* pPlayerManager);
	/// ����
	void Destroy();
	/// ��� NPC�� ���ش�.
	void ClearNPC();
	/// NPC ������Ʈ ����
	/// @param nType					NPC ����
	/// @param nSpawnPositionIndex		���� ��ġ
	MMatchNPCObject* CreateNPCObject(MQUEST_NPC nType, unsigned char nSpawnPositionIndex);
	/// Override: spawn at exact coordinates
	MMatchNPCObject* CreateNPCObject(MQUEST_NPC nType, MVector& pos);
	/// NPC ������Ʈ ����
	/// @param uidNPC					NPC UID
	/// @param outItem					NPC�� ����ϴ� ������ ��ȯ��
	bool DestroyNPCObject(MUID& uidNPC, MQuestDropItem& outItem);
	/// NPC ������Ʈ ��ȯ
	/// @param uidNPC					NPC UID
	MMatchNPCObject* GetNPCObject(MUID& uidNPC);
	/// �÷��̾ ������������ ������ ȣ��ȴ�.
	/// @param uidPlayer				�÷��̾� UID
	void OnDelPlayer(const MUID& uidPlayer);
	/// �ش� �÷��̾ �ش� NPC�� �����ϰ� �ִ��� üũ
	/// @param uidChar					�÷��̾� UID
	/// @param uidNPC					NPC UID
	bool IsControllersNPC(MUID& uidChar, MUID& uidNPC);
	/// NPC ������Ʈ�� ��ȯ
	int GetNPCObjectCount();
	/// �ش� ����Ÿ���� NPC ������Ʈ�� ��ȯ
	/// @param nSpawnType				NPC ���� Ÿ��
	int GetNPCObjectCount(MQuestNPCSpawnType nSpawnType);
	int GetBossCount() { return m_nBossCount; }

	void RemovePlayerControl(const MUID& uidPlayer);
};



// inlines //////////////////////////////////////////////////////////////////////////////////
inline void MMatchNPCObject::SetFlags(unsigned int nFlags)
{
	if (m_nFlags != nFlags)
	{
		m_nFlags = nFlags;
	}
}

inline void MMatchNPCObject::SetFlag(unsigned int nFlag, bool bValue)
{
	if (bValue) m_nFlags |= nFlag;
	else m_nFlags &= ~nFlag;
}

inline bool MMatchNPCObject::CheckFlag(unsigned int nFlag)
{
	return ((m_nFlags & nFlag) != 0);
}

inline u32 MMatchNPCObject::GetFlags()
{ 
	return m_nFlags; 
}

inline int MMatchNPCManager::GetNPCObjectCount()
{
	return (int)m_NPCObjectMap.size();
}

inline bool MMatchNPCObject::HasDropItem()
{
	return (m_DropItem.nDropItemType != QDIT_NA);
}


inline int MMatchNPCManager::GetNPCObjectCount(MQuestNPCSpawnType nSpawnType)
{
	return m_nNPCCount[nSpawnType];
}


#endif