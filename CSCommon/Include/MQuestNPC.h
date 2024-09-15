#pragma once

#include "MQuestConst.h"
#include "MTypes.h"

enum MQUEST_NPC
{
	NPC_NONE					= 0,

	NPC_GOBLIN					= 11,
	NPC_GOBLIN_GUNNER			= 12,
	NPC_GOBLIN_WIZARD			= 13,
	NPC_GOBLIN_COMMANDER		= 14,
	NPC_GOBLIN_CHIEF			= 15,
	NPC_GOBLIN_KING				= 16,

	NPC_KOBOLD					= 21,
	NPC_KOBOLD_SHAMAN			= 22,
	NPC_KOBOLD_COMMANDER		= 23,
	NPC_KOBOLD_KING				= 24,
	NPC_KOBOLD_KING_BOSS		= 25,
	NPC_BROKEN_GOLEM			= 26,

	NPC_SKELETON				= 31,
	NPC_SKELETON_MAGE			= 32,
	NPC_SKELETON_COMMANDER		= 33,
	NPC_GIANT_SKELETON			= 34,
	NPC_THE_UNHOLY				= 35,
	NPC_LICH_BABY				= 36,
	NPC_GIANT_SKELETON_BOSS		= 37,
	NPC_THE_UNHOLY_BOSS			= 38,
	NPC_LICH					= 39,

	NPC_PALMPOW_BABY			= 41,
	NPC_PALMPOA					= 42,
	NPC_PALMPOA_COMMANDER		= 43,
	NPC_PALMPOW					= 44,
	NPC_CURSED_PALMPOW			= 45,

	NPC_SCRIDER					= 51,

	NPC_GOBLIN_EVENT			= 111,
	NPC_SKELETON_EVENT			= 131,

	NPCS_GOBLIN					= 151,
	NPCS_GOBLIN_GUNNER			= 152,
	NPCS_GOBLIN_WIZARD			= 153,
	NPCS_GOBLIN_COMMANDER		= 154,
	NPCS_GOBLIN_CHIEF			= 155,
	NPCS_GOBLIN_KING			= 156,
	NPCS_LITTLE_GOBLIN_KING_JR	= 157,
	NPCS_THUNDER_GOBLIN_KING_JR	= 158,
	NPCS_THUNDER_GOBLIN_KING	= 159,

	NPCS_KOBOLD					= 161,
	NPCS_KOBOLD_SHAMAN			= 162,
	NPCS_KOBOLD_COMMANDER		= 163,
	NPCS_KOBOLD_KING			= 164,
	NPCS_KOBOLD_KING_BOSS		= 165,
	NPCS_BROKEN_GOLEM			= 166,
	NPCS_SKELETON				= 171,
	NPCS_SKELETON_MAGE			= 172,
	NPCS_SKELETON_COMMANDER		= 173,
	NPCS_GIANT_SKELETON			= 174,
	NPCS_THE_UNHOLY				= 175,
	NPCS_LICH_BABY				= 176,
	NPCS_GIANT_SKELETON_BOSS	= 177,
	NPCS_THE_UNHOLY_BOSS		= 178,
	NPCS_LICH					= 179,
	NPCS_END = 180

};

