
--BFState begin
BFState_Bf_Invalid=0;
BFState_Bf_Normal=1;
BFState_Bf_Dead=2;
BFState_Bf_Flag=3;
BFState_Bf_Fighting=4;
BFState_Bf_Leave=5;
--BFState end


--BattleObjectAbility begin
BattleObjectAbility_SELECT_TEAMMATE=1;
BattleObjectAbility_SELECT_ENEMY=2;
BattleObjectAbility_MOVE=3;
BattleObjectAbility_USE_BASE_SKILL=4;
BattleObjectAbility_USE_MAGIC_SKILL=5;
BattleObjectAbility_BE_TEAMMATE_SELECT=6;
BattleObjectAbility_BE_ENEMY_SELECT=7;
BattleObjectAbility_CHANGE_CURRENT_TARGET=8;
BattleObjectAbility_CAN_BE_DAMAGE=9;
--BattleObjectAbility end


--BattleObjectStatus begin
BattleObjectStatus_DEAD=1;
BattleObjectStatus_FREEZE=2;
BattleObjectStatus_LOCK=3;
BattleObjectStatus_SNEER=4;
BattleObjectStatus_MUTE=5;
BattleObjectStatus_CONFUSION=6;
BattleObjectStatus_MOVING=7;
BattleObjectStatus_STUN=8;
BattleObjectStatus_FEAR=9;
BattleObjectStatus_GOD=10;
BattleObjectStatus_IMMUNE=11;
BattleObjectStatus_SHEEP=12;
BattleObjectStatus_FEAKDEAD=13;
BattleObjectStatus_DISAPPEAR=14;
BattleObjectStatus_BLIND=15;
--BattleObjectStatus end


--ActivityGroup begin
ActivityGroup_Dayly=0;
ActivityGroup_TimeLimit=1;
ActivityGroup_HolyDay=2;
--ActivityGroup end


--DrawType begin
DrawType_CrystalOne=0;
DrawType_CrystalTen=1;
DrawType_GoldOne=2;
DrawType_GoldTen=3;
--DrawType end


--EquipmentSlot begin
EquipmentSlot_EQUIPMENT_SLOT_INVALID=-1;
EquipmentSlot_EQUIPMENT_SLOT_HEAD=1;
EquipmentSlot_EQUIPMENT_SLOT_NECK=2;
EquipmentSlot_EQUIPMENT_SLOT_SHOULDER=3;
EquipmentSlot_EQUIPMENT_SLOT_CLOAK=4;
EquipmentSlot_EQUIPMENT_SLOT_CHEST=5;
EquipmentSlot_EQUIPMENT_SLOT_MAIN_HAND=6;
EquipmentSlot_EQUIPMENT_SLOT_CUFF=7;
EquipmentSlot_EQUIPMENT_SLOT_SHOES=8;
EquipmentSlot_EQUIPMENT_SLOT_RING_ONE=9;
EquipmentSlot_EQUIPMENT_SLOT_RING_TWO=10;
EquipmentSlot_EQUIPMENT_SLOT_ORNAMENT=11;
EquipmentSlot_EQUIPMENT_SLOT_ASSI_HAND=12;
--EquipmentSlot end


--EquipmentType begin
EquipmentType_EQUIPMENT_TYPE_INVALID=-1;
EquipmentType_EQUIPMENT_TYPE_MAIN_HAND=1;
EquipmentType_EQUIPMENT_TYPE_ASSI_HAND=2;
EquipmentType_EQUIPMENT_TYPE_HELMET=3;
EquipmentType_EQUIPMENT_TYPE_SHOULDER=4;
EquipmentType_EQUIPMENT_TYPE_CHEST=5;
EquipmentType_EQUIPMENT_TYPE_MAIN_CUFF=6;
EquipmentType_EQUIPMENT_TYPE_SHOES=7;
EquipmentType_EQUIPMENT_TYPE_CLOAK=8;
EquipmentType_EQUIPMENT_TYPE_NECKLACE=9;
EquipmentType_EQUIPMENT_TYPE_RING=10;
EquipmentType_EQUIPMENT_TYPE_ORNAMENT=11;
EquipmentType_EQUIPMENT_TYPE_BOTH_HAND=12;
--EquipmentType end


--WeaponType begin
WeaponType_WEAPONTYPE_INVALID=-1;
WeaponType_WEAPONTYPE_TWO_HAND_AXE=1;
WeaponType_WEAPONTYPE_TWO_HAND_HAMMER=2;
WeaponType_WEAPONTYPE_TWO_HAND_SWORD=3;
WeaponType_WEAPONTYPE_BOW=4;
WeaponType_WEAPONTYPE_SPEAR=5;
WeaponType_WEAPONTYPE_WAND=6;
WeaponType_WEAPONTYPE_AXE=7;
WeaponType_WEAPONTYPE_HAMMER=8;
WeaponType_WEAPONTYPE_DAGGER=9;
WeaponType_WEAPONTYPE_SWORD=10;
WeaponType_WEAPONTYPE_ARROW_BAG=11;
WeaponType_WEAPONTYPE_BULLET_BOX=12;
WeaponType_WEAPONTYPE_HALIDOM=13;
WeaponType_WEAPONTYPE_SHIELD=14;
WeaponType_WEAPONTYPE_MAGIC_WARE=15;
WeaponType_WEAPONTYPE_JAVELIN=16;
WeaponType_WEAPONTYPE_MAGIC_WAND=17;
--WeaponType end


--EquipmentColor begin
EquipmentColor_EQUIP_COLOR_INVALID=0;
EquipmentColor_EQUIP_WHITE=1;
EquipmentColor_EQUIP_GREEN=2;
EquipmentColor_EQUIP_BLUE=3;
EquipmentColor_EQUIP_PURPLE=4;
EquipmentColor_EQUIP_ORANGE=5;
--EquipmentColor end


--ItemType begin
ItemType_ItemTypeInvalid=-1;
ItemType_POTION=1;
ItemType_MATERIAL=2;
ItemType_CARD_FRAGMENT=3;
ItemType_AUTOMEDIC=4;
ItemType_MISSION_SCROLL=5;
ItemType_MISSION=11;
ItemType_CARD=88;
ItemType_CURRENCY=99;
--ItemType end


