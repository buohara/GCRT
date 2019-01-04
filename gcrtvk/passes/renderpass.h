#pragma once

#include "gcrtvk.h"
#include "utils.h"

struct RenderPassVk
{
	virtual void Render() = 0;
};