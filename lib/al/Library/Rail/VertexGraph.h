#pragma once

#include <basis/seadTypes.h>
#include <container/seadObjArray.h>
#include <container/seadPtrArray.h>
#include <math/seadVector.h>

#include "Library/Rail/Graph.h"

namespace al {

void calcShortestPath(sead::ObjArray<Graph::VertexInfo>*, const Graph*, s32, s32);
void calcDistanceAndNearestPos(sead::Vector3f*, const Graph::PosEdge*, const sead::Vector3f&);
f32 findEdgeMinimumWeight(const Graph*);
f32 findEdgeMinimumWeight(const Graph::Vertex*);
f32 findEdgeMinimumWeight(const Graph::Edge**, const sead::PtrArray<Graph::Edge>&);
Graph::Edge* tryFindEdgeStartVertex(const Graph::Vertex*, const Graph::Vertex*);
Graph::Edge* tryFindEdgeEndVertex(const Graph::Vertex*, const Graph::Vertex*);
Graph::Vertex* findNearestPosVertex(const Graph*, const sead::Vector3f&, f32);
Graph::Vertex* findFarthestPosVertex(const Graph*, const sead::Vector3f&, f32);
void findPosEdgeByVertexPosUndirect(const Graph*, const sead::Vector3f&, const sead::Vector3f&);
void findPosEdgeByVertexPos(const Graph*, const sead::Vector3f&, const sead::Vector3f&);
bool isDestinationVertex(const Graph::PosVertex*, const Graph::PosVertex*);
void tryRemoveEdgeFromVertex(Graph::Vertex*, Graph::Edge*);
bool isExistVertex(const Graph*, const Graph::Vertex*);
bool isSpanningTree(const Graph*);
bool isSpanningTree(const sead::PtrArray<Graph::Edge>&);
bool isWalk(const sead::PtrArray<Graph::Edge>&);
bool isTrail(const sead::PtrArray<Graph::Edge>&);
bool isCircuit(const sead::PtrArray<Graph::Edge>&);
bool isPath(const sead::PtrArray<Graph::Edge>&);
bool isCycle(const sead::PtrArray<Graph::Edge>&);
void calcMinimumSpanningTree(sead::PtrArray<Graph::Edge>*, const Graph*);

}  // namespace al
