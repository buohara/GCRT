#pragma once

#include "gcrtcommon12.h"
#include "phong.h"

enum EffectType
{
	PHONG,
};

struct Effect12
{
	EffectType type;

	Effect12(EffectType);
	GCRT_RESULT InitPipelineState(const EffectType type);

private:

	Effect12();
};