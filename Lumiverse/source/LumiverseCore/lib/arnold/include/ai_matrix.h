/*
 * Arnold API header file
 * Copyright (c) 1998-2009 Marcos Fajardo, (c) 2009-2013 Solid Angle SL
 */

/**
 * \file
 * Matrix math type and methods
 */

#pragma once
#include "ai_vector.h"
#include "ai_api.h"

/** \defgroup ai_matrix AtMatrix API
 * \{
 */

/** 4-by-4 matrix */
typedef float AtMatrix[4][4];

AI_API void    AiM4Identity(AtMatrix mout);
AI_API void    AiM4Translation(AtMatrix mout, const AtVector* t);
AI_API void    AiM4RotationX(AtMatrix mout, float x);
AI_API void    AiM4RotationY(AtMatrix mout, float y);
AI_API void    AiM4RotationZ(AtMatrix mout, float z);
AI_API void    AiM4Scaling(AtMatrix mout, const AtVector* s);
AI_API void    AiM4Frame(AtMatrix mout, const AtVector* o, const AtVector* u, const AtVector* v, const AtVector* w);
AI_API void    AiM4PointByMatrixMult(AtPoint* pout, const AtMatrix m, const AtPoint* pin);
AI_API void    AiM4HPointByMatrixMult(AtHPoint* pout, const AtMatrix m, const AtHPoint* pin);
AI_API void    AiM4VectorByMatrixMult(AtVector* vout, const AtMatrix m, const AtVector* vin);
AI_API void    AiM4VectorByMatrixTMult(AtVector* vout, const AtMatrix m, const AtVector* vin);
AI_API void    AiM4Mult(AtMatrix mout, const AtMatrix ma, const AtMatrix mb);
AI_API void    AiM4Copy(AtMatrix dest, const AtMatrix src);
AI_API void    AiM4Transpose(const AtMatrix min, AtMatrix mout);
AI_API void    AiM4Invert(const AtMatrix min, AtMatrix mout);
AI_API float   AiM4Determinant(const AtMatrix m);
AI_API void    AiM4Lerp(AtMatrix mout, float t, const AtMatrix ma, const AtMatrix mb);
AI_API void    AiM4Berp(float bu, float bv, const AtMatrix m0, const AtMatrix m1, const AtMatrix m2, AtMatrix mout);
AI_API bool    AiM4IsIdentity(const AtMatrix m);
AI_API bool    AiM4IsSingular(const AtMatrix m);

AI_API AtMatrix AI_M4_IDENTITY;
AI_API AtMatrix AI_M4_ZERO;

/*\}*/