--ItemAddLogType begin
ItemAddLogType_AddType_Package=1;
ItemAddLogType_AddType_Mission=2;
ItemAddLogType_AddType_Friend=3;
ItemAddLogType_AddType_MonsterDrop=9;
ItemAddLogType_AddType_BankTake=10;
ItemAddLogType_AddType_BankStore=11;
ItemAddLogType_AddType_UseItem=12;
ItemAddLogType_AddType_DismantleItem=13;
ItemAddLogType_AddType_SellItem=14;
ItemAddLogType_AddType_Gem=15;
ItemAddLogType_AddType_Market=16;
ItemAddLogType_AddType_ExpPoint=17;
ItemAddLogType_AddType_Stage_Tax=18;
ItemAddLogType_AddType_Loot=19;
ItemAddLogType_AddType_Draw=20;
ItemAddLogType_AddType_PlayerInit=21;
ItemAddLogType_AddType_Activity=22;
ItemAddLogType_AddType_Treasure=23;
ItemAddLogType_AddType_Guild=25;
ItemAddLogType_AddType_BattleFeild=26;
ItemAddLogType_AddType_TargetReward=27;
ItemAddLogType_AddType_TakeOutGem=29;
ItemAddLogType_AddType_Mail=30;
ItemAddLogType_AddType_Arrange=31;
ItemAddLogType_AddType_OfflineHook=32;
ItemAddLogType_AddType_Charge=33;
ItemAddLogType_AddType_Hook=34;
ItemAddLogType_AddType_Exchange=35;
ItemAddLogType_AddType_Acr=36;
ItemAddLogType_AddType_CDK=37;
ItemAddLogType_AddType_MissReward=38;
ItemAddLogType_AddType_Gm=50;
--ItemAddLogType end


--ItemDelLogType begin
ItemDelLogType_DelType_Package=1;
ItemDelLogType_DelType_Mission=2;
ItemDelLogType_DelType_Friend=3;
ItemDelLogType_DelType_TreasureMission=4;
ItemDelLogType_DelType_RewardMission=5;
ItemDelLogType_DelType_UnionMIssion=6;
ItemDelLogType_DelType_CorpsMIssion=7;
ItemDelLogType_DelType_CarBonsMIssion=8;
ItemDelLogType_DelType_MagicalEquip=9;
ItemDelLogType_DelType_Market=10;
ItemDelLogType_DelType_BankStore=11;
ItemDelLogType_DelType_ExtendContainerSize=12;
ItemDelLogType_DelType_BankTake=13;
ItemDelLogType_DelType_SellItem=14;
ItemDelLogType_DelType_UseItem=15;
ItemDelLogType_DelType_EnhanceEquip=16;
ItemDelLogType_DelType_Gem=17;
ItemDelLogType_DelType_Refix=18;
ItemDelLogType_DelType_ExpPoint=19;
ItemDelLogType_DelType_ChatGarMsg=20;
ItemDelLogType_DelType_Guild=21;
ItemDelLogType_DelType_ChallengeBoss=22;
ItemDelLogType_DelType_Loot=23;
ItemDelLogType_DelType_Draw=24;
ItemDelLogType_DelType_Hell=25;
ItemDelLogType_DelType_WorldBoss=26;
ItemDelLogType_DelType_Actor=27;
ItemDelLogType_DelType_Upgrade_Color=28;
ItemDelLogType_DelType_Upgrade_Level=29;
ItemDelLogType_DelType_Upgrade_Star=30;
ItemDelLogType_DelType_Arena=31;
ItemDelLogType_DelType_Rename=32;
ItemDelLogType_DelType_Activity=33;
ItemDelLogType_DelType_Enchant=34;
ItemDelLogType_DelType_Enchant_Slot=35;
ItemDelLogType_DelType_EmbedGem=36;
ItemDelLogType_DelType_OfflineHook=37;
ItemDelLogType_DelType_DismantleItem=38;
ItemDelLogType_DelType_Exchange=39;
ItemDelLogType_DelType_Acr=40;
ItemDelLogType_DelType_MissReward=41;
ItemDelLogType_DelType_Gm=50;
--ItemDelLogType end


--MarketType begin
MarketType_MarketType_Invalid=-1;
MarketType_MarketType_Gem=0;
MarketType_MarketType_Union=1;
MarketType_MarketType_Equip=2;
MarketType_MarketType_LawFal=3;
MarketType_MarketType_Honor=4;
MarketType_MarketType_Mission=5;
MarketType_MarketType_Charge=6;
MarketType_MarketType_Secret=7;
MarketType_MarketType_Goods=8;
MarketType_MarketType_MonthCard=9;
MarketType_MarketType_Point=10;
MarketType_MarketType_Scroll=11;
MarketType_MarketType_Time=12;
MarketType_MarketTypeMax=13;
--MarketType end


--MarketItemGroup begin
MarketItemGroup_MarketItemGroup_Invalid=-1;
MarketItemGroup_CountGroup_0=0;
MarketItemGroup_CountGroup_1=1;
MarketItemGroup_CountGroup_2=2;
MarketItemGroup_CountGroup_3=3;
MarketItemGroup_CountGroup_4=4;
MarketItemGroup_CountGroup_5=5;
MarketItemGroup_CountGroupMax=6;
--MarketItemGroup end


--FinishType begin
FinishType_InvalidFinish=-1;
FinishType_FinishText=0;
FinishType_CommitItem=1;
FinishType_KillMonster=2;
FinishType_KillNpc=3;
FinishType_UseItem=4;
FinishType_MailItem=5;
FinishType_LevelUp=6;
FinishType_RandomNpc=7;
FinishType_Ratrol=8;
FinishType_EnterRaid=9;
FinishType_CollectItem=10;
FinishType_GenTarget=11;
FinishType_Action=12;
--FinishType end


--TargetFinishType begin
TargetFinishType_InvalidTarget=-1;
TargetFinishType_TFT_KillMonster=1;
TargetFinishType_StageProcess=2;
TargetFinishType_DayToken=3;
TargetFinishType_DrawCard=4;
TargetFinishType_FinishActivity=5;
TargetFinishType_FightHero=6;
TargetFinishType_GetEquip=7;
TargetFinishType_SuitAttr=8;
TargetFinishType_GetMount=9;
TargetFinishType_StepUp=10;
TargetFinishType_StarUp=11;
TargetFinishType_AllEnhanceLv=12;
TargetFinishType_EmbedGem=13;
TargetFinishType_SkillLv=14;
TargetFinishType_EquipEnchant=15;
TargetFinishType_HeroLevelUp=16;
TargetFinishType_EquipEnhanceLv=17;
TargetFinishType_AnyStepUp=18;
TargetFinishType_JoinUnion=19;
TargetFinishType_WinHell=20;
TargetFinishType_LackeysUse=21;
TargetFinishType_WinBurningBattle=22;
TargetFinishType_GainEquip=23;
TargetFinishType_OnEquip=24;
TargetFinishType_UnionSkil=25;
TargetFinishType_WorldBoss=26;
--TargetFinishType end


--TargetResolveType begin
TargetResolveType_NoValue=0;
TargetResolveType_EqualValue=1;
TargetResolveType_MoreValue=2;
TargetResolveType_LessLalue=3;
TargetResolveType_StatisticsValue=4;
TargetResolveType_Special=5;
--TargetResolveType end


--SubFinishType begin
SubFinishType_InvalidSubType=-1;
SubFinishType_KillAndGetWithMonster=1;
SubFinishType_KillAndGetWithStage=2;
SubFinishType_KillAndGetWithScene=3;
SubFinishType_KillMonsterFinish=4;
SubFinishType_KillAndGetOneWithMonster=5;
SubFinishType_KillAnyMonster=6;
SubFinishType_KillStageAnyMonster=7;
--SubFinishType end


