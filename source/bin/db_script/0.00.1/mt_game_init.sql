/*
Navicat MySQL Data Transfer

Source Server         : diablo
Source Server Version : 50537
Source Host           : localhost:3306
Source Database       : diablo_game
Target Server Type    : MYSQL
Target Server Version : 50537
File Encoding         : 65001

Date: 2015-12-13 16:04:19
*/
/*
DROP DATABASE IF EXISTS `diablo_game`;
CREATE DATABASE `diablo_game`;
USE diablo_game;
SET FOREIGN_KEY_CHECKS=0;
*/

-- ----------------------------
-- Table structure for `t_achievement`
-- ----------------------------
DROP TABLE IF EXISTS `t_achievement`;
CREATE TABLE `t_achievement` (
`townerid`  bigint(20) UNSIGNED NOT NULL COMMENT '所属玩家id' ,
`tgroup`  int(10) NOT NULL DEFAULT '-1' COMMENT '成就组' ,
`tvalue`  int(10) NOT NULL DEFAULT 0 COMMENT '成就值' ,
`trewardlevel`  int(10) NOT NULL DEFAULT 0 COMMENT '名称' ,
PRIMARY KEY (`townerid`, `tgroup`)
)
ENGINE=InnoDB
DEFAULT CHARACTER SET=utf8 COLLATE=utf8_general_ci
COMMENT='成就表'

;

-- ----------------------------
-- Table structure for `t_actor`
-- ----------------------------
DROP TABLE IF EXISTS `t_actor`;
CREATE TABLE `t_actor` (
`tid`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '角色英雄唯一id' ,
`tplayer_ownerid`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '玩家id' ,
`tpos`  tinyint(3) UNSIGNED NULL DEFAULT 0 COMMENT '0号初始站位' ,
`tcfgidnex`  mediumint(8) UNSIGNED NOT NULL DEFAULT 0 COMMENT '配置表索引' ,
`tlevel`  int(10) UNSIGNED NOT NULL COMMENT '等级' ,
`texp`  bigint(20) UNSIGNED NULL DEFAULT 0 COMMENT '经验' ,
`thight_fight_value`  int(10) NOT NULL DEFAULT 0 ,
`tlearn_skill_db0`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '学会的技能' ,
`tlearn_skill_db1`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tlearn_skill_db2`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tlearn_skill_db3`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tlearn_skill_db4`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tlearn_skill_db5`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tlearn_skill_db6`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tlearn_skill_db7`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tlearn_skill_db8`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tlearn_skill_db9`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tlearn_skill_db10`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tsetingskill_db0`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '设置的技能' ,
`tsetingskill_db1`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tsetingskill_db2`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tsetingskill_db3`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '设置的技能' ,
`tsetingskill_db4`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tsetingskill_db5`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tsetingskill_db6`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '设置的技能' ,
`tsetingskill_db7`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tsetingskill_db8`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tsetingskill_db9`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '设置的技能' ,
`tsetingskill_db10`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tsetingskill_db11`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tsetingskill_db12`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '设置的技能' ,
`tsetingskill_db13`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tsetingskill_db14`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tsetingskill_db15`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '设置的技能' ,
`tsetingskill_db16`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tsetingskill_db17`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tbuild_id`  int(10) NOT NULL DEFAULT '-1' ,
`tjoin_id`  tinyint(3) UNSIGNED NOT NULL DEFAULT 0 ,
`tprofession_special_id`  int(10) NOT NULL DEFAULT 1 ,
`tmedic_option0`  bigint(20) NOT NULL DEFAULT 0 ,
`tmedic_option1`  bigint(20) NOT NULL DEFAULT 0 ,
`tmedic_option2`  bigint(20) NOT NULL DEFAULT 0 ,
`tequip_build0`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '0号装备build' ,
`tequip_build1`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tequip_build2`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tequip_build3`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tequip_build4`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tequip_build5`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tequip_build6`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tequip_build7`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tequip_build8`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tequip_build9`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tequip_build10`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tequip_build11`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '1号装备build' ,
`tequip_build12`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tequip_build13`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tequip_build14`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tequip_build15`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tequip_build16`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tequip_build17`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tequip_build18`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tequip_build19`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tequip_build20`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tequip_build21`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tequip_build22`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '2号装备build' ,
`tequip_build23`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tequip_build24`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tequip_build25`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tequip_build26`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tequip_build27`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tequip_build28`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tequip_build29`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tequip_build30`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tequip_build31`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tequip_build32`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tequip_build33`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '3号装备build' ,
`tequip_build34`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tequip_build35`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tequip_build36`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tequip_build37`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tequip_build38`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tequip_build39`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tequip_build40`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tequip_build41`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tequip_build42`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tequip_build43`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tequip_build44`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '4号装备build' ,
`tequip_build45`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tequip_build46`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tequip_build47`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tequip_build48`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tequip_build49`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tequip_build50`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tequip_build51`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tequip_build52`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tequip_build53`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tequip_build54`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tequip_build55`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '5号装备build' ,
`tequip_build56`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tequip_build57`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tequip_build58`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tequip_build59`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tequip_build60`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tequip_build61`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tequip_build62`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tequip_build63`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tequip_build64`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tequip_build65`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tglyph0`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '0build神石' ,
`tglyph1`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '0build神石' ,
`tglyph2`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '0build神石' ,
`tglyph3`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '0build神石' ,
`tglyph4`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '0build神石' ,
`tglyph5`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '0build神石' ,
`tglyph6`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '1build神石' ,
`tglyph7`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '1build神石' ,
`tglyph8`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '1build神石' ,
`tglyph9`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '1build神石' ,
`tglyph10`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '1build神石' ,
`tglyph11`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '1build神石' ,
`tglyph12`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '2build神石' ,
`tglyph13`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '2build神石' ,
`tglyph14`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '2build神石' ,
`tglyph15`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '2build神石' ,
`tglyph16`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '2build神石' ,
`tglyph17`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '2build神石' ,
`tglyph18`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '3build神石' ,
`tglyph19`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '3build神石' ,
`tglyph20`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '3build神石' ,
`tglyph21`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '3build神石' ,
`tglyph22`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '3build神石' ,
`tglyph23`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '3build神石' ,
`tglyph24`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '4build神石' ,
`tglyph25`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '4build神石' ,
`tglyph26`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '4build神石' ,
`tglyph27`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '4build神石' ,
`tglyph28`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '4build神石' ,
`tglyph29`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '4build神石' ,
`tglyph30`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '5build神石' ,
`tglyph31`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '5build神石' ,
`tglyph32`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '5build神石' ,
`tglyph33`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '5build神石' ,
`tglyph34`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '5build神石' ,
`tglyph35`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '5build神石' ,
`tequip_prefer0`  int(10) NOT NULL DEFAULT 0 COMMENT '1build equip prefer' ,
`tequip_prefer1`  int(10) NOT NULL DEFAULT 0 COMMENT '1build equip prefer' ,
`tequip_prefer2`  int(10) NOT NULL DEFAULT 0 COMMENT '2build equip prefer' ,
`tequip_prefer3`  int(10) NOT NULL DEFAULT 0 COMMENT '3build equip prefer' ,
`tequip_prefer4`  int(10) NOT NULL DEFAULT 0 COMMENT '4build equip prefer' ,
`tequip_prefer5`  int(10) NOT NULL DEFAULT 0 COMMENT '5build equip prefer' ,
`ttop_cost_point0`  smallint(6) UNSIGNED NOT NULL DEFAULT 0 COMMENT '巅峰加点' ,
`ttop_cost_point1`  smallint(6) UNSIGNED NOT NULL DEFAULT 0 COMMENT '巅峰加点' ,
`ttop_cost_point2`  smallint(6) UNSIGNED NOT NULL DEFAULT 0 COMMENT '巅峰加点' ,
`ttop_cost_point3`  smallint(6) UNSIGNED NOT NULL DEFAULT 0 COMMENT '巅峰加点' ,
`ttop_cost_point4`  smallint(6) UNSIGNED NOT NULL DEFAULT 0 COMMENT '巅峰加点' ,
`ttop_cost_point5`  smallint(6) UNSIGNED NOT NULL DEFAULT 0 COMMENT '巅峰加点' ,
`ttop_cost_point6`  smallint(6) UNSIGNED NOT NULL DEFAULT 0 COMMENT '巅峰加点' ,
`ttop_cost_point7`  smallint(6) UNSIGNED NOT NULL DEFAULT 0 COMMENT '巅峰加点' ,
`ttop_cost_point8`  smallint(6) UNSIGNED NOT NULL DEFAULT 0 COMMENT '巅峰加点' ,
`ttop_cost_point9`  smallint(6) UNSIGNED NOT NULL DEFAULT 0 COMMENT '巅峰加点' ,
`ttop_cost_point10`  smallint(6) UNSIGNED NOT NULL DEFAULT 0 COMMENT '巅峰加点' ,
`ttop_cost_point11`  smallint(6) UNSIGNED NOT NULL DEFAULT 0 COMMENT '巅峰加点' ,
`ttop_cost_point12`  smallint(6) UNSIGNED NOT NULL DEFAULT 0 COMMENT '巅峰加点' ,
`ttop_cost_point13`  smallint(6) UNSIGNED NOT NULL DEFAULT 0 COMMENT '巅峰加点' ,
`ttop_cost_point14`  smallint(6) UNSIGNED NOT NULL DEFAULT 0 COMMENT '巅峰加点' ,
PRIMARY KEY (`tid`),
INDEX `PLAYERID` (`tplayer_ownerid`) USING BTREE 
)
ENGINE=InnoDB
DEFAULT CHARACTER SET=utf8 COLLATE=utf8_general_ci
COMMENT='角色'

;

-- ----------------------------
-- Table structure for `t_cell_data`
-- ----------------------------
DROP TABLE IF EXISTS `t_cell_data`;
CREATE TABLE `t_cell_data` (
`tcellid`  int(10) NOT NULL DEFAULT '-1' ,
`tplayer_ownerid`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tcontent`  tinyblob NOT NULL COMMENT '255字节数据块，实际只用到192字节' ,
`tlogic_type`  tinyint(3) NOT NULL DEFAULT '-1' ,
`tdata_type`  tinyint(3) NOT NULL DEFAULT 0 ,
PRIMARY KEY (`tcellid`, `tplayer_ownerid`)
)
ENGINE=InnoDB
DEFAULT CHARACTER SET=utf8 COLLATE=utf8_general_ci

;

-- ----------------------------
-- Table structure for `t_global_cell_data`
-- ----------------------------
DROP TABLE IF EXISTS `t_global_cell_data`;
CREATE TABLE `t_global_cell_data` (
`tcellid`  int(10) NOT NULL DEFAULT '-1' ,
`tplayer_ownerid`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tcontent`  tinyblob NOT NULL COMMENT '255字节数据块，实际只用到192字节' ,
`tlogic_type`  tinyint(3) NOT NULL DEFAULT '-1' ,
`tdata_type`  tinyint(3) NOT NULL DEFAULT 0 ,
PRIMARY KEY (`tcellid`, `tplayer_ownerid`)
)
ENGINE=InnoDB
DEFAULT CHARACTER SET=utf8 COLLATE=utf8_general_ci

;

