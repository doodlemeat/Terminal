#include "Disg.h"


Disg::Disg()
{
}


Disg::~Disg()
{
}

sf::Clock* Disg::getTimer()
{
	return &m_timer;
}
