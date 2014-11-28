///
/// CellularAutomataTask.h
/// Seed of Andromeda
///
/// Created by Benjamin Arnold on 24 Nov 2014
/// Copyright 2014 Regrowth Studios
/// All Rights Reserved
///
/// Summary:
/// Implements the celluar automata task for multithreaded physics
///

#pragma once

#ifndef CellularAutomataTask_h__
#define CellularAutomataTask_h__

#include "IThreadPoolTask.h"

class Chunk;
class WorkerData;
class RenderTask;

#define CA_TASK_ID 3

enum CA_FLAG {
    CA_FLAG_NONE = 0,
    CA_FLAG_LIQUID = 1,
    CA_FLAG_POWDER = 2 
};

class CellularAutomataTask : public vcore::IThreadPoolTask {
public:
    friend class ChunkManager;
    /// Constructs the task
    /// @param chunk: The the chunk to update
    /// @param flags: Combination of CA_FLAG
    CellularAutomataTask(Chunk* chunk, bool makeMesh, ui32 flags);

    /// Executes the task
    void execute(vcore::WorkerData* workerData) override;

    RenderTask* renderTask = nullptr; ///< A nested to force re-mesh

private:
    ui32 _flags; ///< Flags that tell us what to update
    Chunk* _chunk; ///< The chunk we are updating
};

#endif // CellularAutomataTask_h__