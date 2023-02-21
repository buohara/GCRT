float4 mainVS(float4 pos : POSITION) : SV_POSITION
{
	return pos;
}

#define test_sig                                                        \
    "RootFlags(DENY_HULL_SHADER_ROOT_ACCESS | "                         \
              "DENY_DOMAIN_SHADER_ROOT_ACCESS | "                       \
              "DENY_GEOMETRY_SHADER_ROOT_ACCESS | "                     \
              "ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT)"

[RootSignature(test_sig)]
float4 mainPS() : SV_TARGET
{
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}