#include "vector/child_program_runtime.h"

#include <assert.h>
#include <stddef.h>
#include <string.h>

static void test_version_contract(void) {
  assert(
    vector_child_program_runtime_abi_version() ==
    VECTOR_CHILD_PROGRAM_RUNTIME_ABI_VERSION
  );
  assert(
    vector_child_program_runtime_schema_version() ==
    VECTOR_CHILD_PROGRAM_RUNTIME_SCHEMA_VERSION
  );
}

static void test_descriptor_table(void) {
  vector_child_program_descriptor descriptor;

  assert(vector_child_program_count() == 2u);
  assert(
    vector_child_program_at(0u, &descriptor) == VECTOR_CHILD_PROGRAM_STATUS_OK
  );
  assert(strcmp(descriptor.program_id, "vector-left-menu-program") == 0);
  assert(descriptor.region == VECTOR_WORKSPACE_REGION_LEFT_MENU);
  assert(descriptor.primary_lane == VECTOR_DATA_LANE_DEVBASE);

  assert(
    vector_child_program_at(1u, &descriptor) == VECTOR_CHILD_PROGRAM_STATUS_OK
  );
  assert(strcmp(descriptor.program_id, "vector-main-interaction-program") == 0);
  assert(descriptor.region == VECTOR_WORKSPACE_REGION_MAIN_INTERACTION);
  assert(
    (descriptor.flags & VECTOR_CHILD_PROGRAM_FLAG_REQUIRES_CORTEX_COMPONENT) !=
    0u
  );
}

static void test_left_menu_routes_without_cortex_reference(void) {
  vector_child_program_route route;
  const vector_child_program_route_request request = {
    VECTOR_CHILD_PROGRAM_RUNTIME_ABI_VERSION,
    VECTOR_WORKSPACE_REGION_LEFT_MENU,
    "open-workspace",
    { 0 }
  };

  assert(
    vector_child_program_route_region(&request, &route) ==
    VECTOR_CHILD_PROGRAM_STATUS_OK
  );
  assert(route.status == VECTOR_CHILD_PROGRAM_STATUS_OK);
  assert(strcmp(route.descriptor.program_id, "vector-left-menu-program") == 0);
  assert(route.cortex.character_id == NULL);
}

static void test_main_interaction_requires_cortex_reference(void) {
  vector_child_program_route route;
  const vector_child_program_route_request request = {
    VECTOR_CHILD_PROGRAM_RUNTIME_ABI_VERSION,
    VECTOR_WORKSPACE_REGION_MAIN_INTERACTION,
    "render-turn",
    { "character:atlas", "component:active", "subagent:worker-1" }
  };

  assert(
    vector_child_program_route_region(&request, &route) ==
    VECTOR_CHILD_PROGRAM_STATUS_OK
  );
  assert(route.status == VECTOR_CHILD_PROGRAM_STATUS_OK);
  assert(
    strcmp(route.descriptor.program_id, "vector-main-interaction-program") == 0
  );
  assert(strcmp(route.cortex.character_id, "character:atlas") == 0);
  assert(strcmp(route.cortex.component_id, "component:active") == 0);
  assert(strcmp(route.cortex.subagent_instance_id, "subagent:worker-1") == 0);
}

static void test_main_interaction_rejects_missing_cortex_reference(void) {
  vector_child_program_route route;
  const vector_child_program_route_request request = {
    VECTOR_CHILD_PROGRAM_RUNTIME_ABI_VERSION,
    VECTOR_WORKSPACE_REGION_MAIN_INTERACTION,
    "render-turn",
    { "character:atlas", NULL, NULL }
  };

  assert(
    vector_child_program_route_region(&request, &route) ==
    VECTOR_CHILD_PROGRAM_STATUS_MISSING_CORTEX_REFERENCE
  );
  assert(route.status == VECTOR_CHILD_PROGRAM_STATUS_MISSING_CORTEX_REFERENCE);
  assert(
    strcmp(
      vector_child_program_status_name(route.status),
      "missing_cortex_reference"
    ) == 0
  );
}

static void test_unsupported_abi_rejected(void) {
  vector_child_program_route route;
  const vector_child_program_route_request request = {
    VECTOR_CHILD_PROGRAM_RUNTIME_ABI_VERSION + 1u,
    VECTOR_WORKSPACE_REGION_LEFT_MENU,
    "open-workspace",
    { 0 }
  };

  assert(
    vector_child_program_route_region(&request, &route) ==
    VECTOR_CHILD_PROGRAM_STATUS_UNSUPPORTED_ABI
  );
  assert(route.status == VECTOR_CHILD_PROGRAM_STATUS_UNSUPPORTED_ABI);
  assert(strcmp(vector_child_program_status_name(route.status), "unsupported_abi") == 0);
}

static void test_null_and_unknown_inputs(void) {
  vector_child_program_route route;
  vector_child_program_descriptor descriptor;
  const vector_child_program_route_request request = {
    VECTOR_CHILD_PROGRAM_RUNTIME_ABI_VERSION,
    (vector_workspace_region)99,
    "open-workspace",
    { 0 }
  };

  assert(
    vector_child_program_at(99u, &descriptor) ==
    VECTOR_CHILD_PROGRAM_STATUS_UNKNOWN_REGION
  );
  assert(
    vector_child_program_at(0u, NULL) ==
    VECTOR_CHILD_PROGRAM_STATUS_NULL_ARGUMENT
  );
  assert(
    vector_child_program_route_region(NULL, &route) ==
    VECTOR_CHILD_PROGRAM_STATUS_NULL_ARGUMENT
  );
  assert(route.status == VECTOR_CHILD_PROGRAM_STATUS_NULL_ARGUMENT);
  assert(strcmp(route.status_message, "null argument") == 0);
  assert(
    vector_child_program_route_region(&request, &route) ==
    VECTOR_CHILD_PROGRAM_STATUS_UNKNOWN_REGION
  );
  assert(strcmp(vector_workspace_region_name(request.region), "unknown_region") == 0);
}

static void test_name_helpers(void) {
  assert(strcmp(vector_data_lane_name(VECTOR_DATA_LANE_DEVBASE), "devbase") == 0);
  assert(
    strcmp(
      vector_workspace_region_name(VECTOR_WORKSPACE_REGION_MAIN_INTERACTION),
      "main_interaction"
    ) == 0
  );
  assert(
    strcmp(
      vector_child_program_status_name((vector_child_program_status)99),
      "unknown_status"
    ) == 0
  );
}

int main(void) {
  test_version_contract();
  test_descriptor_table();
  test_left_menu_routes_without_cortex_reference();
  test_main_interaction_requires_cortex_reference();
  test_main_interaction_rejects_missing_cortex_reference();
  test_unsupported_abi_rejected();
  test_null_and_unknown_inputs();
  test_name_helpers();
  return 0;
}
