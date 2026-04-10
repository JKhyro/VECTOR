#ifndef VECTOR_CHILD_PROGRAM_RUNTIME_H
#define VECTOR_CHILD_PROGRAM_RUNTIME_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define VECTOR_CHILD_PROGRAM_RUNTIME_ABI_VERSION 1u
#define VECTOR_CHILD_PROGRAM_RUNTIME_SCHEMA_VERSION 1u

typedef enum vector_child_program_status {
  VECTOR_CHILD_PROGRAM_STATUS_OK = 0,
  VECTOR_CHILD_PROGRAM_STATUS_NULL_ARGUMENT = 1,
  VECTOR_CHILD_PROGRAM_STATUS_UNSUPPORTED_ABI = 2,
  VECTOR_CHILD_PROGRAM_STATUS_UNKNOWN_REGION = 3,
  VECTOR_CHILD_PROGRAM_STATUS_MISSING_CORTEX_REFERENCE = 4
} vector_child_program_status;

typedef enum vector_workspace_region {
  VECTOR_WORKSPACE_REGION_LEFT_MENU = 1,
  VECTOR_WORKSPACE_REGION_MAIN_INTERACTION = 2
} vector_workspace_region;

typedef enum vector_child_program_kind {
  VECTOR_CHILD_PROGRAM_KIND_MAIN_PROGRAM = 1,
  VECTOR_CHILD_PROGRAM_KIND_LEFT_MENU = 2,
  VECTOR_CHILD_PROGRAM_KIND_MAIN_INTERACTION = 3
} vector_child_program_kind;

typedef enum vector_data_lane {
  VECTOR_DATA_LANE_NONE = 0,
  VECTOR_DATA_LANE_FLUXBASE = 1,
  VECTOR_DATA_LANE_DEVBASE = 2,
  VECTOR_DATA_LANE_CODEBASE = 3,
  VECTOR_DATA_LANE_CHATBASE = 4
} vector_data_lane;

typedef enum vector_child_program_flags {
  VECTOR_CHILD_PROGRAM_FLAG_FIRST_CLASS = 1u << 0,
  VECTOR_CHILD_PROGRAM_FLAG_REQUIRES_CORTEX_COMPONENT = 1u << 1,
  VECTOR_CHILD_PROGRAM_FLAG_UPSTREAM_CODEX_ASSIMILATION = 1u << 2
} vector_child_program_flags;

typedef struct vector_cortex_reference {
  const char *character_id;
  const char *component_id;
  const char *subagent_instance_id;
} vector_cortex_reference;

typedef struct vector_child_program_descriptor {
  uint32_t abi_version;
  uint32_t schema_version;
  const char *program_id;
  const char *display_label;
  vector_workspace_region region;
  vector_child_program_kind kind;
  vector_data_lane primary_lane;
  uint32_t flags;
  const char *assimilation_source;
} vector_child_program_descriptor;

typedef struct vector_child_program_route_request {
  uint32_t abi_version;
  vector_workspace_region region;
  const char *operation_id;
  vector_cortex_reference cortex;
} vector_child_program_route_request;

typedef struct vector_child_program_route {
  uint32_t abi_version;
  uint32_t schema_version;
  vector_child_program_status status;
  vector_child_program_descriptor descriptor;
  vector_cortex_reference cortex;
  const char *status_message;
} vector_child_program_route;

uint32_t vector_child_program_runtime_abi_version(void);
uint32_t vector_child_program_runtime_schema_version(void);

size_t vector_child_program_count(void);

vector_child_program_status vector_child_program_at(
  size_t index,
  vector_child_program_descriptor *out_descriptor
);

vector_child_program_status vector_child_program_route_region(
  const vector_child_program_route_request *request,
  vector_child_program_route *out_route
);

const char *vector_child_program_status_name(vector_child_program_status status);
const char *vector_data_lane_name(vector_data_lane lane);
const char *vector_workspace_region_name(vector_workspace_region region);

#ifdef __cplusplus
}
#endif

#endif
