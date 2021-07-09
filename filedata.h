#pragma once
#include "utils.h"
#include <map>

// bogus, but this is the only way

std::map<std::string, std::string> filedataGamemodes = {
	{"bhop.cfg", "exec gamemodes/default\nsv_cheats 1\nsv_enablebunnyhopping 1\nsv_maxvelocity 7000\nsv_staminamax 0\nsv_staminalandcost 0\nsv_staminajumpcost 0\nsv_accelerate_use_weapon_speed 0\nsv_staminarecoveryrate 0\nsv_autobunnyhopping 1\nsv_airaccelerate 2000\nsv_cheats 0"},
	{"default.cfg", "sv_vote_issue_kick_allowed 0\nmp_autokick 0\nmp_friendlyfire 0\nsv_pausable 1"},
	{"hidenseek.cfg", "exec gamemodes/default\nmp_solid_teammates 0\nsv_infinite_ammo 1\nmp_ct_default_grenades \"weapon_decoy\"\nmp_t_default_grenades \"weapon_decoy\"\nmp_match_end_restart 1\nmp_match_end_changelevel 0\nmp_roundtime_defuse 10\nmp_roundtime 10\nmp_ct_default_secondary ""\nmp_t_default_secondary ""\nmp_maxrounds 100\nmp_match_can_clinch 0\nbot_quota 0\nmp_autoteambalance 0\nmp_limitteams 0\nmp_freezetime 0\nmp_drop_knife_enable 0"},
	{"kamikaze.cfg", "exec gamemodes/defaultserver\nmp_t_default_melee \"weapon_knife\"\nmp_ct_default_melee \"weapon_knife\"\nmp_t_default_grenades weapon_hegrenade\nmp_ct_default_grenades weapon_hegrenade\nmp_t_default_secondary ""\nmp_ct_default_secondary ""\nmp_t_default_primary \"\"\nmp_ct_default_primary \"\"\nmp_maxmoney 0\nsv_infinite_ammo 1\nmp_teammates_are_enemies 1\nmp_randomspawn 1\nmp_roundtime_defuse 10\nmp_roundtime 10\nmp_restartgame 1\nmp_warmup_end\nmp_respawn_on_death_t 1\nmp_respawn_on_death_ct 1"},
	{"knife_round.cfg", "'mp_t_default_secondary \"\"\nmp_ct_default_secondary \"\"\nmp_roundtime 2\nmp_ignore_round_win_conditions 0\nmp_maxmoney 0\nmp_free_armor 1\nmp_restartgame 1\nmp_give_player_c4 0\nmp_drop_knife_enable 1\nmp_freezetime 5\nsay \"Knife round!\""},
	{"long.cfg", "host_timescale 0.85\nexec gamemodes/defaultserver\nexec gamemode_competitive\nmp_maxrounds 50\nmp_c4timer 100\nmp_respawn_on_death_t 1\nmp_respawn_on_death_ct 1\nmp_defuser_allocation 0\nmp_death_drop_defuser 0\nmp_death_drop_gun 0\nmp_respawn_immunitytime 5\nmp_match_can_clinch 1\nsv_allow_votes 0\nmp_restartgame 1"},
	{"normal_round.cfg", "mp_t_default_secondary \"weapon_glock\"\nmp_ct_default_secondary \"weapon_usp_silencer\"\nmp_roundtime 1.92\nmp_ignore_round_win_conditions 0\nmp_maxmoney 16000\nmp_free_armor 0\nmp_restartgame 1\nmp_give_player_c4 1\nmp_drop_knife_enable 0\nmp_freezetime 15"},
	{"practice.cfg", "exec gamemodes/defaultserver\nsv_cheats 1\nsv_infinite_ammo 1\nammo_grenade_limit_total 5\nmp_warmup_end\nmp_freezetime 0\nmp_roundtime 60\nmp_roundtime_defuse 60\nsv_grenade_trajectory 1\nsv_grenade_trajectory_time 10\nsv_showimpacts 1\nmp_limitteams 0\nmp_autoteambalance 0\nmp_maxmoney 60000\nmp_startmoney 60000\nmp_buytime 9999\nmp_buy_anywhere 1\nmp_restartgame 1\nbot_kick\nmp_drop_knife_enable 1"},
	{"surf.cfg", "exec gamemodes/default\nsv_accelerate 10\nsv_airaccelerate 800\nmp_limitteams 0\nmp_autoteambalance 0\nbot_kick\nmp_respawn_on_death_ct 1\nmp_friendlyfire 0\nmp_respawn_on_death_t 1\nmp_ignore_round_win_conditions 1\nmp_autokick 0\nmp_solid_teammates 0\nmp_restartgame 1\nmp_t_default_melee weapon_knife\nmp_ct_default_melee weapon_knife"}
};

