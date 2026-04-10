#include "vector/child_program_runtime.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VECTOR_STATE_LINE_CAPACITY 1024u

static const vector_child_program_descriptor k_child_programs[] = {
  {
    VECTOR_CHILD_PROGRAM_RUNTIME_ABI_VERSION,
    VECTOR_CHILD_PROGRAM_RUNTIME_SCHEMA_VERSION,
    "vector-left-menu-program",
    "Left-side menu",
    VECTOR_WORKSPACE_REGION_LEFT_MENU,
    VECTOR_CHILD_PROGRAM_KIND_LEFT_MENU,
    VECTOR_DATA_LANE_DEVBASE,
    VECTOR_CHILD_PROGRAM_FLAG_FIRST_CLASS,
    "openai/codex app navigation and workspace selection behavior"
  },
  {
    VECTOR_CHILD_PROGRAM_RUNTIME_ABI_VERSION,
    VECTOR_CHILD_PROGRAM_RUNTIME_SCHEMA_VERSION,
    "vector-main-interaction-program",
    "Main interaction",
    VECTOR_WORKSPACE_REGION_MAIN_INTERACTION,
    VECTOR_CHILD_PROGRAM_KIND_MAIN_INTERACTION,
    VECTOR_DATA_LANE_DEVBASE,
    VECTOR_CHILD_PROGRAM_FLAG_FIRST_CLASS |
      VECTOR_CHILD_PROGRAM_FLAG_REQUIRES_CORTEX_COMPONENT |
      VECTOR_CHILD_PROGRAM_FLAG_UPSTREAM_CODEX_ASSIMILATION,
    "openai/codex session, turn, approval, artifact, and tool interaction behavior"
  },
  {
    VECTOR_CHILD_PROGRAM_RUNTIME_ABI_VERSION,
    VECTOR_CHILD_PROGRAM_RUNTIME_SCHEMA_VERSION,
    "vector-session-runtime-program",
    "Session runtime",
    VECTOR_WORKSPACE_REGION_SESSION_RUNTIME,
    VECTOR_CHILD_PROGRAM_KIND_SESSION_RUNTIME,
    VECTOR_DATA_LANE_DEVBASE,
    VECTOR_CHILD_PROGRAM_FLAG_FIRST_CLASS |
      VECTOR_CHILD_PROGRAM_FLAG_REQUIRES_CORTEX_COMPONENT |
      VECTOR_CHILD_PROGRAM_FLAG_UPSTREAM_CODEX_ASSIMILATION |
      VECTOR_CHILD_PROGRAM_FLAG_NON_PERSISTENT_HELPER_SURFACE,
    "openai/codex core session lifecycle and resumable turn behavior"
  },
  {
    VECTOR_CHILD_PROGRAM_RUNTIME_ABI_VERSION,
    VECTOR_CHILD_PROGRAM_RUNTIME_SCHEMA_VERSION,
    "vector-tool-execution-program",
    "Tool execution",
    VECTOR_WORKSPACE_REGION_TOOL_EXECUTION,
    VECTOR_CHILD_PROGRAM_KIND_TOOL_EXECUTION,
    VECTOR_DATA_LANE_CODEBASE,
    VECTOR_CHILD_PROGRAM_FLAG_FIRST_CLASS |
      VECTOR_CHILD_PROGRAM_FLAG_REQUIRES_CORTEX_COMPONENT |
      VECTOR_CHILD_PROGRAM_FLAG_UPSTREAM_CODEX_ASSIMILATION |
      VECTOR_CHILD_PROGRAM_FLAG_NON_PERSISTENT_HELPER_SURFACE,
    "openai/codex exec, apply-patch, file-search, and governed tool behavior"
  },
  {
    VECTOR_CHILD_PROGRAM_RUNTIME_ABI_VERSION,
    VECTOR_CHILD_PROGRAM_RUNTIME_SCHEMA_VERSION,
    "vector-approval-review-program",
    "Approval review",
    VECTOR_WORKSPACE_REGION_APPROVAL_REVIEW,
    VECTOR_CHILD_PROGRAM_KIND_APPROVAL_REVIEW,
    VECTOR_DATA_LANE_DEVBASE,
    VECTOR_CHILD_PROGRAM_FLAG_FIRST_CLASS |
      VECTOR_CHILD_PROGRAM_FLAG_REQUIRES_CORTEX_COMPONENT |
      VECTOR_CHILD_PROGRAM_FLAG_UPSTREAM_CODEX_ASSIMILATION,
    "openai/codex approval request and operator confirmation behavior"
  },
  {
    VECTOR_CHILD_PROGRAM_RUNTIME_ABI_VERSION,
    VECTOR_CHILD_PROGRAM_RUNTIME_SCHEMA_VERSION,
    "vector-artifact-program",
    "Artifact",
    VECTOR_WORKSPACE_REGION_ARTIFACT,
    VECTOR_CHILD_PROGRAM_KIND_ARTIFACT,
    VECTOR_DATA_LANE_CODEBASE,
    VECTOR_CHILD_PROGRAM_FLAG_FIRST_CLASS |
      VECTOR_CHILD_PROGRAM_FLAG_REQUIRES_CORTEX_COMPONENT |
      VECTOR_CHILD_PROGRAM_FLAG_UPSTREAM_CODEX_ASSIMILATION,
    "openai/codex patch, diff, generated file, and preview artifact behavior"
  },
  {
    VECTOR_CHILD_PROGRAM_RUNTIME_ABI_VERSION,
    VECTOR_CHILD_PROGRAM_RUNTIME_SCHEMA_VERSION,
    "vector-status-activity-program",
    "Status and activity",
    VECTOR_WORKSPACE_REGION_STATUS_ACTIVITY,
    VECTOR_CHILD_PROGRAM_KIND_STATUS_ACTIVITY,
    VECTOR_DATA_LANE_DEVBASE,
    VECTOR_CHILD_PROGRAM_FLAG_FIRST_CLASS |
      VECTOR_CHILD_PROGRAM_FLAG_UPSTREAM_CODEX_ASSIMILATION,
    "openai/codex progress, diagnostics, background activity, and notification behavior"
  }
};

