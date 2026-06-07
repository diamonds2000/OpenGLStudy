#ifndef PERFORMANCE_H
#define PERFORMANCE_H

#include "scene.h"

void performance_test_onscreen(SceneContext& context);
void performance_test_offscreen(SceneContext& context, int width, int height, int numFrames);

#endif // PERFORMANCE_H