-- ----------------------------
-- Table structure for `t_formation_actor`
-- ----------------------------
DROP TABLE IF EXISTS `t_formation_actor`;
CREATE TABLE `t_formation_actor` (
`tplayerid`  bigint(20) UNSIGNED NOT NULL COMMENT '玩家GUID' ,
`tplayerslotid`  int(10) NOT NULL DEFAULT 0 COMMENT '阵容ID' ,
`tactorslotid`  int(10) NOT NULL DEFAULT 0 COMMENT 'Actor位置' ,
`tactorid`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'Actorguid' ,
`tsoulset_id`  int(10) NOT NULL DEFAULT '-1' COMMENT '魂组id' ,
`tequip_1`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'Actor装备guid' ,
`tequip_2`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'Actor装备guid' ,
`tequip_3`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'Actor装备guid' ,
`tequip_4`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'Actor装备guid' ,
`tequip_5`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'Actor装备guid' ,
`tequip_6`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'Actor装备guid' ,
`tequip_7`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'Actor装备guid' ,
`tequip_8`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'Actor装备guid' ,
`tequip_9`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'Actor装备guid' ,
`tequip_10`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'Actor装备guid' ,
`tequip_11`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT 'Actor装备guid' ,
`tmedic_option0`  bigint(20) NOT NULL DEFAULT 0 COMMENT 'Actor药水' ,
`tmedic_option1`  bigint(20) NOT NULL DEFAULT 0 COMMENT 'Actor药水' ,
`tmedic_option2`  bigint(20) NOT NULL DEFAULT 0 COMMENT 'Actor药水' ,
PRIMARY KEY (`tplayerid`, `tplayerslotid`, `tactorslotid`)
)
ENGINE=InnoDB
DEFAULT CHARACTER SET=utf8 COLLATE=utf8_general_ci
COMMENT='玩家阵容英雄表'

;



-- ----------------------------
-- Table structure for `t_other_formation_actor`
-- ----------------------------
DROP TABLE IF EXISTS `t_other_formation_actor`;
CREATE TABLE `t_other_formation_actor` (
`tplayerid`  bigint(20) UNSIGNED NOT NULL COMMENT '玩家GUID' ,
`tformationnum`  int(10) NOT NULL DEFAULT 0 COMMENT '阵容ID' ,
`tactorindex`  int(10) NOT NULL DEFAULT 0 COMMENT '角色index' ,
`tactorid`  bigint(20) UNSIGNED NOT NULL COMMENT '角色GUID' ,
`tequip_buildnum`  int(10) NOT NULL DEFAULT 0 COMMENT '装备buildnum' ,
`tskill_buildnum`  int(10) NOT NULL DEFAULT 0 COMMENT '技能buildnum' ,
`tglyph_buildnum`  int(10) NOT NULL DEFAULT 0 COMMENT '传奇宝石buildnum' ,
PRIMARY KEY (`tplayerid`, `tformationnum`, `tactorindex`)
)
ENGINE=InnoDB
DEFAULT CHARACTER SET=utf8 COLLATE=utf8_general_ci
COMMENT='玩家特殊阵容英雄表'

;
-- ----------------------------
-- Table structure for `t_friend`
-- ----------------------------
DROP TABLE IF EXISTS `t_friend`;
CREATE TABLE `t_friend` (
`townerid`  bigint(20) UNSIGNED NOT NULL COMMENT '所属玩家id' ,
`ttype`  tinyint(3) NOT NULL DEFAULT 0 COMMENT '类型' ,
`tfriendid`  bigint(20) UNSIGNED NOT NULL COMMENT '关系玩家id' ,
`tname`  varchar(45) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL COMMENT '名称' ,
`thead`  tinyint(3) UNSIGNED NOT NULL DEFAULT 0 COMMENT '头像' ,
`tlevel`  int(10) NOT NULL DEFAULT 0 COMMENT '等级' ,
`tfightvalue`  int(10) NOT NULL DEFAULT 0 COMMENT '战斗力' ,
`tviplevel`  int(10) NOT NULL DEFAULT 0 COMMENT 'vip等级' ,
`tguild`  int(10) NOT NULL DEFAULT 0 COMMENT '公会id' ,
`tbgift`  int(10) NOT NULL DEFAULT 0 COMMENT '是否送礼' ,
`tgifttime`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '最后收礼时间' ,
PRIMARY KEY (`townerid`,`ttype`,`tfriendid`)
)
ENGINE=InnoDB
DEFAULT CHARACTER SET=utf8 COLLATE=utf8_general_ci
COMMENT='friend表'

;

-- ----------------------------
-- Table structure for `t_gathering`
-- ----------------------------
DROP TABLE IF EXISTS `t_gathering`;
CREATE TABLE `t_gathering` (
`tplayer_ownerid`  bigint(20) UNSIGNED NOT NULL COMMENT '玩家playerid' ,
`thookindex`  int(10) NOT NULL COMMENT '挂机索引' ,
`titemid`  int(10) NOT NULL COMMENT '材料id' ,
`tdonated`  int(10) NOT NULL COMMENT '材料分配' ,
PRIMARY KEY (`tplayer_ownerid`, `thookindex`, `titemid`)
)
ENGINE=InnoDB
DEFAULT CHARACTER SET=utf8 COLLATE=utf8_general_ci
COMMENT='采集材料分配表'

;

-- ----------------------------
-- Table structure for `t_glyph`
-- ----------------------------
DROP TABLE IF EXISTS `t_glyph`;
CREATE TABLE `t_glyph` (
`tguid`  bigint(20) UNSIGNED NOT NULL DEFAULT 18446744073709551615 COMMENT 'guid' ,
`tslotid`  smallint(6) NOT NULL DEFAULT '-1' COMMENT '索引' ,
`tplayer_ownerid`  bigint(20) UNSIGNED NOT NULL DEFAULT 18446744073709551615 COMMENT '所属玩家id' ,
`tlevel`  int(10) NOT NULL DEFAULT 0 COMMENT '等级' ,
`texp`  int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '经验' ,
`tcfgindex`  int(10) NOT NULL DEFAULT '-1' COMMENT '配置id' ,
`titem_flag`  int(10) NOT NULL DEFAULT 0 COMMENT '��Ʒ���' ,
PRIMARY KEY (`tguid`),
INDEX `PLAYERID` (`tplayer_ownerid`) USING BTREE 
)
ENGINE=InnoDB
DEFAULT CHARACTER SET=utf8 COLLATE=utf8_general_ci
COMMENT='雕文'

;

-- ----------------------------
-- Table structure for `t_item`
-- ----------------------------
DROP TABLE IF EXISTS `t_item`;
CREATE TABLE `t_item` (
`tid`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '唯一id' ,
`tslotid`  smallint(6) NOT NULL DEFAULT '-1' ,
`tplayer_ownerid`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '所属玩家id' ,
`tcount`  int(10) UNSIGNED NOT NULL COMMENT '叠加数量' ,
`tcfgindex`  int(10) UNSIGNED NOT NULL COMMENT '配置索引' ,
`titem_flag`  int(10) NOT NULL DEFAULT 0 COMMENT '��Ʒ���' ,
PRIMARY KEY (`tid`),
INDEX `PLAYERID` (`tplayer_ownerid`) USING BTREE 
)
ENGINE=InnoDB
DEFAULT CHARACTER SET=utf8 COLLATE=utf8_general_ci
COMMENT='物品列表'

;

-- ----------------------------
-- Table structure for `t_item_equip`
-- ----------------------------
DROP TABLE IF EXISTS `t_item_equip`;
CREATE TABLE `t_item_equip` (
`tid`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '唯一id' ,
`tslotid`  smallint(6) NOT NULL DEFAULT '-1' ,
`tplayer_ownerid`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '所属玩家id' ,
`tcount`  tinyint(3) UNSIGNED NULL DEFAULT 0 COMMENT '叠加数量' ,
`tcfgindex`  int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '配置表索引' ,
`tattach_count`  tinyint(3) UNSIGNED NULL DEFAULT 0 COMMENT '附加强化数' ,
`tattach0`  bigint(20) UNSIGNED NULL DEFAULT 0 ,
`tattach1`  bigint(20) UNSIGNED NULL DEFAULT 0 ,
`tattach2`  bigint(20) UNSIGNED NULL DEFAULT 0 ,
`tattach3`  bigint(20) UNSIGNED NULL DEFAULT 0 ,
`tattach4`  bigint(20) UNSIGNED NULL DEFAULT 0 ,
`tattach5`  bigint(20) UNSIGNED NULL DEFAULT 0 ,
`tattach6`  bigint(20) UNSIGNED NULL DEFAULT 0 ,
`tattach7`  bigint(20) UNSIGNED NULL DEFAULT 0 ,
`tattach8`  bigint(20) UNSIGNED NULL DEFAULT 0 ,
`tattach9`  bigint(20) UNSIGNED NULL DEFAULT 0 ,
`trandom_count`  tinyint(3) UNSIGNED NULL DEFAULT 0 COMMENT '随机属性数' ,
`trandom0`  bigint(20) UNSIGNED NULL DEFAULT 0 ,
`trandom1`  bigint(20) UNSIGNED NULL DEFAULT 0 ,
`trandom2`  bigint(20) UNSIGNED NULL DEFAULT 0 ,
`trandom3`  bigint(20) UNSIGNED NULL DEFAULT 0 ,
`trandom4`  bigint(20) UNSIGNED NULL DEFAULT 0 ,
`trandom5`  bigint(20) UNSIGNED NULL DEFAULT 0 ,
`trandom6`  bigint(20) UNSIGNED NULL DEFAULT 0 ,
`trandom7`  bigint(20) UNSIGNED NULL DEFAULT 0 ,
`tgem_slot_count`  tinyint(3) UNSIGNED NULL DEFAULT 0 COMMENT '孔数' ,
`tgem_slot0`  bigint(20) UNSIGNED NULL DEFAULT 0 ,
`tgem_slot1`  bigint(20) UNSIGNED NULL DEFAULT 0 ,
`tgem_slot2`  bigint(20) UNSIGNED NULL DEFAULT 0 ,
`tgem_slot3`  bigint(20) UNSIGNED NULL DEFAULT 0 ,
`trune_slot_count`  tinyint(3) UNSIGNED NULL DEFAULT 0 COMMENT '孔数' ,
`trune_slot0`  bigint(20) UNSIGNED NULL DEFAULT 0 ,
`trune_slot1`  bigint(20) UNSIGNED NULL DEFAULT 0 ,
`trune_slot2`  bigint(20) UNSIGNED NULL DEFAULT 0 ,
`trune_slot3`  bigint(20) UNSIGNED NULL DEFAULT 0 ,
`tprefix_count`  tinyint(3) UNSIGNED NULL DEFAULT 0 COMMENT '词缀数' ,
`tprefix`  bigint(20) UNSIGNED NULL DEFAULT 0 ,
`tenhance_level`  smallint(6) NOT NULL DEFAULT 0 ,
`tlegend_attrid`  int(10) NOT NULL DEFAULT '-1' COMMENT '传奇宝石id' ,
`tlegend_attrlv`  smallint(6) NOT NULL DEFAULT 0 COMMENT '传奇宝石等级' ,
`titem_flag`  int(10) NOT NULL DEFAULT 0 COMMENT '标记' ,
`tstar_level` smallint(6) NOT NULL DEFAULT 0 COMMENT '星级' ,
PRIMARY KEY (`tid`),
INDEX `PLAYERID` (`tplayer_ownerid`) USING BTREE 
)
ENGINE=InnoDB
DEFAULT CHARACTER SET=utf8 COLLATE=utf8_general_ci
COMMENT='装备物品'

