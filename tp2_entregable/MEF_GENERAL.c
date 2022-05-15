#include "clock.h"
#include "MEF_GENERAL.h"
#include "edition_mef.h"

static MEF_state state;
static TIME time;

void MEF_GENERAL_INIT() {
  state = DEFAULT;
  edition_mef_init();
}

void MEF_GENERAL_UPDATE() {
  switch (state) {
    case DEFAULT:
      break;
    case EDITION:
      edition_mef_update(time);
      break;
    default:
      break;
  }
}