static const MQUEST_NPC AllNpcs[] = {
	MQUEST_NPC::NPC_NONE,
	MQUEST_NPC::NPC_GOBLIN,
	MQUEST_NPC::NPC_GOBLIN_GUNNER,
	MQUEST_NPC::NPC_GOBLIN_WIZARD,
	MQUEST_NPC::NPC_GOBLIN_COMMANDER,
	MQUEST_NPC::NPC_GOBLIN_CHIEF,
	MQUEST_NPC::NPC_GOBLIN_KING,
	MQUEST_NPC::NPC_KOBOLD,
	MQUEST_NPC::NPC_KOBOLD_SHAMAN,
	MQUEST_NPC::NPC_KOBOLD_COMMANDER,
	MQUEST_NPC::NPC_KOBOLD_KING,
	MQUEST_NPC::NPC_KOBOLD_KING_BOSS,
	MQUEST_NPC::NPC_BROKEN_GOLEM,
	MQUEST_NPC::NPC_SKELETON,
	MQUEST_NPC::NPC_SKELETON_MAGE,
	MQUEST_NPC::NPC_SKELETON_COMMANDER,
	MQUEST_NPC::NPC_GIANT_SKELETON,
	MQUEST_NPC::NPC_THE_UNHOLY,
	MQUEST_NPC::NPC_LICH_BABY,
	MQUEST_NPC::NPC_GIANT_SKELETON_BOSS,
	MQUEST_NPC::NPC_THE_UNHOLY_BOSS,
	MQUEST_NPC::NPC_LICH,
	MQUEST_NPC::NPC_PALMPOW_BABY,
	MQUEST_NPC::NPC_PALMPOA,
	MQUEST_NPC::NPC_PALMPOA_COMMANDER,
	MQUEST_NPC::NPC_PALMPOW,
	MQUEST_NPC::NPC_CURSED_PALMPOW,
	MQUEST_NPC::NPC_SCRIDER,
	MQUEST_NPC::NPC_GOBLIN_EVENT,
	MQUEST_NPC::NPC_SKELETON_EVENT,
	MQUEST_NPC::NPCS_GOBLIN,
	MQUEST_NPC::NPCS_GOBLIN_GUNNER,
	MQUEST_NPC::NPCS_GOBLIN_WIZARD,
	MQUEST_NPC::NPCS_GOBLIN_COMMANDER,
	MQUEST_NPC::NPCS_GOBLIN_CHIEF,
	MQUEST_NPC::NPCS_GOBLIN_KING,
	MQUEST_NPC::NPCS_LITTLE_GOBLIN_KING_JR,
	MQUEST_NPC::NPCS_THUNDER_GOBLIN_KING_JR,
	MQUEST_NPC::NPCS_THUNDER_GOBLIN_KING,
	MQUEST_NPC::NPCS_KOBOLD,
	MQUEST_NPC::NPCS_KOBOLD_SHAMAN,
	MQUEST_NPC::NPCS_KOBOLD_COMMANDER,
	MQUEST_NPC::NPCS_KOBOLD_KING,
	MQUEST_NPC::NPCS_KOBOLD_KING_BOSS,
	MQUEST_NPC::NPCS_BROKEN_GOLEM,
	MQUEST_NPC::NPCS_SKELETON,
	MQUEST_NPC::NPCS_SKELETON_MAGE,
	MQUEST_NPC::NPCS_SKELETON_COMMANDER,
	MQUEST_NPC::NPCS_GIANT_SKELETON,
	MQUEST_NPC::NPCS_THE_UNHOLY,
	MQUEST_NPC::NPCS_LICH_BABY,
	MQUEST_NPC::NPCS_GIANT_SKELETON_BOSS,
	MQUEST_NPC::NPCS_THE_UNHOLY_BOSS,
	MQUEST_NPC::NPCS_LICH
};

enum MQUEST_NPC_GRADE
{
	NPC_GRADE_REGULAR = 0,
	NPC_GRADE_VETERAN,
	NPC_GRADE_ELITE,
	NPC_GRADE_BOSS,
	NPC_GRADE_LEGENDARY,
	NPC_GRADE_END
};

enum MQUEST_NPC_ATTACK
{
	NPC_ATTACK_NONE			= 0,
	NPC_ATTACK_MELEE		= 0x1,
	NPC_ATTACK_RANGE		= 0x2,
	NPC_ATTACK_MAGIC		= 0x4,
};

enum MQUEST_NPC_BEHAVIOR
{

};

enum MQUEST_NPC_SOUND
{
	NPC_SOUND_ATTACK = 0,
	NPC_SOUND_WOUND,
	NPC_SOUND_DEATH,
	NPC_SOUND_END
};

struct MQuestNPCInfo
{
	MQUEST_NPC			nID;
	MQUEST_NPC_GRADE	nGrade;
	char				szName[256];
	char				szDesc[256];
	u32	nNPCAttackTypes;
	float				fSpeed;
	float				fWeight;
	int					nMaxHP;
	int					nMaxAP;
	int					nDC;
	char				szMeshName[256];
	int					nWeaponDamage;
	float				fRotateSpeed;
	float				fCollRadius;
	float				fCollHeight;
	bool				bColPick;
	MVector				vScale;
	MVector				vColor;
	unsigned char		nSpawnWeight;
	u32	nWeaponItemID;
	float				fDyingTime;
	float				fTremble;

	char				szSoundName[NPC_SOUND_END][128];

	char				nIntelligence;
	char				nAgility;

	float				fAttackRange;
	float				fAttackRangeAngle;
	float				fAttackHitRate;
	float				fAttackCoolTime;
	float				fCollisionCheckSpeed;
	float				fViewAngle;
	float				fViewDistance;
	float				fViewDistance360;
	
	bool				bNeverBlasted;
	bool				bMeleeWeaponNeverDamaged;
	bool				bRangeWeaponNeverDamaged;
	bool				bShadow;
	bool				bNeverPushed;
	bool				bNeverAttackCancel;

	int					nSkills;
	int					nSkillIDs[MAX_SKILL];

	int					nDropTableID;
	char				szDropTableName[8];

