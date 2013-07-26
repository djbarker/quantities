#include "lists.hpp"

std::ostream& operator<<(std::ostream& out, const lists::end_element& el)
{
	return out << "END";
}
