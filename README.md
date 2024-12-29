# STASTIC Gunz

This is our test client+server souce code, based on Refined gunz: https://github.com/Asunaya/RefinedGunz
If you're looking for a good source for your server, you will not find it here. However, there is some fun stuff in here.

# Fun stuff:
Starfire element (enchant item) which requires no additional files on the client.
Special element - another enchant element, meant for testing new enchants.
Custom sword trail color defined in zitem.xml, e.g.: trail_color="#FF4593A3"
Exploration game mode (MMatchRuleExploration) - a new game mode, which combines normal training/deadmatch with quest mechanics. TOTALLY INCOMPLETE.
STASTIC items

# STASTIC Items
Special powers on items. 
It is currently achieved by simply setting property "STASTIC" on the item inside zitem.xml, with its integer id. Some powers also use effect_level (existing property, typically used for enchant levels).
The dream implementation would be that these powers be granted separetely, on any item, and ALWAYS time-limited. No idea how to achieve that.

Once an item has the power, inside the source are simple if/if-else checks on critical sports, whether a STASTIC power is active.

Example: inside ZGame.cpp there is additional check at end of function ZGame::DoOneShot
If the owner's selected weapon has got STASTIC power 2020, it will automatically trigger a reload action. 
This is the autoreload power, which will automatically perform reload shot for noobs.

/// STASTIC 2020: Autoreload
if (pOwner->GetItems()->GetSelectedWeapon()->GotSTASTIC(2020)) {
	if ( !g_pGame->IsReplay())
		ZGetGameInterface()->Reload();
}

Some of the powers and their ids:

STASTIC 2000: get wintered - keeps the owner alive in the oblivion. Only checked on left finger ring slot
STASTIC 2010: Leap - on gun shot, teleports the owner to the hit position 
STASTIC 2020: Autoreload - automatically performs reloadshot
STASTIC 2030: Flier - while holding the fire key, fly up. Strength controlled by effect_level
STASTIC 5001: Superjump - modify jump velocity. Strength controlled by effect_level
STASTIC 20001: Spawn npc - on gun shot, spawns a NPC at the hit position, NPC id defined by item's effect_level. Only works on Exploration game mode

example usage on zitem.xml:
 <ITEM id="102000" name="Winter Ring" STASTIC="2000" type="equip" res_sex="a" res_level="4" slot="finger" weight="1" bt_price="100000" hp="0" ap="0" maxwt="5" color="#FF4593A3" trail_color="#FF4593A3" desc="Wear this special ring on your left finger to get wintered." />
 <ITEM id="102021" name="Flier Wings" mesh_name="eq_blade_wing" STASTIC="2030" effect_level="10" type="custom" res_sex="a" res_level="50" slot="custom" weapon="medkit" weight="1" bt_price="1000000" delay="50" damage="0" ctrl_ability="0" magazine="9999" reloadtime="2" slug_output="false" gadget_id="0" hp="0" ap="0" maxwt="0" sf="0" fr="0" cr="0" pr="0" lr="0" color="#FF4593A3" image_id="3" bullet_image_id="0" magazine_image_id="0" desc="Wings of flying - now in your pocket." snd_fire="we_grenade_fire" maxbullet="8000" />

# Exploration
Vision: you run around like in normal deadmatch or training room. Then suddently a dragon spawns and if you kill before it kills you, you can get a STASTIC item reward. You could also bring your own adventures-in-a-box you got from some shady merchant erlier to spawn a special scenario.

Current status: it works, but only as proof on concept and is a mess. At the start of the game a some npcs spawn at 0 coordinate (remnants of quest game mode). On death you don't respawn (you should). You can use STASTIC 20001 to spawn some NPCs.
Unfortunatelly, the NPCs brains rely on .nav file for each map to know how to move around. For maps without it, will fallback to "quest/maps/Mansion_Hall1/Mansion_Hall1.RS.nav" instead of crashing, but then the NPCs are STUPID. For this to work, npc brains need to be totally reprogramed to see their surroundings without any nav file. Also need to make them die in oblivion and for them to avoid it. This is the biggest challenge.

Source code is at MMatchRuleExploration.cpp, MMatchRuleExploration.h. it's an altered copy of MMatchRuleQuest, and remains very messy. Most stuff copied is not necessary.
Many places in the code check ZGameTypeManager.IsQuestDerived. This does not include Explodation, but there we added another check IsQuestDerivedEX for places where this needs to be considered as quest game mode.

on SharedCommandTable there is a new api method MC_EXPLORATION_REQUEST_NPC_SPAWN, which is like MC_QUEST_NPC_SPAWN but instead of specifying the spawn sector id, you pass exact xyz coordinates

contact: GjaGta@Gmail.com