static const size_t k_child_program_count =
  sizeof(k_child_programs) / sizeof(k_child_programs[0]);

static int is_blank(const char *value) {
  return value == NULL || value[0] == '\0';
}

static int copy_owned(char *target, size_t capacity, const char *source) {
  size_t length = source == NULL ? 0u : strlen(source);

  if (target == NULL || capacity == 0u || length >= capacity) {
    return 0;
  }

  if (source == NULL) {
    target[0] = '\0';
  } else {
    memcpy(target, source, length + 1u);
  }

  return 1;
}

static void strip_line_endings(char *value) {
  size_t length = 0u;

  if (value == NULL) {
    return;
  }

  length = strlen(value);
  while (length > 0u &&
         (value[length - 1u] == '\r' || value[length - 1u] == '\n')) {
    value[length - 1u] = '\0';
    --length;
  }
}

static int parse_size_field(const char *value, size_t *out_value) {
  char *end = NULL;
  unsigned long parsed = 0u;

  if (is_blank(value) || out_value == NULL) {
    return 0;
  }

  parsed = strtoul(value, &end, 10);
  if (end == value || *end != '\0') {
    return 0;
  }

  *out_value = (size_t)parsed;
  return 1;
}

static size_t split_tabs(char *line, char *fields[], size_t field_capacity) {
  size_t count = 0u;
  char *cursor = line;

  while (count < field_capacity) {
    fields[count++] = cursor;
    cursor = strchr(cursor, '\t');
    if (cursor == NULL) {
      break;
    }
    *cursor = '\0';
    ++cursor;
  }

  return count;
}

