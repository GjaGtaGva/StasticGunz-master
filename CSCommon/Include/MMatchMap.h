#pragma once

#define MAPNAME_LENGTH		32

enum MMATCH_MAP
{
	MMATCH_MAP_MANSION,
	MMATCH_MAP_PRISON,
	MMATCH_MAP_STATION,
	MMATCH_MAP_PRISON_II,
	MMATCH_MAP_BATTLE_ARENA,
	MMATCH_MAP_DOJO,
	MMATCH_MAP_TOWN,
	MMATCH_MAP_DUNGEON,
	MMATCH_MAP_RUIN,
	MMATCH_MAP_ISLAND,
	MMATCH_MAP_GARDEN,
	MMATCH_MAP_CASTLE,
	MMATCH_MAP_FACTORY,
	MMATCH_MAP_PORT,
	MMATCH_MAP_LOST_SHRINE,
	MMATCH_MAP_STAIRWAY,
	MMATCH_MAP_SNOWTOWN,
	MMATCH_MAP_HALL,
	MMATCH_MAP_CATACOMB,
	MMATCH_MAP_JAIL,
	MMATCH_MAP_SHOWERROOM,
	MMATCH_MAP_CLOCKWORK_TIME,

	MMATCH_MAP_MAX
};

#define MMATCH_MAP_COUNT	MMATCH_MAP_MAX


const struct MMatchMapDesc
{
	int			nMapID;
	char		szMapName[MAPNAME_LENGTH];
	char		szMapImageName[MAPNAME_LENGTH];
	char		szBannerName[MAPNAME_LENGTH];
	float		fExpRatio;
	int			nMaxPlayers;
	bool		bOnlyDuelMap;

} g_MapDesc[MMATCH_MAP_COUNT] = 
{ 
	{MMATCH_MAP_MANSION,		"Mansion",		"map_Mansion.bmp",		"banner_Mansion.tga",		1.0f,	16,		false}, 
	{MMATCH_MAP_PRISON,			"Prison",		"map_Prison.bmp",		"banner_Prison.tga",		1.0f,	16,		false}, 
	{MMATCH_MAP_STATION,		"Station",		"map_Station.bmp",		"banner_Station.tga",		1.0f,	16,		false}, 
	{MMATCH_MAP_PRISON_II,		"Prison II",	"map_Prison II.bmp",	"banner_Prison II.tga",		1.0f,	16,		false},
	{MMATCH_MAP_BATTLE_ARENA,	"Battle Arena", "map_Battle Arena.bmp", "banner_Battle Arena.tga",	1.0f,	16,		false},
	{MMATCH_MAP_DOJO,			"Dojo",			"",						"",							1.0f,	16,		false},
	{MMATCH_MAP_TOWN,			"Town",			"map_Town.bmp",			"banner_Town.tga",			1.0f,	16,		false},
	{MMATCH_MAP_DUNGEON,		"Dungeon",		"map_Dungeon.bmp",		"banner_Dungeon.tga",		1.0f,	16,		false},
	{MMATCH_MAP_RUIN,			"Ruin",			"map_Ruin.bmp",			"banner_Ruin.tga",			1.0f,	16,		false},
	{MMATCH_MAP_ISLAND,			"Island",		"map_island.bmp",		"banner_island.tga",		1.0f,	16,		false},
	{MMATCH_MAP_GARDEN,			"Garden",		"map_Garden.bmp",		"banner_Garden.tga",		1.0f,	16,		false},
	{MMATCH_MAP_CASTLE,			"Castle",		"map_Castle.bmp",		"banner_Castle.tga",		1.0f,	16,		false},
	{MMATCH_MAP_FACTORY,		"Factory",		"map_Factory.bmp",		"banner_Factory.tga",		0.8f,	8,		false},
	{MMATCH_MAP_PORT,			"Port",			"map_Port.bmp",			"banner_port.tga",			1.0f,	16,		false},
	{MMATCH_MAP_LOST_SHRINE,	"Lost Shrine",	"map_Lost Shrine.bmp",	"banner_Lost Shrine.tga",	1.0f,	16,		false},
	{MMATCH_MAP_STAIRWAY,		"Stairway",		"map_Stairway.bmp",		"banner_Stairway.tga",		1.0f,	16,		false},
	{MMATCH_MAP_SNOWTOWN,		"Snow_Town",	"map_Snow_Town.bmp",	"banner_Snow_Town.tga",		1.1f,	16,		false},
	{MMATCH_MAP_HALL,			"Hall",			"map_Mansion.bmp",		"banner_Mansion.tga",		1.0f,	16,		true},
	{MMATCH_MAP_CATACOMB,		"Catacomb",		"map_Dungeon.bmp",		"banner_Dungeon.tga",		1.0f,	16,		true},
	{MMATCH_MAP_JAIL,			"Jail",			"map_Prison.bmp",		"banner_Prison.tga",		1.0f,	16,		true},
	{MMATCH_MAP_SHOWERROOM,		"Shower Room",	"map_Prison II.bmp",	"banner_Prison II.tga",		1.0f,	16,		true},
	{MMATCH_MAP_CLOCKWORK_TIME,	"Clockwork Time",	"map_Clockwork Time.bmp",	"banner_Clockwork Time.tga",		1.0f,	16,		false},
};


inline bool MIsCorrectMap(const int nMapID)
{
	if ((nMapID < 0) || (nMapID >= MMATCH_MAP_MAX)) return false;
	return true;
}

inline const char* MGetMapName(const int nMapID)
{
	if (MIsCorrectMap(nMapID))
		return g_MapDesc[nMapID].szMapName;
	else
		return 0;
}

inline const char* MGetMapImageName(const char* szMapName)
{
	for ( int i = 0;  i < MMATCH_MAP_COUNT;  i++)
	{
		if ( _stricmp( szMapName, g_MapDesc[ i].szMapName) == 0)
			return g_MapDesc[ i].szMapImageName;
	}

	return 0;
}

inline const char* MGetBannerName(const char* szMapName)
{
	for ( int i = 0;  i < MMATCH_MAP_COUNT;  i++)
	{
		if ( _stricmp( szMapName, g_MapDesc[ i].szMapName) == 0)
			return g_MapDesc[ i].szBannerName;
	}

	return 0;
}

inline bool MIsMapOnlyDuel( const int nMapID)
{
	if ( MIsCorrectMap(nMapID))
		return g_MapDesc[ nMapID].bOnlyDuelMap;
	else
		return 0;
}

inline int QuestMapNameToID(StringView Name)
{
	const char* Mapsets[] = {"mansion", "prison", "dungeon"};
	for (int i = 0; i < int(std::size(Mapsets)); ++i)
		if (iequals(Name, Mapsets[i]))
			return i + 1;
	return 1;
}