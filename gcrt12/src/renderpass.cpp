#include "renderpass.h"

/**
 * @brief Render effect node constructor.
 * 
 * @param type Type of render effect for this node.
 */

Effect12::Effect12(const EffectType type)
{
	InitPipelineState(type);
}

/**
 * @brief Initialize this effect's pipeline state and shaders.
 * 
 * @param type Type of effect/pipeline state to initialize.
 *  
 * @return GCRT_OK
 */

GCRT_RESULT Effect12::InitPipelineState(const EffectType type)
{
	switch (type)
	{
		case PHONG:

			InitPhongPSO();
			break;

	default:

		break;
	}

	return GCRT_OK;
}