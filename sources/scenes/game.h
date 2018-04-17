#include <cstdio>
#include <set>
#include <vector>
#include <map>

#include <cage-core/core.h>
#include <cage-core/log.h>
#include <cage-core/math.h>
#include <cage-core/filesystem.h>
#include <cage-core/entities.h>
#include <cage-core/config.h>
#include <cage-core/assets.h>
#include <cage-core/utility/timer.h>
#include <cage-core/utility/hashString.h>
#include <cage-core/utility/color.h>

#include <cage-client/core.h>
#include <cage-client/window.h>
#include <cage-client/sound.h>
#include <cage-client/gui.h>
#include <cage-client/engine.h>
#include <cage-client/utility/cameraController.h>
#include <cage-client/utility/engineProfiling.h>

using namespace cage;
