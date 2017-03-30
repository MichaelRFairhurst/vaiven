#include "dominator_builder.h"

#include <algorithm>

using namespace vaiven::ssa;

void DominatorBuilder::firstBuild(Block& firstBlock) {
  Block* curBlock = &firstBlock;
  set<Block*> visitedBlocks;

  while (true) {
    visitedBlocks.insert(curBlock);
    for (vector<unique_ptr<BlockExit>>::iterator it = curBlock->exits.begin();
        it != curBlock->exits.end();
        ++it) {

      if (visitedBlocks.find((*it)->toGoTo) != visitedBlocks.end()) {
        (*it)->toGoTo->backReferences.insert(curBlock);
      } else {
        (*it)->toGoTo->immPredecessors.insert(curBlock);
      }

      (*it)->toGoTo->allPredecessors.insert(curBlock);
      (*it)->toGoTo->allPredecessors.insert(curBlock->allPredecessors.begin(),
          curBlock->allPredecessors.end());
    }

    if (curBlock->next.get() != NULL) {
      curBlock = curBlock->next.get();
    } else {
      break;
    }
  }

  rebuild(firstBlock);
}

void DominatorBuilder::rebuild(Block& firstBlock) {
  Block* curBlock = &firstBlock;
  while (true) {
    rebuildBlock(curBlock);

    if (curBlock->next.get() != NULL) {
      curBlock = curBlock->next.get();
    } else {
      break;
    }
  }

}

/**
 * At this point, we only know edges. We must be careful to traverse incoming
 * edges before back edges, as loop headers have multiple incoming edges but
 * still dominate the body of the loop (as opposed to places with multiple
 * forward edges, where only the union of the dominators dominate the subsequent
 * block).
 *
 * If -> IfTrue -> ForHeader -> ForBody
 *  `--> IfFalse ---^ v ^----------'
 *                 ForEnd
 *
 * If dominates all blocks
 * ForHeader dominates ForBody, ForEnd
 *
 * If we recurse forward through the edges, 
 */
void DominatorBuilder::rebuildBlock(Block* curBlock) {
  set<Block*> incomingForwardEdges = curBlock->immPredecessors;

  if (incomingForwardEdges.size() == 0) {
    return;
  }

  // TODO this all assumes dominators have been built for the incoming
  // forward refs already. In theory should track unvisited blocks and visit
  // them in a stack (or use recursion).
  if (incomingForwardEdges.size() == 1) {
    Block* predecessor = *incomingForwardEdges.begin();
    curBlock->dominators = predecessor->dominators;
    curBlock->dominators.insert(predecessor);
  } else {
    set<Block*>::iterator it = incomingForwardEdges.begin();
    curBlock->dominators = (*it)->dominators;
    set<Block*> workingSet;

    for (++it;
        it != incomingForwardEdges.end();
        ++it) {
      auto wsIt = set_intersection(
          curBlock->dominators.begin(),
          curBlock->dominators.end(),
          (*it)->dominators.begin(),
          (*it)->dominators.end(),
          inserter(workingSet, workingSet.begin()));

      curBlock->dominators = workingSet;
    }
  }

  for (vector<unique_ptr<BlockExit>>::iterator it = curBlock->exits.begin();
      it != curBlock->exits.end();
      ++it) {
    // loop detected!
    if (curBlock->dominators.find((*it)->toGoTo) != curBlock->dominators.end()) {
      Block* loopHeader = (*it)->toGoTo;
      set<Block*> predecessorsFromLoopHeader;

      // find predecessors that hail from the loop header
      // NOTE: we could go from predecessors with loop header as a predecessor
      // to get blocks which MIGHT be in the loop. But for our purposes, w
      // get predecessors which ALWAYS hail from the loop.
      for (set<Block*>::iterator pIt = curBlock->allPredecessors.begin();
          pIt != curBlock->allPredecessors.end();
          ++pIt) {
        set<Block*>& pDominators = (*pIt)->dominators;
        if (pDominators.find(loopHeader) != pDominators.end()) {
          predecessorsFromLoopHeader.insert(*pIt);
        }
      }

      // find all blocks in the loop
      set<Block*> workingSet;
      set_intersection(
          predecessorsFromLoopHeader.begin(),
          predecessorsFromLoopHeader.end(),
          loopHeader->allPredecessors.begin(),
          loopHeader->allPredecessors.end(),
          inserter(workingSet, workingSet.begin()));

      // note: the loop header may already have items, which is OK.
      loopHeader->loopHeaderWithBlocks.insert(workingSet.begin(), workingSet.end());
      loopHeader->loopHeaderWithBlocks.insert(curBlock);
    }
  }
}

void DominatorBuilder::reset(Block& firstBlock) {
  Block* curBlock = &firstBlock;

  while (true) {
    curBlock->dominators.clear();
    curBlock->allPredecessors.clear();
    curBlock->loopHeaderWithBlocks.clear();

    if (curBlock->next.get() != NULL) {
      curBlock = curBlock->next.get();
    } else {
      break;
    }
  }

  curBlock = &firstBlock;
  // TODO, can allPredecessors be tracked incrementally or with a better
  // algorithm?
  while (true) {
    for (vector<unique_ptr<BlockExit>>::iterator it = curBlock->exits.begin();
        it != curBlock->exits.end();
        ++it) {
      (*it)->toGoTo->allPredecessors.insert(curBlock);
      (*it)->toGoTo->allPredecessors.insert(curBlock->allPredecessors.begin(),
          curBlock->allPredecessors.end());

      // TODO do back references need updating?
    }

    if (curBlock->next.get() != NULL) {
      curBlock = curBlock->next.get();
    } else {
      break;
    }
  }

  rebuild(firstBlock);
}
