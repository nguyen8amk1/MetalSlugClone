#pragma once 
#include "../Util.h"

namespace MetalSlug {
class TimeBoundedLerp {
private: 
	float timeAccumulator = 0;
	float t = 0;
	float duration = 0;
public: 
	TimeBoundedLerp(float duration) {
		this->duration =duration;
	}


	void resetHard() {
		reset();
		t = 0;
	}

	void resetSmooth() {
		reset();
		t -= 1;
	}

	void update(double dt, bool clampValue = true) {
		timeAccumulator += dt;
		t = Util::Math::normalizef(timeAccumulator, duration);
		if (clampValue) {
			t = Util::Math::clampf(t, 0, duration);
		}
	}

	float getCurrentT() {
		return t;
	}

	void setDuration(float duration) {
		this->duration = duration;
	}

private: 
	void reset() {
		timeAccumulator = 0;
	}
};

}