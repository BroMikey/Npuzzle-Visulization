#pragma once

#include "InteractionManager.h"
#include "../Visual/CanvasView.h"
#include "../Visual/TreeRenderer.h"
#include "../Visual/DisplayManager.h"
#include "../Core/TreeNode.h"
#include <memory>

namespace CommandRegistrar
{
    void registerDefaultCommands(
        InteractionManager &manager,
        CanvasView &canvasView,
        TreeRenderer &treeRenderer,
        std::shared_ptr<Tree> tree,
        DisplayManager *displayManager = nullptr);
}