--MissionType begin
MissionType_InvalidMission=-1;
MissionType_MainMission=0;
MissionType_PartMission=1;
MissionType_UnionMission=2;
MissionType_ArchaeologyMission=3;
MissionType_RewardMission=4;
MissionType_CorpsMission=5;
MissionType_GoblinCarbonMission=6;
MissionType_ExtraRewardMission=7;
MissionType_TinyThreeCarbonMission=8;
MissionType_BlackCarbonMission=9;
MissionType_BloodCarbonMission=10;
MissionType_CommonCarbonMission=11;
MissionType_BigThreeCarbonMission=12;
MissionType_GradeUpMission=13;
MissionType_DayKillMission=14;
MissionType_LevelMission=15;
MissionType_Carbon2001Mission=16;
MissionType_Carbon2002Mission=17;
MissionType_Carbon2003Mission=18;
MissionType_Carbon2004Mission=19;
MissionType_Carbon2005Mission=20;
MissionType_Carbon2006Mission=21;
MissionType_Carbon2007Mission=22;
MissionType_ScrollMission=23;
MissionType_TowerMission=24;
--MissionType end


--NpcType begin
NpcType_Npc=0;
NpcType_Monster=1;
--NpcType end


--VisibleType begin
VisibleType_StaticPublic=0;
VisibleType_CreatePublic=1;
VisibleType_CraatePrivate=2;
--VisibleType end


--MonsterType begin
MonsterType_Normal=0;
MonsterType_Elite=1;
MonsterType_Boss=2;
--MonsterType end


--AttackType begin
AttackType_PHYSICAL=1;
AttackType_MAGIc=2;
--AttackType end


--ItemPackageType begin
ItemPackageType_RANDOM=1;
ItemPackageType_STABLE=2;
ItemPackageType_DRAW=3;
--ItemPackageType end


--SkillType begin
SkillType_Trigger=0;
SkillType_AiCast=1;
SkillType_SubSkill=2;
--SkillType end


--AreaType begin
AreaType_Circle=0;
AreaType_Sector=1;
--AreaType end


--TargetCamp begin
TargetCamp_TEAM=0;
TargetCamp_ENEMY=1;
TargetCamp_BOTH=2;
--TargetCamp end


--PropertyReviseValueType begin
PropertyReviseValueType_BY_NUMBER=0;
PropertyReviseValueType_BY_PERCENTEGY=1;
--PropertyReviseValueType end


--BuffType begin
BuffType_add_level2=1;
BuffType_reduce_level2=2;
BuffType_bleed=3;
BuffType_add_status=4;
BuffType_immune=5;
BuffType_sweep_away=6;
BuffType_add_hp_max=7;
BuffType_weapon_enhence=8;
BuffType_flash_ball=9;
BuffType_shocker=10;
BuffType_sneer=11;
BuffType_electric_overload=12;
BuffType_sting=13;
BuffType_ice_block=14;
BuffType_combo_point=15;
BuffType_sheep=16;
BuffType_burning=17;
BuffType_ice_shield=18;
BuffType_holy_shield=19;
BuffType_divine_shield=20;
BuffType_fake_dead=21;
BuffType_seal_of_command=22;
BuffType_instant_poison=23;
BuffType_deadly_poison=24;
BuffType_blade_flurry=25;
BuffType_add_critical_percent=26;
BuffType_ghostly_strike=27;
BuffType_disappear=28;
BuffType_hemorrhage=29;
BuffType_seal_of_justice=30;
BuffType_damage_link=31;
BuffType_continue_cure=32;
BuffType_life_burst=33;
BuffType_corruption=34;
BuffType_death_coil=35;
BuffType_redemption_ghost=36;
BuffType_pain=37;
BuffType_dark_mark=38;
BuffType_plague=39;
BuffType_add_source_hp=40;
BuffType_mark=41;
BuffType_add_cure_effect=42;
BuffType_wild=43;
BuffType_add_dodge_percent=44;
BuffType_impaling=45;
BuffType_reduce_cure_effect=46;
BuffType_be_attack_damage=47;
BuffType_reduce_damage=48;
BuffType_fury=49;
BuffType_reckless=50;
BuffType_life_steal=51;
BuffType_slow_down=52;
BuffType_weapon_deterrence=53;
BuffType_transform=54;
BuffType_sacrifice=55;
BuffType_devotion=56;
BuffType_hunter_mark=57;
BuffType_double_attack=58;
BuffType_shield=59;
BuffType_knight_bless=60;
BuffType_warlock_sacrifice=61;
BuffType_warlock_curse=62;
BuffType_add_damage=63;
BuffType_punish_halo=64;
BuffType_reduce_accurate_rate=65;
BuffType_poison=66;
BuffType_pulse_instance=67;
--BuffType end


--TargetSortType begin
TargetSortType_INVALID=0;
TargetSortType_RANGE=1;
TargetSortType_RANDOM_SEL=2;
TargetSortType_HATRED=3;
TargetSortType_LOW_HP_PERCENT=4;
TargetSortType_LOW_HP=5;
TargetSortType_REVERSER_RANGE=6;
TargetSortType_USE_SKILL_TAR_LOGIC=99;
--TargetSortType end


--ImpactEffectType begin
ImpactEffectType_BadImpact=-1;
ImpactEffectType_NormalImpact=0;
ImpactEffectType_GoodImpact=1;
--ImpactEffectType end


--PlayerTeamStatus begin
PlayerTeamStatus_Team_Follow=0;
PlayerTeamStatus_Team_AFKING=1;
PlayerTeamStatus_Team_AFK=2;
--PlayerTeamStatus end


--ActorType begin
ActorType_Main=1;
ActorType_Hero=2;
ActorType_Monster=3;
ActorType_Boss=4;
ActorType_SUMMON_NORMAL=5;
ActorType_SUMMON_NO_SELECT=6;
--ActorType end


--BattleActorType begin
BattleActorType_BAT_MAIN=1;
BattleActorType_BAT_HERO=2;
BattleActorType_BAT_MONSTER=3;
BattleActorType_BAT_BOSS=4;
BattleActorType_BAT_SUMMON=5;
--BattleActorType end


--LastHpType begin
LastHpType_HellLastHp=0;
LastHpType_BFLastHp=1;
LastHpType_GBLastHp=2;
LastHpType_GBOSSLastHp=3;
LastHpType_LastHpMax=5;
--LastHpType end


--ActorStar begin
ActorStar_ONE=1;
ActorStar_TWO=2;
ActorStar_THREE=3;
ActorStar_FOUR=4;
ActorStar_FIVE=5;
--ActorStar end


--ActorColor begin
ActorColor_WHILE=1;
ActorColor_GREEN_0=2;
ActorColor_GREEN_1=3;
ActorColor_BLUE_0=4;
ActorColor_BLUE_1=5;
ActorColor_BLUE_2=6;
ActorColor_PURPLE_0=7;
--ActorColor end


