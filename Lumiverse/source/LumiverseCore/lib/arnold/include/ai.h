/*
 * Arnold API header file
 * Copyright (c) 1998-2009 Marcos Fajardo, (c) 2009-2013 Solid Angle SL
 */

/**
 * \file
 * Uber-include file, convenient way of making the entire Arnold API available
 * 
 * Note that the order of includes is _not_ random. For more information about
 * correct include ordering conventions:
 *    http://www.topology.org/linux/include.html
 */

#pragma once

// dso's
#include "ai_procedural.h"
#include "ai_plugins.h"

// node types
#include "ai_drivers.h"
#include "ai_driver_utils.h"
#include "ai_filters.h"
#include "ai_cameras.h"
#include "ai_shaders.h"
#include "ai_nodes.h"
#include "ai_params.h"
#include "ai_metadata.h"

// scene loading/saving
#include "ai_dotass.h"

// error/warning messages
#include "ai_msg.h"

// shader functions
#include "ai_sampler.h"
#include "ai_shader_aovs.h"
#include "ai_shader_brdf.h"
#include "ai_shaderglobals.h"
#include "ai_shader_lights.h"
#include "ai_shader_message.h"
#include "ai_shader_radiance.h"
#include "ai_shader_sample.h"
#include "ai_shader_sss.h"
#include "ai_shader_userdef.h"
#include "ai_shader_parameval.h"
#include "ai_shader_util.h"
#include "ai_shader_volume.h"
#include "ai_noise.h"
#include "ai_texture.h"

// ray tracing
#include "ai_ray.h"
#include "ai_render.h"

// core
#include "ai_unit_test.h"
#include "ai_universe.h"

// system
#include "ai_critsec.h"
#include "ai_threads.h"

// simple data types
#include "ai_array.h"
#include "ai_bbox.h"
#include "ai_matrix.h"
#include "ai_vector.h"
#include "ai_color.h"
#include "ai_enum.h"
#include "ai_types.h"
#include "ai_constants.h"

// version info
#include "ai_version.h"

// licensing
#include "ai_license.h"

// memory allocation
#include "ai_allocate.h"
