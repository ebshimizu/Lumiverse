
#include "PhotometricGobo.h"

namespace {
	inline float sphericalPhi(const AtVector &v)
	{
		float p = atan2f(v.z, v.x);
		return (p < 0.f) ? p + 2.f*float(AI_PI) : p;
	}

	inline float sphericalTheta(const AtVector &v)
	{
		return acosf(v.y);
	}

	inline static void computeUV(AtShaderGlobals *sg, float deg, float rot) {
		// https://bitbucket.org/anderslanglands/alshaders/overview
		AtMatrix lightToWorld, worldToLight;
		AiNodeGetMatrix(sg->Lp, "matrix", lightToWorld);
		AiM4Invert(lightToWorld, worldToLight);

		// get the major axis of the light in local space
		AtVector L;
		AiM4VectorByMatrixMult(&L, worldToLight, &sg->Ld);

		// rotate the coordinate
		AtMatrix lightRotation;
		AtVector L_torot = L;
		AiM4RotationY(lightRotation, rot);
		AiM4VectorByMatrixMult(&L, lightRotation, &L_torot);

		// just in case the user scaled the light
		L = AiV3Normalize(L);

		float theta = sphericalTheta(L);

		if (theta > deg) {
			sg->u = sg->v = 0.f;
			return;
		}
		float phi = sphericalPhi(L);

		float c_u = tanf(theta) * sinf(phi);
		float c_v = tanf(theta) * cosf(phi);

		sg->u = (c_u + tanf(deg)) / tanf(deg) / 2;
		sg->v = (c_v + tanf(deg)) / tanf(deg) / 2;
	}

/////////////////

AI_SHADER_NODE_EXPORT_METHODS(PhotometricGobo);
    
node_loader
{
    if (i > 0)
        return false;
    
	node->methods = (AtNodeMethods*)PhotometricGobo;
    node->output_type = AI_TYPE_RGB;
    node->name = "photometric_gobo";
    node->node_type = AI_NODE_SHADER;
    strcpy(node->version, AI_VERSION);
    
    return true;
}
    
node_parameters
{
	AiParameterStr("filename", "J:/gobo/000202_L.jpg");
	AiParameterFlt("degree", 50);
	AiParameterFlt("rotation", 0);
}
    
node_initialize
{
	AtTextureHandle *handle = NULL;
	AiNodeSetLocalData(node, handle);
}
    
node_update
{
	AtTextureHandle *prev = (AtTextureHandle *)AiNodeGetLocalData(node);
	if (prev)
		AiTextureHandleDestroy(prev);

	AtTextureHandle *handle = AiTextureHandleCreate(AiNodeGetStr(node, "filename"));
	AiNodeSetLocalData(node, handle);
}
    
node_finish
{
	AtTextureHandle *handle = (AtTextureHandle *)AiNodeGetLocalData(node);
	if (handle)
		AiTextureHandleDestroy(handle);
}

shader_evaluate
{
	AtTextureHandle *handle = (AtTextureHandle *)AiNodeGetLocalData(node);
	bool success = false;
	AtTextureParams params;
	AiTextureParamsSetDefaults(&params);

	float orig_u = sg->u;
	float orig_v = sg->v;

	float deg = AiNodeGetFlt(node, "degree");
	float rot = AiNodeGetFlt(node, "rotation");

	computeUV(sg, deg * AI_PI / 180, rot);

	AtRGBA rgba = AiTextureHandleAccess(sg, handle, &params, &success);
	if (success)
	{
		
		if (rgba.r > 1e-3 || rgba.g > 1e-3 || rgba.b > 1e-3) {
			sg->Liu *= rgba.rgb();
		}
		else
			sg->Liu = AiColorCreate(0.f, 0.f, 0.f);
	}

	sg->u = orig_u;
	sg->v = orig_v;
}

}