--ActorBattleType begin
ActorBattleType_USE_PHYSICAL=1;
ActorBattleType_USE_MAGIC=2;
--ActorBattleType end


--ActorFigthFormation begin
ActorFigthFormation_INVALID_FORMATION=0;
ActorFigthFormation_AFF_NORMAL_HOOK=1;
ActorFigthFormation_AFF_PVE_NORMAL=1;
ActorFigthFormation_AFF_PVP_ONLINE=2;
ActorFigthFormation_AFF_STAGE_CAPTURE=3;
ActorFigthFormation_AFF_STAGE_GUARD=4;
ActorFigthFormation_AFF_PVP_LOOT_FORMATION=5;
ActorFigthFormation_AFF_PVP_ARENA=6;
ActorFigthFormation_AFF_PVP_TRIAL_FIELD=7;
ActorFigthFormation_AFF_PVP_GUARD=8;
ActorFigthFormation_AFF_PVP_BATTLE_FIELD=9;
ActorFigthFormation_AFF_PVP_WORLDBOSS_Sun=10;
ActorFigthFormation_AFF_PVP_WORLDBOSS_Mon=11;
ActorFigthFormation_AFF_PVP_WORLDBOSS_Tues=12;
ActorFigthFormation_AFF_PVP_WORLDBOSS_Wed=13;
ActorFigthFormation_AFF_PVP_WORLDBOSS_Thur=14;
ActorFigthFormation_AFF_PVP_WORLDBOSS_Fri=15;
ActorFigthFormation_AFF_PVP_WORLDBOSS_Sat=16;
ActorFigthFormation_AFF_PVE_GUILD_BOSS=17;
ActorFigthFormation_AFF_PVE_GUILD_COPY=18;
ActorFigthFormation_AFF_PVE_REWARD_MISSION=19;
ActorFigthFormation_AFF_PVE_ARCHAEOLOGY_MISSION=20;
ActorFigthFormation_AFF_PVE_GOBLIN_CARBON=21;
ActorFigthFormation_AFF_PVE_BLOOD_CARBON=22;
--ActorFigthFormation end


--DamageType begin
DamageType_PHYSICAL=1;
DamageType_FLASH=2;
DamageType_ICE=3;
DamageType_FIRE=4;
DamageType_NATURE=5;
DamageType_SHADOW=6;
DamageType_HOLY=7;
--DamageType end


--DamageExpression begin
DamageExpression_ActorOnDamage_NORMAL=1;
DamageExpression_ActorOnDamage_CRITICAL=2;
DamageExpression_ActorOnDamage_DODGE=3;
DamageExpression_ActorOnDamage_CURE=4;
--DamageExpression end


--SkillPositionLogicType begin
SkillPositionLogicType_CHARGE_LOGIC=1;
SkillPositionLogicType_TELEPORT_LOGIC=2;
SkillPositionLogicType_CAPTURE_LOGIC=3;
SkillPositionLogicType_HITOFF_LOGIC=4;
--SkillPositionLogicType end


--BattleGroundType begin
BattleGroundType_NO_BATTLE=0;
BattleGroundType_PVE_SINGLE=1;
BattleGroundType_PVE_HOOK=2;
BattleGroundType_PVP_OFFLINE=3;
BattleGroundType_PVP_ONLINE=4;
BattleGroundType_PVP_PVE_CAPTURE_STAGE=5;
BattleGroundType_PVE_STAGE_BOSS=6;
BattleGroundType_PVP_LOOT=9;
BattleGroundType_PVP_ARENA=10;
BattleGroundType_PVP_TRIAL_FIELD=11;
BattleGroundType_PVP_BATTEL_FIELD=12;
BattleGroundType_PVP_GUILD_BATTLE=13;
BattleGroundType_PVE_GUILD_BOSS=14;
BattleGroundType_PVP_WORLDBOSS_BATTLE=15;
BattleGroundType_PVE_GUILD_COPY=16;
BattleGroundType_PVP_GUILD_CAPTURE_POINT=17;
BattleGroundType_PVE_SIMULATE=18;
--BattleGroundType end


--BattleObjectType begin
BattleObjectType_INVALID_TYPE=0;
BattleObjectType_TRAP=1;
BattleObjectType_MAIN_ACTOR=2;
BattleObjectType_HERO_ACTOR=3;
BattleObjectType_NORMAL_MONSTER=4;
BattleObjectType_BOSS=5;
--BattleObjectType end


--ActorStateType begin
ActorStateType_ACTOR_IDEL=1;
ActorStateType_ACTOR_SELECT_TAR=2;
ActorStateType_ACTOR_MOVE=3;
ActorStateType_ACTOR_FIGHT=4;
ActorStateType_ACTOR_DEAD=5;
--ActorStateType end


--ActorControlType begin
ActorControlType_AI=0;
ActorControlType_MANUAL=1;
--ActorControlType end


--Property begin
Property_INVALID_PROPERTY=0;
Property_ENDURANCE=1;
Property_PHYSICAL_STRENTH=2;
Property_AGILE=3;
Property_INTELLIGENCE=4;
Property_SPIRIT=5;
Property_HP=6;
Property_PHYSICAL_ATTACK=7;
Property_PHYSICAL_ARMOR=8;
Property_MAGIC_ATTACK=9;
Property_MAGIC_ARMOR=10;
Property_ACCURATE=11;
Property_DODGE=12;
Property_CRITICAL=13;
Property_TOUGHNESS=14;
Property_MOVE_SPEED=15;
Property_ATTACK_SPEED=16;
Property_SKILL=17;
Property_PERCENT_HP=18;
Property_PERCENT_PHYSICAL_ATTACK=19;
Property_PERCENT_PHYSICAL_ARMOR=20;
Property_PERCENT_MAGIC_ATTACK=21;
Property_PERCENT_MAGIC_ARMOR=22;
Property_PERCENT_CURE_ABILITY=23;
--Property end


--ChatChannel begin
ChatChannel_SYSTEM=0;
ChatChannel_WORLD=1;
ChatChannel_SCENE=2;
ChatChannel_GUILD=3;
ChatChannel_TEAM=4;
ChatChannel_TEAMINFO=5;
ChatChannel_FARMSG=6;
ChatChannel_RUNMSG=7;
ChatChannel_MAX=8;
--ChatChannel end


--CellLogicType begin
CellLogicType_InvalidCellType=-1;
CellLogicType_NumberCell=0;
CellLogicType_BitFlagCell=1;
CellLogicType_MissionCell=2;
CellLogicType_GuideCell=3;
CellLogicType_AcRewardCell=4;
CellLogicType_ActivityCell=5;
CellLogicType_MountCell=6;
CellLogicType_TargetData=7;
CellLogicType_ActionData=8;
CellLogicType_ResetTime=9;
CellLogicType_GuildUserCell=10;
--CellLogicType end


