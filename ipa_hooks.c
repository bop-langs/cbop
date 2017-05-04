#include "bop_api.h"
#include "ipa.h"
#include "ipa_hooks.h"
#include "ipa_sync.h"

extern int monitor_process_id;

bool speculating() {
  return !(bop_mode == SERIAL || BOP_task_status() == SEQ || BOP_task_status() == UNDY);
}

void record_allocation(void * payload, size_t bytes) {
  // perror("Recording allocation");
  BOP_record_write(payload, bytes);
}

int getuniqueid() {
  return monitor_process_id;
}

bop_port_t bop_alloc_port = {
	.ppr_group_init		= beginspec,
	.task_group_commit	= endspec,
  .on_main_complete = promote_list
};