static void reset_route(vector_child_program_route *route) {
  if (route != NULL) {
    memset(route, 0, sizeof(*route));
    route->abi_version = VECTOR_CHILD_PROGRAM_RUNTIME_ABI_VERSION;
    route->schema_version = VECTOR_CHILD_PROGRAM_RUNTIME_SCHEMA_VERSION;
  }
}

static void reset_assignment(vector_helper_assignment *assignment) {
  if (assignment != NULL) {
    memset(assignment, 0, sizeof(*assignment));
    assignment->abi_version = VECTOR_CHILD_PROGRAM_RUNTIME_ABI_VERSION;
    assignment->schema_version = VECTOR_CHILD_PROGRAM_RUNTIME_SCHEMA_VERSION;
  }
}

static void reset_owned_assignment(vector_owned_helper_assignment *assignment) {
  if (assignment != NULL) {
    memset(assignment, 0, sizeof(*assignment));
    assignment->abi_version = VECTOR_CHILD_PROGRAM_RUNTIME_ABI_VERSION;
    assignment->schema_version = VECTOR_CHILD_PROGRAM_RUNTIME_SCHEMA_VERSION;
  }
}

static const vector_child_program_descriptor *find_descriptor(
  vector_workspace_region region
) {
  size_t index = 0u;

  for (index = 0u; index < k_child_program_count; ++index) {
    if (k_child_programs[index].region == region) {
      return &k_child_programs[index];
    }
  }

  return NULL;
}

uint32_t vector_child_program_runtime_abi_version(void) {
  return VECTOR_CHILD_PROGRAM_RUNTIME_ABI_VERSION;
}

uint32_t vector_child_program_runtime_schema_version(void) {
  return VECTOR_CHILD_PROGRAM_RUNTIME_SCHEMA_VERSION;
}

size_t vector_child_program_count(void) {
  return k_child_program_count;
}

vector_child_program_status vector_child_program_at(
  size_t index,
  vector_child_program_descriptor *out_descriptor
) {
  if (out_descriptor == NULL) {
    return VECTOR_CHILD_PROGRAM_STATUS_NULL_ARGUMENT;
  }

  memset(out_descriptor, 0, sizeof(*out_descriptor));

  if (index >= k_child_program_count) {
    return VECTOR_CHILD_PROGRAM_STATUS_UNKNOWN_REGION;
  }

  *out_descriptor = k_child_programs[index];
  return VECTOR_CHILD_PROGRAM_STATUS_OK;
}

vector_child_program_status vector_child_program_route_region(
  const vector_child_program_route_request *request,
  vector_child_program_route *out_route
) {
  const vector_child_program_descriptor *descriptor = NULL;

  if (out_route == NULL) {
    return VECTOR_CHILD_PROGRAM_STATUS_NULL_ARGUMENT;
  }

  reset_route(out_route);

  if (request == NULL) {
    out_route->status = VECTOR_CHILD_PROGRAM_STATUS_NULL_ARGUMENT;
    out_route->status_message = "null argument";
    return out_route->status;
  }

  if (request->abi_version != VECTOR_CHILD_PROGRAM_RUNTIME_ABI_VERSION) {
    out_route->status = VECTOR_CHILD_PROGRAM_STATUS_UNSUPPORTED_ABI;
    out_route->status_message = "unsupported ABI version";
    return out_route->status;
  }

  descriptor = find_descriptor(request->region);
  if (descriptor == NULL) {
    out_route->status = VECTOR_CHILD_PROGRAM_STATUS_UNKNOWN_REGION;
    out_route->status_message = "workspace region is not routed to a child program";
    return out_route->status;
  }

  out_route->descriptor = *descriptor;
  out_route->cortex = request->cortex;

  if (
    (descriptor->flags & VECTOR_CHILD_PROGRAM_FLAG_REQUIRES_CORTEX_COMPONENT) !=
    0u
  ) {
    if (
      is_blank(request->cortex.character_id) ||
      is_blank(request->cortex.component_id)
    ) {
      out_route->status = VECTOR_CHILD_PROGRAM_STATUS_MISSING_CORTEX_REFERENCE;
      out_route->status_message =
        "routed child program requires CORTEX character and component references";
      return out_route->status;
    }
  }

  out_route->status = VECTOR_CHILD_PROGRAM_STATUS_OK;
  out_route->status_message = "routed";
  return out_route->status;
}