--NumberCellIndex begin
NumberCellIndex_NumberCellIndex_Invalid=-1;
NumberCellIndex_CrystalToCoinCount=0;
NumberCellIndex_TargetRewardFlag=1;
NumberCellIndex_DrawLuckyValue=2;
NumberCellIndex_ActivityRewardFlag=3;
NumberCellIndex_DrawLuckyValue2=4;
NumberCellIndex_DrawLuckyValue3=5;
NumberCellIndex_InspireTimes=7;
NumberCellIndex_HellReliveTimes=8;
NumberCellIndex_GuildCommonData=9;
NumberCellIndex_ArenaChallengeTimeToday=10;
NumberCellIndex_ArenaBuyTimesToday=11;
NumberCellIndex_HellDayWaveCount=12;
NumberCellIndex_ReNameCount=13;
NumberCellIndex_GuildFreezeContribute=14;
NumberCellIndex_GuildCurContribute=15;
NumberCellIndex_GuildAllContribute=16;
NumberCellIndex_CrystalExtendEquipBagTimes=17;
NumberCellIndex_CrystalExtendBankTimes=18;
NumberCellIndex_CrystalExtendItemBagTimes=19;
NumberCellIndex_NumberCellIndex_Max=20;
--NumberCellIndex end


--BitFlagCellIndex begin
BitFlagCellIndex_BitFlagCellIndex_Invalid=-1;
BitFlagCellIndex_FirstCrystalDrawFlag=0;
BitFlagCellIndex_FirstCoinDrawFlag=1;
BitFlagCellIndex_WorldBossAutoFightFlag=2;
BitFlagCellIndex_FirstDayKillFlag=3;
BitFlagCellIndex_SecondCoinDrawFlag=4;
BitFlagCellIndex_FirstBattleFieldFlag=5;
BitFlagCellIndex_FirstWorldBossFlag=6;
BitFlagCellIndex_FirstTriFieldFlag=7;
BitFlagCellIndex_MonthCardFlag=8;
BitFlagCellIndex_LifeCardFlag=9;
BitFlagCellIndex_InvestFlag=10;
BitFlagCellIndex_DayTriFieldFlag=11;
BitFlagCellIndex_DayTriGuildBossFlag=12;
BitFlagCellIndex_InnerHookInitFlag=13;
BitFlagCellIndex_MissionAutoBuyItemFlag=14;
BitFlagCellIndex_BitFlagCellIndex_Max=32;
--BitFlagCellIndex end


--MissionCellIndex begin
MissionCellIndex_MissionCellStart=0;
MissionCellIndex_MissionCellEnd=128;
--MissionCellIndex end


--GuideCellIndex begin
GuideCellIndex_GuideGroupStart=0;
GuideCellIndex_GuideGroupEnd=64;
--GuideCellIndex end


--ActivityDataIndex begin
ActivityDataIndex_ActivityDataStart=0;
ActivityDataIndex_ActivityDataEnd=128;
--ActivityDataIndex end


--AcRewardIndex begin
AcRewardIndex_RewardDataStart=0;
AcRewardIndex_RewardDataEnd=256;
--AcRewardIndex end


--TargetDataIndex begin
TargetDataIndex_TargetDataStart=0;
TargetDataIndex_TargetDataEnd=128;
--TargetDataIndex end


--ActionDataIndex begin
ActionDataIndex_ActionDataStart=0;
ActionDataIndex_ActionDataEnd=128;
--ActionDataIndex end


--ResetTimeIndex begin
ResetTimeIndex_ResetTimeStart=0;
ResetTimeIndex_ResetTimeEnd=64;
--ResetTimeIndex end


--Cell64LogicType begin
Cell64LogicType_InvalidCell64Type=-1;
Cell64LogicType_NumberCell64=0;
Cell64LogicType_LootCell64Type=1;
Cell64LogicType_EquipBook=2;
--Cell64LogicType end


--LootCellIndex begin
LootCellIndex_LAST_LOOT_PLAYER_GUID=0;
LootCellIndex_UNBIND_COIN=1;
LootCellIndex_LOOT_COUNT_TODAY=2;
LootCellIndex_LAST_BE_LOOT_TIME=3;
LootCellIndex_LAST_LOOT_TIME=4;
--LootCellIndex end


--GuildUserCellIndex begin
GuildUserCellIndex_DONATION=0;
GuildUserCellIndex_GUILDUSER_MAX=1;
--GuildUserCellIndex end


--NumberCell64Index begin
NumberCell64Index_TestGuid=0;
NumberCell64Index_LastGuildId=1;
NumberCell64Index_ActorBook=2;
NumberCell64Index_GuideFlagData=3;
NumberCell64Index_GuideFirstFlag=4;
NumberCell64Index_SceneFirstFlag=5;
NumberCell64Index_MissRewardFlag=6;
NumberCell64Index_InnerHookFlag=7;
--NumberCell64Index end


--EquipBook64Index begin
EquipBook64Index_EquipBookStart=0;
EquipBook64Index_EquipBookEnd=64;
--EquipBook64Index end


--ZoneCellIndex begin
ZoneCellIndex_AddPopularity=0;
ZoneCellIndex_OffNewMessage=1;
--ZoneCellIndex end


--FriendMessageType begin
FriendMessageType_SystemM=1;
FriendMessageType_FriendTips=3;
FriendMessageType_FriendM=0;
--FriendMessageType end


--ContainerType begin
ContainerType_Layer_Invalid=-1;
ContainerType_Layer_EquipBag=0;
ContainerType_Layer_ItemBag=1;
ContainerType_Layer_MissionBag=2;
ContainerType_Layer_Equip=3;
ContainerType_Layer_Bank=4;
ContainerType_Layer_EquipAndItemBag=7;
ContainerType_Layer_AllBag=8;
ContainerType_Layer_TokenBag=9;
ContainerType_Layer_MissionAndItemBag=10;
ContainerType_Layer_ArrangeBag=11;
--ContainerType end


--AccountType begin
AccountType_FAST=0;
AccountType_BIND=1;
AccountType_THIRD=2;
--AccountType end


--MissionState begin
MissionState_MissionTodo=0;
MissionState_MissionFinish=1;
MissionState_MissionFailed=2;
MissionState_MissionComplete=3;
MissionState_MissionDrop=4;
--MissionState end


--MissionOption begin
MissionOption_Accept=0;
MissionOption_Finish=1;
MissionOption_Drop=2;
MissionOption_Fight=3;
MissionOption_Add=4;
MissionOption_Raid=5;
--MissionOption end


--MissionDB_Param begin
MissionDB_Param_Npc_Index=0;
MissionDB_Param_Monster_Group=1;
MissionDB_Param_Pos_Id=2;
MissionDB_Param_Npc_Series=3;
MissionDB_Param_Commit_Npc_Series=4;
MissionDB_Param_Finish_Value=5;
MissionDB_Param_CarbonRuntimeId=6;
MissionDB_Param_MultiFinish_Value1=7;
MissionDB_Param_MultiFinish_Value2=8;
MissionDB_Param_MultiFinish_Value3=9;
MissionDB_Param_MultiFinish_Value4=10;
MissionDB_Param_StoryStage_Value=11;
--MissionDB_Param end


