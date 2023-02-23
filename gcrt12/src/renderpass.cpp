#include "renderpass.h"

/**
 * @brief Render effect node constructor.
 * 
 * @param type Type of render effect for this node.
 */

Effect12::Effect12(
	const EffectType type,
	ComPtr<ID3D12Device> device,
	ComPtr<ID3D12PipelineLibrary> pipelineLibrary
)
{
	InitPipelineState(type, device, pipelineLibrary);
}

/**
 * @brief Initialize this effect's pipeline state and shaders.
 * 
 * @param type				[in] Type of effect/pipeline state to initialize.
 * @param pDevice			[in] D3D device to create root signature/PSO.
 * @param pPipelineLibrary	[in/out] If pipeline state for effect is already cached, 
 *							load it. Otherwise create and store it.
 *  
 * @return GCRT_OK
 */

GCRT_RESULT Effect12::InitPipelineState(
	const EffectType type,
	ComPtr<ID3D12Device> pDevice,
	ComPtr<ID3D12PipelineLibrary> pPipelineLibrary
)
{
	switch (type)
	{
		case PHONG:

			InitPhongPSO(pDevice, pPipelineLibrary);
			break;

	default:

		break;
	}

	return GCRT_OK;
}