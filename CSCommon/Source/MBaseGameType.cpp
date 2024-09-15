#include "stdafx.h"
#include "MBaseGameType.h"
#include "MMatchMap.h"

#define MMATCH_GAMETYPE_DEATHMATCH_SOLO_STR		"Death Match(����)"
#define MMATCH_GAMETYPE_DEATHMATCH_TEAM_STR		"Death Match(��ü)"
#define MMATCH_GAMETYPE_GLADIATOR_SOLO_STR		"Gladiator(����)"
#define MMATCH_GAMETYPE_GLADIATOR_TEAM_STR		"Gladiator(��ü)"
#define MMATCH_GAMETYPE_ASSASSINATE_STR			"�ϻ���"
#define MMATCH_GAMETYPE_TRAINING_STR			"Ʈ���̴�"
#define MMATCH_GAMETYPE_CLASSIC_SOLO_STR		"Ŭ����(����)"
#define MMATCH_GAMETYPE_CLASSIC_TEAM_STR		"Ŭ����(��ü)"
#define MMATCH_GAMETYPE_SURVIVAL_STR			"�����̹�"
#define MMATCH_GAMETYPE_QUEST_STR				"����Ʈ"
#define MMATCH_GAMETYPE_BERSERKER_STR			"����Ŀ"
#define MMATCH_GAMETYPE_DEATHMATCH_TEAM2_STR	"������ġ(��ü ����)"
#define MMATCH_GAMETYPE_DUEL_STR		"Duel"

void MMatchGameTypeInfo::Set(const MMATCH_GAMETYPE a_nGameTypeID, const char* a_szGameTypeStr, const float a_fGameExpRatio,
		    const float a_fTeamMyExpRatio, const float a_fTeamBonusExpRatio)
{
	MMatchGameTypeInfo::nGameTypeID = a_nGameTypeID;
	strcpy_safe(MMatchGameTypeInfo::szGameTypeStr, a_szGameTypeStr);
	MMatchGameTypeInfo::fGameExpRatio = a_fGameExpRatio;
	MMatchGameTypeInfo::fTeamMyExpRatio = a_fTeamMyExpRatio;
	MMatchGameTypeInfo::fTeamBonusExpRatio = a_fTeamBonusExpRatio;
}

void MMatchGameTypeInfo::AddMap(int nMapID)
{
	MapSet.insert(nMapID);
}

void MMatchGameTypeInfo::AddAllMap()
{
	for (int i = 0; i < MMATCH_MAP_MAX; i++)
	{
		AddMap(i);
	}
}

MBaseGameTypeCatalogue::MBaseGameTypeCatalogue()
{
#define _InitGameType(index, id, szGameTypeStr, fGameExpRatio, fTeamMyExpRatio, fTeamBonusExpRatio)		\
m_GameTypeInfo[index].Set(id, szGameTypeStr, fGameExpRatio, fTeamMyExpRatio, fTeamBonusExpRatio);

// index,		id,									����Ÿ���̸�,	����ġ��� ����, ���� ���� ����ġ �����, ���� �� ����ġ �����
_InitGameType(0, MMATCH_GAMETYPE_DEATHMATCH_SOLO,	MMATCH_GAMETYPE_DEATHMATCH_SOLO_STR, 1.0f,	1.0f,	0.0f);
_InitGameType(1, MMATCH_GAMETYPE_DEATHMATCH_TEAM,	MMATCH_GAMETYPE_DEATHMATCH_TEAM_STR, 1.0f,	0.8f,	0.3f);
_InitGameType(2, MMATCH_GAMETYPE_GLADIATOR_SOLO,	MMATCH_GAMETYPE_GLADIATOR_SOLO_STR,  0.5f,	1.0f,	0.0f);
_InitGameType(3, MMATCH_GAMETYPE_GLADIATOR_TEAM,	MMATCH_GAMETYPE_GLADIATOR_TEAM_STR,  0.5f,	0.8f,	0.3f);
_InitGameType(4, MMATCH_GAMETYPE_ASSASSINATE,		MMATCH_GAMETYPE_ASSASSINATE_STR,	 1.0f,	0.8f,	0.3f);
_InitGameType(5, MMATCH_GAMETYPE_TRAINING,			MMATCH_GAMETYPE_TRAINING_STR,		 0.0f,	0.0f,	0.0f);

#ifdef _QUEST
_InitGameType(MMATCH_GAMETYPE_SURVIVAL, MMATCH_GAMETYPE_SURVIVAL,	MMATCH_GAMETYPE_SURVIVAL_STR,	0.0f,	0.0f,	0.0f);
_InitGameType(MMATCH_GAMETYPE_QUEST,	MMATCH_GAMETYPE_QUEST,		MMATCH_GAMETYPE_QUEST_STR,		0.0f,	0.0f,	0.0f);
#endif

_InitGameType(MMATCH_GAMETYPE_BERSERKER,	MMATCH_GAMETYPE_BERSERKER, MMATCH_GAMETYPE_BERSERKER_STR,		1.0f,	1.0f,	0.0f);

_InitGameType(MMATCH_GAMETYPE_DEATHMATCH_TEAM2,	MMATCH_GAMETYPE_DEATHMATCH_TEAM2, MMATCH_GAMETYPE_DEATHMATCH_TEAM2_STR,		1.0f,	0.6f,	0.5f);

_InitGameType(MMATCH_GAMETYPE_DUEL, MMATCH_GAMETYPE_DUEL, MMATCH_GAMETYPE_DUEL_STR, 1.0f, 1.0f, 0.0f);

_InitGameType(MMATCH_GAMETYPE_SKILLMAP, MMATCH_GAMETYPE_SKILLMAP, "Skillmap", 1.0f, 1.0f, 0.0f);

_InitGameType(MMATCH_GAMETYPE_GUNGAME, MMATCH_GAMETYPE_GUNGAME, "GunGame", 1.0f, 1.0f, 0.0f);

_InitGameType(MMATCH_GAMETYPE_QUEST_CHALLENGE,	MMATCH_GAMETYPE_QUEST_CHALLENGE,		"Challenge",		0.0f,	0.0f,	0.0f);
_InitGameType(MMATCH_GAMETYPE_EXPLORATION, MMATCH_GAMETYPE_EXPLORATION,	"Exploration",	0.5f,	0.5f,	0.5f);

/*
#ifdef _CLASSIC
_InitGameType(MMATCH_GAMETYPE_CLASSIC_SOLO, MMATCH_GAMETYPE_CLASSIC_SOLO,
			  MMATCH_GAMETYPE_CLASSIC_SOLO_STR,		 1.0f,	1.0f,	0.0f);
_InitGameType(MMATCH_GAMETYPE_CLASSIC_TEAM, MMATCH_GAMETYPE_CLASSIC_TEAM,
			  MMATCH_GAMETYPE_CLASSIC_TEAM_STR,		 1.0f,	0.8f,	0.3f);
#endif
*/

	// �� ����Ÿ�Կ��� �÷��� ������ �� - ����� ��� ���� �� ����
	for (int i = 0; i < MMATCH_GAMETYPE_DUEL; i++)
	{
		m_GameTypeInfo[i].AddAllMap();
	}
}

MBaseGameTypeCatalogue::~MBaseGameTypeCatalogue()
{

}