--Camp begin
Camp_Alliance=0;
Camp_Horde=1;
--Camp end


--ArrangeType begin
ArrangeType_AT_Invalid=-1;
ArrangeType_AT_Leader=0;
ArrangeType_AT_Roll=1;
--ArrangeType end


--Race begin
Race_Non_Race=0;
Race_Tauren=1;
Race_Undead=2;
Race_Orc=3;
Race_Troll=4;
Race_Human=5;
Race_NigthElf=6;
Race_Gnome=7;
--Race end


--Professions begin
Professions_Non_Prof=0;
Professions_Warrior=1;
Professions_Shaman=2;
Professions_Hunter=3;
Professions_Mage=4;
Professions_Druid=5;
Professions_Warlock=6;
Professions_Priest=7;
Professions_Paladin=8;
Professions_Rogue=9;
--Professions end


--Talent begin
Talent_Non_Talent=0;
Talent_Warrior_Weapon=1;
Talent_Warrior_Rage=2;
Talent_Warrior_Armor=3;
Talent_Shaman_Element=4;
Talent_Shaman_Strenth=5;
Talent_Shaman_Cure=6;
Talent_Hunter_Beast=7;
Talent_Hunter_Shoot=8;
Talent_Hunter_Suivival=9;
Talent_Mage_Profound=10;
Talent_Mage_Ice=11;
Talent_Mage_Fire=12;
Talent_Druid_Cat=13;
Talent_Druid_Bear=14;
Talent_Druid_Cure=15;
Talent_Druid_Bird=16;
Talent_Warlock_Pain=17;
Talent_Warlock_Demon=18;
Talent_Warlock_Ruin=19;
Talent_Priest_Precept=20;
Talent_Priest_Holy=21;
Talent_Priest_Dark=22;
Talent_Paladin_Cure=23;
Talent_Paladin_Armor=24;
Talent_Paladin_Precept=25;
Talent_Rogue_Assassin=26;
Talent_Rogue_Fighter=27;
Talent_Rogue_Agile=28;
--Talent end


--Gender begin
Gender_UNKNOWN=1;
Gender_MALE=2;
Gender_FEMAIL=3;
--Gender end


--HeroPosition begin
HeroPosition_PhysicalAttack=0;
HeroPosition_MagicAttack=1;
HeroPosition_Tank=2;
HeroPosition_Nanny=3;
HeroPosition_Max_Position=4;
--HeroPosition end


--TokenType begin
TokenType_Token_Invalid=0;
TokenType_Token_Gold=29910001;
TokenType_Token_Crystal=29910002;
TokenType_Token_Honor=29910003;
TokenType_Token_Reward=29910004;
TokenType_Token_Day=29910005;
TokenType_Token_BankGold=29910006;
TokenType_Token_ExpPoint=29910007;
TokenType_Token_Exp=29910008;
TokenType_Token_Hero_Exp=29910009;
TokenType_Token_Friend=29910010;
TokenType_Token_Chivalrous=29910011;
TokenType_Token_ColdExpPoint=29910012;
TokenType_Token_GuildMoney=29910013;
TokenType_Token_GuildContribute=29910014;
TokenType_Token_GuildBonus=29910015;
TokenType_Token_LawFul=29910016;
TokenType_Token_FormationExp=29910017;
--TokenType end


--GuildPosition begin
GuildPosition_TRAINEE=0;
GuildPosition_FOUR_GROUP=1;
GuildPosition_THREE_GROUP=2;
GuildPosition_SECOND_GROUP=3;
GuildPosition_ONE_GROUP=4;
GuildPosition_FOUR_LEADER=5;
GuildPosition_THREE_LEADER=6;
GuildPosition_SECOND_LEADER=7;
GuildPosition_ONE_LEADER=8;
GuildPosition_VICE_CHIEFTAIN=9;
GuildPosition_CHIEFTAIN=10;
--GuildPosition end


--GuildBuildType begin
GuildBuildType_GUILD_MAIN=0;
GuildBuildType_GUILD_CASHBOX=1;
GuildBuildType_GUILD_SHOP=2;
GuildBuildType_GUILD_STORAGE=3;
--GuildBuildType end


--GuildBattle_Stage begin
GuildBattle_Stage_GuildBattle_Stage_None=0;
GuildBattle_Stage_GuildBattle_Stage_Prepare=1;
GuildBattle_Stage_GuildBattle_Stage_War=2;
GuildBattle_Stage_GuildBattle_Stage_End=3;
--GuildBattle_Stage end


--PlayerSceneStatus begin
PlayerSceneStatus_NONE=0;
PlayerSceneStatus_WAIT_FOR_ENTER=1;
PlayerSceneStatus_READY_TO_ENTER=2;
PlayerSceneStatus_IDEL=3;
PlayerSceneStatus_MOVING=4;
PlayerSceneStatus_CHANGE_SCENE=5;
--PlayerSceneStatus end


--RaidState begin
RaidState_RaidInit=0;
RaidState_RaidEnter=1;
RaidState_RaidRunning=2;
RaidState_RaidEnd=3;
RaidState_ToDestroy=4;
RaidState_DBLoading=5;
--RaidState end


--ResultOption begin
ResultOption_Default_Opt=-1;
ResultOption_Item_Common=0;
ResultOption_Magical_Equip=1;
ResultOption_Sell_Item=2;
ResultOption_Bank_Option=3;
ResultOption_Scene_Enter=4;
ResultOption_Dismantle_Equip=5;
ResultOption_Enhance_Equips=6;
ResultOption_Gem_Compose=7;
ResultOption_Npc_Option=8;
ResultOption_Market_Option=9;
ResultOption_Equip_Refix=10;
ResultOption_ExpPoint_Option=11;
ResultOption_Team_Opt=13;
ResultOption_Chat=14;
ResultOption_Raid_Opt=15;
ResultOption_Mission_Opt=16;
ResultOption_Activity_Opt=17;
ResultOption_BATTLE=18;
ResultOption_Guild_Opt=19;
ResultOption_SCENE_STAGE=20;
ResultOption_OnHook_Opt=21;
ResultOption_ChallengeBoss=22;
ResultOption_CaptureStage_Opt=23;
ResultOption_Player_Loot=24;
ResultOption_Player_Draw=25;
ResultOption_WolrdBoss_Opt=26;
ResultOption_Hell_Opt=27;
ResultOption_ArenaOp=28;
ResultOption_Actor_Opt=30;
ResultOption_Upgrade_Relate=31;
ResultOption_Wear_Equip=32;
ResultOption_Friend_Opt=33;
ResultOption_BattleField_Opt=34;
ResultOption_Equip_Enchant=35;
ResultOption_Equip_EnchantSlot=36;
ResultOption_Equip_Inset=37;
ResultOption_Equip_Inherit=38;
ResultOption_Skill_Opt=39;
ResultOption_Message_Opt=40;
--ResultOption end


