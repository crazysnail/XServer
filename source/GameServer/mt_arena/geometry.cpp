#include "geometry.h"

const Packet::Position zxero::string2pos(std::string pos)
{
	Packet::Position ret;
	std::vector<std::string> string_pos;
	boost::split(string_pos, pos, boost::is_any_of("|"));
	ret.set_x(int(1 * (boost::lexical_cast<int>(string_pos[0]))));
	ret.set_y(int(1 * (boost::lexical_cast<int>(string_pos[1]))));
	ret.set_z(int(1 * (boost::lexical_cast<int>(string_pos[2]))));
	return ret;
}