std::map<std::string, std::string> filedataKnives = {
	{"aliases.cfg", "alias enable \"sv_cheats 1; mp_items_prohibited 0; mp_drop_knife_enable 1; slot3; drop\"\nalias axe \"enable; exec knives/axe\"\nalias bayonet \"enable; exec knives/bayonet\"\nalias bowie \"enable; exec knives/bowie\"\nalias butterfly \"enable; exec knives/butterfly\"\nalias classic \"enable; exec knives/classic\"\nalias daggers \"enable; exec knives/daggers\"\nalias falchion \"enable; exec knives/falchion\"\nalias fists \"enable; exec knives/fists\"\nalias flip \"enable; exec knives/flip\"\nalias ghost \"enable; exec knives/ghost\"\nalias golden \"enable; exec knives/golden\"\nalias gut \"enable; exec knives/gut\"\nalias hammer \"enable; exec knives/hammer\"\nalias huntsman \"enable; exec knives/huntsman\"\nalias karambit \"enable; exec knives/karambit\"\nalias m9 \"enable; exec knives/m9\"\nalias navaja \"enable; exec knives/navaja\"\nalias normad \"enable; exec knives/normad\"\nalias paracord \"enable; exec knives/paracord\"\nalias skeleton \"enable; exec knives/skeleton\"\nalias spanner \"enable; exec knives/spanner\"\nalias stiletto \"enable; exec knives/stiletto\"\nalias survival \"enable; exec knives/survival\"\nalias talon \"enable; exec knives/talon\"\nalias ursus \"enable; exec knives/ursus\""},
	{"axe.cfg", "give weapon_axe;ent_fire weapon_melee addoutput \"classname weapon_knifegg\""},
	{"bayonet.cfg", "give weapon_bayonet;ent_fire weapon_knife addoutput \"classname weapon_knifegg\""},
	{"bowie.cfg", "give weapon_knife_survival_bowie;ent_fire weapon_knife addoutput \"classname weapon_knifegg\""},
	{"butterfly.cfg", "give weapon_knife_butterfly;ent_fire weapon_knife addoutput \"classname weapon_knifegg\""},
	{"classic.cfg", "give weapon_knife_css;ent_fire weapon_knife addoutput \"classname weapon_knifegg\""},
	{"daggers.cfg", "give weapon_knife_push;ent_fire weapon_knife addoutput \"classname weapon_knifegg\""},
	{"enable.cfg", "sv_cheats 1\nmp_items_prohibited 0\nmp_drop_knife_enable 1\nslot3\n"},
	{"falchion.cfg", "give weapon_knife_falchion;ent_fire weapon_knife addoutput \"classname weapon_knifegg\""},
	{"fists.cfg", "give weapon_fists; ent_fire weapon_fists addoutput \"classname weapon_knifegg\""},
	{"flip.cfg", "give weapon_knife_flip;ent_fire weapon_knife addoutput \"classname weapon_knifegg\""},
	{"ghost.cfg", "give weapon_knife_ghost;ent_fire weapon_knife addoutput \"classname weapon_knifegg\""},
	{"golden.cfg", "give weapon_knifegg;ent_fire weapon_knife addoutput \"classname weapon_knifegg\""},
	{"gut.cfg", "give weapon_knife_gut;ent_fire weapon_knife addoutput \"classname weapon_knifegg\""},
	{"hammer.cfg", "give weapon_hammer;ent_fire weapon_melee addoutput \"classname weapon_knifegg\""},
	{"huntsman.cfg", "give weapon_knife_tactical;ent_fire weapon_knife addoutput \"classname weapon_knifegg\""},
	{"karambit.cfg", "give weapon_knife_karambit;ent_fire weapon_knife addoutput \"classname weapon_knifegg\""},
	{"m9.cfg", " give weapon_knife_m9_bayonet;ent_fire weapon_knife addoutput \"classname weapon_knifegg\""},
	{"navaja.cfg", "give weapon_knife_gypsy_jackknife;ent_fire weapon_knife addoutput \"classname weapon_knifegg\""},
	{"normad.cfg", "give weapon_knife_outdoor;ent_fire weapon_knife addoutput \"classname weapon_knifegg\""},
	{"paracord.cfg", "give weapon_knife_cord;ent_fire weapon_knife addoutput \"classname weapon_knifegg\""},
	{"skeleton.cfg", "give weapon_knife_skeleton;ent_fire weapon_knife addoutput \"classname weapon_knifegg\""},
	{"spanner.cfg", "give weapon_spanner;ent_fire weapon_melee addoutput \"classname weapon_knifegg\""},
	{"stiletto.cfg", "give weapon_knife_stiletto;ent_fire weapon_knife addoutput \"classname weapon_knifegg\""},
	{"survival.cfg", "give weapon_knife_canis;ent_fire weapon_knife addoutput \"classname weapon_knifegg\""},
	{"talon.cfg", "give weapon_knife_widowmaker;ent_fire weapon_knife addoutput \"classname weapon_knifegg\""},
	{"ursus.cfg", "give weapon_knife_ursus;ent_fire weapon_knife addoutput \"classname weapon_knifegg\""},
};