vector_child_program_status vector_child_program_assign_helper(
  const vector_helper_assignment_request *request,
  vector_helper_assignment *out_assignment
) {
  vector_child_program_route route;
  vector_child_program_route_request route_request;
  vector_child_program_status status = VECTOR_CHILD_PROGRAM_STATUS_OK;

  if (out_assignment == NULL) {
    return VECTOR_CHILD_PROGRAM_STATUS_NULL_ARGUMENT;
  }

  reset_assignment(out_assignment);

  if (request == NULL) {
    out_assignment->status = VECTOR_CHILD_PROGRAM_STATUS_NULL_ARGUMENT;
    out_assignment->status_message = "null argument";
    return out_assignment->status;
  }

  if (request->abi_version != VECTOR_CHILD_PROGRAM_RUNTIME_ABI_VERSION) {
    out_assignment->status = VECTOR_CHILD_PROGRAM_STATUS_UNSUPPORTED_ABI;
    out_assignment->status_message = "unsupported ABI version";
    return out_assignment->status;
  }

  if (is_blank(request->helper.helper_id)) {
    out_assignment->status = VECTOR_CHILD_PROGRAM_STATUS_MISSING_HELPER_REFERENCE;
    out_assignment->status_message =
      "helper assignment requires an imported helper reference";
    return out_assignment->status;
  }

  memset(&route_request, 0, sizeof(route_request));
  route_request.abi_version = request->abi_version;
  route_request.region = request->region;
  route_request.operation_id = request->operation_id;
  route_request.cortex = request->cortex;

  status = vector_child_program_route_region(&route_request, &route);
  out_assignment->status = status;
  out_assignment->descriptor = route.descriptor;
  out_assignment->cortex = route.cortex;
  out_assignment->helper = request->helper;

  if (status != VECTOR_CHILD_PROGRAM_STATUS_OK) {
    out_assignment->status_message = route.status_message;
    return out_assignment->status;
  }

  if (
    (route.descriptor.flags &
      VECTOR_CHILD_PROGRAM_FLAG_NON_PERSISTENT_HELPER_SURFACE) == 0u
  ) {
    out_assignment->status =
      VECTOR_CHILD_PROGRAM_STATUS_HELPER_ASSIGNMENT_NOT_ALLOWED;
    out_assignment->status_message =
      "routed child program does not accept imported helper assignment";
    return out_assignment->status;
  }

  out_assignment->status = VECTOR_CHILD_PROGRAM_STATUS_OK;
  out_assignment->status_message = "helper assigned";
  return out_assignment->status;
}

vector_child_program_status vector_child_program_assign_cortex_export(
  const vector_cortex_export_assignment_request *request,
  vector_helper_assignment *out_assignment
) {
  vector_helper_assignment_request assignment_request;

  if (out_assignment == NULL) {
    return VECTOR_CHILD_PROGRAM_STATUS_NULL_ARGUMENT;
  }

  reset_assignment(out_assignment);

  if (request == NULL) {
    out_assignment->status = VECTOR_CHILD_PROGRAM_STATUS_NULL_ARGUMENT;
    out_assignment->status_message = "null argument";
    return out_assignment->status;
  }

  if (request->abi_version != VECTOR_CHILD_PROGRAM_RUNTIME_ABI_VERSION) {
    out_assignment->status = VECTOR_CHILD_PROGRAM_STATUS_UNSUPPORTED_ABI;
    out_assignment->status_message = "unsupported ABI version";
    return out_assignment->status;
  }

  if (
    request->runtime_export.character_ready == 0u ||
    request->runtime_export.component_ready == 0u ||
    request->runtime_export.helper_can_bind == 0u
  ) {
    out_assignment->status =
      VECTOR_CHILD_PROGRAM_STATUS_CORTEX_EXPORT_NOT_READY;
    out_assignment->cortex = request->runtime_export.cortex;
    out_assignment->helper = request->runtime_export.helper;
    out_assignment->status_message =
      "CORTEX runtime export is not ready for VECTOR helper assignment";
    return out_assignment->status;
  }

  memset(&assignment_request, 0, sizeof(assignment_request));
  assignment_request.abi_version = request->abi_version;
  assignment_request.region = request->region;
  assignment_request.operation_id = request->operation_id;
  assignment_request.cortex = request->runtime_export.cortex;
  assignment_request.helper = request->runtime_export.helper;

  return vector_child_program_assign_helper(&assignment_request, out_assignment);
}

