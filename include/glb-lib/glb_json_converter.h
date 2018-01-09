#pragma once

#include "glb_config.h"
#include "glb_output.h"

#include <stddef.h>

extern int glb_json_to_gsl(const char *json, size_t json_size,
                           size_t *handled, struct glbOutput *output);
