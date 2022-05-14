#include "edition_mef.h"
#include "clock.h"

static MEF_state state;
static TIME time;

void MEF_GENERAL_INIT() {
  state = DEFAULT;
  edition_mef_init();
}

void MEF_GENERAL_UPDATE() {
  switch (state) {
    case DEFAULT:
      state = MEF_state_IDLE;
      break;
    case EDITION:
      edition_mef_update(time);
      break;
    default:
      state = MEF_state_INIT;
      break;
  }
}