;

-- ----------------------------
-- Table structure for `t_mail`
-- ----------------------------
DROP TABLE IF EXISTS `t_mail`;
CREATE TABLE `t_mail` (
`tmailid`  bigint(20) UNSIGNED NOT NULL COMMENT '邮件id' ,
`tstarttime`  bigint(20) UNSIGNED NOT NULL COMMENT '邮件时间' ,
`tmail_ownerid`  bigint(20) UNSIGNED NOT NULL COMMENT '所属玩家id' ,
`tmail_fromid`  bigint(20) UNSIGNED NOT NULL COMMENT '来源id' ,
`tmail_fromname`  varchar(45) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL COMMENT '来源名称' ,
`tmail_type`  tinyint(3) NULL DEFAULT 0 COMMENT '邮件类型' ,
`tmail_title`  varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL COMMENT 'title' ,
`tmail_content`  varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL COMMENT '内容' ,
`titemid0`  int(10) NOT NULL DEFAULT 0 COMMENT '物品0' ,
`titemcount0`  int(10) NOT NULL DEFAULT 0 COMMENT '物品0' ,
`titemid1`  int(10) NOT NULL DEFAULT 0 COMMENT '物品1' ,
`titemcount1`  int(10) NOT NULL DEFAULT 0 COMMENT '物品1' ,
`titemid2`  int(10) NOT NULL DEFAULT 0 COMMENT '物品2' ,
`titemcount2`  int(10) NOT NULL DEFAULT 0 COMMENT '物品2' ,
`titemid3`  int(10) NOT NULL DEFAULT 0 COMMENT '物品3' ,
`titemcount3`  int(10) NOT NULL DEFAULT 0 COMMENT '物品3' ,
`titemid4`  int(10) NOT NULL DEFAULT 0 COMMENT '物品4' ,
`titemcount4`  int(10) NOT NULL DEFAULT 0 COMMENT '物品4' ,
PRIMARY KEY (`tmailid`)
)
ENGINE=InnoDB
DEFAULT CHARACTER SET=utf8 COLLATE=utf8_general_ci
COMMENT='邮件'

;

-- ----------------------------
-- Table structure for `t_manager`
-- ----------------------------
DROP TABLE IF EXISTS `t_manager`;
CREATE TABLE `t_manager` (
`tserverid`  int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '默认主键都为0' ,
`tshutdowntime`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '关服时间' ,
PRIMARY KEY (`tserverid`)
)
ENGINE=InnoDB
DEFAULT CHARACTER SET=utf8 COLLATE=utf8_general_ci
COMMENT='管理器表'

;

-- ----------------------------
-- Table structure for `t_monster_pet`
-- ----------------------------
DROP TABLE IF EXISTS `t_monster_pet`;
CREATE TABLE `t_monster_pet` (
`townerid`  bigint(20) UNSIGNED NOT NULL COMMENT '所属玩家id' ,
`tpetid`  int(10) NOT NULL DEFAULT 0 COMMENT '唯一id' ,
`tcharactertype`  int(10) NOT NULL DEFAULT 0 COMMENT '性格' ,
`tloyalvalue`  int(10) NOT NULL DEFAULT 0 COMMENT '忠诚度' ,
`tonhookstate`  int(10) NOT NULL DEFAULT 0 COMMENT '挂机状态' ,
`tonhookid`  int(10) NOT NULL DEFAULT 0 COMMENT '挂机id' ,
`tstarttime`  int(10) NOT NULL DEFAULT 0 COMMENT '挂机开始时间' ,
`tlefttime`  int(10) NOT NULL DEFAULT 0 COMMENT '挂机剩余时间' ,
`tpartner`  tinyint(3) NOT NULL DEFAULT 0 COMMENT 'B合作者' ,
PRIMARY KEY (`townerid`, `tpetid`)
)
ENGINE=InnoDB
DEFAULT CHARACTER SET=utf8 COLLATE=utf8_general_ci
COMMENT='怪物宝宝表'

;

-- ----------------------------
-- Table structure for `t_onhook`
-- ----------------------------
DROP TABLE IF EXISTS `t_onhook`;
CREATE TABLE `t_onhook` (
`tplayer_ownerid`  bigint(20) UNSIGNED NOT NULL COMMENT '玩家playerid' ,
`thooktype`  int(10) NOT NULL COMMENT '挂机类型' ,
`thookindex`  int(10) NOT NULL COMMENT '挂机索引' ,
`tcurrentonhookid`  int(10) NOT NULL COMMENT '当前挂机ID' ,
`tcurrentonhookstarttime`  bigint(20) UNSIGNED NOT NULL COMMENT '当前挂机开始时间' ,
`tcurrentonhooklefttime`  int(10) NOT NULL COMMENT '当前挂机剩余时间' ,
`tpetid1`  int(10) NOT NULL DEFAULT '-1' COMMENT '萌宠ID' ,
`tpetid2`  int(10) NOT NULL DEFAULT '-1' COMMENT '萌宠ID' ,
`tpetid3`  int(10) NOT NULL DEFAULT '-1' COMMENT '萌宠ID' ,
`tpetid4`  int(10) NOT NULL DEFAULT '-1' COMMENT '萌宠ID' ,
`tonhookid1`  int(10) NOT NULL DEFAULT '-1' COMMENT '挂机ID' ,
`tonhookid2`  int(10) NOT NULL DEFAULT '-1' COMMENT '挂机ID' ,
`tonhookid3`  int(10) NOT NULL DEFAULT '-1' COMMENT '挂机ID' ,
`tonhookid4`  int(10) NOT NULL DEFAULT '-1' COMMENT '挂机ID' ,
`tonhookid5`  int(10) NOT NULL DEFAULT '-1' COMMENT '挂机ID' ,
`tonhookid6`  int(10) NOT NULL DEFAULT '-1' COMMENT '挂机ID' ,
`tonhookid7`  int(10) NOT NULL DEFAULT '-1' COMMENT '挂机ID' ,
`tonhookid8`  int(10) NOT NULL DEFAULT '-1' COMMENT '挂机ID' ,
`tonhookid9`  int(10) NOT NULL DEFAULT '-1' COMMENT '挂机ID' ,
`tonhookid10`  int(10) NOT NULL DEFAULT '-1' COMMENT '挂机ID' ,
PRIMARY KEY (`tplayer_ownerid`, `thooktype`, `thookindex`)
)
ENGINE=InnoDB
DEFAULT CHARACTER SET=utf8 COLLATE=utf8_general_ci
COMMENT='挂机管理表'

;