vector_child_program_status vector_child_program_assign_cortex_state_file(
  const vector_cortex_state_assignment_request *request,
  vector_owned_helper_assignment *out_assignment
) {
  FILE *file = NULL;
  char line[VECTOR_STATE_LINE_CAPACITY];
  char *fields[16];
  size_t component_index = 0u;
  size_t selected_component_index = (size_t)-1;
  vector_cortex_export_assignment_request export_request;
  vector_helper_assignment assignment;

  if (out_assignment == NULL) {
    return VECTOR_CHILD_PROGRAM_STATUS_NULL_ARGUMENT;
  }

  reset_owned_assignment(out_assignment);

  if (request == NULL) {
    out_assignment->status = VECTOR_CHILD_PROGRAM_STATUS_NULL_ARGUMENT;
    copy_owned(
      out_assignment->status_message,
      sizeof(out_assignment->status_message),
      "null argument"
    );
    return out_assignment->status;
  }

  if (request->abi_version != VECTOR_CHILD_PROGRAM_RUNTIME_ABI_VERSION) {
    out_assignment->status = VECTOR_CHILD_PROGRAM_STATUS_UNSUPPORTED_ABI;
    copy_owned(
      out_assignment->status_message,
      sizeof(out_assignment->status_message),
      "unsupported ABI version"
    );
    return out_assignment->status;
  }

  if (is_blank(request->state_path)) {
    out_assignment->status = VECTOR_CHILD_PROGRAM_STATUS_CORTEX_STATE_IO;
    copy_owned(
      out_assignment->status_message,
      sizeof(out_assignment->status_message),
      "CORTEX state path is required"
    );
    return out_assignment->status;
  }

#if defined(_MSC_VER)
  if (fopen_s(&file, request->state_path, "rb") != 0) {
    file = NULL;
  }
#else
  file = fopen(request->state_path, "rb");
#endif

  if (file == NULL) {
    out_assignment->status = VECTOR_CHILD_PROGRAM_STATUS_CORTEX_STATE_IO;
    copy_owned(
      out_assignment->status_message,
      sizeof(out_assignment->status_message),
      "CORTEX state file could not be opened"
    );
    return out_assignment->status;
  }

  while (fgets(line, sizeof(line), file) != NULL) {
    strip_line_endings(line);
    if (line[0] == '\0') {
      continue;
    }

    if (line[0] == 'A' && line[1] == '\t') {
      size_t count = split_tabs(line, fields, 16u);
      size_t can_bind = 0u;
      if (count == 16u &&
          parse_size_field(fields[15], &can_bind) &&
          can_bind == 1u &&
          out_assignment->helper_id[0] == '\0') {
        if (!copy_owned(out_assignment->helper_id,
                        sizeof(out_assignment->helper_id), fields[7]) ||
            !copy_owned(out_assignment->source_manifest_ref,
                        sizeof(out_assignment->source_manifest_ref), fields[5]) ||
            !copy_owned(out_assignment->upstream_alias,
                        sizeof(out_assignment->upstream_alias), fields[3])) {
          (void)fclose(file);
          out_assignment->status =
            VECTOR_CHILD_PROGRAM_STATUS_CORTEX_STATE_PARSE_FAILED;
          copy_owned(
            out_assignment->status_message,
            sizeof(out_assignment->status_message),
            "CORTEX audit values exceed VECTOR capacity"
          );
          return out_assignment->status;
        }
      }
    } else if (line[0] == 'C' && line[1] == '\t') {
      size_t count = split_tabs(line, fields, 7u);
      size_t lifecycle_state = 0u;
      if (count == 7u &&
          parse_size_field(fields[4], &lifecycle_state) &&
          lifecycle_state >= 3u &&
          out_assignment->character_id[0] == '\0') {
        if (!copy_owned(out_assignment->character_id,
                        sizeof(out_assignment->character_id), fields[1])) {
          (void)fclose(file);
          out_assignment->status =
            VECTOR_CHILD_PROGRAM_STATUS_CORTEX_STATE_PARSE_FAILED;
          copy_owned(
            out_assignment->status_message,
            sizeof(out_assignment->status_message),
            "CORTEX character id exceeds VECTOR capacity"
          );
          return out_assignment->status;
        }
      }
    } else if (line[0] == 'M' && line[1] == '\t') {
      size_t count = split_tabs(line, fields, 8u);
      size_t lifecycle_state = 0u;
      if (count == 8u &&
          parse_size_field(fields[5], &lifecycle_state) &&
          lifecycle_state >= 3u &&
          out_assignment->component_id[0] == '\0') {
        if (!copy_owned(out_assignment->component_id,
                        sizeof(out_assignment->component_id), fields[1])) {
          (void)fclose(file);
          out_assignment->status =
            VECTOR_CHILD_PROGRAM_STATUS_CORTEX_STATE_PARSE_FAILED;
          copy_owned(
            out_assignment->status_message,
            sizeof(out_assignment->status_message),
            "CORTEX component id exceeds VECTOR capacity"
          );
          return out_assignment->status;
        }
        selected_component_index = component_index;
      }
      ++component_index;
    } else if (line[0] == 'S' && line[1] == '\t') {
      size_t count = split_tabs(line, fields, 6u);
      size_t subagent_component_index = 0u;
      size_t lifecycle_state = 0u;
      size_t imported_definition = 0u;
      if (count == 6u &&
          parse_size_field(fields[3], &subagent_component_index) &&
          parse_size_field(fields[4], &lifecycle_state) &&
          parse_size_field(fields[5], &imported_definition) &&
          lifecycle_state >= 2u &&
          imported_definition == 1u &&
          subagent_component_index == selected_component_index &&
          out_assignment->subagent_instance_id[0] == '\0') {
        if (!copy_owned(out_assignment->subagent_instance_id,
                        sizeof(out_assignment->subagent_instance_id), fields[1])) {
          (void)fclose(file);
          out_assignment->status =
            VECTOR_CHILD_PROGRAM_STATUS_CORTEX_STATE_PARSE_FAILED;
          copy_owned(
            out_assignment->status_message,
            sizeof(out_assignment->status_message),
            "CORTEX subagent id exceeds VECTOR capacity"
          );
          return out_assignment->status;
        }
      }
    }
  }

  if (ferror(file)) {
    (void)fclose(file);
    out_assignment->status = VECTOR_CHILD_PROGRAM_STATUS_CORTEX_STATE_IO;
    copy_owned(
      out_assignment->status_message,
      sizeof(out_assignment->status_message),
      "CORTEX state file could not be read"
    );
    return out_assignment->status;
  }
  (void)fclose(file);

  out_assignment->cortex.character_id = out_assignment->character_id;
  out_assignment->cortex.component_id = out_assignment->component_id;
  out_assignment->cortex.subagent_instance_id =
    out_assignment->subagent_instance_id[0] == '\0'
      ? NULL
      : out_assignment->subagent_instance_id;
  out_assignment->helper.helper_id = out_assignment->helper_id;
  out_assignment->helper.source_manifest_ref =
    out_assignment->source_manifest_ref;
  out_assignment->helper.upstream_alias = out_assignment->upstream_alias;

  memset(&export_request, 0, sizeof(export_request));
  export_request.abi_version = request->abi_version;
  export_request.region = request->region;
  export_request.operation_id = request->operation_id;
  export_request.runtime_export.cortex = out_assignment->cortex;
  export_request.runtime_export.helper = out_assignment->helper;
  export_request.runtime_export.character_ready =
    out_assignment->character_id[0] == '\0' ? 0u : 1u;
  export_request.runtime_export.component_ready =
    out_assignment->component_id[0] == '\0' ? 0u : 1u;
  export_request.runtime_export.helper_can_bind =
    out_assignment->helper_id[0] == '\0' ? 0u : 1u;

  out_assignment->status =
    vector_child_program_assign_cortex_export(&export_request, &assignment);
  out_assignment->descriptor = assignment.descriptor;
  copy_owned(
    out_assignment->status_message,
    sizeof(out_assignment->status_message),
    assignment.status_message
  );
  return out_assignment->status;
}