--ResultCode begin
ResultCode_UnknownError=-1;
ResultCode_ResultOK=0;
ResultCode_BagIsFull=1;
ResultCode_GoldNotEnough=2;
ResultCode_CrystalNotEnough=3;
ResultCode_InvalidItem=4;
ResultCode_InvalidEquip=5;
ResultCode_RemoveItemFailed=6;
ResultCode_CostItemNotEnough=7;
ResultCode_InvalidConfigData=8;
ResultCode_EquipNotOnBody=9;
ResultCode_LevelLimit=10;
ResultCode_InvalidRace=11;
ResultCode_InvalidProfession=12;
ResultCode_InvalidTalent=13;
ResultCode_InvalidEquipType=14;
ResultCode_CountLimit=15;
ResultCode_InvalidContainerType=16;
ResultCode_SpaceIsFull=17;
ResultCode_InternalResult=18;
ResultCode_InvalidDestScene=19;
ResultCode_CantUseItem=20;
ResultCode_CoolDownLimit=21;
ResultCode_CantDestroyItem=22;
ResultCode_CantSellItem=23;
ResultCode_MaxLimit=24;
ResultCode_InvalidGem=25;
ResultCode_GemNotEnough=26;
ResultCode_GemComposeFailed=27;
ResultCode_NpcNotExist=28;
ResultCode_NpcLock=29;
ResultCode_TokenHonorNotEnough=30;
ResultCode_TokenRewardNotEnough=31;
ResultCode_TokenDayNotEnough=32;
ResultCode_InvalidMarketData=33;
ResultCode_ItemIsLocked=34;
ResultCode_ExpPointLimit=35;
ResultCode_ColdExpPointNotEnough=36;
ResultCode_InvalidNpcPosition=37;
ResultCode_InvalidMissionPosition=38;
ResultCode_CreateRaidFailed=39;
ResultCode_InvalidMission=40;
ResultCode_LimitedScene=41;
ResultCode_PlayerNoExist=42;
ResultCode_InvalidMissionData=43;
ResultCode_DiffMissionProcess=44;
ResultCode_SyncMissionFailed=45;
ResultCode_ActivityRewardBoxDone=46;
ResultCode_InvalidDrowGroup=47;
ResultCode_InvalidDrawType=48;
ResultCode_InvalidDrawConfigData=49;
ResultCode_MemenberTempLeave=50;
ResultCode_Team_In=51;
ResultCode_Team_Out=52;
ResultCode_Team_Exist=53;
ResultCode_Team_NoExist=54;
ResultCode_Team_NotLeader=55;
ResultCode_Team_Full=56;
ResultCode_Team_InApply=57;
ResultCode_Team_LimitLevel=58;
ResultCode_Team_NotPurpose=59;
ResultCode_Team_NotHeroPostion=60;
ResultCode_InspireFailed=61;
ResultCode_SingleLimit=62;
ResultCode_MemberNotEnough=63;
ResultCode_InvalidPosition=64;
ResultCode_ColorMax=65;
ResultCode_MissionCountLimit=66;
ResultCode_NoActor=67;
ResultCode_InvalidColor=68;
ResultCode_InvalidNpcOption=69;
ResultCode_Chat_CD=70;
ResultCode_Chat_WordLevel=71;
ResultCode_SETTING_UNACCEPTABLE=72;
ResultCode_NoBattle=73;
ResultCode_ReachPos=74;
ResultCode_OnHookMove=75;
ResultCode_OnHookBattle=76;
ResultCode_InvalidTargetPlayer=77;
ResultCode_ActorIsDead=78;
ResultCode_Guild_NoExist=80;
ResultCode_Guild_Full=81;
ResultCode_Guild_Have=82;
ResultCode_Guild_HaveDonation=83;
ResultCode_Guild_Notchieftain=84;
ResultCode_Guild_CDKNotEnough=85;
ResultCode_Guild_HaveCDK=86;
ResultCode_Guild_BuildLevelNotEnough=87;
ResultCode_Guild_BuildLevelMore=88;
ResultCode_Guild_MoneyNotEnough=89;
ResultCode_Guild_PracticePlayerlevel=90;
ResultCode_Guild_PracticeGuildlevel=91;
ResultCode_BattleCreateFailed=92;
ResultCode_ChallengeTimeLimit=94;
ResultCode_Wrong_Scene=95;
ResultCode_Wrong_Status=96;
ResultCode_HookStartOK=97;
ResultCode_HookEndOK=98;
ResultCode_BattleEnd=99;
ResultCode_HookRefreshOK=100;
ResultCode_JoinRoomFailed=101;
ResultCode_InvalidPlayerData=102;
ResultCode_NotJoinRoom=103;
ResultCode_BattleFieldOver=104;
ResultCode_RoomIsFull=105;
ResultCode_InvalidBFCamp=106;
ResultCode_SCENE_STAGE_NO_TAX=120;
ResultCode_Wait_Loading=121;
ResultCode_Win=122;
ResultCode_Lost=123;
ResultCode_SCENE_STAGE_TAX_COLLECT_OK=124;
ResultCode_SameScene=125;
ResultCode_TargetInBattle=126;
ResultCode_InvalidFormData=127;
ResultCode_InvalidBattleGroup=128;
ResultCode_InvalidBattleState=129;
ResultCode_Activity_NotStart=130;
ResultCode_Invalid_Request=131;
ResultCode_InvalidSceneStage=132;
ResultCode_TimeOut=133;
ResultCode_NoFlag=134;
ResultCode_InvalidName=135;
ResultCode_TimesLimit=136;
ResultCode_RewardAlready=137;
ResultCode_Guild_InApplyList=140;
ResultCode_Guild_DuplicateName=141;
ResultCode_Guild_BossDead=142;
ResultCode_Guild_LevelUpCD=143;
ResultCode_Guild_NoHave=144;
ResultCode_Scene_Level_Too_Low=200;
ResultCode_Target_Max_Loot_Count=201;
ResultCode_Self_Max_Loot_Count=202;
ResultCode_Target_In_Protect=203;
ResultCode_Target_Dismatch=204;
ResultCode_Target_Challenge_Done=205;
ResultCode_Target_Busy=206;
ResultCode_Team_TargetLevelLimitTen=230;
ResultCode_Team_LevelLimitTen=231;
ResultCode_Team_CreateResultOK=232;
ResultCode_Team_TargetHave=233;
ResultCode_Team_CreateInBattle=234;
ResultCode_Team_InBattleOpt=235;
ResultCode_Guild_Battle_NoOpen=250;
ResultCode_Guild_Battle_NoEnter=251;
ResultCode_Guild_Battle_NoOpponent=252;
ResultCode_Guild_Battle_NoQualify=253;
ResultCode_Guild_Battle_Stop=254;
ResultCode_FRIEND_SUCCESS=301;
ResultCode_FRIEND_FULL=302;
ResultCode_ADD_SELF_FRIEND=303;
ResultCode_PLAYER_IS_FRIEND=304;
ResultCode_FRIEND_NOTEXIST=305;
ResultCode_PLAYER_OFFLINE=306;
ResultCode_PLAYER_NOTEXIST=307;
ResultCode_PLAYER_NOTONLINE=310;
ResultCode_MESSAGE_NOT_EXIT=311;
ResultCode_MISSION_NOT_EXIT=312;
ResultCode_InvalidMissionType=350;
ResultCode_DropLimit=351;
ResultCode_InvalidMissionState=352;
ResultCode_InvalidMissionId=353;
ResultCode_InvalidFinishType=354;
ResultCode_InvalidPlayer=355;
ResultCode_NoMoreMission=356;
ResultCode_InvalidItemType=357;
ResultCode_InvalidSlot=358;
ResultCode_SlotOpened=359;
ResultCode_BatchEnchantFirst=360;
ResultCode_InvalidIndex=361;
ResultCode_MountAreadyGet=362;
ResultCode_MountIsLocked=363;
ResultCode_MountLimited=364;
ResultCode_RewardConditionLimit=365;
ResultCode_LuaScriptException=366;
ResultCode_InvalidPlayerName=367;
ResultCode_DuplicatePlayerName=368;
ResultCode_InvalidPlayerConfig=369;
ResultCode_CreatePlayerOk=370;
ResultCode_MaxLevel=371;
ResultCode_InvalidSkill=372;
ResultCode_RewardLimit=373;
ResultCode_GameServerBusy=374;
ResultCode_Bind_Actor=375;
ResultCode_TeamGroupLimit=376;
--ResultCode end