-- ----------------------------
-- Table structure for `t_player`
-- ----------------------------
DROP TABLE IF EXISTS `t_player`;
CREATE TABLE `t_player` (
`tid`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '玩家id' ,
`taccount`  varchar(45) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '' COMMENT '玩家账号' ,
`tname`  varchar(45) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL COMMENT '角色名称' ,
`tgender`  tinyint(3) UNSIGNED NOT NULL COMMENT '性别' ,
`thead`  tinyint(3) UNSIGNED NOT NULL DEFAULT 0 COMMENT '头像' ,
`ttitle`  varchar(45) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL COMMENT '称号' ,
`tprestige`  int(10) UNSIGNED NOT NULL COMMENT '声望' ,
`tprestige_killing`  int(10) UNSIGNED NOT NULL DEFAULT 0 ,
`tcoin`  bigint(20) UNSIGNED NOT NULL COMMENT '金币' ,
`ttoken`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '（RMB）代币' ,
`tbind_token`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '（RMB）bind代币' ,
`thonor`  bigint(20) NOT NULL DEFAULT 0 COMMENT '荣誉值' ,
`trank`  int(10) NOT NULL DEFAULT 0 COMMENT '军阶' ,
`tbabeltoken`  bigint(20) NOT NULL DEFAULT 0 COMMENT '通天塔代币' ,
`tachieve`  bigint(20) NOT NULL DEFAULT 0 COMMENT '成就值' ,
`tlevel`  tinyint(3) NOT NULL DEFAULT 0 COMMENT '等级' ,
`texp`  bigint(20) NOT NULL DEFAULT 0 COMMENT '经验' ,
`thight_fight_value`  int(10) NOT NULL DEFAULT 0 ,
`tfirst_kill_boss_scene_id_1`  int(10) NOT NULL DEFAULT 0 ,
`tfirst_kill_boss_scene_id_2`  int(10) NOT NULL DEFAULT 0 ,
`tscene_explored_1`  int(10) NOT NULL DEFAULT 0 ,
`tscene_explored_2`  int(10) NOT NULL DEFAULT 0 ,
`tbabel_floor`  int(10) NOT NULL DEFAULT 0 COMMENT '通天塔层数' ,
`tbabel_challenge_times`  int(10) NOT NULL DEFAULT 0 COMMENT '通天塔挑战次数' ,
`tauto_smelt`  tinyint(3) NOT NULL DEFAULT 0 ,
`tauto_sell`  tinyint(3) NOT NULL DEFAULT 0 ,
`thprevert_limit`  int(10) NOT NULL DEFAULT 100 ,
`tredmedicine_option`  int(10) NOT NULL DEFAULT 0 COMMENT '吃药角色血量设置',
`tlogout_time`  bigint(20) UNSIGNED NOT NULL ,
`tbcreate`  tinyint(3) NOT NULL DEFAULT 0 ,
`tguidestep`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tvip_level`  tinyint(3) NULL DEFAULT 0 COMMENT 'vip等级' ,
`tvip_exp`  int(10) NULL DEFAULT 0 COMMENT 'vip经验' ,
`tvip_day_reward_time`  bigint(20) UNSIGNED NULL DEFAULT 0 COMMENT 'vip每日奖励领取时间' ,
`tlevel_reward_index`  int(10) NULL DEFAULT 0 COMMENT '等级奖励索引' ,
`tscene_reward_index`  int(10) NULL DEFAULT 0 COMMENT '通关奖励索引' ,
`tbattle_talent_left_points`  int(10) NULL DEFAULT 0 COMMENT '剩余战斗天赋点' ,
`tproduct_talent_left_points`  int(10) NULL DEFAULT 0 COMMENT '剩余生成天赋点' ,
`tnewbie_step`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '玩家新手' ,
`tcollect_altars_count`  tinyint(3) NULL DEFAULT 0 COMMENT '可收集祭坛数' ,
`tfriends_cout`  int(10) NULL DEFAULT 0 COMMENT '最大好友数量' ,
`tcow_farm_count`  tinyint(3) NOT NULL DEFAULT 0 COMMENT '奶牛关次数' ,
`train_bow_count`  tinyint(3) NOT NULL DEFAULT 0 COMMENT '彩虹关次数' ,
`take_rank_reward_today`  tinyint(3) UNSIGNED NOT NULL DEFAULT 0 ,
`tdonate_today`  tinyint(3) UNSIGNED NOT NULL DEFAULT 0 ,
`tcurinvade_scene`  int(10) NOT NULL DEFAULT 0 COMMENT '当前争夺场景ID' ,
`tcurinvade_kill_count`  int(10) NOT NULL DEFAULT 0 COMMENT '当前争夺场景的杀怪数量' ,
`tcurinvade_fightboss_damage`  bigint(20) NOT NULL DEFAULT 0 COMMENT '当前争夺场景的杀boss伤害' ,
`tpvp_challenge_times`  int(10) NOT NULL DEFAULT 0 COMMENT '竞技场挑战次数' ,
`tbuildlevel0`  int(10) NOT NULL DEFAULT 1 COMMENT '0号建筑等级' ,
`tbuildlevel1`  int(10) NOT NULL DEFAULT 1 COMMENT '1号建筑等级' ,
`trefreshmonster_time`  int(10) NOT NULL DEFAULT 100 COMMENT '刷怪时间设置百分比' ,
`tfriend_value`  int(10) NOT NULL DEFAULT 0 COMMENT '友好值' ,
`tbi_create_channel`  varchar(45) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL COMMENT '创建时的渠道' ,
`tbi_create_sub_channel`  varchar(45) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL COMMENT '创建时的子渠道' ,
`tbi_create_plat`  varchar(45) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL COMMENT '创建时的平台' ,
`tbi_create_udid`  varchar(45) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL COMMENT '创建时的设备唯一号' ,
`tbi_create_ip`  varchar(45) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL COMMENT '创建时的ip' ,
`tbi_create_time`  bigint(20) UNSIGNED NOT NULL COMMENT '创建时间' ,
`tgm_level`  int(10) NOT NULL DEFAULT 0 COMMENT 'GM权限等级' ,
`tlast_common_hook_scene_id`  int(10) NOT NULL DEFAULT 0 COMMENT '最后一次挂机的普通场景id' ,
`tshutup_time`  int(10) NOT NULL DEFAULT 0 COMMENT '禁言时间' ,
`tmax_formation_count`  int(10) NOT NULL DEFAULT 0 COMMENT '最大阵容数量' ,
`thours_pvp_refreshtimes`  int(10) NOT NULL DEFAULT 0 COMMENT '小竞技场刷新次数' ,
`tbrushboss_refreshtimes`  int(10) NOT NULL DEFAULT 0 COMMENT '刷BOSS次数' ,
`thours_win_times`  int(10) NOT NULL DEFAULT 0 COMMENT '角斗场胜利次数' ,
`thours_challenge_times`  int(10) NOT NULL DEFAULT 0 COMMENT '角斗场挑战次数' ,
`tinvade`  tinyint(3) NOT NULL DEFAULT 0 COMMENT '是否在推图' ,
`tpoint_formation_0`  int(10) NOT NULL DEFAULT 0 COMMENT '指向性阵容ONHOOK' ,
`tpoint_formation_1`  int(10) NOT NULL DEFAULT 0 COMMENT '指向性阵容PVP' ,
`tpoint_formation_2`  int(10) NOT NULL DEFAULT 0 COMMENT '指向性阵容BOSS' ,
`tpoint_formation_3`  int(10) NOT NULL DEFAULT 0 COMMENT '指向性阵容BABEL' ,
`tpoint_formation_4`  int(10) NOT NULL DEFAULT 0 COMMENT '指向性阵容RAIN' ,
`tpoint_formation_5`  int(10) NOT NULL DEFAULT 0 COMMENT '指向性阵容COW' ,
`ttop_level`  int(10) NOT NULL DEFAULT 0 COMMENT '巅峰等级' ,
`ttop_exp`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '巅峰经验' ,
PRIMARY KEY (`tid`)
)
ENGINE=InnoDB
DEFAULT CHARACTER SET=utf8 COLLATE=utf8_general_ci
COMMENT=' 玩家数据'

;


-- ----------------------------
-- Table structure for `t_player_talent`
-- ----------------------------
DROP TABLE IF EXISTS `t_player_talent`;
CREATE TABLE `t_player_talent` (
`tplayer_ownerid`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '玩家id' ,
`tid`  int(10) NOT NULL DEFAULT 0 COMMENT '天赋ID' ,
PRIMARY KEY (`tplayer_ownerid`, `tid`)
)
ENGINE=InnoDB
DEFAULT CHARACTER SET=utf8 COLLATE=utf8_general_ci
COMMENT='玩家天赋'

;

-- ----------------------------
-- Table structure for `t_pvp_distribution`
-- ----------------------------
DROP TABLE IF EXISTS `t_pvp_distribution`;
CREATE TABLE `t_pvp_distribution` (
`t_segment_id`  int(10) NOT NULL DEFAULT 0 ,
`t_segment_count`  int(10) NOT NULL DEFAULT 0 ,
PRIMARY KEY (`t_segment_id`)
)
ENGINE=InnoDB
DEFAULT CHARACTER SET=utf8 COLLATE=utf8_general_ci
COMMENT='pvp_各段分布'

;

-- ----------------------------
-- Table structure for `t_pvp_player`
-- ----------------------------
DROP TABLE IF EXISTS `t_pvp_player`;
CREATE TABLE `t_pvp_player` (
`t_playerguid`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '玩家guid' ,
`t_playeraccount`  varchar(45) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '' COMMENT '玩家账号' ,
`t_name`  varchar(45) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL COMMENT '角色名称' ,
`t_level`  int(10) NOT NULL DEFAULT 0 COMMENT '等级' ,
`t_starlevel`  int(10) NOT NULL DEFAULT 0 COMMENT '星级' ,
`t_wintimes`  int(10) NOT NULL DEFAULT 0 COMMENT '胜利次数' ,
`t_pvptimes`  int(10) NOT NULL DEFAULT 0 COMMENT '战斗次数' ,
`t_lastwin`  tinyint(3) NOT NULL DEFAULT 0 COMMENT '上次挑战是否胜利' ,
`t_win_streak`  int(10) NOT NULL DEFAULT 0 COMMENT '连胜次数' ,
`t_actor_1`  bigint(20) UNSIGNED NOT NULL COMMENT '英雄1guid' ,
`t_actor_2`  bigint(20) UNSIGNED NOT NULL COMMENT '英雄2guid' ,
`t_actor_3`  bigint(20) UNSIGNED NOT NULL COMMENT '英雄3guid' ,
PRIMARY KEY (`t_playerguid`)
)
ENGINE=InnoDB
DEFAULT CHARACTER SET=utf8 COLLATE=utf8_general_ci
COMMENT='pvp英雄表'

;

-- ----------------------------
-- Table structure for `t_pvp_ranklist`
-- ----------------------------
DROP TABLE IF EXISTS `t_pvp_ranklist`;
CREATE TABLE `t_pvp_ranklist` (
`t_ranknum`  int(10) NOT NULL DEFAULT 0 COMMENT '排名' ,
`t_playerguid`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '玩家guid' ,
PRIMARY KEY (`t_ranknum`)
)
ENGINE=InnoDB
DEFAULT CHARACTER SET=utf8 COLLATE=utf8_general_ci
COMMENT='pvp排名表'

;

-- ----------------------------
-- Table structure for `t_scene`
-- ----------------------------
DROP TABLE IF EXISTS `t_scene`;
CREATE TABLE `t_scene` (
`tid`  bigint(20) UNSIGNED NOT NULL ,
`tkillmonster_count`  int(10) NOT NULL COMMENT '场景地图杀怪数' ,
`tboss_lefthp`  bigint(20) NOT NULL DEFAULT 0 COMMENT 'boss剩余血量' ,
`tkillboss_lefttime`  int(10) NOT NULL DEFAULT 0 COMMENT 'boss剩余时间' ,
`tstate`  tinyint(3) NOT NULL COMMENT '状态ID' ,
PRIMARY KEY (`tid`)
)
ENGINE=InnoDB
DEFAULT CHARACTER SET=utf8 COLLATE=utf8_general_ci
COMMENT='场景地图'

;

-- ----------------------------
-- Table structure for `t_soul`
-- ----------------------------
DROP TABLE IF EXISTS `t_soul`;
CREATE TABLE `t_soul` (
`tid`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '唯一id' ,
`tslotid`  int(10) NOT NULL DEFAULT '-1' ,
`tplayer_ownerid`  bigint(20) UNSIGNED NOT NULL COMMENT '所属玩家id' ,
`tcfgindex`  int(10) UNSIGNED NOT NULL DEFAULT 0 COMMENT '配置表索引' ,
`tlevel`  tinyint(3) UNSIGNED NULL DEFAULT 0 COMMENT '等级' ,
`texp`  bigint(20) UNSIGNED NULL DEFAULT 0 COMMENT '经验' ,
`tstep`  tinyint(3) UNSIGNED NULL DEFAULT 0 COMMENT '阶段' ,
PRIMARY KEY (`tslotid`, `tplayer_ownerid`),
INDEX `PLAYERID` (`tplayer_ownerid`) USING BTREE 
)
ENGINE=InnoDB
DEFAULT CHARACTER SET=utf8 COLLATE=utf8_general_ci
COMMENT='魂魄'

;

-- ----------------------------
-- Table structure for `t_soul_part`
-- ----------------------------
DROP TABLE IF EXISTS `t_soul_part`;
CREATE TABLE `t_soul_part` (
`tcfgindex`  int(10) NOT NULL DEFAULT 0 ,
`tplayer_ownerid`  bigint(20) UNSIGNED NOT NULL DEFAULT 0 ,
`tcount`  int(10) NOT NULL DEFAULT 0 ,
PRIMARY KEY (`tcfgindex`, `tplayer_ownerid`)
)
ENGINE=InnoDB
DEFAULT CHARACTER SET=utf8 COLLATE=utf8_general_ci
COMMENT='魂魄碎片'

;

-- ----------------------------
-- Table structure for `t_brushboss`
-- ----------------------------
DROP TABLE IF EXISTS `t_brushboss`;
CREATE TABLE `t_brushboss` (
`id` int(10) NOT NULL AUTO_INCREMENT,
`tbossid0` int(10) NOT NULL DEFAULT 0 COMMENT 'bossid',
`tlegenditemid0` int(10) NOT NULL DEFAULT 0 COMMENT '传奇宝石ID',
`tbossid1` int(10) NOT NULL DEFAULT 0 COMMENT 'bossid',
`tlegenditemid1` int(10) NOT NULL DEFAULT 0 COMMENT '传奇宝石ID',
`tbossid2` int(10) NOT NULL DEFAULT 0 COMMENT 'bossid',
`tlegenditemid2` int(10) NOT NULL DEFAULT 0 COMMENT '传奇宝石ID',
PRIMARY KEY (`id`)
)
ENGINE=InnoDB
DEFAULT CHARACTER SET=utf8 COLLATE=utf8_general_ci
COMMENT='刷BOSS'

;


-- ----------------------------
-- Table structure for `t_playerears`
-- ----------------------------
DROP TABLE IF EXISTS `t_playerears`;
CREATE TABLE `t_playerears` (
`townerid`	bigint(20) UNSIGNED NOT NULL DEFAULT 0  COMMENT '所属玩家id' ,
`tplayer_guid`	bigint(20) UNSIGNED NOT NULL DEFAULT 0 COMMENT '对方耳朵ID' ,
`tplayer_name`	varchar(45) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL COMMENT '名称' ,
`tlayer_headindex`	tinyint(3) UNSIGNED NOT NULL DEFAULT 0 COMMENT '头像' ,
PRIMARY KEY (`townerid`, `tplayer_guid`)
)
ENGINE=InnoDB
DEFAULT CHARACTER SET=utf8 COLLATE=utf8_general_ci
COMMENT='耳朵表'
;
-- ----------------------------
-- Procedure structure for `pro_db_getversion`
-- ----------------------------
DROP PROCEDURE IF EXISTS `pro_db_getversion`;
DELIMITER ;;
CREATE PROCEDURE `pro_db_getversion`(
out `o_db_version` varchar(10)
)
BEGIN

DECLARE db_version    varchar(10);
SET db_version = '0.00.01';
SELECT db_version into o_db_version;

END
;;
DELIMITER ;


-- ----------------------------
-- Procedure structure for `pro_pvp_distribution`
-- ----------------------------
DROP PROCEDURE IF EXISTS `pro_pvp_distribution`;
DELIMITER ;;
CREATE PROCEDURE `pro_pvp_distribution`(
s_segment_id 		int,
s_segment_count		int)
BEGIN
	declare r_segment_id  int;
    set r_segment_id=-1;
#START TRANSACTION;
SELECT t_segment_id INTO r_segment_id FROM t_pvp_distribution
    WHERE t_segment_id=s_segment_id LIMIT 1;
	IF r_segment_id<>-1 THEN
		UPDATE t_pvp_distribution SET t_segment_id=s_segment_id,
		t_segment_count=s_segment_count
		WHERE
		t_segment_id=s_segment_id LIMIT 1;
	ELSE
		INSERT INTO t_pvp_distribution (t_segment_id,t_segment_count) 
		VALUES (s_segment_id,s_segment_count);
	END IF;
#COMMIT;
END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `pro_pvp_ranklist`
-- ----------------------------
DROP PROCEDURE IF EXISTS `pro_pvp_ranklist`;
DELIMITER ;;
CREATE PROCEDURE `pro_pvp_ranklist`(
s_ranknum 			int,
s_playerguid		bigint unsigned)
BEGIN
	declare r_ranknum  int;
    set r_ranknum=-1;
#START TRANSACTION;
SELECT t_ranknum INTO r_ranknum FROM t_pvp_ranklist
    WHERE t_ranknum=s_ranknum LIMIT 1;
	IF r_ranknum<>-1 THEN
		UPDATE t_pvp_ranklist SET t_ranknum=s_ranknum,
		t_playerguid=s_playerguid
		WHERE
		t_ranknum=s_ranknum LIMIT 1;
	ELSE
		INSERT INTO t_pvp_ranklist (t_ranknum,t_playerguid) 
		VALUES (s_ranknum,s_playerguid);
	END IF;
#COMMIT;
END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `pro_save_formation_actor`
-- ----------------------------
DROP PROCEDURE IF EXISTS `pro_save_formation_actor`;
DELIMITER ;;
CREATE PROCEDURE `pro_save_formation_actor`(
splayerid		bigint unsigned,
sformationnum	int,
sactorindex		int,
sactorid		bigint unsigned,
sequip_buildnum	int,
sskill_buildnum	int,
sglyph_buildnum	int
)
BEGIN
  declare rformationnum  int;
  set rformationnum=-1;
#START TRANSACTION;
SELECT tformationnum INTO rformationnum FROM t_other_formation_actor
    WHERE tplayerid=splayerid AND tformationnum=sformationnum AND tactorindex=sactorindex LIMIT 1;
	IF rformationnum<>-1 THEN
		UPDATE t_other_formation_actor SET 
		tplayerid=splayerid,
		tformationnum=sformationnum,
		tactorindex=sactorindex,
		tactorid=sactorid,
		tequip_buildnum=sequip_buildnum,
		tskill_buildnum=sskill_buildnum,
		tglyph_buildnum=sglyph_buildnum
		WHERE tplayerid=splayerid AND tformationnum=sformationnum AND tactorindex=sactorindex LIMIT 1;
	ELSE
		INSERT INTO t_other_formation_actor (tplayerid,tformationnum,tactorindex,tactorid,tequip_buildnum,tskill_buildnum,tglyph_buildnum) 
		VALUES (splayerid,sformationnum,sactorindex,sactorid,sequip_buildnum,sskill_buildnum,sglyph_buildnum);
	END IF;
#COMMIT;
END
;;
DELIMITER ;


-- ----------------------------
-- Procedure structure for `pro_save_player`
-- ----------------------------
DROP PROCEDURE IF EXISTS `pro_save_player`;
DELIMITER ;;
CREATE PROCEDURE `pro_save_player`(
sid bigint unsigned,
saccount varchar(45),
sname varchar(45),
sgender int,
shead int,
stitle varchar(45),
sprestige int unsigned,
sprestige_killing int unsigned,
scoin bigint unsigned,
stoken bigint unsigned,
shonor bigint,
srank int,
sbabeltoken bigint,
sachieve bigint,
slevel int,
sexp bigint,
shight_fight_value int,
sfirst_kill_boss_scene_id_1 int,
sfirst_kill_boss_scene_id_2 int,
sscene_explored_1 int,
sscene_explored_2 int,
sbabel_floor int,
sbabel_challenge_times int,
sauto_smelt int,
sauto_sell int,
shprevert_limit int,
slogout_time bigint unsigned,
sbcreate int,
sguidestep bigint unsigned,
svip_level int,
svip_exp int,
svip_day_reward_time bigint unsigned,
slevel_reward_index int,
sscene_reward_index int,
sbattle_talent_left_points int,
sproduct_talent_left_points int,
snewbie_step bigint unsigned,
scollect_altars_count int,
sfriends_cout int,
scow_farm_count int,
srain_bow_count int,
sake_rank_reward_today int,
sdonate_today int,
scurinvade_scene int,
scurinvade_kill_count int,
scurinvade_fightboss_damage bigint unsigned,
spvp_challenge_times int,
sbuildlevel0 int,
sbuildlevel1 int,
srefreshmonster_time int,
sfriend_value int,
sbi_create_channel varchar(45),
sbi_create_sub_channel varchar(45),
sbi_create_plat varchar(45),
sbi_create_udid varchar(45),
sbi_create_ip varchar(45),
sbi_create_time bigint unsigned,
sbindtoken bigint unsigned,
sgm_level int,
slast_common_hook_scene_id int,
sshutup_time int,
smax_formation_count int,
shours_pvp_refreshtimes int,
sredmedicine_option int,
sbrushboss_refreshtimes int,
shours_win_times int,
shours_challenge_times int,
sinvade int,
spoint_formation_0 int,
spoint_formation_1 int,
spoint_formation_2 int,
spoint_formation_3 int,
spoint_formation_4 int,
spoint_formation_5 int,
stop_level int,
stop_exp bigint unsigned
)
BEGIN
	declare rid  bigint unsigned;
    set rid=18446744073709551615;
#START TRANSACTION;
SELECT tid INTO rid FROM t_player
    WHERE tid=sid LIMIT 1;
	IF rid<>18446744073709551615 THEN
		UPDATE t_player SET tid=sid,
		taccount=saccount,
		tname=sname,
		tgender=sgender,
		thead=shead,
		ttitle=stitle,
		tprestige=sprestige,
		tcoin=scoin,
		ttoken=stoken,
		tbind_token=sbindtoken,
		thonor=shonor,
		trank=srank,
		tbabeltoken=sbabeltoken,
		tachieve=sachieve,
		tlevel=slevel,
		texp=sexp,
		thight_fight_value=shight_fight_value,
		tfirst_kill_boss_scene_id_1=sfirst_kill_boss_scene_id_1,
		tfirst_kill_boss_scene_id_2=sfirst_kill_boss_scene_id_2,
		tscene_explored_1=sscene_explored_1,
		tscene_explored_2=sscene_explored_2,
		tbabel_floor=sbabel_floor,
		tbabel_challenge_times=sbabel_challenge_times,
		tauto_smelt=sauto_smelt,
		tauto_sell=sauto_sell,
		thprevert_limit=shprevert_limit,
		tlogout_time=slogout_time,
		tbcreate=sbcreate,
		tguidestep=sguidestep,
		tvip_level=svip_level,
		tvip_exp=svip_exp,
		tvip_day_reward_time=svip_day_reward_time,
		tlevel_reward_index=slevel_reward_index,
		tscene_reward_index=sscene_reward_index,
		tbattle_talent_left_points=sbattle_talent_left_points,
		tproduct_talent_left_points=sproduct_talent_left_points,
		tnewbie_step=snewbie_step,
		tcollect_altars_count=scollect_altars_count,
		tfriends_cout=sfriends_cout,
		tcow_farm_count=scow_farm_count,
		train_bow_count=srain_bow_count,
		take_rank_reward_today=sake_rank_reward_today,
		tdonate_today=sdonate_today,
		tprestige_killing=sprestige_killing,
		tcurinvade_scene=scurinvade_scene,
		tcurinvade_kill_count=scurinvade_kill_count,
		tcurinvade_fightboss_damage=scurinvade_fightboss_damage,
		tpvp_challenge_times=spvp_challenge_times,
		tbuildlevel0=sbuildlevel0,
		tbuildlevel1=sbuildlevel1,
		trefreshmonster_time=srefreshmonster_time,
		tfriend_value=sfriend_value,
		tbi_create_channel=sbi_create_channel,
		tbi_create_sub_channel=sbi_create_sub_channel,
		tbi_create_plat=sbi_create_plat,
		tbi_create_udid=sbi_create_udid,
		tbi_create_ip=sbi_create_ip,
		tbi_create_time=sbi_create_time,
		tgm_level=sgm_level,
		tlast_common_hook_scene_id=slast_common_hook_scene_id,
		tshutup_time = sshutup_time,
		tmax_formation_count = smax_formation_count,
		thours_pvp_refreshtimes = shours_pvp_refreshtimes,
		tredmedicine_option = sredmedicine_option,
		tbrushboss_refreshtimes=sbrushboss_refreshtimes,
		thours_win_times=shours_win_times,
		thours_challenge_times=shours_challenge_times,
		tinvade=sinvade,
		tpoint_formation_0 = spoint_formation_0,
		tpoint_formation_1 = spoint_formation_1,
		tpoint_formation_2 = spoint_formation_2,
		tpoint_formation_3 = spoint_formation_3,
		tpoint_formation_4 = spoint_formation_4,
		tpoint_formation_5 = spoint_formation_5,
		ttop_level = stop_level,
		ttop_exp = stop_exp
		WHERE tid=sid LIMIT 1;
	ELSE
		INSERT INTO t_player (tid,taccount,tname,tgender,thead,ttitle,tprestige,tcoin,ttoken,tbind_token,thonor,trank,tbabeltoken,tachieve,tlevel,texp,thight_fight_value,tfirst_kill_boss_scene_id_1,tfirst_kill_boss_scene_id_2,tscene_explored_1,tscene_explored_2,tbabel_floor,tbabel_challenge_times,
		tauto_smelt,tauto_sell,thprevert_limit,tlogout_time,tbcreate,tguidestep,tvip_level,tvip_exp,tvip_day_reward_time,tlevel_reward_index,tscene_reward_index,tbattle_talent_left_points,tproduct_talent_left_points,
		tnewbie_step,tcollect_altars_count,tfriends_cout,tcow_farm_count,train_bow_count,take_rank_reward_today,tdonate_today,tprestige_killing,tcurinvade_scene,tcurinvade_kill_count,tcurinvade_fightboss_damage,
		tpvp_challenge_times,tbuildlevel0,tbuildlevel1,trefreshmonster_time,tfriend_value,tbi_create_channel,tbi_create_sub_channel,tbi_create_plat,tbi_create_udid,tbi_create_ip,tbi_create_time,tgm_level,tlast_common_hook_scene_id,tshutup_time,tmax_formation_count,
		thours_pvp_refreshtimes,tredmedicine_option,tbrushboss_refreshtimes,thours_win_times,thours_challenge_times,tinvade,tpoint_formation_0,tpoint_formation_1,tpoint_formation_2,tpoint_formation_3,tpoint_formation_4,tpoint_formation_5,ttop_level,ttop_exp)  
		VALUES (sid,saccount,sname,sgender,shead,stitle,sprestige,scoin,stoken,sbindtoken,shonor,srank,sbabeltoken,sachieve,slevel,sexp,shight_fight_value,sfirst_kill_boss_scene_id_1,sfirst_kill_boss_scene_id_2,sscene_explored_1,sscene_explored_2,sbabel_floor,sbabel_challenge_times,
		sauto_smelt,sauto_sell,shprevert_limit,slogout_time,sbcreate,sguidestep,svip_level,svip_exp,svip_day_reward_time,slevel_reward_index,sscene_reward_index,sbattle_talent_left_points,sproduct_talent_left_points,
		snewbie_step,scollect_altars_count,sfriends_cout,scow_farm_count,srain_bow_count,sake_rank_reward_today,sdonate_today,sprestige_killing,scurinvade_scene,scurinvade_kill_count,scurinvade_fightboss_damage,
		spvp_challenge_times,sbuildlevel0,sbuildlevel1,srefreshmonster_time,sfriend_value,sbi_create_channel,sbi_create_sub_channel,sbi_create_plat,sbi_create_udid,sbi_create_ip,sbi_create_time,sgm_level,slast_common_hook_scene_id,sshutup_time,smax_formation_count,
		shours_pvp_refreshtimes,sredmedicine_option,sbrushboss_refreshtimes,shours_win_times,shours_challenge_times,sinvade,spoint_formation_0,spoint_formation_1,spoint_formation_2,spoint_formation_3,spoint_formation_4,spoint_formation_5,stop_level,stop_exp);
	END IF;
#COMMIT;
END
;;
DELIMITER ;


-- ----------------------------
-- Procedure structure for `pro_save_player_formation`
-- ----------------------------
DROP PROCEDURE IF EXISTS `pro_save_player_formation`;
DELIMITER ;;
CREATE PROCEDURE `pro_save_player_formation`(
splayerslotid			int,
splayerid 		bigint unsigned,
sname			varchar(45),
sicon			varchar(45),
sfight_value 	int)
BEGIN
	declare rid  int;
    set rid=-1;
#START TRANSACTION;
SELECT tid INTO rid FROM t_formation_player
    WHERE tplayerid=splayerid AND tplayerslotid=splayerslotid LIMIT 1;
	IF rid<>-1 THEN
		UPDATE t_formation_player SET tname=sname,
		ticon=sicon,
		tfight_value=sfight_value WHERE
		tplayerid=splayerid AND tplayerslotid=splayerslotid;
	ELSE
		INSERT INTO t_formation_player (tplayerid,tplayerslotid,tname,ticon,tfight_value) VALUES (splayerid,splayerslotid,sname,sicon,sfight_value);
	END IF;
#COMMIT;
END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `pro_save_pvp_player`
-- ----------------------------
DROP PROCEDURE IF EXISTS `pro_save_pvp_player`;
DELIMITER ;;
CREATE PROCEDURE `pro_save_pvp_player`(
s_playerguid 		bigint unsigned,
s_playeraccount		varchar(45),
s_name				varchar(45),
s_starlevel			int,
s_level				int,
s_wintimes			int,
s_pvptimes			int,
s_lastwin			int,
s_win_streak		int,
s_actor_1			bigint unsigned,
s_actor_2			bigint unsigned,
s_actor_3			bigint unsigned)
BEGIN
	declare r_playerguid  bigint unsigned;
    set r_playerguid=18446744073709551615;
#START TRANSACTION;
SELECT t_playerguid INTO r_playerguid FROM t_pvp_player
    WHERE t_playerguid=s_playerguid LIMIT 1;
	IF r_playerguid<>18446744073709551615 THEN
		UPDATE t_pvp_player SET t_playerguid=s_playerguid,
		t_name=s_name,
		t_playeraccount=s_playeraccount,
		t_level=s_level,
		t_starlevel=s_starlevel,
		t_wintimes=s_wintimes,
		t_pvptimes=s_pvptimes,
		t_lastwin=s_lastwin,
		t_win_streak=s_win_streak,
		t_actor_1=s_actor_1,
		t_actor_2=s_actor_2,
		t_actor_3=s_actor_3
		WHERE
		t_playerguid=s_playerguid LIMIT 1;
	ELSE
		INSERT INTO t_pvp_player (t_playerguid,t_playeraccount,t_name,t_level,t_starlevel,t_wintimes,t_pvptimes,t_lastwin,t_win_streak,t_actor_1,t_actor_2,t_actor_3) 
		VALUES (s_playerguid,s_playeraccount,s_name,s_level,s_starlevel,s_wintimes,s_pvptimes,s_lastwin,s_win_streak,s_actor_1,s_actor_2,s_actor_3);
	END IF;
#COMMIT;
END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `pro_save_t_cell_data`
-- ----------------------------
DROP PROCEDURE IF EXISTS `pro_save_t_cell_data`;
DELIMITER ;;
CREATE PROCEDURE `pro_save_t_cell_data`(
cellid int,
player_ownerid bigint unsigned,
content tinyblob,
logic_type tinyint,
data_type tinyint)
BEGIN

DECLARE rcellid_count  int;
DECLARE rowner_count   int;
DECLARE rcellid   int;
DECLARE rplayer_ownerid     bigint unsigned;
set rcellid=-1;

#START TRANSACTION;
SELECT tcellid,tplayer_ownerid,Count(tcellid),Count(tplayer_ownerid) INTO rcellid,rplayer_ownerid,rcellid_count,rowner_count FROM t_cell_data
    WHERE tcellid=cellid AND tplayer_ownerid=player_ownerid LIMIT 1;
	IF rcellid<>-1 THEN
		UPDATE t_cell_data SET 
		tcellid=cellid,
		tplayer_ownerid=player_ownerid,
		tcontent=content,
    tlogic_type=logic_type,
    tdata_type=data_type
		WHERE
		tcellid=cellid AND tplayer_ownerid=player_ownerid;
	ELSE
		INSERT INTO t_cell_data (tcellid,tplayer_ownerid,tcontent,tlogic_type,tdata_type) VALUES (cellid,player_ownerid,content,logic_type,data_type);
	END IF;
#COMMIT;
END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `pro_save_t_global_cell_data`
-- ----------------------------
DROP PROCEDURE IF EXISTS `pro_save_t_global_cell_data`;
DELIMITER ;;
CREATE PROCEDURE `pro_save_t_global_cell_data`(
cellid int,
player_ownerid bigint unsigned,
content tinyblob,
logic_type tinyint,
data_type tinyint)
BEGIN

DECLARE rcellid_count  int;
DECLARE rowner_count   int;
DECLARE rcellid   int;
DECLARE rplayer_ownerid     bigint unsigned;
set rcellid=-1;

#START TRANSACTION;
SELECT tcellid,tplayer_ownerid,Count(tcellid),Count(tplayer_ownerid) INTO rcellid,rplayer_ownerid,rcellid_count,rowner_count FROM t_global_cell_data
    WHERE tcellid=cellid AND tplayer_ownerid=player_ownerid LIMIT 1;
	IF rcellid<>-1 THEN
		UPDATE t_global_cell_data SET 
		tcellid=cellid,
		tplayer_ownerid=player_ownerid,
		tcontent=content,
    tlogic_type=logic_type,
    tdata_type=data_type
		WHERE
		tcellid=cellid AND tplayer_ownerid=player_ownerid;
	ELSE
		INSERT INTO t_global_cell_data (tcellid,tplayer_ownerid,tcontent,tlogic_type,tdata_type) VALUES (cellid,player_ownerid,content,logic_type,data_type);
	END IF;
#COMMIT;
END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `pro_save_t_glyph`
-- ----------------------------
DROP PROCEDURE IF EXISTS `pro_save_t_glyph`;
DELIMITER ;;
CREATE PROCEDURE `pro_save_t_glyph`(guid bigint unsigned,
slotid int,player_ownerid bigint unsigned,
level int,
exp int unsigned,
cfgindex int,item_flag int)
BEGIN

declare rguid     bigint unsigned;
set 	rguid 	=  18446744073709551615;
##START TRANSACTION;
SELECT tguid INTO rguid FROM t_glyph
    WHERE tguid=guid LIMIT 1;
	IF rguid<>18446744073709551615 THEN
		UPDATE t_glyph SET 
		tguid=guid,
		tslotid=slotid,
		tplayer_ownerid=player_ownerid,
		tlevel=level,		
		texp=exp,
		tcfgindex=cfgindex,
		titem_flag=item_flag WHERE
		tguid=guid LIMIT 1;
	ELSE
		INSERT INTO t_glyph (tguid,tslotid,tplayer_ownerid,tlevel,texp,tcfgindex,titem_flag) VALUES (guid,slotid,player_ownerid,level,exp,cfgindex,item_flag);
	END IF;
##COMMIT;
END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `pro_save_t_item`
-- ----------------------------
DROP PROCEDURE IF EXISTS `pro_save_t_item`;
DELIMITER ;;
CREATE PROCEDURE `pro_save_t_item`(id bigint unsigned,
slotid int,
player_ownerid bigint unsigned,
count int,
cfgindex int unsigned,item_flag int)
BEGIN

declare rid     bigint unsigned;
set 	rid 	=  18446744073709551615;
##START TRANSACTION;
SELECT tid INTO rid FROM t_item
    WHERE tid=id LIMIT 1;
	IF rid<>18446744073709551615 THEN
		UPDATE t_item SET
		tid=id,
		tslotid=slotid,
		tplayer_ownerid=player_ownerid,
		tcount=count,
		tcfgindex=cfgindex,
		titem_flag=item_flag  WHERE
		tid=id LIMIT 1;
	ELSE
		INSERT INTO t_item (tid,tslotid,tplayer_ownerid,tcount,tcfgindex,titem_flag) VALUES (id,slotid,player_ownerid,count,cfgindex,item_flag);
	END IF;
##COMMIT;
END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `pro_save_t_item_equip`
-- ----------------------------
DROP PROCEDURE IF EXISTS `pro_save_t_item_equip`;
DELIMITER ;;
CREATE PROCEDURE `pro_save_t_item_equip`(id bigint unsigned,
slotid int,
player_ownerid bigint unsigned,
count int unsigned,
cfgindex int unsigned,
attach_count int unsigned,
attach0 bigint unsigned,
attach1 bigint unsigned,
attach2 bigint unsigned,
attach3 bigint unsigned,
attach4 bigint unsigned,
attach5 bigint unsigned,
attach6 bigint unsigned,
attach7 bigint unsigned,
attach8 bigint unsigned,
attach9 bigint unsigned,
random_count int unsigned,
random0 bigint unsigned,
random1 bigint unsigned,
random2 bigint unsigned,
random3 bigint unsigned,
random4 bigint unsigned,
random5 bigint unsigned,
random6 bigint unsigned,
random7 bigint unsigned,
gem_slot_count int unsigned,
gemslot0 bigint unsigned,
gemslot1 bigint unsigned,
gemslot2 bigint unsigned,
gemslot3 bigint unsigned,
rune_slot_count int unsigned,
runeslot0 bigint unsigned,
runeslot1 bigint unsigned,
runeslot2 bigint unsigned,
runeslot3 bigint unsigned,
prefix_count int unsigned,
prefix bigint unsigned,
enhance_level smallint,
legend_attrid int,
legend_attrlv smallint,
item_flag int,
star_level smallint)
BEGIN

declare rid     bigint unsigned;
set 	rid 	=  18446744073709551615;
##START TRANSACTION;
SELECT tid INTO rid FROM t_item_equip
    WHERE tid=id LIMIT 1;
	IF rid<>18446744073709551615 THEN
		UPDATE t_item_equip SET 
		tid=id,
		tslotid=slotid,
		tplayer_ownerid=player_ownerid,
		tcount=count,
		tcfgindex=cfgindex,
		tattach_count=attach_count,
		tattach0=attach0,
		tattach1=attach1,
		tattach2=attach2,
		tattach3=attach3,
		tattach4=attach4,
		tattach5=attach5,
		tattach6=attach6,
		tattach7=attach7,
		tattach8=attach8,
		tattach9=attach9,
		trandom_count=random_count,
		trandom0=random0,
		trandom1=random1,
		trandom2=random2,
		trandom3=random3,
		trandom4=random4,
		trandom5=random5,
		trandom6=random6,
		trandom7=random7,
		tgem_slot_count=gem_slot_count,
		tgem_slot0=gemslot0,
		tgem_slot1=gemslot1,
		tgem_slot2=gemslot2,
		tgem_slot3=gemslot3,
		trune_slot_count=rune_slot_count,
		trune_slot0=runeslot0,
		trune_slot1=runeslot1,
		trune_slot2=runeslot2,
		trune_slot3=runeslot3,
		tprefix_count=prefix_count,
		tprefix=prefix,
		tenhance_level=enhance_level,
		tlegend_attrid=legend_attrid,
		tlegend_attrlv=legend_attrlv,
		titem_flag=item_flag,
		tstar_level=star_level WHERE
		tid=id LIMIT 1;
	ELSE
		INSERT INTO t_item_equip (
    tid,
    tslotid,
		tplayer_ownerid,
		tcount,
		tcfgindex,
		tattach_count,
		tattach0,
		tattach1,
		tattach2,
		tattach3,
		tattach4,
		tattach5,
		tattach6,
		tattach7,
		tattach8,
		tattach9,
		trandom_count,
		trandom0,
		trandom1,
		trandom2,
		trandom3,
		trandom4,
		trandom5,
		trandom6,
		trandom7,
		tgem_slot_count,
		tgem_slot0,
		tgem_slot1,
		tgem_slot2,
		tgem_slot3,
		trune_slot_count,
		trune_slot0,
		trune_slot1,
		trune_slot2,
		trune_slot3,
		tprefix_count,
		tprefix,
		tenhance_level,
		tlegend_attrid,
		tlegend_attrlv,
		titem_flag,
		tstar_level)
    VALUES( 
		id,
		slotid,
		player_ownerid,
		count,
		cfgindex,
		attach_count,
		attach0,
		attach1,
		attach2,
		attach3,
		attach4,
		attach5,
		attach6,
		attach7,
		attach8,
		attach9,
		random_count,
		random0,
		random1,
		random2,
		random3,
		random4,
		random5,
		random6,
		random7,
		gem_slot_count,
		gemslot0,
		gemslot1,
		gemslot2,
		gemslot3,
		rune_slot_count,
		runeslot0,
		runeslot1,
		runeslot2,
		runeslot3,
		prefix_count,
		prefix,
		enhance_level,
		legend_attrid,
		legend_attrlv,
		item_flag,
		star_level);
	END IF;
##COMMIT;
END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `pro_save_t_soul`
-- ----------------------------
DROP PROCEDURE IF EXISTS `pro_save_t_soul`;
DELIMITER ;;
CREATE PROCEDURE `pro_save_t_soul`(
id bigint unsigned,
slotid int,
player_ownerid bigint unsigned,
cfgindex int unsigned,
level tinyint unsigned,
exp bigint unsigned,
step tinyint unsigned )
BEGIN

DECLARE rslot_count   int unsigned;
DECLARE rowner_count   int unsigned;
DECLARE rslotid   int;
DECLARE rplayer_ownerid     bigint unsigned;

#START TRANSACTION;
SELECT tslotid,tplayer_ownerid,Count(tslotid),Count(tplayer_ownerid) INTO rslotid,rplayer_ownerid,rslot_count,rowner_count FROM t_soul
    WHERE tslotid=slotid AND tplayer_ownerid=player_ownerid LIMIT 1;
	IF rslotId=slotid THEN
		UPDATE t_soul SET 
		tid=id,
		tslotid=slotid,
		tplayer_ownerid=player_ownerid,
		tcfgindex=cfgindex,
		tlevel=level,
		texp=exp,
		tstep=step 
		WHERE
		tslotid=slotid AND tplayer_ownerid=player_ownerid;
	ELSEIF rslot_count=0 And rowner_count=0 THEN
		INSERT INTO t_soul (tid,tslotid,tplayer_ownerid,tcfgindex,tlevel,texp,tstep) VALUES (id,slotid,player_ownerid,cfgindex,level,exp,step);
	ELSE
		DELETE FROM t_soul WHERE tslotid=rslotId AND tplayer_ownerid=rplayer_ownerid;
	END IF;
#COMMIT;
END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `pro_save_t_soul_part`
-- ----------------------------
DROP PROCEDURE IF EXISTS `pro_save_t_soul_part`;
DELIMITER ;;
CREATE PROCEDURE `pro_save_t_soul_part`(
cfgindex int,
player_ownerid bigint unsigned,
count int)
BEGIN

DECLARE rcfgindex_count   int;
DECLARE rowner_count   int;
DECLARE rcfgindex   int;
DECLARE rplayer_ownerid     bigint unsigned;

#START TRANSACTION;
SELECT tcfgindex,tplayer_ownerid,Count(tcfgindex),Count(tplayer_ownerid) INTO rcfgindex,rplayer_ownerid,rcfgindex_count,rowner_count FROM t_soul_part
    WHERE tcfgindex=cfgindex AND tplayer_ownerid=player_ownerid LIMIT 1;
	IF rcfgindex=cfgindex THEN
		UPDATE t_soul_part SET 
		tcfgindex=cfgindex,
		tplayer_ownerid=player_ownerid,
		tcount=count
		WHERE
		tcfgindex=cfgindex AND tplayer_ownerid=player_ownerid;
	ELSEIF rcfgindex_count=0 And rowner_count=0 THEN
		INSERT INTO t_soul_part (tcfgindex,tplayer_ownerid,tcount) VALUES (cfgindex,player_ownerid,count);
	ELSE
		DELETE FROM t_soul_part WHERE tcfgindex=rcfgindex AND tplayer_ownerid=rplayer_ownerid;
	END IF;
#COMMIT;
END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `pro_saveachievement`
-- ----------------------------
DROP PROCEDURE IF EXISTS `pro_saveachievement`;
DELIMITER ;;
CREATE PROCEDURE `pro_saveachievement`(
sownerid bigint unsigned,
sgroup int,
svalue int,
srewardlevel int)
BEGIN

DECLARE rgroup     int;
set 	rgroup 	=  -1;
#START TRANSACTION;
SELECT tgroup INTO rgroup FROM t_achievement
    WHERE townerid=sownerid AND tgroup=sgroup LIMIT 1;
	IF rgroup<>-1 THEN
		UPDATE t_achievement SET townerid=sownerid,
		tgroup=sgroup,
		tvalue=svalue,
		trewardlevel=srewardlevel WHERE
		townerid=sownerid AND tgroup=sgroup LIMIT 1;
	ELSE
		INSERT INTO t_achievement(townerid,tgroup,tvalue,trewardlevel)
		VALUES(sownerid,sgroup,svalue,srewardlevel);
	END IF;
#COMMIT;
END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `pro_savefriend`
-- ----------------------------
DROP PROCEDURE IF EXISTS `pro_savefriend`;
DELIMITER ;;
CREATE PROCEDURE `pro_savefriend`(
sownerid 		bigint unsigned,
stype			tinyint,
sfriendid		bigint unsigned,
sname			varchar(45),
shead			tinyint unsigned,
slevel			int,
sfightvalue		int,
sviplevel		int,
sguild			int,
sbgift			int,
sgifttime			bigint unsigned)
BEGIN
	declare rfriendguid   bigint unsigned;
    set rfriendguid=18446744073709551615;
#START TRANSACTION;
SELECT tfriendid INTO rfriendguid FROM t_friend
    WHERE townerid=sownerid AND ttype=stype AND tfriendid=sfriendid LIMIT 1;
	IF rfriendguid<>18446744073709551615 THEN
		UPDATE t_friend SET townerid=sownerid,
		ttype=stype,
		tfriendid=sfriendid,
		tname=sname,
		thead=shead,
		tlevel=slevel,
		tfightvalue=sfightvalue,
		tviplevel=sviplevel,
		tguild=sguild,
		tbgift=sbgift,
		tgifttime=sgifttime
		WHERE
		townerid=sownerid AND ttype=stype AND tfriendid=sfriendid LIMIT 1;
	ELSE
		INSERT INTO t_friend (townerid,ttype,tfriendid,tname,thead,tlevel,tfightvalue,tviplevel,tguild,tbgift,tgifttime) 
		VALUES (sownerid,stype,sfriendid,sname,shead,slevel,sfightvalue,sviplevel,sguild,sbgift,sgifttime);
	END IF;
#COMMIT;
END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `pro_savegathering`
-- ----------------------------
DROP PROCEDURE IF EXISTS `pro_savegathering`;
DELIMITER ;;
CREATE PROCEDURE `pro_savegathering`(
splayer_ownerid bigint unsigned,
shookindex int,
sitemid int,
sdonated int)
BEGIN

DECLARE rhookindex     int;
set 	rhookindex 	=  -1;
#START TRANSACTION;
SELECT thookindex INTO rhookindex FROM t_gathering
    WHERE tplayer_ownerid=splayer_ownerid  AND thookindex=shookindex AND titemid=sitemid LIMIT 1;
	IF rhookindex<>-1 THEN
		UPDATE t_gathering SET tplayer_ownerid=splayer_ownerid,
		thookindex=shookindex,
		titemid=sitemid,
		tdonated=sdonated
		WHERE
		tplayer_ownerid=splayer_ownerid AND thookindex=shookindex AND titemid=sitemid LIMIT 1;
	ELSE
		INSERT INTO t_gathering(tplayer_ownerid,thookindex,titemid,tdonated)
		VALUES(splayer_ownerid,shookindex,sitemid,sdonated);
	END IF;
#COMMIT;
END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `pro_savemail`
-- ----------------------------
DROP PROCEDURE IF EXISTS `pro_savemail`;
DELIMITER ;;
CREATE PROCEDURE `pro_savemail`(
smailid bigint unsigned,
sstarttime bigint unsigned,
smail_ownerid bigint unsigned,
smail_fromid bigint unsigned,
smail_fromname varchar(45),
smail_type tinyint,
smail_title varchar(255),
smail_content varchar(255),
sitemid0 int,
sitemcount0 int,
sitemid1 int,
sitemcount1 int,
sitemid2 int,
sitemcount2 int,
sitemid3 int,
sitemcount3 int,
sitemid4 int,
sitemcount4 int)
BEGIN

declare rmailid     bigint unsigned;
set 	rmailid 	=  18446744073709551615;
#START TRANSACTION;
SELECT tmailid INTO rmailid FROM t_mail
    WHERE tmailid=smailid LIMIT 1;
	IF rmailid<>18446744073709551615 THEN
		UPDATE t_mail SET 
		tmailid				=smailid,
		tstarttime			=sstarttime,
		tmail_ownerid		=smail_ownerid,
		tmail_fromid		=smail_fromid,
		tmail_fromname		=smail_fromname,
		tmail_type 			=smail_type,
		tmail_title			=smail_title, 
		tmail_content		=smail_content,
		titemid0			=sitemid0,
		titemcount0			=sitemcount0,
		titemid1			=sitemid1,
		titemcount1			=sitemcount1,
		titemid2			=sitemid2,
		titemcount2			=sitemcount2,
		titemid3			=sitemid3,
		titemcount3			=sitemcount3,
		titemid4			=sitemid4,
		titemcount4			=sitemcount4
		WHERE
			tmailid=smailid LIMIT 1;
	ELSE
		INSERT INTO t_mail(tmailid,tstarttime,tmail_ownerid,tmail_fromid,tmail_fromname,tmail_type,tmail_title,tmail_content,titemid0,titemcount0,titemid1,titemcount1,titemid2,titemcount2,titemid3,titemcount3,titemid4,titemcount4)
		VALUES(smailid,sstarttime,smail_ownerid,smail_fromid,smail_fromname,smail_type,smail_title,smail_content,sitemid0,sitemcount0,sitemid1,sitemcount1,sitemid2,sitemcount2,sitemid3,sitemcount3,sitemid4,sitemcount4);
	END IF;
#COMMIT;
END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `pro_savemanager`
-- ----------------------------
DROP PROCEDURE IF EXISTS `pro_savemanager`;
DELIMITER ;;
CREATE PROCEDURE `pro_savemanager`(
sserverid int,
sshutdowntime int)
BEGIN

DECLARE rserverid     int;
set 	rserverid 	=  -1;

#START TRANSACTION;
SELECT tserverid INTO rserverid FROM t_manager
    WHERE tserverid=sserverid LIMIT 1;
	IF rserverid<>-1 THEN
		UPDATE t_manager SET tserverid=sserverid,
		tshutdowntime=sshutdowntime WHERE
		tserverid=sserverid;
	ELSE
		INSERT INTO t_manager(tserverid,tshutdowntime)
		VALUES(sserverid,sshutdowntime);
	END IF;
#COMMIT;
END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `pro_savemonsterpet`
-- ----------------------------
DROP PROCEDURE IF EXISTS `pro_savemonsterpet`;
DELIMITER ;;
CREATE PROCEDURE `pro_savemonsterpet`(
playerid bigint unsigned,
petid int,
charactertype int,
loyalvalue int,
onhookstate int,
onhookid int,
startetime int,
lefttime int,
partner tinyint)
BEGIN

declare exictpetid     int;
set 	exictpetid 	=  -1;
#START TRANSACTION;
SELECT tpetid INTO exictpetid FROM t_monster_pet
    WHERE townerid=playerid and tpetid=petid LIMIT 1;
	IF exictpetid<>-1 THEN
		UPDATE t_monster_pet SET 
		townerid=playerid,
		tpetid=petid,
		tcharactertype=charactertype,
		tloyalvalue=loyalvalue,
		tonhookstate=onhookstate,
		tonhookid=onhookid,
		tstarttime=startetime,
		tlefttime=lefttime,
		tpartner=partner
		WHERE
		townerid=playerid and tpetid=petid LIMIT 1;
	ELSE
		INSERT INTO t_monster_pet(townerid,tpetid,tcharactertype,tloyalvalue,tonhookstate,tonhookid,tstarttime,tlefttime,tpartner)
		VALUES(playerid,petid,charactertype,loyalvalue,onhookstate,onhookid,startetime,lefttime,partner);
	END IF;
#COMMIT;
END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `pro_saveonhook`
-- ----------------------------
DROP PROCEDURE IF EXISTS `pro_saveonhook`;
DELIMITER ;;
CREATE PROCEDURE `pro_saveonhook`(
splayer_ownerid bigint unsigned,
shooktype int,
shookindex int,
scurrentonhookid int,
scurrentonhookstarttime bigint unsigned,
scurrentonhooklefttime int,
spetid1 int,
spetid2 int,
spetid3 int,
spetid4 int,
sonhookid1 int,
sonhookid2 int,
sonhookid3 int,
sonhookid4 int,
sonhookid5 int,
sonhookid6 int,
sonhookid7 int,
sonhookid8 int,
sonhookid9 int,
sonhookid10 int)
BEGIN

DECLARE rhookindex     int;
set 	rhookindex 	=  -1;
#START TRANSACTION;
SELECT thookindex INTO rhookindex FROM t_onhook
    WHERE tplayer_ownerid=splayer_ownerid AND thooktype=shooktype AND thookindex=shookindex LIMIT 1;
	IF rhookindex<>-1 THEN
		UPDATE t_onhook SET tplayer_ownerid=splayer_ownerid,
		thooktype=shooktype,
		thookindex=shookindex,
		tcurrentonhookid=scurrentonhookid,
		tcurrentonhookstarttime=scurrentonhookstarttime,
		tcurrentonhooklefttime=scurrentonhooklefttime,
		tpetid1=spetid1,
		tpetid2=spetid2,
		tpetid3=spetid3,
		tpetid4=spetid4,
		tonhookid1=sonhookid1,
		tonhookid2=sonhookid2,
		tonhookid3=sonhookid3,
		tonhookid4=sonhookid4,
		tonhookid5=sonhookid5,
		tonhookid6=sonhookid6,
		tonhookid7=sonhookid7,
		tonhookid8=sonhookid8,
		tonhookid9=sonhookid9,
		tonhookid10=sonhookid10
		WHERE
		tplayer_ownerid=splayer_ownerid AND thooktype=shooktype AND thookindex=shookindex LIMIT 1;
	ELSE
		INSERT INTO t_onhook(tplayer_ownerid,thooktype,thookindex,tcurrentonhookid,tcurrentonhookstarttime,tcurrentonhooklefttime,tpetid1,tpetid2,tpetid3,tpetid4,tonhookid1,tonhookid2,tonhookid3,tonhookid4,tonhookid5
		,tonhookid6,tonhookid7,tonhookid8,tonhookid9,tonhookid10)
		VALUES(splayer_ownerid,shooktype,shookindex,scurrentonhookid,scurrentonhookstarttime,scurrentonhooklefttime,spetid1,spetid2,spetid3,spetid4,sonhookid1,sonhookid2,sonhookid3,sonhookid4,sonhookid5
		,sonhookid6,sonhookid7,sonhookid8,sonhookid9,sonhookid10);
	END IF;
#COMMIT;
END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `pro_saveplayer_talents`
-- ----------------------------
DROP PROCEDURE IF EXISTS `pro_saveplayer_talents`;
DELIMITER ;;
CREATE PROCEDURE `pro_saveplayer_talents`(
splayer_ownerid bigint unsigned,
sid 	int)
BEGIN

DECLARE rid     int;
set 	rid 	=  -1;

#START TRANSACTION;
SELECT tid INTO rid FROM t_player_talent
    WHERE tplayer_ownerid=splayer_ownerid and tid=sid LIMIT 1;
	IF rid<>-1 THEN
		UPDATE t_player_talent SET tplayer_ownerid=splayer_ownerid,
		tid=sid
		WHERE tplayer_ownerid=splayer_ownerid and tid=sid LIMIT 1;
	ELSE
		INSERT INTO t_player_talent(tplayer_ownerid,tid)
		VALUES(splayer_ownerid,sid);
	END IF;
#COMMIT;
END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `pro_savescene`
-- ----------------------------
DROP PROCEDURE IF EXISTS `pro_savescene`;
DELIMITER ;;
CREATE PROCEDURE `pro_savescene`(
id int,
killmonstercount int,
boss_lefthp bigint,
killboss_lefttime int,
scenestate int)
BEGIN

DECLARE rid     int;
DECLARE rplayerid     bigint;
DECLARE ractorid      bigint;

#START TRANSACTION;
SELECT tid INTO rid FROM t_scene
    WHERE tid=id LIMIT 1;
	IF rid=id THEN
		UPDATE t_scene SET tid=id,
		tkillmonster_count=killmonstercount,
		tboss_lefthp = boss_lefthp,
		tkillboss_lefttime = killboss_lefttime,
		tstate=scenestate WHERE
		tid=id;
	ELSE
		INSERT INTO t_scene(tid,tkillmonster_count,tboss_lefthp,tkillboss_lefttime,tstate)
		VALUES(id,killmonstercount,boss_lefthp,killboss_lefttime,scenestate);
	END IF;
#COMMIT;
END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `pro_savebrushboss`
-- ----------------------------
DROP PROCEDURE IF EXISTS `pro_savebrushboss`;
DELIMITER ;;
CREATE PROCEDURE `pro_savebrushboss`(
sbossid0 int,
slegenditemid0 int,
sbossid1 int,
slegenditemid1 int,
sbossid2 int,
slegenditemid2 int
)
BEGIN
#START TRANSACTION;
	INSERT INTO t_brushboss(tbossid0,tlegenditemid0,tbossid1,tlegenditemid1,tbossid2,tlegenditemid2)
	VALUES(sbossid0,slegenditemid0,sbossid1,slegenditemid1,sbossid2,slegenditemid2);
#COMMIT;
END
;;
DELIMITER ;


-- ----------------------------
-- Procedure structure for `pro_saveplayerears`
-- ----------------------------
DROP PROCEDURE IF EXISTS `pro_saveplayerears`;
DELIMITER ;;
CREATE PROCEDURE `pro_saveplayerears`(
sownerid bigint unsigned,
splayer_guid bigint unsigned,
splayer_name varchar(45),
slayer_headindex tinyint
)
BEGIN

declare rownerid     bigint unsigned;
set 	rownerid 	=  18446744073709551615;
#START TRANSACTION;
SELECT townerid INTO rownerid FROM t_playerears
    WHERE townerid=sownerid LIMIT 1;
	IF rownerid<>18446744073709551615 THEN
		UPDATE t_playerears SET 
		townerid			=sownerid,
		tplayer_guid		=splayer_guid,
		tplayer_name		=splayer_name,
		tlayer_headindex	=slayer_headindex
		WHERE
			townerid=sownerid LIMIT 1;
	ELSE
		INSERT INTO t_playerears(townerid,tplayer_guid,tplayer_name,tlayer_headindex)
		VALUES(sownerid,splayer_guid,splayer_name,slayer_headindex);
	END IF;
#COMMIT;
END
;;
DELIMITER ;


DROP PROCEDURE IF EXISTS `pro_save_pvp_actor`;
DROP PROCEDURE IF EXISTS `pro_save_actor_formation`;
DROP PROCEDURE IF EXISTS `pro_cdk_code`;
DROP TABLE IF EXISTS `t_pvp_actor`;
DROP TABLE IF EXISTS `t_formation_actor`;
DROP TABLE IF EXISTS `t_cdkcode`;
DROP TABLE IF EXISTS `t_player_mail`;
DROP TABLE IF EXISTS `t_common_formation_player`;
DROP TABLE IF EXISTS `t_formation_player`;