const char *vector_child_program_status_name(vector_child_program_status status) {
  switch (status) {
    case VECTOR_CHILD_PROGRAM_STATUS_OK:
      return "ok";
    case VECTOR_CHILD_PROGRAM_STATUS_NULL_ARGUMENT:
      return "null_argument";
    case VECTOR_CHILD_PROGRAM_STATUS_UNSUPPORTED_ABI:
      return "unsupported_abi";
    case VECTOR_CHILD_PROGRAM_STATUS_UNKNOWN_REGION:
      return "unknown_region";
    case VECTOR_CHILD_PROGRAM_STATUS_MISSING_CORTEX_REFERENCE:
      return "missing_cortex_reference";
    case VECTOR_CHILD_PROGRAM_STATUS_HELPER_ASSIGNMENT_NOT_ALLOWED:
      return "helper_assignment_not_allowed";
    case VECTOR_CHILD_PROGRAM_STATUS_MISSING_HELPER_REFERENCE:
      return "missing_helper_reference";
    case VECTOR_CHILD_PROGRAM_STATUS_CORTEX_EXPORT_NOT_READY:
      return "cortex_export_not_ready";
    case VECTOR_CHILD_PROGRAM_STATUS_CORTEX_STATE_IO:
      return "cortex_state_io";
    case VECTOR_CHILD_PROGRAM_STATUS_CORTEX_STATE_PARSE_FAILED:
      return "cortex_state_parse_failed";
    default:
      return "unknown_status";
  }
}

