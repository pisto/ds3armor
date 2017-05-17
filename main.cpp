#include <vector>
#include <set>
#include <iostream>
#include <cstring>
#include <string>
#include <algorithm>
#include <boost/program_options.hpp>
using namespace std;

//vector ops to make things fast
typedef float __attribute__((vector_size(32))) float8;
typedef float __attribute__((vector_size(16))) float4;
typedef float __attribute__((vector_size(8))) float2;

union vec_absorptions {
	struct {
		float physical, vs_strike, vs_slash, vs_thrust, magic, fire, lightning, dark;
	};
	struct { float4 v4[2]; };
	struct { float2 v2[2]; };
	float8 all;
};

enum armor_type { HEAD, BODY, ARMS, LEGS };
struct armor {
	unsigned short id = 0;
	const char* name;
	unsigned short weight;
	vec_absorptions absorptions;
	float bleed, poison, frost, curse, poise;
	armor_type type;
};

//from mugenmonkey
const armor ds3armors[] {
		{22,"Alva Armor",90,12.4,10.2,12.4,11.4,9.5,11.4,9.5,10.9,59,35,37,21,11,BODY},
		{23,"Alva Gauntlets",33,3.1,2.5,3.1,2.8,2.3,2.8,2.3,2.7,22,14,15,10,2.5,ARMS},
		{21,"Alva Helm",42,4.4,3.6,4.4,4,3.4,4,3.4,3.8,28,18,19,12,3.7,HEAD},
		{24,"Alva Leggings",55,7.1,5.8,7.1,6.5,5.4,6.5,5.4,6.2,36,21,22,12,6.7,LEGS},
		{3,"Antiquated Dress",31,3.2,3.6,3.2,3.2,12.3,13.9,11.8,13.2,21,38,27,86,1.2,BODY},
		{4,"Antiquated Gloves",11,0.9,1,0.9,0.9,3.3,3.7,3.2,3.6,8,13,10,29,0.6,ARMS},
		{360,"Antiquated Plain Garb",31,3.7,3.1,2.7,2.4,12.7,11.1,12.2,12.3,30,47,25,60,0.9,BODY},
		{5,"Antiquated Skirt",21,1.6,1.9,1.6,1.4,6.7,7.5,6.4,7,13,23,16,54,0.4,LEGS},
		{7,"Archdeacon Holy Garb",42,3.7,6.8,4.4,4.4,12.4,11.5,12.4,13.7,18,39,37,83,3.4,BODY},
		{8,"Archdeacon Skirt",26,2.4,4.2,2.9,2.9,7.7,7.2,7.7,8.5,14,27,26,55,2.7,LEGS},
		{6,"Archdeacon White Crown",18,1.6,2.7,1.9,1.9,5,4.6,5,5.4,11,19,19,38,1.6,HEAD},
		{166,"Aristocrat's Mask",57,6,2.7,5.8,5.5,4.1,4.3,2.5,3.9,25,17,17,28,6.8,HEAD},
		{10,"Armor of the Sun",86,11.2,8.3,11.2,11.2,10.5,10.5,9.8,10.5,34,28,31,26,11.3,BODY},
		{18,"Armor of Thorns",85,11,8.1,14.2,11.5,7.3,7.3,7.3,5.9,66,27,33,22,7.5,BODY},
		{26,"Assassin Armor",69,8.6,10,8.6,9.3,10.4,11.4,10.4,9,40,62,47,52,4.8,BODY},
		{27,"Assassin Gloves",20,2,1.2,2.3,1.8,2.6,2.7,1.9,1.5,12,20,13,16,0.4,ARMS},
		{25,"Assassin Hood",18,1.9,2.7,1.9,2.2,3.3,4.3,3.3,2.5,17,27,20,20,1,HEAD},
		{28,"Assassin Trousers",43,5.2,3.8,6,4.8,6.6,6.9,5.2,4.3,29,43,32,38,2.1,LEGS},
		{92,"Billed Mask",37,4,2.8,4.6,3.6,2.6,4.1,2,3.7,24,16,14,18,2.2,HEAD},
		{39,"Black Dress",65,7.7,7.7,10,6.8,9.7,12.5,10.9,12.1,40,60,47,71,2.2,BODY},
		{40,"Black Gauntlets",35,3.2,2.4,3.6,2.9,2.1,3.2,2.2,3.4,19,13,11,15,2.3,ARMS},
		{34,"Black Hand Armor",78,9,8.1,11.5,9.7,12.6,12.8,10.4,9,46,67,51,58,4.9,BODY},
		{33,"Black Hand Hat",25,2.6,2.1,3.8,2.9,4.2,4.4,3.3,2.7,19,30,21,25,1.1,HEAD},
		{36,"Black Iron Armor",158,18.1,12.2,18.1,18.1,12.2,15.8,11.5,12.2,65,51,57,35,20.9,BODY},
		{37,"Black Iron Gauntlets",51,4.6,2.9,4.6,4.4,2.8,3.8,2.8,3.1,24,16,20,14,4.4,ARMS},
		{35,"Black Iron Helm",65,6.5,4.5,6.9,6.6,4.8,5.9,4.4,4.8,26,19,21,13,7.6,HEAD},
		{38,"Black Iron Leggings",95,10.8,7,10.8,10.3,7.5,9.3,6.9,7.5,42,32,35,24,13.4,LEGS},
		{30,"Black Knight Armor",134,15.8,12.2,14.4,13.8,11,14.1,10.5,12,58,42,39,39,15.8,BODY},
		{31,"Black Knight Gauntlets",39,3.8,3.1,4,3.3,2.7,3.5,2.5,2.9,18,12,11,11,3.7,ARMS},
		{29,"Black Knight Helm",54,5.3,4,4.8,4.6,3.4,4.6,3.2,3.8,27,19,19,19,4.6,HEAD},
		{32,"Black Knight Leggings",82,9,7,8.3,7.9,6.6,8.5,6.2,7.2,38,27,26,26,9.5,LEGS},
		{43,"Black Leather Armor",59,8.7,7.8,9.4,6.9,8.6,7.8,10.4,8.2,46,65,55,46,5.2,BODY},
		{45,"Black Leather Boots",36,4.8,4.3,5.2,3.8,4.8,4.3,5.6,4.3,25,37,31,25,2.3,LEGS},
		{44,"Black Leather Gloves",23,2.2,2,2.3,1.8,2.1,1.9,2.9,2.3,14,20,18,14,1.7,ARMS},
		{41,"Black Leggings",45,5.2,5.2,6.4,4.7,6.7,8.1,7.3,7.8,26,38,32,46,2.8,LEGS},
		{355,"Black Witch Garb",36,3.7,4.2,4.2,2.7,13.8,13.5,13.5,14.1,15,39,28,69,0.5,BODY},
		{354,"Black Witch Hat",18,1.9,1.1,1.1,1.4,4.9,4.8,4.8,5,6,17,12,30,0.2,HEAD},
		{357,"Black Witch Trousers",26,2.9,2.4,2.4,2.1,8.1,7.9,7.9,8.3,11,25,19,45,0.5,LEGS},
		{358,"Black Witch Veil",14,0.8,0.9,0.8,0.8,4.7,4.5,4.6,4.9,4,14,8,44,0,HEAD},
		{356,"Black Witch Wrappings",12,0.5,1.2,1.2,0.7,3.4,3.3,3.3,3.5,5,13,9,23,0.1,ARMS},
		{359,"Blindfold Mask",11,1,0.9,0.8,0.9,3.6,2.8,4.4,0,6,15,10,32,0,HEAD},
		{47,"Brass Armor",105,13.3,10.7,13.3,11.8,13.6,10,6.9,10,60,33,36,27,9.9,BODY},
		{48,"Brass Gauntlets",34,3.5,2.8,3.5,3.1,3.6,2.7,1.9,2.7,19,10,11,8,2.4,ARMS},
		{46,"Brass Helm",47,4.7,3.7,4.7,4.1,4.7,3.4,2.3,3.4,27,15,17,13,3.1,HEAD},
		{49,"Brass Leggings",67,7.8,6.3,7.8,6.9,8.1,6,4.2,6,39,22,24,18,6.3,LEGS},
		{51,"Brigand Armor",48,9,8.1,9,9,5.9,7.3,8.2,5.9,37,59,57,37,2.9,BODY},
		{52,"Brigand Gauntlets",24,2.5,2.3,2.5,2.5,1.9,2.1,2.3,1.9,14,20,19,14,1,ARMS},
		{50,"Brigand Hood",27,3.5,3.2,3.5,3.5,2.7,3,3.3,2.7,19,29,28,19,1.4,HEAD},
		{53,"Brigand Trousers",50,6,5.6,6,6,4.8,5.2,5.6,4.8,32,43,41,32,3.1,LEGS},
		{55,"Catarina Armor",171,18.3,13.9,18.5,17.5,11.9,12.9,15,13.3,74,57,57,33,23.8,BODY},
		{56,"Catarina Gauntlets",57,4.8,3.7,4.9,4.8,3.1,3.3,3.9,3.4,29,22,22,14,5.3,ARMS},
		{54,"Catarina Helm",73,6.8,5,6.8,6.3,4.5,4.9,5.7,5.1,34,27,27,17,8.6,HEAD},
		{57,"Catarina Leggings",106,10.6,8.3,10.9,10.6,6.3,6.7,8,6.9,47,35,35,21,14.8,LEGS},
		{59,"Cathedral Knight Armor",170,17.8,18,15.6,18.8,14.8,13.6,14.4,12.7,66,62,62,47,23.6,BODY},
		{60,"Cathedral Knight Gauntlets",53,4.3,4.4,3.8,4.9,3.7,3.5,3.6,3.1,22,21,21,15,5.3,ARMS},
		{58,"Cathedral Knight Helm",72,6,6.1,5.2,6.4,4.9,4.4,4.7,4.1,28,26,26,20,7.8,HEAD},
		{61,"Cathedral Knight Leggings",97,8.9,9.1,7.7,10.3,7.6,7,7.4,6.2,40,38,38,26,13,LEGS},
		{63,"Chain Armor",88,11.1,6.8,13.2,10,8.3,6.6,4.2,9,63,33,33,22,6.8,BODY},
		{62,"Chain Helm",39,3.8,2.2,4.6,3.4,2.6,2,1.1,2.9,28,15,15,11,1.9,HEAD},
		{65,"Chain Leggings",51,6.7,4.2,7.9,6,5.5,4.5,3.1,5.9,36,17,17,10,4.9,LEGS},
		{302,"Clandestine Coat",30,4.2,4.2,4.2,3.6,13.5,12.9,13.1,13.5,25,36,30,65,0.9,BODY},
		{67,"Cleric Blue Robe",60,4.9,7.3,6,4.2,12.8,12.3,13.2,13.2,45,61,49,68,1.4,BODY},
		{68,"Cleric Gloves",15,1.1,0.7,0.7,0.9,2.8,2.1,2.8,2.9,17,21,11,25,0.5,ARMS},
		{66,"Cleric Hat",14,1.9,1.4,1.4,1.6,5,3.8,4.9,5,18,25,12,29,0.5,HEAD},
		{69,"Cleric Trousers",21,2.4,1.6,1.6,1.9,6.9,5,6.8,7,26,36,18,42,0.7,LEGS},
		{73,"Conjurator Boots",26,2.3,2.3,2.3,2,7,8.1,7.9,7.3,19,39,27,38,1,LEGS},
		{70,"Conjurator Hood",18,1.5,1.5,1.5,1.3,4.4,5.1,5,4.6,13,27,19,26,0.6,HEAD},
		{72,"Conjurator Manchettes",14,0.8,0.8,0.8,0.6,2.7,3.2,3.1,2.9,10,21,15,20,0.1,ARMS},
		{71,"Conjurator Robe",42,3.8,3.8,3.8,3.2,11.7,13.6,13.3,12.2,31,63,44,61,1.1,BODY},
		{80,"Cornyx's Garb",41,4.1,4.1,4.8,3.5,13,13.7,12.8,13.2,22,40,30,88,4.9,BODY},
		{82,"Cornyx's Skirt",20,1.7,1.7,1.7,1.5,6.9,7.1,6.6,7,15,24,18,48,1.4,LEGS},
		{81,"Cornyx's Wrap",13,0.9,0.9,1.1,0.8,3.6,3.4,3.3,3.4,7,13,11,26,0.8,ARMS},
		{76,"Court Sorcerer Gloves",10,1,1.1,1,1.1,3.2,3,3.6,3.5,7,12,7,21,0.5,ARMS},
		{74,"Court Sorcerer Hood",15,0.9,1.1,0.9,1.1,4.1,3.7,4.5,4.4,14,21,15,33,0.1,HEAD},
		{75,"Court Sorcerer Robe",42,3.6,4.4,3.6,4.4,12.3,11.6,13.5,13.2,29,46,32,74,1.6,BODY},
		{77,"Court Sorcerer Trousers",22,2.2,2.5,2.2,2.5,7.4,6.8,8,7.9,19,29,21,47,1.1,LEGS},
		{200,"Creighton's Steel Mask",57,4.5,3.9,6,4.1,3.9,4.2,2.5,4.1,26,19,19,12,6.6,HEAD},
		{2,"Crown of Dusk",10,0.8,0,0.2,0,0,4,3,3.2,5,12,8,33,0,HEAD},
		{84,"Dancer's Armor",73,10.5,7.4,12.2,11.7,8.3,6,9.1,9.8,41,33,55,22,6.7,BODY},
		{83,"Dancer's Crown",28,3.3,1.8,4.4,4,2.7,2.2,2.5,2.8,14,10,23,7,1.5,HEAD},
		{85,"Dancer's Gauntlets",24,2.8,2,3.2,3.1,2.2,1.7,2.4,2.6,15,12,20,9,1.7,ARMS},
		{86,"Dancer's Leggings",44,6,4.2,7,6.7,4.7,3.4,4.7,5.1,22,17,31,10,3.6,LEGS},
		{88,"Dark Armor",91,12.6,9.2,11.6,11.1,9.2,7.7,4.4,9.2,38,30,33,14,8.8,BODY},
		{89,"Dark Gauntlets",32,3.5,2.6,3.2,3.1,2.6,2.2,1.4,2.6,12,9,10,4,2.3,ARMS},
		{90,"Dark Leggings",61,8,6.1,7.1,6.8,6.5,5.3,2.9,6.1,26,20,23,10,6.4,LEGS},
		{87,"Dark Mask",40,4.3,3.1,3.9,3.7,3.1,2.5,1.3,3.1,18,14,15,7,2.7,HEAD},
		{93,"Deacon Robe",35,3.6,3.6,4.4,3.6,12.5,12.7,11.6,13.2,19,40,33,79,1,BODY},
		{94,"Deacon Skirt",23,1.1,1.7,2.1,1.7,6.8,6.9,6.3,7.2,13,26,22,50,1.1,LEGS},
		{351,"Desert Pyromancer Garb",23,3.7,2.9,2.4,2.4,10.5,11.1,12.7,11.7,16,64,25,66,0.9,BODY},
		{352,"Desert Pyromancer Gloves",11,1.1,1,1,0.9,3,2.8,3.4,2.9,9,25,11,25,0.3,ARMS},
		{350,"Desert Pyromancer Hood",12,1.9,0.8,1.2,1.2,3.6,4.5,4.2,4.7,10,30,13,31,0.3,HEAD},
		{353,"Desert Pyromancer Skirt",24,3.5,1.6,1.9,1.9,6.2,7.3,7.1,7.4,13,43,18,44,1.2,LEGS},
		{96,"Deserter Armor",86,11.7,9.3,11.7,11.1,6.6,7.5,4.2,10.4,50,33,36,22,8.6,BODY},
		{97,"Deserter Trousers",31,4.2,4.7,4.2,4.2,5,5,5.9,7,21,34,28,25,2.1,LEGS},
		{106,"Dragonscale Armor",124,14.9,12.7,14.3,12.2,10.6,13.7,7.5,10.6,62,43,46,28,16.6,BODY},
		{108,"Dragonscale Waistcloth",63,7.1,5.9,6.8,5.9,4.1,6.8,1.8,4.1,31,21,22,10,7.6,LEGS},
		{110,"Dragonslayer Armor",138,15.6,12.7,17,17,12.9,13.8,13.8,10.7,64,40,43,36,19.1,BODY},
		{111,"Dragonslayer Gauntlets",41,3.7,3,4.1,4.1,3.3,3.7,3.7,2.6,21,13,14,12,4.2,ARMS},
		{109,"Dragonslayer Helm",56,5,4,5.5,5.5,3.7,4.1,4.1,2.9,28,18,18,16,5.5,HEAD},
		{112,"Dragonslayer Leggings",81,8.1,6.5,8.9,8.9,6.3,6.9,6.9,4.9,38,23,24,21,10.3,LEGS},
		{99,"Drakeblood Armor",139,14.2,14.1,16.9,13.6,13.2,12.6,11.6,11.6,78,43,46,33,16.4,BODY},
		{100,"Drakeblood Gauntlets",46,3.5,3.4,4.3,3.3,3.1,2.9,2.7,2.7,25,13,14,9,3.8,ARMS},
		{98,"Drakeblood Helm",61,5.2,5.1,6.2,5,4.8,4.6,4.2,4.2,34,19,21,15,5.7,HEAD},
		{101,"Drakeblood Leggings",83,7.9,7.8,9.5,7.5,7.2,6.9,6.3,6.3,45,23,25,17,9.8,LEGS},
		{102,"Drang Armor",51,9.3,8.6,7.7,5.5,9.7,7.5,10.9,9.7,42,62,66,45,0.8,BODY},
		{103,"Drang Gauntlets",17,2.1,1.9,1.7,1.1,2,1.4,2.3,2,13,19,21,14,0.3,ARMS},
		{104,"Drang Shoes",42,4.7,5.2,5.2,5.2,6.3,5,7,6.3,24,37,39,26,1.3,LEGS},
		{114,"Eastern Armor",108,12.6,9.3,16.9,11.5,7.6,11.5,5.3,6.7,87,37,43,25,12.6,BODY},
		{115,"Eastern Gauntlets",29,2.8,2.2,3.8,2.5,1.8,3.1,1.5,1.8,25,9,11,7,2.6,ARMS},
		{113,"Eastern Helm",39,4.1,2.5,5.6,3.7,1.6,3.4,1.2,1.6,32,12,15,9,3.3,HEAD},
		{116,"Eastern Leggings",50,5.1,4.7,7.5,4.6,5.4,7.2,6.6,5.4,47,40,41,39,4.3,LEGS},
		{118,"Elite Knight Armor",89,12.1,9.2,12.1,11.1,9.2,10.6,6.8,8.5,46,32,33,18,8.8,BODY},
		{119,"Elite Knight Gauntlets",34,3.4,2.8,3.5,3.4,2.6,3,2,2.4,18,13,14,9,2.3,ARMS},
		{117,"Elite Knight Helm",52,4.9,3.9,4.9,4.5,3.9,4.2,3.3,3.7,24,15,15,9,4.7,HEAD},
		{120,"Elite Knight Leggings",68,8.3,6.8,8.3,7.4,6.8,7.4,5.7,6.5,38,25,26,16,7,LEGS},
		{14,"Embraced Armor of Favor",128,13.2,12.1,13.2,13.7,11.1,11.6,8.5,11.6,63,46,51,35,18.3,BODY},
		{123,"Evangelist Gloves",24,1.8,2.5,1.6,1.4,2.4,2.8,2.8,1.4,13,22,19,21,0.7,ARMS},
		{121,"Evangelist Hat",35,3.5,4.3,3.2,2.9,4.1,4.6,4.6,2.9,15,27,24,25,2.1,HEAD},
		{122,"Evangelist Robe",119,13.7,13.1,13.2,11.6,11.1,12.5,10.6,7.7,49,42,38,35,12.7,BODY},
		{124,"Evangelist Trousers",46,5.5,6.9,5.1,4.6,6.6,7.3,7.3,4.6,22,39,35,37,3.6,LEGS},
		{126,"Executioner Armor",160,18.3,12.4,17.5,18.3,13.5,14.5,11.4,13.3,65,58,61,42,21.4,BODY},
		{127,"Executioner Gauntlets",53,4.6,3.1,4.6,4.2,3.3,3.6,2.8,3.3,24,22,23,17,4.9,ARMS},
		{125,"Executioner Helm",68,6.7,4.4,6.7,6.1,4.9,5,4,4.7,31,28,29,21,7.5,HEAD},
		{128,"Executioner Leggings",99,10.6,7.1,10.1,10.6,7.9,8.1,6.5,7.6,39,35,37,25,13.2,LEGS},
		{130,"Exile Armor",173,18.2,17.6,16.6,15.9,12.4,14.3,12.6,13.1,97,74,62,50,23.1,BODY},
		{131,"Exile Gauntlets",55,4.8,4.4,4.1,4,3.3,3.7,3.3,3.4,33,25,22,17,5,ARMS},
		{132,"Exile Leggings",104,10.8,10.4,9.8,9.3,7.4,8.5,7.5,7.8,57,43,35,28,14.8,LEGS},
		{129,"Exile Mask",71,6.5,6.3,7,5.9,4.4,5,3.6,4.5,39,29,24,19,8.7,HEAD},
		{134,"Fallen Knight Armor",92,12.7,12.2,10,11.1,8.3,12.5,10.4,9,43,28,39,22,8.7,BODY},
		{135,"Fallen Knight Gauntlets",31,3.3,3.2,2.6,2.9,2.4,3.7,3,2.2,11,5,10,4,2.1,ARMS},
		{133,"Fallen Knight Helm ",46,4.6,4.5,3.6,4,3.1,4.2,3.5,2.6,21,14,19,12,3.5,HEAD},
		{136,"Fallen Knight Trousers",53,7.3,7,5.6,6.4,5,7.8,6.3,5.5,22,10,20,9,4.9,LEGS},
		{138,"Faraam Armor",127,13.9,13.8,13.9,12.8,10.7,12.2,11.2,11.2,59,40,60,27,17.3,BODY},
		{140,"Faraam Boots",67,7.2,7.5,7.2,6.6,5.6,6.4,5.6,5.6,34,24,37,15,8.2,LEGS},
		{139,"Faraam Gauntlets",47,3.6,3.7,3.6,3.3,3,3.3,3.1,3.1,21,16,22,11,4.5,ARMS},
		{137,"Faraam Helm",63,5.6,5.5,5.6,5.1,4.2,4.4,4.3,4.3,27,22,28,14,7,HEAD},
		{142,"Fire Keeper Gloves",13,0.8,0.9,1.1,0.7,3.1,3.3,2.9,3.6,9,15,11,23,0.2,ARMS},
		{141,"Fire Keeper Robe",51,5.7,7.1,7.1,8,7.1,11.9,8.8,12.4,36,54,49,42,1.9,BODY},
		{143,"Fire Keeper Skirt",21,1.9,2.6,1.9,1.9,7.1,7.6,6.8,8.3,13,24,18,40,0.5,LEGS},
		{145,"Fire Witch Armor",109,13.2,8.5,10.6,10.6,12.1,12.3,8.5,11.1,47,34,36,27,9.8,BODY},
		{146,"Fire Witch Gauntlets",26,2.8,1.4,2.4,2.2,2.4,2.6,1.4,2.2,15,10,12,9,1.5,ARMS},
		{144,"Fire Witch Helm",39,4.7,2.3,3.7,3.7,4.1,4.5,2.8,3.9,18,13,15,11,3,HEAD},
		{147,"Fire Witch Leggings",55,7.4,3.5,5.7,5.7,6.5,7.1,4.3,6.1,24,16,20,13,5.3,LEGS},
		{149,"Firelink Armor",86,11.9,8,12.9,10.2,6.9,10.7,5.5,5.5,47,30,43,19,8.9,BODY},
		{150,"Firelink Gauntlets",27,2.3,2.1,2.7,1.9,3,3.3,3.1,2.5,16,22,24,21,1.4,ARMS},
		{148,"Firelink Helm",41,4.8,2.5,4.8,4.4,2.6,4.4,1.8,1.4,20,12,16,8,4.2,HEAD},
		{151,"Firelink Leggings",48,5.4,5,6.2,4.5,6,7.3,7,5.6,28,40,44,37,3.5,LEGS},
		{309,"Follower Armor",56,7.1,10.2,8,9.5,10.2,8.8,10.9,8.8,37,59,65,48,2.7,BODY},
		{312,"Follower Boots",33,4,5.8,4.5,5.4,5.8,5,6.2,5,23,36,40,30,1.6,LEGS},
		{310,"Follower Gloves",17,1.7,2.5,1.9,2.3,2.5,2.1,2.7,2.1,12,20,22,16,0.6,ARMS},
		{311,"Follower Helm",37,4.2,4,4.4,4.6,3.4,3.1,2.5,2.8,21,14,21,10,3,HEAD},
		{15,"Gauntlets of Favor",40,3.5,3.2,3.5,3.5,3,3.1,2.2,3.1,19,13,14,10,4,ARMS},
		{19,"Gauntlets of Thorns",28,2.8,2.1,3.6,2.9,1.9,1.9,1.9,1.6,21,8,10,7,1.8,ARMS},
		{107,"Golden Bracelets",15,1.7,1.9,1.5,0.9,1.7,2.3,1.7,1.7,10,18,16,13,0.2,ARMS},
		{105,"Golden Crown",35,3.6,3,3,2.2,4.2,4.8,3.8,4.2,21,29,28,27,2.2,HEAD},
		{152,"Grave Warden Hood",15,1.5,1.2,2.9,1.8,3.3,3.3,4,1.4,30,28,8,25,0.5,HEAD},
		{153,"Grave Warden Robe",36,4.1,3.1,8.1,4.8,9,9,11.1,3.6,63,59,13,51,1,BODY},
		{154,"Grave Warden Skirt",22,2,1.5,4.3,2.5,4.8,4.8,6,1.7,42,40,11,35,0.3,LEGS},
		{307,"Grave Warden Wrap",12,0.8,0.5,1.8,1,1.9,1.9,2.5,0.6,20,18,3,16,0,ARMS},
		{289,"Gundyr's Armor",172,16.9,14.1,19.7,16.2,13.2,13.7,13,13.2,86,62,62,62,24.6,BODY},
		{290,"Gundyr's Gauntlets",58,4.4,3.6,5.1,4.1,3.4,3.5,3.4,3.4,29,21,21,21,6,ARMS},
		{288,"Gundyr's Helm",72,5.9,4.8,7,5.6,4.4,4.6,4.4,4.4,35,25,25,25,8.3,HEAD},
		{291,"Gundyr's Leggings",98,9,7.5,10.7,8.6,7.1,7.4,7,7.1,48,36,36,36,14.4,LEGS},
		{338,"Harald Legion Armor",172,18.3,16.7,18.6,18.3,12.6,12.7,12.2,13.6,68,62,61,60,20.7,BODY},
		{339,"Harald Legion Gauntlets",19,1.9,2.2,2.1,2.1,1.7,1.7,2.1,2.4,10,19,15,19,0.5,ARMS},
		{340,"Harald Legion Leggings",108,11,10,11.1,11,7.6,7.7,7.3,8.1,40,37,36,35,12.9,LEGS},
		{155,"Hard Leather Armor",83,10.3,10.8,10.3,10.3,8.7,9.4,7,8.7,47,64,59,52,6.8,BODY},
		{157,"Hard Leather Boots",47,5.1,5.5,5.1,5.1,4.1,4.6,2.8,4.1,26,38,35,29,2.9,LEGS},
		{156,"Hard Leather Gauntlets",20,2.1,2.2,2.1,2.1,1.5,1.7,1,1.5,16,22,21,17,0.8,ARMS},
		{159,"Havel's Armor",216,18.5,17,18.4,17.5,14.1,16.2,12.8,14.1,72,69,69,61,27.4,BODY},
		{160,"Havel's Gauntlets",91,4.7,4.3,4.6,4.4,3.5,4,3.2,3.5,24,23,23,20,7.2,ARMS},
		{158,"Havel's Helm",108,6.7,6.1,6.7,6.3,5,5.8,4.6,5,31,30,30,26,10.2,HEAD},
		{161,"Havel's Leggings",154,10.7,9.8,10.6,10.1,8.1,9.3,7.3,8.1,45,43,43,38,17.6,LEGS},
		{13,"Helm Of Favor",57,4.5,4.1,4.5,4.7,3.7,3.9,2.8,3.9,30,23,25,18,6,HEAD},
		{17,"Helm Of Thorns",38,3.7,2.6,4.9,3.9,2.3,2.3,2.3,1.8,30,13,15,11,2.2,HEAD},
		{163,"Herald Armor",86,12.4,8,12.9,11.9,9.2,8.4,3.8,8.4,52,30,36,19,9.5,BODY},
		{164,"Herald Gloves",29,3.1,2.1,3.1,2.9,1.7,2.1,0.7,1.5,16,12,12,8,1.6,ARMS},
		{162,"Herald Helm",37,4.2,2.5,4.4,4,3.5,3.2,1.6,3.2,24,14,17,10,3.4,HEAD},
		{165,"Herald Trousers",53,7.1,6.5,6.8,6.5,4.6,4.6,1.8,4.1,26,20,20,11,5.1,LEGS},
		{228,"Hood Of Prayer",13,0.8,0.8,0.8,0.8,3.9,3.5,3.7,4.4,8,28,13,40,0.2,HEAD},
		{11,"Iron Bracelets",29,2.4,1.6,2.4,2.4,2.2,2.2,2,2.2,11,9,10,9,2.2,ARMS},
		{342,"Iron Dragonslayer Armor",148,15.7,11.6,16.5,17.2,9.9,11.6,13.8,13.2,81,54,60,29,20.1,BODY},
		{343,"Iron Dragonslayer Gauntlets",51,3.7,2.6,3.9,4.1,2.2,2.6,3.2,3,26,17,19,8,4.8,ARMS},
		{341,"Iron Dragonslayer Helm",63,6.1,4.5,6.4,6.6,3.9,4.5,5.3,5.1,33,22,24,11,7,HEAD},
		{344,"Iron Dragonslayer Leggings",95,9.5,7.1,9.9,10.4,6.1,7.1,8.4,8,48,32,35,16,12.5,LEGS},
		{9,"Iron Helm",58,4.9,5.5,4.7,4.7,4.8,4.8,4.7,4.8,17,17,17,15,6.6,HEAD},
		{12,"Iron Leggings",50,5.9,4.2,5.9,5.9,5.5,5.5,5.1,5.5,21,17,19,16,6.4,LEGS},
		{168,"Jailer Gloves",16,1.7,0.7,1.7,1.1,2.7,2.6,2.2,2.7,10,15,13,32,0.3,ARMS},
		{167,"Jailer Robe",48,8.2,4.2,8.2,6,13.5,13.1,11.4,13.5,24,37,32,87,2,BODY},
		{169,"Jailer Trousers",28,4.2,1.9,4.2,3,6.9,6.7,5.7,6.9,12,22,19,53,1,LEGS},
		{284,"Karla's Coat",36,3.9,3.9,3.9,2.9,14.1,13.3,13.8,13.8,20,35,31,53,1.3,BODY},
		{285,"Karla's Gloves",12,0.8,0.9,0.9,0.9,3.5,3.3,3.4,3.4,7,12,10,18,0.3,ARMS},
		{283,"Karla's Pointed Hat",17,1.6,1.4,1.4,1.2,5,4.7,4.9,4.9,9,15,13,23,0.4,HEAD},
		{286,"Karla's Trousers",26,2.6,2.6,2.6,1.9,8.3,7.7,8.1,8.1,14,23,21,36,1.1,LEGS},
		{171,"Knight Armor",106,13.8,11.7,13.8,13.2,8.4,11,6.7,8.4,45,36,38,24,12.6,BODY},
		{172,"Knight Gauntlets",35,3.7,3.2,3.7,3.6,2.5,2.9,2.1,2.5,14,12,11,7,2.5,ARMS},
		{170,"Knight Helm",51,5.3,4.2,5.3,4.8,3,4,2.7,3.2,22,17,18,12,4.6,HEAD},
		{173,"Knight Leggings",66,8.3,7,8.3,7.9,5.4,6.2,4.4,5.4,26,23,22,13,7.6,LEGS},
		{363,"Lapp's Armor",156,18.2,15.5,18.2,18.2,12.3,12.7,11.2,12.3,77,61,54,38,20.7,BODY},
		{332,"Lapp's Gauntlets",51,4.6,3.9,4.6,4.6,3.1,3.2,2.8,3.1,24,19,17,11,4.9,ARMS},
		{331,"Lapp's Helm",65,6.8,5.8,6.8,6.8,4.6,4.7,4.2,4.6,31,25,22,15,7.3,HEAD},
		{333,"Lapp's Leggings",97,10.8,9.1,10.8,10.7,7.2,7.5,6.6,7.2,46,36,32,22,12.9,LEGS},
		{175,"Leather Armor",54,6.8,6.8,6.8,6.8,8.3,9,9.7,8.3,36,58,58,43,3.3,BODY},
		{177,"Leather Boots",33,4.2,4.2,4.2,4.2,5.5,5.9,6.3,5.5,24,37,37,28,2.8,LEGS},
		{64,"Leather Gauntlets",25,2.2,2,2.9,2,3.2,2.6,3.2,3.3,18,18,16,16,1.1,ARMS},
		{176,"Leather Gloves",15,1.4,1.4,1.4,1.4,1.9,2.1,2.3,1.9,11,18,17,13,0.1,ARMS},
		{91,"Leggings of Favor",75,7.7,7.1,7.7,7.7,6.5,6.8,4.8,6.8,41,29,32,24,10.3,LEGS},
		{20,"Leggings of Thorns",54,6,4.3,7.9,6.3,3.8,3.8,3.8,3,42,18,22,15,4.2,LEGS},
		{179,"Leonhard's Garb",69,9.8,9.8,9.1,9.8,12.2,12.7,9.8,8.3,41,55,62,55,6,BODY},
		{180,"Leonhard's Gauntlets",28,2.6,3,2.8,2.6,3.4,3.4,2.8,2.4,12,18,18,18,1.9,ARMS},
		{181,"Leonhard's Trousers",36,4.2,5.1,4.2,4.7,6.2,6.5,4.7,3.7,21,28,34,28,2.3,LEGS},
		{300,"Loincloth",11,1.9,1.9,1.9,1.6,5.8,4.9,5.4,5.8,20,35,10,43,0.4,LEGS},
		{364,"Loincloth (TRC)",20,2.4,2.1,1.9,1.7,7.7,6.7,7.3,7.4,18,29,15,37,0.6,LEGS},
		{297,"Lorian's Armor",147,16.6,10.1,16.6,15.9,13.3,11.2,11.2,13.6,67,41,61,50,18.4,BODY},
		{298,"Lorian's Gauntlets",30,3.2,2.3,3,2.9,2.9,1.5,1.5,2.9,16,7,11,10,1.8,ARMS},
		{296,"Lorian's Helm",55,5.4,3.5,5.4,3.8,4.8,3.6,3.6,4.8,24,12,17,15,4.6,HEAD},
		{299,"Lorian's Leggings",69,8.2,5.9,8.2,7.5,7.4,5.2,5.2,7.5,38,21,27,25,6.3,LEGS},
		{183,"Lothric Knight Armor",150,16,12.4,18.4,15.3,12.6,13.8,10.2,11.9,75,43,58,34,20.8,BODY},
		{184,"Lothric Knight Gauntlets",48,3.8,2.9,4.6,3.6,3.1,3.3,2.3,2.8,25,15,19,12,4.5,ARMS},
		{182,"Lothric Knight Helm",58,5.2,4,6.3,5,4.2,4.7,3.1,3.8,31,18,22,14,6.3,HEAD},
		{185,"Lothric Knight Leggings",89,8.8,6.8,10.6,8.4,7.1,7.7,5.4,6.5,44,24,32,20,12.1,LEGS},
		{196,"Lucatiel's Mask",33,3.9,3.6,3,2.8,4,3.8,4.8,3.8,22,26,22,24,1.6,HEAD},
		{192,"Maiden Gloves",13,0.9,1.1,0.9,0.8,3.5,3.3,3.4,3.5,8,14,11,26,0.3,ARMS},
		{190,"Maiden Hood",14,1,1.2,1,0.9,4.6,4.3,4.4,4.6,8,16,12,31,0,HEAD},
		{191,"Maiden Robe",35,3.5,4.1,3.5,3.1,13.7,12.8,13,13.7,19,38,30,75,0.7,BODY},
		{193,"Maiden Skirt",23,1.7,2,1.7,1.5,7.5,7,7.1,7.5,13,25,20,48,0.1,LEGS},
		{194,"Master's Attire",20,2.7,2.7,2.7,2.1,9.1,7.6,8.4,9.1,37,62,22,74,0,BODY},
		{195,"Master's Gloves",3,0.4,0.4,0.4,0.3,1.9,1.5,1.7,1.9,14,22,9,26,0,ARMS},
		{313,"Millwood Knight Armor",156,18.3,13.9,17.5,15.3,13,12.6,13,12.6,67,61,69,37,20.2,BODY},
		{314,"Millwood Knight Gauntlets",53,4.6,3.4,4.4,3.8,3.2,3.1,3.2,3.1,22,20,23,12,4.7,ARMS},
		{315,"Millwood Knight Helm",67,6.7,5,6.3,5.5,4.6,4.5,4.6,4.5,29,26,30,16,6.9,HEAD},
		{316,"Millwood Knight Leggings",97,10.6,8,10.1,8.8,7.4,7.2,7.4,7.2,41,38,43,23,12.6,LEGS},
		{202,"Mirrah Chain Gloves",33,3.2,2.4,3.8,2.6,2.8,2.8,1.7,2.2,18,13,14,11,2.5,ARMS},
		{203,"Mirrah Chain Leggings",59,6.5,4.7,7.7,5.1,5.5,5.5,2.9,4.2,32,23,25,18,5.8,LEGS},
		{201,"Mirrah Chain Mail",104,12.7,10.5,14.9,10.5,11.7,11.2,7.4,9.1,51,33,37,26,11.3,BODY},
		{198,"Mirrah Gloves",21,2.1,2.1,1.5,1.5,1.9,1.7,2.5,1.7,17,21,17,19,0.2,ARMS},
		{199,"Mirrah Trousers",35,5.1,5.1,3.7,3.7,5,4.6,6.4,4.6,29,37,29,32,1.2,LEGS},
		{197,"Mirrah Vest",70,10.5,10.5,8.2,8.2,10.6,9.9,12.8,9.9,54,64,54,60,4.2,BODY},
		{187,"Morne's Armor",155,17.4,14.3,17.4,16.6,13.9,13.6,13.3,9.3,67,39,60,37,20.6,BODY},
		{188,"Morne's Gauntlets",48,3.8,3.2,3.8,3.7,3.1,3.1,3,1.7,21,12,18,11,4.1,ARMS},
		{186,"Morne's Helm",77,6.8,5.8,6.8,6.8,5.5,5.4,5.2,4,31,17,27,17,8,HEAD},
		{189,"Morne's Leggings",93,9.8,8.1,9.8,9.3,8.1,7.9,7.8,5.2,40,23,36,22,12.8,LEGS},
		{205,"Nameless Knight Armor",93,11.7,10,12.7,11.1,7.6,11,8.4,7.6,61,38,35,21,8.7,BODY},
		{206,"Nameless Knight Gauntlets",28,3,2.6,3.3,2.9,2.1,3.1,2.3,2.1,19,10,9,6,2.2,ARMS},
		{204,"Nameless Knight Helm",38,3.8,3.1,4.2,3.6,2.1,3.4,2.4,2.1,26,15,14,10,2.3,HEAD},
		{207,"Nameless Knight Leggings",56,7,6,7.6,6.7,4.9,6.9,5.4,4.9,36,22,20,11,5.9,LEGS},
		{209,"Northern Armor",106,12.6,12.6,12.1,11.6,7.7,11.1,4.4,6.8,60,38,64,27,10.7,BODY},
		{210,"Northern Gloves",23,2.2,2.9,2.2,2.2,0.8,2.5,2,0.8,15,19,24,19,0.7,ARMS},
		{208,"Northern Helm",48,4.8,4.7,4.6,4.4,2.9,4.2,1.8,2.6,24,15,26,10,4,HEAD},
		{211,"Northern Trousers",43,6.1,7.6,6.1,6.1,2.9,6.8,5.7,2.9,24,32,42,32,3.1,LEGS},
		{79,"Old Sage's Blindfold",10,0.8,0.8,0.8,0.8,4,4.4,3.8,4.2,10,14,11,28,0,HEAD},
		{215,"Old Sorcerer Boots",23,2.1,1.8,1.8,2.1,7.3,6.7,7,7.3,18,23,20,42,1.1,LEGS},
		{213,"Old Sorcerer Coat",37,3.6,3.1,3.1,3.6,12.7,11.6,12.1,12.7,32,40,35,70,1,BODY},
		{214,"Old Sorcerer Gauntlets",13,0.9,0.8,0.8,0.9,3.2,2.9,3.1,3.2,11,14,12,24,0,ARMS},
		{212,"Old Sorcerer Hat",12,1.5,1.3,1.3,1.5,4.9,4.3,4.5,4.9,11,15,12,28,0.6,HEAD},
		{317,"Ordained Dress",44,5.7,3.9,8,2.9,14.5,11.4,12.6,12.6,20,53,48,69,2.7,BODY},
		{318,"Ordained Hood",19,2,1.4,2.8,1,5.2,4,4.6,4.5,9,19,21,32,0.4,HEAD},
		{319,"Ordained Trousers",28,3.2,2.2,4.5,1.6,8.3,6.5,7.2,7.2,12,33,30,43,1.6,LEGS},
		{217,"Outrider Knight Armor",120,14.4,10.6,13.8,13.8,9.7,11.4,9.7,7.5,50,43,91,22,12.5,BODY},
		{218,"Outrider Knight Gauntlets",29,3.4,2.4,3.3,3.4,2.4,3,2.4,1.6,12,10,25,4,2.7,ARMS},
		{216,"Outrider Knight Helm",38,4.4,2.8,4.2,4.4,2.3,3.1,2.3,1.1,17,12,32,5,2.8,HEAD},
		{219,"Outrider Knight Leggings",68,7.9,6,7.6,7.9,5.9,6.7,5.5,3.7,32,24,55,14,7.2,LEGS},
		{226,"Painting Guardian Gloves",13,0.8,0.5,0.8,0.8,2.5,0.8,1.1,0.8,21,25,13,21,0.2,ARMS},
		{225,"Painting Guardian Gown",35,4.4,3.1,4.4,4.4,11.1,4.4,5.4,4.4,59,71,36,60,0.5,BODY},
		{224,"Painting Guardian Hood",14,1.9,1.5,1.9,1.9,4.3,1.9,2.3,1.9,25,30,15,25,0.6,HEAD},
		{227,"Painting Guardian Waistcloth",44,3.5,2.9,3.5,4.8,6.5,5.3,2.9,2.9,42,31,17,22,4.2,LEGS},
		{222,"Pale Shade Gloves",10,1,1,1,0.9,3.6,3.3,3.1,3.2,9,17,8,21,0.2,ARMS},
		{221,"Pale Shade Robe",36,4.2,4.2,4.2,3.2,13.4,12.8,11.8,12.3,35,60,32,70,2,BODY},
		{223,"Pale Shade Trousers",22,1.9,1.9,1.9,1.4,7,6.7,6.1,6.4,20,35,18,44,1,LEGS},
		{174,"Pharis's Hat",23,2.7,2.2,2.7,2.7,3.6,3.9,4.1,3.6,17,26,24,20,1.4,HEAD},
		{236,"Pontiff Knight Armor",73,9.3,5.5,9.3,8.6,12.9,9.3,11.9,13.3,40,36,81,29,6,BODY},
		{235,"Pontiff Knight Crown",27,2.8,1.4,2.8,2.5,4.7,3.3,4.3,5,15,12,31,10,2,HEAD},
		{237,"Pontiff Knight Gauntlets",22,2.1,1.1,2.1,1.9,3.6,2.6,3.3,3.7,14,13,28,10,0.7,ARMS},
		{238,"Pontiff Knight Leggings",43,5.1,3,5.1,4.7,7.6,5.5,7,7.8,23,21,48,16,2.8,LEGS},
		{231,"Pyromancer Crown ",11,0.7,0.7,0.7,0.7,4.2,4.8,3.6,4,9,14,15,21,0,HEAD},
		{232,"Pyromancer Garb",42,4.4,3.7,3.7,3.7,12,13.1,12,14.1,22,50,43,74,0.9,BODY},
		{234,"Pyromancer Trousers",26,2.7,2.2,2.2,2.2,7.1,8.1,7.5,7.8,14,43,26,47,1.2,LEGS},
		{233,"Pyromancer Wrap",15,1.4,1.4,1.2,1.2,3.5,3.6,3.5,3.6,6,14,11,22,0.8,ARMS},
		{287,"Ragged Mask",7,1.1,1.1,1.1,1.1,3.8,3.2,3.5,3.8,13,25,7,29,0.4,HEAD},
		{335,"Ringed Knight Armor",126,14.9,11.6,16.3,14.3,11.1,13.2,12.2,9.8,60,36,40,34,14.9,BODY},
		{336,"Ringed Knight Gauntlets",44,3.9,3,4.2,3.7,2.9,3.5,3.2,2.6,21,13,15,13,3.2,ARMS},
		{334,"Ringed Knight Hood",14,1.1,0.8,1.7,0.9,4.2,4.4,4.5,4,12,18,14,33,0.2,HEAD},
		{337,"Ringed Knight Leggings",81,8.1,6.2,8.9,7.8,5.9,7.2,6.5,5.1,37,22,24,21,9.2,LEGS},
		{229,"Robe Of Prayer",34,3.6,3.2,3.6,3.2,13.5,13.1,13.1,14.6,13,61,24,89,1.3,BODY},
		{347,"Ruin Armor",129,13.7,13.1,16.3,13.7,12.8,11.6,12.2,10.5,62,60,35,32,17.4,BODY},
		{348,"Ruin Gauntlets",42,3.6,3.4,4.2,3.6,3.4,3,3.2,2.7,19,19,10,9,4.1,ARMS},
		{346,"Ruin Helm",54,4.6,4.3,5.5,4.6,4.2,3.8,4,3.4,25,24,13,12,6.1,HEAD},
		{349,"Ruin Leggings",80,7.4,7.1,8.9,7.4,6.9,6.2,6.5,5.5,37,35,20,18,10.7,LEGS},
		{305,"Sage's Big Hat",19,2.5,2,2,1.6,5.2,3.6,4.6,4.6,15,21,13,26,0.4,HEAD},
		{308,"Scholar's Robe",42,6.9,2.8,4.5,5.6,13.3,11.2,12.8,12.6,36,43,39,71,3.6,BODY},
		{240,"Sellsword Armor",106,12.2,12.7,12.2,12.2,11.1,9.1,7.3,10.5,59,39,35,26,11.5,BODY},
		{241,"Sellsword Gauntlets",14,1.4,1.4,0.8,1.6,0.6,0.8,0.8,0.6,15,10,11,8,0,ARMS},
		{239,"Sellsword Helm",31,3.1,3.3,3.6,3.9,2.7,2.7,2.7,2.7,27,17,16,12,3.5,HEAD},
		{242,"Sellsword Trousers",36,4.7,5.1,4.2,4.7,2.3,3.7,2.3,2.3,28,17,19,9,3.4,LEGS},
		{244,"Shadow Garb",37,6.8,5.5,4.4,6.8,4.2,5.2,6.6,4.2,27,40,38,34,2.2,BODY},
		{245,"Shadow Gauntlets",13,1.6,1.4,1.6,1.6,1.6,1.9,2.2,1.6,10,15,14,13,1.1,ARMS},
		{246,"Shadow Leggings",23,4.2,3.5,2.9,4.2,3.1,3.7,4.5,3.1,16,24,23,21,2.1,LEGS},
		{243,"Shadow Mask",15,2.5,2.2,1.7,2.5,2,1.5,2,1.1,15,18,20,18,0.4,HEAD},
		{328,"Shira's Armor",67,7.7,9.3,7.7,10,11.1,11.7,11.1,8.6,38,62,61,62,3.7,BODY},
		{362,"Shira's Crown",10,0.8,0.9,0.8,1,4.4,4.3,4.1,3.9,5,16,11,30,0.2,HEAD},
		{329,"Shira's Gloves",21,1.7,2.1,1.7,2.3,2.7,2.8,2.7,1.9,11,19,19,19,0.7,ARMS},
		{330,"Shira's Trousers",38,4.2,5.2,4.2,5.6,6.4,6.7,6.4,4.7,21,36,35,36,1.5,LEGS},
		{248,"Silver Knight Armor",144,14.3,13.1,17,16.3,13.1,12.9,11.9,11.9,65,55,50,39,19.9,BODY},
		{249,"Silver Knight Gauntlets",46,3.6,3.1,4.3,4.1,3.1,2.9,2.7,2.7,22,18,17,14,3.8,ARMS},
		{247,"Silver Knight Helm",59,5.1,4.7,6,5.8,4.9,4.7,4.3,4.3,24,19,17,13,6.8,HEAD},
		{250,"Silver Knight Leggings",88,7.9,7.3,9.4,9,7.6,7.3,6.7,6.7,36,28,25,20,11,LEGS},
		{178,"Silver Mask",33,3.3,3.9,3.3,3.3,3.5,4.2,3.3,2.8,19,27,28,27,1.9,HEAD},
		{230,"Skirt Of Prayer",20,1.6,1.9,1.6,1.4,6.9,6.7,6.7,7.6,14,39,18,58,0,LEGS},
		{320,"Slave Knight Armor",87,13.5,10.9,13.5,10.2,5.7,8,5.7,7.1,61,34,35,20,9.1,BODY},
		{321,"Slave Knight Gauntlets",30,3.3,2.7,3.3,2.5,1.4,1.9,1.4,1.7,20,11,12,7,2,ARMS},
		{322,"Slave Knight Hood",38,4.8,3.8,4.8,3.6,2,2.8,2,2.5,26,14,15,9,3,HEAD},
		{323,"Slave Knight Leggings",54,7.7,6.2,7.7,5.8,3.2,4.5,3.2,4,38,21,22,12,5.5,LEGS},
		{276,"Smough's Armor",230,18.8,19.2,18.3,16,13.3,13,13.8,13.1,69,69,69,62,27.1,BODY},
		{277,"Smough's Gauntlets",94,4.8,4.9,4.6,4,3.3,3.2,3.4,3.5,20,20,20,18,6.9,ARMS},
		{275,"Smough's Helm",118,6.8,7,6.7,5.8,4.7,4.6,4.9,4.9,27,27,27,24,9.9,HEAD},
		{278,"Smough's Leggings",158,10.9,11.1,10.6,9.2,7.6,7.4,7.9,7.6,43,43,43,39,17.3,LEGS},
		{220,"Sneering Mask",21,1.4,1.7,1.4,1.1,3.6,4.1,2.4,1.6,35,28,22,20,0.5,HEAD},
		{253,"Sorcerer Gloves",13,1.3,1.3,1.3,1.1,3.5,3.3,3.4,3.5,8,13,10,23,0.6,ARMS},
		{251,"Sorcerer Hood",14,1,1,1,0.9,4.4,4,4.2,4.5,11,18,14,30,0.1,HEAD},
		{252,"Sorcerer Robe",41,4.5,4.5,4.5,3.8,13.1,12.4,12.6,13.1,25,40,33,71,1.7,BODY},
		{254,"Sorcerer Trousers",32,3.4,4.1,3.4,3.4,6,7.5,7.6,6.7,22,26,31,28,1.3,LEGS},
		{304,"Standard Helm",37,3.7,3.7,3.7,3.7,3,3.3,2.4,3,22,29,27,24,2.5,HEAD},
		{95,"Steel Soldier Helm",46,4.4,3.8,4.4,4.2,2.9,2.9,2,3.7,27,18,18,13,3.2,HEAD},
		{293,"Sunless Armor",92,11.7,9.1,12.2,11.7,11.2,9.8,6,11.7,43,25,33,37,8.5,BODY},
		{294,"Sunless Gauntlets",35,2.6,2.2,2.8,2.6,2.6,2.2,1.4,2.6,13,7,9,7,1.7,ARMS},
		{295,"Sunless Leggings",57,6.2,4.7,6.5,6.2,5.9,5.1,2.9,6.2,27,18,21,18,4.7,LEGS},
		{292,"Sunless Veil",18,1.7,1.2,1.2,1.3,5.2,4.7,4.8,5.3,7,14,10,30,0.7,HEAD},
		{256,"Sunset Armor",133,14.3,11.7,15.6,14.3,9.8,11.7,9.1,11.2,63,58,39,42,16.5,BODY},
		{257,"Sunset Gauntlets",44,4.1,3.5,4.1,3.9,2.6,3.1,2.4,3,24,22,16,17,4,ARMS},
		{255,"Sunset Helm",48,5.3,4.5,5.3,4.9,3.2,3.7,2.6,3.7,26,25,14,15,3.4,HEAD},
		{258,"Sunset Leggings",70,7.2,5.9,7.8,7.2,4.6,5.5,3.3,5.1,40,35,26,26,5.8,LEGS},
		{306,"Symbol of Avarice",106,5.8,5.6,5,4.4,4.1,3.4,4.8,3.8,27,27,27,22,8.9,HEAD},
		{42,"Thief Mask",21,2.2,2.5,2.2,2.2,2.2,2.5,2.8,2.2,23,26,21,17,0.6,HEAD},
		{303,"Thrall Hood",15,2,1.4,1.6,1.2,4.2,3.8,4,4.2,17,28,19,38,0.8,HEAD},
		{260,"Undead Legion Armor",76,10.4,10.4,5.9,9,12.6,13,13.2,10.4,39,51,65,42,4.9,BODY},
		{261,"Undead Legion Gauntlet",24,2.3,2.3,1.2,2,2.9,3,3,2.3,16,20,24,17,0.8,ARMS},
		{259,"Undead Legion Helm",40,3.8,4,2.6,3.5,4.7,4.8,4.9,4.2,19,24,29,21,2.3,HEAD},
		{262,"Undead Legion Leggings",46,5.6,5.6,3.1,4.8,6.9,7.1,7.2,5.6,27,35,43,29,2.6,LEGS},
		{324,"Vilhelm's Armor",108,12.9,10.2,12.9,14.6,9.5,12.4,5.7,11.4,44,35,37,31,13,BODY},
		{325,"Vilhelm's Gauntlets",37,3.2,2.5,3.2,3.6,2.3,3.1,1.4,2.8,15,12,12,10,2.9,ARMS},
		{326,"Vilhelm's Helm",48,4.6,3.6,4.6,5.2,3.4,4.4,2,4,19,15,16,13,4.4,HEAD},
		{327,"Vilhelm's Leggings",68,7.4,5.8,7.4,8.4,5.4,7.1,3.2,6.5,27,22,23,19,8,LEGS},
		{361,"Violet Wrappings",9,0.7,0.5,0.4,0.4,3,2.5,2.8,2.9,11,17,10,21,0.1,ARMS},
		{345,"White Preacher Head",35,2.6,4.2,4,4,4.6,3.8,4,2.9,17,28,24,29,2.6,HEAD},
		{264,"Winged Knight Armor",182,17.5,14.4,17.5,16.8,14.3,14,15,16.7,73,60,68,45,24.9,BODY},
		{265,"Winged Knight Gauntlets",60,4.8,3.7,4.8,4.4,3.5,4,3.8,4.5,22,21,21,13,5.5,ARMS},
		{263,"Winged Knight Helm",70,6.7,5.4,6.7,6.3,4.3,5,4.7,5.7,31,29,29,18,6.9,HEAD},
		{266,"Winged Knight Leggings",105,10.1,8.3,10.1,9.6,7.2,7.1,7.6,8.4,42,32,39,24,14.1,LEGS},
		{280,"Wolf Knight Armor",90,11.8,10.1,11.8,11.8,7.8,11.2,5.5,10,46,32,33,33,8,BODY},
		{281,"Wolf Knight Gauntlets",31,3.2,2.8,3.3,3.3,2,3.1,1.4,2.7,18,13,14,14,1.6,ARMS},
		{279,"Wolf Knight Helm",42,4.4,3.8,4.4,4.4,3,4.2,2.2,3.8,23,16,17,17,2.9,HEAD},
		{282,"Wolf Knight Leggings",51,6.6,5.5,6.6,6.6,4.2,6.4,2.8,5.6,25,15,19,19,4.6,LEGS},
		{301,"Wolnir's Crown",34,3.1,3.1,3.1,2.8,3.9,4.3,4.5,4.9,18,29,27,37,1.8,HEAD},
		{268,"Worker Garb",42,4.4,6.8,5.5,5.5,6.6,5.2,5.2,10.9,34,68,43,60,2.3,BODY},
		{269,"Worker Gloves",14,0.9,1.5,1.1,1.1,1.2,0.9,0.9,2.3,11,22,14,19,0.8,ARMS},
		{267,"Worker Hat",23,1.9,2.7,2.2,2.2,3,2.5,2.5,4.5,16,31,20,27,1.1,HEAD},
		{270,"Worker Trousers",29,2.9,4.2,3.5,3.5,4.5,3.7,3.7,7,23,44,28,39,1.6,LEGS},
		{271,"Xanthous Crown",30,2.1,2.6,2.6,2.1,4.6,2.2,4.8,4.6,21,27,24,27,1.3,HEAD},
		{273,"Xanthous Gloves",20,1.8,2,1.2,1.8,2.7,1.2,2.9,2.6,15,22,22,18,0.8,ARMS},
		{272,"Xanthous Overcoat",86,10.1,6.9,10.7,9.4,11.7,5.5,10,11.7,39,46,33,22,7.7,BODY},
		{274,"Xanthous Trousers",39,4.6,5.1,3.4,4.6,7,3.4,7.4,6.7,22,35,35,28,1.5,LEGS}
};