	void SetDefault()
	{
		nID					= NPC_GOBLIN;
		nGrade				= NPC_GRADE_REGULAR;
		strcpy_safe(szName, "Noname");
		szDesc[0]			= 0;
		nNPCAttackTypes		= NPC_ATTACK_MELEE;
		fSpeed				= 300.0f;
		fWeight				= 1.0f;
		nMaxHP				= 100;
		nMaxAP				= 0;
		nDC					= 5;
		szMeshName[0]		= 0;
		nWeaponDamage		= 5;
		fRotateSpeed		= 6.28318f;
		fCollRadius			= 35.0f;
		fCollHeight			= 180.0f;
		bColPick			= false;
		vScale				= MVector(1.0f,1.0f,1.0f);
		vColor				= MVector(0.6f,0.6f,0.6f);
		nSpawnWeight		= 100;
		nWeaponItemID		= 300000;
		fTremble			= 30.0f;
		
		nIntelligence		= 3;
		nAgility			= 3;
		fDyingTime			= 5.0f;

		fAttackRange		= 130.0f;
		fAttackRangeAngle	= 1.570796f;
		fAttackHitRate		= 1.0f;
		fAttackCoolTime		= 0.0f;
		fCollisionCheckSpeed = 0.0f;


		fViewAngle			= 3.14159f;
		fViewDistance		= 800.0f;
		fViewDistance360	= 800.0f;

		bNeverBlasted				= false;
		bMeleeWeaponNeverDamaged	= false;
		bRangeWeaponNeverDamaged	= false;
		bShadow						= true;
		bNeverPushed				= false;
		bNeverAttackCancel			= false;

		nSkills				= 0;

		nDropTableID		= 0;
		szDropTableName[0]	= 0;

		for (int i = 0; i < NPC_SOUND_END; i++) szSoundName[i][0] = 0;
	}

	MQuestNPCSpawnType GetSpawnType();
};


#define NPC_INTELLIGENCE_STEPS		5
#define NPC_AGILITY_STEPS			5

struct MQuestNPCGlobalAIValue
{
	// shaking ratio
	float		m_fPathFinding_ShakingRatio;
	float		m_fAttack_ShakingRatio;
	float		m_fSpeed_ShakingRatio;

	// update time
	float		m_fPathFindingUpdateTime[NPC_INTELLIGENCE_STEPS];
	float		m_fAttackUpdateTime[NPC_AGILITY_STEPS];
};

class MQuestNPCCatalogue : public std::map<MQUEST_NPC, MQuestNPCInfo*>
{
private:
	MQuestNPCGlobalAIValue			m_GlobalAIValue;

	void ParseNPC(MXmlElement& element);
	void Insert(MQuestNPCInfo* pNPCInfo);
	void ParseGlobalAIValue(MXmlElement& element);

	std::map< int, MQUEST_NPC > m_MonsterBibleCatalogue;

public:
	MQuestNPCCatalogue();
	~MQuestNPCCatalogue();

	MQuestNPCInfo* GetIndexInfo( int nIndex );

	bool ReadXml(const char* szFileName);
	bool ReadXml(class MZFileSystem* pFileSystem,const char* szFileName);
	void Clear();

	MQuestNPCInfo* GetInfo(MQUEST_NPC nNpc);
	MQuestNPCInfo* GetPageInfo( int nPage);

	MQuestNPCGlobalAIValue* GetGlobalAIValue() { return &m_GlobalAIValue; }
};

struct MNPCSetNPC
{
	MQUEST_NPC		nNPC;
	int				nMinRate;
	int				nMaxRate;
	int				nMaxSpawnCount;

	MNPCSetNPC()
	{
		nNPC = NPC_NONE;
		nMinRate = 0;
		nMaxRate = 0;
		nMaxSpawnCount = 0;
	}
};

struct MQuestNPCSetInfo
{
	int					nID;
	char				szName[16];
	MQUEST_NPC			nBaseNPC;
	std::vector<MNPCSetNPC>	vecNPCs;
};

class MQuestNPCSetCatalogue : public std::map<int, MQuestNPCSetInfo*>
{
private:
	std::map<std::string, MQuestNPCSetInfo*>		m_NameMap;

	void Clear();
	void ParseNPCSet(MXmlElement& element);
	void Insert(MQuestNPCSetInfo* pNPCSetInfo);
public:
	MQuestNPCSetCatalogue();
	~MQuestNPCSetCatalogue();

	bool ReadXml(const char* szFileName);
	bool ReadXml(MZFileSystem* pFileSystem,const char* szFileName);	

	MQuestNPCSetInfo* GetInfo(int nID);
	MQuestNPCSetInfo* GetInfo(const char* szName);
};