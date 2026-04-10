#include "vector/child_program_runtime.h"

#include <stdio.h>
#include <string.h>

enum vector_host_exit {
  VECTOR_HOST_EXIT_OK = 0,
  VECTOR_HOST_EXIT_USAGE = 2,
  VECTOR_HOST_EXIT_RUNTIME = 3
};

static void print_usage(void) {
  puts("vector_host commands:");
  puts("  version");
  puts("  list");
  puts("  route <region> <operation> [character-id] [component-id] [subagent-id]");
  puts("  assign-state <cortex-state-file> <region> <operation>");
  puts("");
  puts("regions: left-menu, main-interaction, session-runtime, tool-execution, approval-review, artifact, status-activity");
}

static vector_workspace_region parse_region(const char *region) {
  if (region == NULL) {
    return (vector_workspace_region)0;
  }
  if (strcmp(region, "left-menu") == 0) {
    return VECTOR_WORKSPACE_REGION_LEFT_MENU;
  }
  if (strcmp(region, "main-interaction") == 0) {
    return VECTOR_WORKSPACE_REGION_MAIN_INTERACTION;
  }
  if (strcmp(region, "session-runtime") == 0) {
    return VECTOR_WORKSPACE_REGION_SESSION_RUNTIME;
  }
  if (strcmp(region, "tool-execution") == 0) {
    return VECTOR_WORKSPACE_REGION_TOOL_EXECUTION;
  }
  if (strcmp(region, "approval-review") == 0) {
    return VECTOR_WORKSPACE_REGION_APPROVAL_REVIEW;
  }
  if (strcmp(region, "artifact") == 0) {
    return VECTOR_WORKSPACE_REGION_ARTIFACT;
  }
  if (strcmp(region, "status-activity") == 0) {
    return VECTOR_WORKSPACE_REGION_STATUS_ACTIVITY;
  }
  return (vector_workspace_region)0;
}

static int exit_for_status(vector_child_program_status status) {
  return status == VECTOR_CHILD_PROGRAM_STATUS_OK
    ? VECTOR_HOST_EXIT_OK
    : VECTOR_HOST_EXIT_RUNTIME;
}

static int command_version(void) {
  printf(
    "vector_host abi=%u schema=%u child_programs=%zu\n",
    vector_child_program_runtime_abi_version(),
    vector_child_program_runtime_schema_version(),
    vector_child_program_count()
  );
  return VECTOR_HOST_EXIT_OK;
}

static int command_list(void) {
  size_t index = 0u;

  for (index = 0u; index < vector_child_program_count(); ++index) {
    vector_child_program_descriptor descriptor;
    vector_child_program_status status =
      vector_child_program_at(index, &descriptor);
    if (status != VECTOR_CHILD_PROGRAM_STATUS_OK) {
      fprintf(stderr, "descriptor[%zu] failed: %s\n",
              index, vector_child_program_status_name(status));
      return VECTOR_HOST_EXIT_RUNTIME;
    }
    printf("program[%zu] id=%s region=%s lane=%s flags=%u\n",
           index,
           descriptor.program_id,
           vector_workspace_region_name(descriptor.region),
           vector_data_lane_name(descriptor.primary_lane),
           descriptor.flags);
  }

  return VECTOR_HOST_EXIT_OK;
}

static int command_route(int argc, char **argv) {
  vector_child_program_route route;
  vector_child_program_route_request request;
  vector_child_program_status status = VECTOR_CHILD_PROGRAM_STATUS_OK;

  if (argc < 5) {
    print_usage();
    return VECTOR_HOST_EXIT_USAGE;
  }

  memset(&request, 0, sizeof(request));
  request.abi_version = VECTOR_CHILD_PROGRAM_RUNTIME_ABI_VERSION;
  request.region = parse_region(argv[2]);
  request.operation_id = argv[3];
  request.cortex.character_id = argc > 4 ? argv[4] : NULL;
  request.cortex.component_id = argc > 5 ? argv[5] : NULL;
  request.cortex.subagent_instance_id = argc > 6 ? argv[6] : NULL;

  status = vector_child_program_route_region(&request, &route);
  printf("route status=%s program=%s region=%s lane=%s message=\"%s\"\n",
         vector_child_program_status_name(status),
         route.descriptor.program_id == NULL ? "" : route.descriptor.program_id,
         vector_workspace_region_name(route.descriptor.region),
         vector_data_lane_name(route.descriptor.primary_lane),
         route.status_message == NULL ? "" : route.status_message);
  return exit_for_status(status);
}

static int command_assign_state(int argc, char **argv) {
  vector_owned_helper_assignment assignment;
  vector_cortex_state_assignment_request request;
  vector_child_program_status status = VECTOR_CHILD_PROGRAM_STATUS_OK;

  if (argc != 5) {
    print_usage();
    return VECTOR_HOST_EXIT_USAGE;
  }

  memset(&request, 0, sizeof(request));
  request.abi_version = VECTOR_CHILD_PROGRAM_RUNTIME_ABI_VERSION;
  request.state_path = argv[2];
  request.region = parse_region(argv[3]);
  request.operation_id = argv[4];

  status = vector_child_program_assign_cortex_state_file(&request, &assignment);
  printf(
    "assign-state status=%s program=%s helper=%s character=%s component=%s subagent=%s message=\"%s\"\n",
    vector_child_program_status_name(status),
    assignment.descriptor.program_id == NULL ? "" : assignment.descriptor.program_id,
    assignment.helper_id,
    assignment.character_id,
    assignment.component_id,
    assignment.subagent_instance_id,
    assignment.status_message
  );
  return exit_for_status(status);
}

int main(int argc, char **argv) {
  if (argc < 2) {
    print_usage();
    return VECTOR_HOST_EXIT_USAGE;
  }

  if (strcmp(argv[1], "version") == 0) {
    return command_version();
  }

  if (strcmp(argv[1], "list") == 0) {
    return command_list();
  }

  if (strcmp(argv[1], "route") == 0) {
    return command_route(argc, argv);
  }

  if (strcmp(argv[1], "assign-state") == 0) {
    return command_assign_state(argc, argv);
  }

  print_usage();
  return VECTOR_HOST_EXIT_USAGE;
}