struct armorset{
	vec_absorptions absorptions;
	const armor *head = 0, *body, *arms, *legs;
	float score = 0;
	unsigned short weight;
	bool operator <(const armorset& o) const { return score != o.score ? score > o.score : weight < o.weight; }
};

const int BESTN_TOT = 50;
armorset weightrank[601][BESTN_TOT];	//60.0 is the maximum weight

#ifdef __x86_64__
//XXX this would be so nice but it crashes gcc 6.3
//__attribute__((target_clones("default","avx","avx2")))
#endif
int main(int argc, char** argv){
	memset(weightrank, 0, sizeof(weightrank));

	vec_absorptions weights;
	bool harmonic_mean, duskcrown;
	unsigned int maxtiers;
	set<string> exclusions;

	boost::program_options::options_description options("Options for ds3armor");
	try {
		using namespace boost::program_options;
		vector<string> exclusions_v;
		options.add_options()
			("physical", value(&weights.physical)->default_value(0.25), "physical absorption weight")
			("vs_strike", value(&weights.vs_strike)->default_value(0.25), "vs_strike absorption weight")
			("vs_slash", value(&weights.vs_slash)->default_value(0.25), "vs_slash absorption weight")
			("vs_thrust", value(&weights.vs_thrust)->default_value(0.25), "vs_thrust absorption weight")
			("magic,m", value(&weights.magic)->default_value(0), "magic absorption weight")
			("fire,f", value(&weights.fire)->default_value(0), "fire absorption weight ")
			("lightning,l", value(&weights.lightning)->default_value(0), "lightning absorption weight")
			("dark,d", value(&weights.dark)->default_value(0), "dark absorption weight")
			("balanced", value(&harmonic_mean)->default_value(true), "penalize sets with a specific weakness using harmonic averages")
			("duskcrown", value(&duskcrown)->default_value(false), "force the Crown of Dusk")
			("maxtiers", value(&maxtiers)->default_value(10), "max number of tiers to display")
			("exclude,e", value(&exclusions_v), "exclude armor pieces by name")
			("help", "produce help message")
			;
		variables_map vm;
		store(parse_command_line(argc, argv, options), vm);
		notify(vm);
		if(vm.count("help")){
			cerr<<options<<endl;
			return 0;
		}
		if(!maxtiers || maxtiers > BESTN_TOT){
			cerr<<"maxtiers must be between 1 and"<<BESTN_TOT<<endl;
			return 1;
		}
		exclusions.insert(exclusions_v.begin(), exclusions_v.end());
	} catch(const boost::program_options::error& e){
		cerr<<"Failed to parse command line: "<<e.what()<<endl<<options<<endl;
		return 1;
	}

	float weight_total_inverse = 0;
	for(int i = 0; i < 8; i++) if(weights.all[i] < 0){
		cerr<<"no weight can be negative"<<endl;
		return 1;
	} else weight_total_inverse += weights.all[i];
	if(weight_total_inverse == 0.){
		cerr<<"At least one weight must be > 0"<<endl;
		return 1;
	}
	weight_total_inverse = 1 / weight_total_inverse;
	auto score = [&](const armorset& set){	//basically a weighted sum of the absorptions
		auto x = set.absorptions;
		if(harmonic_mean) x.all = 1 / x.all;
		x.all *= weights.all;
		x.v4[0] += x.v4[1];
		x.v2[0] += x.v2[1];
		float r = (x.v2[0][0] + x.v2[0][1]);
		return harmonic_mean ? weight_total_inverse / r : weight_total_inverse * r;
	};

	vector<const armor*> armor_by_type[4];	//HEAD, BODY, ARMS, LEGS
	for(auto& a: ds3armors){
		auto it = exclusions.find(a.name);
		if(it != exclusions.end()){
			exclusions.erase(it);
			continue;
		}
		if(a.id == 306) continue;	//exclude Symbol of Avarice
		if(a.type == HEAD ? duskcrown == (a.id == 2) : true) armor_by_type[a.type].push_back(&a);	//use Crown of Dusk only if selected by the user (and force it)
	}
	if(!exclusions.empty()){
		cerr<<"Unrecognized exclusions:"<<endl;
		for(auto& e: exclusions) cerr<<e<<endl;
		return 1;
	}

	armorset candidate;
	for(auto body: armor_by_type[BODY]){
		candidate.body = body;
		for(auto legs: armor_by_type[LEGS]){
			candidate.legs = legs;
			auto body_legs_abs = (body->absorptions.all - 100) * (legs->absorptions.all - 100);
			unsigned short body_legs_weight = body->weight + legs->weight;
			for(auto arms: armor_by_type[ARMS]){
				candidate.arms = arms;
				auto body_legs_arms_abs = body_legs_abs * (arms->absorptions.all - 100);
				unsigned short body_legs_arms_weight = body_legs_weight + arms->weight;
				for(auto head: armor_by_type[HEAD]){
					candidate.head = head;
					candidate.absorptions.all = 100 - (body_legs_arms_abs * (head->absorptions.all - 100)) / 1000000;
					candidate.weight = body_legs_arms_weight + head->weight;
					candidate.score = score(candidate);
					auto& bestn = weightrank[candidate.weight];
					if(!bestn[BESTN_TOT - 1].head || bestn[BESTN_TOT - 1].score < candidate.score){
						bestn[BESTN_TOT - 1] = candidate;
						sort(bestn, bestn + BESTN_TOT);
					}
				}
			}
		}
	}

	const char* absorption_names[]{"physical", "vs_strike", "vs_slash", "vs_thrust", "magic", "fire", "lightning", "dark"};
	int namelens[8], prevlen = printf("    weight | ");
	for(int i = 0; i < 8; i++){
		namelens[i] = max((int)strlen(absorption_names[i]), 6);
		prevlen += printf("%6s | ", absorption_names[i]);
	}
	prevlen += printf("armor pieces");
	cout<<endl;
	float bestscore = 0;
	vector<armorset> tiers;
	for(auto& bestn: weightrank){
		tiers.insert(tiers.end(), bestn, bestn + BESTN_TOT);
		sort(tiers.begin(), tiers.end());
		if(bestscore >= tiers[0].score) continue;
		bestscore = tiers[0].score;
		for(unsigned int i = 0; i < min(maxtiers, (unsigned int)tiers.size()); i++){
			auto& best = tiers[i];
			if(!best.head) break;
			if(!i) cout<<string(prevlen, '=')<<endl;
			prevlen = printf(!i ? "best" : "    ");
			prevlen += printf("%6.1f | ", best.weight / 10.);
			if(duskcrown) best.absorptions.magic -= 30;
			for(int i = 0; i < 8; i++) prevlen += printf(duskcrown && i == 4 ? "%*.2f | " : "%*.3f | ", namelens[i], best.absorptions.all[i]);
			prevlen += printf("%s, %s, %s, %s", best.head->name, best.body->name, best.arms->name, best.legs->name);
			cout<<endl;
		}
		tiers.clear();
	}
}
