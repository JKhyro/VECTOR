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

  assert(vector_child_program_count() == 7u);
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

  assert(
    vector_child_program_at(2u, &descriptor) == VECTOR_CHILD_PROGRAM_STATUS_OK
  );
  assert(strcmp(descriptor.program_id, "vector-session-runtime-program") == 0);
  assert(descriptor.region == VECTOR_WORKSPACE_REGION_SESSION_RUNTIME);
  assert(
    (descriptor.flags &
      VECTOR_CHILD_PROGRAM_FLAG_NON_PERSISTENT_HELPER_SURFACE) != 0u
  );

  assert(
    vector_child_program_at(3u, &descriptor) == VECTOR_CHILD_PROGRAM_STATUS_OK
  );
  assert(strcmp(descriptor.program_id, "vector-tool-execution-program") == 0);
  assert(descriptor.primary_lane == VECTOR_DATA_LANE_CODEBASE);
  assert(
    (descriptor.flags & VECTOR_CHILD_PROGRAM_FLAG_REQUIRES_CORTEX_COMPONENT) !=
    0u
  );

  assert(
    vector_child_program_at(4u, &descriptor) == VECTOR_CHILD_PROGRAM_STATUS_OK
  );
  assert(strcmp(descriptor.program_id, "vector-approval-review-program") == 0);
  assert(descriptor.region == VECTOR_WORKSPACE_REGION_APPROVAL_REVIEW);

  assert(
    vector_child_program_at(5u, &descriptor) == VECTOR_CHILD_PROGRAM_STATUS_OK
  );
  assert(strcmp(descriptor.program_id, "vector-artifact-program") == 0);
  assert(descriptor.primary_lane == VECTOR_DATA_LANE_CODEBASE);

  assert(
    vector_child_program_at(6u, &descriptor) == VECTOR_CHILD_PROGRAM_STATUS_OK
  );
  assert(strcmp(descriptor.program_id, "vector-status-activity-program") == 0);
  assert(
    (descriptor.flags & VECTOR_CHILD_PROGRAM_FLAG_REQUIRES_CORTEX_COMPONENT) ==
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

static void test_session_runtime_routes_with_cortex_reference(void) {
  vector_child_program_route route;
  const vector_child_program_route_request request = {
    VECTOR_CHILD_PROGRAM_RUNTIME_ABI_VERSION,
    VECTOR_WORKSPACE_REGION_SESSION_RUNTIME,
    "resume-session",
    { "character:atlas", "component:memory", NULL }
  };

  assert(
    vector_child_program_route_region(&request, &route) ==
    VECTOR_CHILD_PROGRAM_STATUS_OK
  );
  assert(
    strcmp(route.descriptor.program_id, "vector-session-runtime-program") == 0
  );
  assert(route.descriptor.primary_lane == VECTOR_DATA_LANE_DEVBASE);
  assert(strcmp(route.cortex.component_id, "component:memory") == 0);
}

static void test_tool_execution_routes_with_cortex_reference(void) {
  vector_child_program_route route;
  const vector_child_program_route_request request = {
    VECTOR_CHILD_PROGRAM_RUNTIME_ABI_VERSION,
    VECTOR_WORKSPACE_REGION_TOOL_EXECUTION,
    "apply-patch",
    { "character:atlas", "component:active", "subagent:worker-1" }
  };

  assert(
    vector_child_program_route_region(&request, &route) ==
    VECTOR_CHILD_PROGRAM_STATUS_OK
  );
  assert(
    strcmp(route.descriptor.program_id, "vector-tool-execution-program") == 0
  );
  assert(route.descriptor.primary_lane == VECTOR_DATA_LANE_CODEBASE);
  assert(
    (route.descriptor.flags &
      VECTOR_CHILD_PROGRAM_FLAG_NON_PERSISTENT_HELPER_SURFACE) != 0u
  );
}

static void test_tool_execution_rejects_missing_cortex_reference(void) {
  vector_child_program_route route;
  const vector_child_program_route_request request = {
    VECTOR_CHILD_PROGRAM_RUNTIME_ABI_VERSION,
    VECTOR_WORKSPACE_REGION_TOOL_EXECUTION,
    "apply-patch",
    { NULL, "component:active", "subagent:worker-1" }
  };

  assert(
    vector_child_program_route_region(&request, &route) ==
    VECTOR_CHILD_PROGRAM_STATUS_MISSING_CORTEX_REFERENCE
  );
  assert(
    strcmp(route.descriptor.program_id, "vector-tool-execution-program") == 0
  );
}

static void test_helper_assignment_routes_to_session_runtime(void) {
  vector_helper_assignment assignment;
  const vector_helper_assignment_request request = {
    VECTOR_CHILD_PROGRAM_RUNTIME_ABI_VERSION,
    VECTOR_WORKSPACE_REGION_SESSION_RUNTIME,
    "resume-session",
    { "character:atlas", "component:memory", NULL },
    { "helper:retrieval", "manifest:codex-core", "codex-rs/core" }
  };

  assert(
    vector_child_program_assign_helper(&request, &assignment) ==
    VECTOR_CHILD_PROGRAM_STATUS_OK
  );
  assert(
    strcmp(
      assignment.descriptor.program_id,
      "vector-session-runtime-program"
    ) == 0
  );
  assert(strcmp(assignment.helper.helper_id, "helper:retrieval") == 0);
  assert(strcmp(assignment.helper.source_manifest_ref, "manifest:codex-core") == 0);
  assert(
    strcmp(assignment.helper.upstream_alias, "codex-rs/core") == 0
  );
}

static void test_helper_assignment_routes_to_tool_execution(void) {
  vector_helper_assignment assignment;
  const vector_helper_assignment_request request = {
    VECTOR_CHILD_PROGRAM_RUNTIME_ABI_VERSION,
    VECTOR_WORKSPACE_REGION_TOOL_EXECUTION,
    "apply-patch",
    { "character:atlas", "component:active", "subagent:worker-1" },
    { "helper:patcher", "manifest:codex-apply-patch", "codex-rs/apply-patch" }
  };

  assert(
    vector_child_program_assign_helper(&request, &assignment) ==
    VECTOR_CHILD_PROGRAM_STATUS_OK
  );
  assert(
    strcmp(assignment.descriptor.program_id, "vector-tool-execution-program") ==
    0
  );
  assert(strcmp(assignment.cortex.subagent_instance_id, "subagent:worker-1") == 0);
  assert(strcmp(assignment.helper.helper_id, "helper:patcher") == 0);
}

static void test_helper_assignment_rejects_left_menu(void) {
  vector_helper_assignment assignment;
  const vector_helper_assignment_request request = {
    VECTOR_CHILD_PROGRAM_RUNTIME_ABI_VERSION,
    VECTOR_WORKSPACE_REGION_LEFT_MENU,
    "open-workspace",
    { NULL, NULL, NULL },
    { "helper:navigation", "manifest:codex-app", "codex-rs/app-server" }
  };

  assert(
    vector_child_program_assign_helper(&request, &assignment) ==
    VECTOR_CHILD_PROGRAM_STATUS_HELPER_ASSIGNMENT_NOT_ALLOWED
  );
  assert(
    strcmp(
      vector_child_program_status_name(assignment.status),
      "helper_assignment_not_allowed"
    ) == 0
  );
}

static void test_helper_assignment_rejects_missing_helper(void) {
  vector_helper_assignment assignment;
  const vector_helper_assignment_request request = {
    VECTOR_CHILD_PROGRAM_RUNTIME_ABI_VERSION,
    VECTOR_WORKSPACE_REGION_TOOL_EXECUTION,
    "apply-patch",
    { "character:atlas", "component:active", NULL },
    { NULL, "manifest:codex-apply-patch", "codex-rs/apply-patch" }
  };

  assert(
    vector_child_program_assign_helper(&request, &assignment) ==
    VECTOR_CHILD_PROGRAM_STATUS_MISSING_HELPER_REFERENCE
  );
  assert(
    strcmp(
      vector_child_program_status_name(assignment.status),
      "missing_helper_reference"
    ) == 0
  );
}

static void test_helper_assignment_preserves_cortex_gate(void) {
  vector_helper_assignment assignment;
  const vector_helper_assignment_request request = {
    VECTOR_CHILD_PROGRAM_RUNTIME_ABI_VERSION,
    VECTOR_WORKSPACE_REGION_TOOL_EXECUTION,
    "apply-patch",
    { "character:atlas", NULL, NULL },
    { "helper:patcher", "manifest:codex-apply-patch", "codex-rs/apply-patch" }
  };

  assert(
    vector_child_program_assign_helper(&request, &assignment) ==
    VECTOR_CHILD_PROGRAM_STATUS_MISSING_CORTEX_REFERENCE
  );
  assert(
    strcmp(
      vector_child_program_status_name(assignment.status),
      "missing_cortex_reference"
    ) == 0
  );
}

static void test_approval_and_artifact_routes_require_cortex_reference(void) {
  vector_child_program_route route;
  const vector_child_program_route_request approval_request = {
    VECTOR_CHILD_PROGRAM_RUNTIME_ABI_VERSION,
    VECTOR_WORKSPACE_REGION_APPROVAL_REVIEW,
    "resolve-approval",
    { "character:atlas", "component:active", NULL }
  };
  const vector_child_program_route_request artifact_request = {
    VECTOR_CHILD_PROGRAM_RUNTIME_ABI_VERSION,
    VECTOR_WORKSPACE_REGION_ARTIFACT,
    "preview-patch",
    { "character:atlas", "component:active", "subagent:worker-1" }
  };
  const vector_child_program_route_request missing_cortex_request = {
    VECTOR_CHILD_PROGRAM_RUNTIME_ABI_VERSION,
    VECTOR_WORKSPACE_REGION_APPROVAL_REVIEW,
    "resolve-approval",
    { "character:atlas", NULL, NULL }
  };

  assert(
    vector_child_program_route_region(&approval_request, &route) ==
    VECTOR_CHILD_PROGRAM_STATUS_OK
  );
  assert(
    strcmp(route.descriptor.program_id, "vector-approval-review-program") == 0
  );

  assert(
    vector_child_program_route_region(&artifact_request, &route) ==
    VECTOR_CHILD_PROGRAM_STATUS_OK
  );
  assert(strcmp(route.descriptor.program_id, "vector-artifact-program") == 0);
  assert(route.descriptor.primary_lane == VECTOR_DATA_LANE_CODEBASE);

  assert(
    vector_child_program_route_region(&missing_cortex_request, &route) ==
    VECTOR_CHILD_PROGRAM_STATUS_MISSING_CORTEX_REFERENCE
  );
}

static void test_status_activity_routes_without_cortex_reference(void) {
  vector_child_program_route route;
  const vector_child_program_route_request request = {
    VECTOR_CHILD_PROGRAM_RUNTIME_ABI_VERSION,
    VECTOR_WORKSPACE_REGION_STATUS_ACTIVITY,
    "show-background-activity",
    { NULL, NULL, NULL }
  };

  assert(
    vector_child_program_route_region(&request, &route) ==
    VECTOR_CHILD_PROGRAM_STATUS_OK
  );
  assert(
    strcmp(route.descriptor.program_id, "vector-status-activity-program") == 0
  );
}

static void test_helper_assignment_rejects_artifact_program(void) {
  vector_helper_assignment assignment;
  const vector_helper_assignment_request request = {
    VECTOR_CHILD_PROGRAM_RUNTIME_ABI_VERSION,
    VECTOR_WORKSPACE_REGION_ARTIFACT,
    "preview-patch",
    { "character:atlas", "component:active", NULL },
    { "helper:patcher", "manifest:codex-apply-patch", "codex-rs/apply-patch" }
  };

  assert(
    vector_child_program_assign_helper(&request, &assignment) ==
    VECTOR_CHILD_PROGRAM_STATUS_HELPER_ASSIGNMENT_NOT_ALLOWED
  );
}

static void test_cortex_export_assigns_ready_tool_helper(void) {
  vector_helper_assignment assignment;
  const vector_cortex_export_assignment_request request = {
    VECTOR_CHILD_PROGRAM_RUNTIME_ABI_VERSION,
    VECTOR_WORKSPACE_REGION_TOOL_EXECUTION,
    "apply-patch",
    {
      { "character:atlas", "component:active", "subagent:worker-1" },
      {
        "helper:patcher",
        "manifest:codex-apply-patch",
        "codex-rs/apply-patch"
      },
      "ready",
      "active",
      "active",
      1u,
      1u,
      1u
    }
  };

  assert(
    vector_child_program_assign_cortex_export(&request, &assignment) ==
    VECTOR_CHILD_PROGRAM_STATUS_OK
  );
  assert(
    strcmp(
      assignment.descriptor.program_id,
      "vector-tool-execution-program"
    ) == 0
  );
  assert(strcmp(assignment.cortex.character_id, "character:atlas") == 0);
  assert(strcmp(assignment.helper.helper_id, "helper:patcher") == 0);
}

static void test_cortex_export_assigns_ready_session_helper(void) {
  vector_helper_assignment assignment;
  const vector_cortex_export_assignment_request request = {
    VECTOR_CHILD_PROGRAM_RUNTIME_ABI_VERSION,
    VECTOR_WORKSPACE_REGION_SESSION_RUNTIME,
    "resume-session",
    {
      { "character:atlas", "component:memory", NULL },
      { "helper:retrieval", "manifest:codex-core", "codex-rs/core" },
      "ready",
      "ready",
      NULL,
      1u,
      1u,
      1u
    }
  };

  assert(
    vector_child_program_assign_cortex_export(&request, &assignment) ==
    VECTOR_CHILD_PROGRAM_STATUS_OK
  );
  assert(
    strcmp(
      assignment.descriptor.program_id,
      "vector-session-runtime-program"
    ) == 0
  );
  assert(strcmp(assignment.cortex.component_id, "component:memory") == 0);
}

static void test_cortex_export_rejects_not_ready_state(void) {
  vector_helper_assignment assignment;
  const vector_cortex_export_assignment_request request = {
    VECTOR_CHILD_PROGRAM_RUNTIME_ABI_VERSION,
    VECTOR_WORKSPACE_REGION_TOOL_EXECUTION,
    "apply-patch",
    {
      { "character:atlas", "component:active", "subagent:worker-1" },
      {
        "helper:patcher",
        "manifest:codex-apply-patch",
        "codex-rs/apply-patch"
      },
      "draft",
      "active",
      "active",
      0u,
      1u,
      1u
    }
  };

  assert(
    vector_child_program_assign_cortex_export(&request, &assignment) ==
    VECTOR_CHILD_PROGRAM_STATUS_CORTEX_EXPORT_NOT_READY
  );
  assert(
    strcmp(
      vector_child_program_status_name(assignment.status),
      "cortex_export_not_ready"
    ) == 0
  );
  assert(strcmp(assignment.cortex.character_id, "character:atlas") == 0);
}

static void test_cortex_export_preserves_assignment_validation(void) {
  vector_helper_assignment assignment;
  const vector_cortex_export_assignment_request request = {
    VECTOR_CHILD_PROGRAM_RUNTIME_ABI_VERSION,
    VECTOR_WORKSPACE_REGION_ARTIFACT,
    "preview-patch",
    {
      { "character:atlas", "component:active", NULL },
      {
        "helper:patcher",
        "manifest:codex-apply-patch",
        "codex-rs/apply-patch"
      },
      "ready",
      "active",
      NULL,
      1u,
      1u,
      1u
    }
  };

  assert(
    vector_child_program_assign_cortex_export(&request, &assignment) ==
    VECTOR_CHILD_PROGRAM_STATUS_HELPER_ASSIGNMENT_NOT_ALLOWED
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
      vector_workspace_region_name(VECTOR_WORKSPACE_REGION_SESSION_RUNTIME),
      "session_runtime"
    ) == 0
  );
  assert(
    strcmp(
      vector_workspace_region_name(VECTOR_WORKSPACE_REGION_TOOL_EXECUTION),
      "tool_execution"
    ) == 0
  );
  assert(
    strcmp(
      vector_workspace_region_name(VECTOR_WORKSPACE_REGION_APPROVAL_REVIEW),
      "approval_review"
    ) == 0
  );
  assert(
    strcmp(vector_workspace_region_name(VECTOR_WORKSPACE_REGION_ARTIFACT), "artifact") ==
    0
  );
  assert(
    strcmp(
      vector_workspace_region_name(VECTOR_WORKSPACE_REGION_STATUS_ACTIVITY),
      "status_activity"
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
  test_session_runtime_routes_with_cortex_reference();
  test_tool_execution_routes_with_cortex_reference();
  test_tool_execution_rejects_missing_cortex_reference();
  test_helper_assignment_routes_to_session_runtime();
  test_helper_assignment_routes_to_tool_execution();
  test_helper_assignment_rejects_left_menu();
  test_helper_assignment_rejects_missing_helper();
  test_helper_assignment_preserves_cortex_gate();
  test_approval_and_artifact_routes_require_cortex_reference();
  test_status_activity_routes_without_cortex_reference();
  test_helper_assignment_rejects_artifact_program();
  test_cortex_export_assigns_ready_tool_helper();
  test_cortex_export_assigns_ready_session_helper();
  test_cortex_export_rejects_not_ready_state();
  test_cortex_export_preserves_assignment_validation();
  test_unsupported_abi_rejected();
  test_null_and_unknown_inputs();
  test_name_helpers();
  return 0;
}
