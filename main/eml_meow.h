#ifndef EMLEARN_MEOW_H
#define EMLEARN_MEOW_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int featureIndex;
    float threshold;
    int leftChild;
    int rightChild;
} EmlTreesNode;

typedef struct {
    int nodeCount;
    EmlTreesNode* nodes;
    int treeCount;
    int32_t* treeRoots;
} EmlTrees;

extern EmlTreesNode meow_nodes[80];
extern int32_t meow_tree_roots[10];
extern EmlTrees meow;

// Function to predict meow based on input features
int32_t meow_predict(const float *features, int32_t features_length);

#ifdef __cplusplus
}
#endif

#endif // EMLEARN_MEOW_H
