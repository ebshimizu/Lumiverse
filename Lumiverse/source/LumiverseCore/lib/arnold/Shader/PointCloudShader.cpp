
#include "PointCloudShader.h"

namespace {
    
inline static AtColor getSurfaceKd(std::string name) {
	size_t offset = name.find("_");
	AtColor result = AiColorCreate(0.f, 0.f, 0.f);
	int r, g, b;

	sscanf(name.c_str() + offset + 1, "%03d%03d%03d", &r, &g, &b);
	result.r = static_cast<float>(r) / 0xff;
	result.g = static_cast<float>(g) / 0xff;
	result.b = static_cast<float>(b) / 0xff;

	return result;
}
    
/////////////////////////////////
    
AI_SHADER_NODE_EXPORT_METHODS(PointCloudShaderMtd);
    
node_loader
{
    if (i > 0)
        return false;
    
	node->methods = (AtNodeMethods*)PointCloudShaderMtd;
    node->output_type = AI_TYPE_RGB;
    node->name = "point_cloud_shader";
    node->node_type = AI_NODE_SHADER;
    strcpy(node->version, AI_VERSION);
    
    return true;
}
    
node_parameters
{
	AiParameterFlt("roughness", 0.5f);
}
    
node_initialize
{
    
}
    
node_update
{

}
    
node_finish
{

}

shader_evaluate
{
	const AtParamValue *params = AiNodeGetParams(node);
	const char *name = AiNodeGetName(sg->Op);

	// Kd (diffuse color), Ks (specular color), and roughness (scalar)
	AtColor Kd = getSurfaceKd(std::string(name));
	float r = AiNodeGetFlt(node, "roughness");

	r = std::fmax(std::fmin(1.f, r), 0.f);

	// direct specular and diffuse accumulators, 
	// and indirect diffuse and specular accumulators...
	AtColor Dda, IDd;
	Dda = IDd = AI_RGB_BLACK;
	void *diff_data = AiOrenNayarMISCreateData(sg, r);
	AiLightsPrepare(sg);
	while (AiLightsGetSample(sg)) // loop over the lights to compute direct effects
	{
		// direct diffuse
		if (AiLightGetAffectDiffuse(sg->Lp))
			Dda += AiEvaluateLightSample(sg, diff_data, AiOrenNayarMISSample, AiOrenNayarMISBRDF, AiOrenNayarMISPDF);
	}
	// indirect diffuse
	IDd = AiOrenNayarIntegrate(&sg->Nf, sg, r);

	// add up indirect and direct contributions
	sg->out.RGB = Kd * (Dda + IDd);
}

}