--NpcSourceType begin
NpcSourceType_InvalidSourceType=-1;
NpcSourceType_PlayerMission=0;
NpcSourceType_ScenePublic=1;
NpcSourceType_BurningUnion=2;
--NpcSourceType end


--NpcOption begin
NpcOption_NpcAdd=0;
NpcOption_NpcDel=1;
NpcOption_NpcUpdate=2;
--NpcOption end


--BattleFieldState begin
BattleFieldState_bf_state_invalid=-1;
BattleFieldState_bf_state_inroom=1;
BattleFieldState_bf_state_room_waiting=2;
BattleFieldState_bf_state_inbattle=3;
BattleFieldState_bf_state_battle_waiting=4;
BattleFieldState_bf_state_wait_leave=5;
BattleFieldState_bf_state_disagree=6;
--BattleFieldState end


--FlagOption begin
FlagOption_BeginPick=0;
FlagOption_CancelPick=1;
FlagOption_SuccessPick=2;
FlagOption_FlagDrop=3;
--FlagOption end


--RankGroup begin
RankGroup_Rank_FightValue=1;
RankGroup_Rank_Arena=2;
RankGroup_Rank_FormationFightValue=3;
RankGroup_Rank_Tower=4;
RankGroup_Rank_Level=5;
--RankGroup end


--SimpleImpactEffectType begin
SimpleImpactEffectType_EF_InvalidType=-1;
SimpleImpactEffectType_EF_PlayerSpeed=1;
SimpleImpactEffectType_EF_ActorAttribute=2;
SimpleImpactEffectType_EF_PlayerSpecial=3;
SimpleImpactEffectType_EF_ActorSpecial=4;
SimpleImpactEffectType_EF_PlayerLackeys=5;
--SimpleImpactEffectType end


--BattleGroundStatus begin
BattleGroundStatus_WAIT=0;
BattleGroundStatus_FIGHTING=1;
BattleGroundStatus_PAUSE=2;
BattleGroundStatus_WAIT_TO_END=3;
BattleGroundStatus_END=4;
BattleGroundStatus_NEXT=5;
BattleGroundStatus_RECYCLE=6;
--BattleGroundStatus end


--CaptureStatus begin
CaptureStatus_CS_INITED=0;
CaptureStatus_CS_LOAD_FIGHTING_DATA=1;
CaptureStatus_CS_DECLARE_WAR=2;
CaptureStatus_CS_FIGHTING=3;
CaptureStatus_CS_PROTECT=4;
--CaptureStatus end


--StrongHoldStatus begin
StrongHoldStatus_SHS_IDLE=0;
StrongHoldStatus_SHS_FIGHTING=1;
StrongHoldStatus_SHS_ATTACKER_WIN=2;
StrongHoldStatus_SHS_DEFENDER_WIN=3;
StrongHoldStatus_SHS_ATTACKER_WIN_BYE=4;
StrongHoldStatus_SHS_DEFENDER_WIN_BYE=5;
--StrongHoldStatus end


--SkillBuffStubType begin
SkillBuffStubType_after_do_dodge_stub_func=0;
SkillBuffStubType_after_do_critical_stub_func=1;
SkillBuffStubType_after_battle_start_stub_func=2;
SkillBuffStubType_before_battle_start_stub_func=3;
SkillBuffStubType_after_source_value_calc_stub_func=4;
SkillBuffStubType_target_after_source_value_calc_stub_func=5;
SkillBuffStubType_target_after_target_value_calc_stub_func=6;
SkillBuffStubType_after_target_value_calc_stub_func=7;
SkillBuffStubType_after_source_critical_etc_value_calc_stub_func=8;
SkillBuffStubType_before_buff_add_stub_func=9;
SkillBuffStubType_after_buff_add_stub_func=10;
SkillBuffStubType_on_time_elapse_stub_func=11;
SkillBuffStubType_after_control_prob_calc_stub_func=12;
SkillBuffStubType_after_target_critical_etc_value_calc_stub_func=13;
SkillBuffStubType_after_be_cure_stub_func=14;
SkillBuffStubType_before_be_cure_stub_func=15;
SkillBuffStubType_before_do_cure_stub_func=16;
SkillBuffStubType_after_do_cure_stub_func=17;
SkillBuffStubType_before_be_damage_stub_func=18;
SkillBuffStubType_after_be_damage_stub_func=19;
SkillBuffStubType_before_do_damage_stub_func=20;
SkillBuffStubType_after_do_damage_stub_func=21;
SkillBuffStubType_after_be_dodge_stub_func=22;
SkillBuffStubType_after_be_critical_stub_func=23;
SkillBuffStubType_after_calc_critical_percent_stub_func=24;
SkillBuffStubType_after_calc_dodge_percent_stub_func=25;
SkillBuffStubType_after_select_skill_stub_func=26;
SkillBuffStubType_after_hp_change_stub_func=27;
SkillBuffStubType_combo_point_overflow_stub_func=28;
SkillBuffStubType_after_use_active_skill_stub_func=29;
SkillBuffStubType_before_dead_stub_func=30;
SkillBuffStubType_actor_use_long_time_skill_stub_func=31;
SkillBuffStubType_max_skill_buff_stub_type=32;
--SkillBuffStubType end


--ServerType begin
ServerType_kUnknownServer=0;
ServerType_kAccountServer=1;
ServerType_kWorldServer=2;
ServerType_kBalanceServer=3;
ServerType_kGameServer=4;
--ServerType end

-- @@protoc_insertion_point(Anchor)