const char *vector_data_lane_name(vector_data_lane lane) {
  switch (lane) {
    case VECTOR_DATA_LANE_NONE:
      return "none";
    case VECTOR_DATA_LANE_FLUXBASE:
      return "fluxbase";
    case VECTOR_DATA_LANE_DEVBASE:
      return "devbase";
    case VECTOR_DATA_LANE_CODEBASE:
      return "codebase";
    case VECTOR_DATA_LANE_CHATBASE:
      return "chatbase";
    default:
      return "unknown_lane";
  }
}

const char *vector_workspace_region_name(vector_workspace_region region) {
  switch (region) {
    case VECTOR_WORKSPACE_REGION_LEFT_MENU:
      return "left_menu";
    case VECTOR_WORKSPACE_REGION_MAIN_INTERACTION:
      return "main_interaction";
    case VECTOR_WORKSPACE_REGION_SESSION_RUNTIME:
      return "session_runtime";
    case VECTOR_WORKSPACE_REGION_TOOL_EXECUTION:
      return "tool_execution";
    case VECTOR_WORKSPACE_REGION_APPROVAL_REVIEW:
      return "approval_review";
    case VECTOR_WORKSPACE_REGION_ARTIFACT:
      return "artifact";
    case VECTOR_WORKSPACE_REGION_STATUS_ACTIVITY:
      return "status_activity";
    default:
      return "unknown_region";
  }
}
