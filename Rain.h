#pragma once

#include "GameObject.h"

class Rain : public GameObject {

public:
	Rain();

	void UpdateParticles() override;
	void update() override;


private:



};