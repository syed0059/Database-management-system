// Import Libraries
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cmath>
#include <cstring>
#include <stack>
#include <queue>
#include <vector>
#include <tuple>
#include "bPlusTree.h"
#include "utils.h"
#include "../loadData.h"

using namespace std;

/**
 * @brief Function to count the number of nodes in a B+ Tree
 * 
 * @param rootNode 
 * @return int 
 */
int BPlusTree::countNodesInBPlusTree(Node* rootNode, int currHeight) {
    // Base Case - Tree is Empty
    if (rootNode == NULL) {
        return 0;
    }

    if (currHeight == 1) {
        // Root Node only
        return 1;
    }
    // Any other scenario must be an internal node
    InternalNode *internalNode = (InternalNode*) rootNode;

    // Initialise as at least Root exists
    int numNodes = 1;

    // Iterate over all the Child Nodes of the Root
    for (int i = 0; i <= rootNode->numKeysInserted; i += 1) {
        // Add all the nodes of a child subtree
        numNodes += countNodesInBPlusTree(internalNode->children[i], currHeight - 1);
    }

    return numNodes;
}

/**
 * @brief Helper Function for insertKeyInTree(). Needs a way to insert new keys or records
 * into the index while maintaining the sorted order.
 * 
 * @param key 
 * @param targetRecord 
 * @return int 
 */
int insertionSortInsertLeafNode(unsigned short int key, Record *targetRecord, int numKeysInTarget, LeafNode *targetLeafNode) {
    int currKey = key;
    Record *currRecord = targetRecord;
    int tmpKey;
    Record* tmpRecord;

    // Iterate over the array to compare the values of the key
    for (int i = 0; i < numKeysInTarget; i++) {
        // Current Key Value is Larger should be swapped.
        if (targetLeafNode->keys[i] > currKey) {
            // Swap the Keys First
            tmpKey = currKey;
            currKey = targetLeafNode->keys[i];
            targetLeafNode->keys[i] = tmpKey;

            // Swap the Records Next
            tmpRecord = currRecord;
            currRecord = targetLeafNode->records[i];
            targetLeafNode->records[i] = tmpRecord;
        }
    }

    // After the Loop, the curr values are the largest
    // Insert the Key into the Array in the Leaf Node
    targetLeafNode->keys[numKeysInTarget] = currKey;

    // Insert the Record into the Ptr
    targetLeafNode->records[numKeysInTarget] = currRecord;
    
    // No issues
    return 0;
}

/**
 * @brief Same Insertion Sort Method as above. However built to allow sorting for arrays
 * with keys of size (n+1).
 * 
 * @param key 
 * @param targetRecord 
 * @param numKeysInTarget 
 * @param combinedKeysArray 
 * @param combinedRecordsArray 
 * @return std::pair<float*, Record**> 
 */
std::pair<unsigned short int*, Record**> insertionSortInsertArray(unsigned short int key, Record *targetRecord, int numKeysInTarget, unsigned short int combinedKeysArray[]
, Record *combinedRecordsArray[]) {
    int currKey = key;
    Record *currRecord = targetRecord;

    // Iterate over the array to compare the values of the key
    for (int i = 0; i < numKeysInTarget; i++) {
        // Current Key Value is Larger should be swapped.
        if (combinedKeysArray[i] > currKey) {
            // Swap the Keys First
            int tmpKey = currKey;
            currKey = combinedKeysArray[i];
            combinedKeysArray[i] = tmpKey;

            // Swap the Records Next
            Record *tmpRecord = currRecord;
            currRecord = combinedRecordsArray[i];
            combinedRecordsArray[i] = tmpRecord;
        }
    }

    // After the Loop, the curr values are the largest
    combinedKeysArray[numKeysInTarget] = currKey;
    combinedRecordsArray[numKeysInTarget] = currRecord;
    
    // No issues
    return std::make_pair(combinedKeysArray, combinedRecordsArray);
}

/**
 * @brief Updates the Key for Insertion of Key into indexes.
 * Target record must be already on disk
 * 
 * @param key 
 * @param targetRecord 
 * @return 0 if there is nothing wrong 
 */
int BPlusTree::insertKeyInTree(unsigned short int key, Record* targetRecord) {
    // Retrieve attributes of the B+ Tree
    // int heightTree = this->getHeight();
    // Node *BPlusTreeRoot = this->getRoot();
    // Prepare the Variables for the findRecordInTree() function
    std::stack<Node*> myStack;
    std::stack<Node*> *myStackPtr = &myStack;
    // Will retrieve the actual record, but won't be relevant in this function
    Record **recordPtr = NULL;
    // Search the Tree to see if the B+ tree contains the Record Key
    std::pair<bool, int> result = findRecordInTree(key, myStackPtr, recordPtr);
    bool searchResult = result.first;

    // Check if the Key Value exists in the B+ Tree
    if (searchResult) {
        // Record exists, do nothing
        return 0;
    }

    // Retrieve the Target Leaf Node, i.e. the node where the target exist or where target is supposed to be inserted
    Node *targetLeafNode = myStackPtr->top();
    myStackPtr->pop();
    // If the key does not exists in tree, check if there is space
    int numKeysInTarget = targetLeafNode->numKeysInserted;
    // int numRecordsInTarget = targetLeafNode->numRecordsInserted;

    // Case 1 - Leaf Node has space to directly insert
    if (numKeysInTarget < NUM_KEYS) {
        // Insert the Record and Key
        insertionSortInsertLeafNode(key, targetRecord, numKeysInTarget, (LeafNode*) targetLeafNode);\
        // Update the numkeys and numRecords
        targetLeafNode->numKeysInserted += 1;
        // targetLeafNode->numRecordsInserted += 1;
        return 0;
    }

    // Case 2 - Leaf Node is full, has (n+1) keys now. 
    // Create a new sibling leaf node
    Node *newLeafNode = (Node*) (new LeafNode());
    newLeafNode->numKeysInserted = 0;
   ((LeafNode*) newLeafNode)->next = NULL;

    // Distribute the keys - MUST ALWAYS BE 40 else, sth is wrong
    if ((numKeysInTarget + 1) % 2 == 0) {

        // Even number of keys. Create a New Array to store all the (n + 1) keys since the class array cannot store
        unsigned short int combinedKeysArray[NUM_KEYS + 1];
        Record *combinedRecordsArray[NUM_KEYS + 1];
        for (int i = 0; i < numKeysInTarget; i++) {
            combinedKeysArray[i] = targetLeafNode->keys[i];
            combinedRecordsArray[i] = ((LeafNode*)targetLeafNode)->records[i];
        }

        // Insert the n+1th key and sort all (n+1) keys. Okay to pass by value since don't need the arrays afterwards
        std::pair<unsigned short int*, Record**> insertionResult = insertionSortInsertArray(key, targetRecord, numKeysInTarget, combinedKeysArray, combinedRecordsArray);
        unsigned short int *insertionKeysArray = insertionResult.first;
        Record **insertionRecordsArray = insertionResult.second;
        // Determine the index to start copying for the larger half for the newly created node
        int numKeysToCopy = (numKeysInTarget+1) / 2;
        // Update the Old Leaf Node, i.e. original leaf
        for (int i = 0; i < numKeysToCopy; i++) {
            // Copy the Keys over
            targetLeafNode->keys[i] = *(insertionKeysArray + i);

            // Copy the Record over
            ((LeafNode*)targetLeafNode)->records[i] = *(insertionRecordsArray + i);
        }

        // Update the New Leaf Node
        int counter = 0;
        for (int i = numKeysToCopy; i < numKeysInTarget+1; i++) {
            // New leaf should have no records, so start from 0
            newLeafNode->keys[counter] = *(insertionKeysArray + i);
            ((LeafNode*) newLeafNode)->records[counter] = *(insertionRecordsArray + i);

            counter += 1;
        }

        // Update the number of keys inserted in the nodes, split evenly since 39 + 1 == 40
        targetLeafNode->numKeysInserted = numKeysToCopy;
        // targetLeafNode->numRecordsInserted = numKeysToCopy;
        newLeafNode->numKeysInserted = numKeysToCopy;
        // newLeafNode->numRecordsInserted = numKeysToCopy;

        // Update the nextLeafNode Ptr, which might point to NULL
        LeafNode *nextLeafNode = ((LeafNode*)targetLeafNode)->next;
        ((LeafNode*)targetLeafNode)->next = (LeafNode*) newLeafNode;
        ((LeafNode*) newLeafNode)->next = nextLeafNode;

    }

    // Check if the Parent Node exists
    InternalNode *parentNode; // = targetLeafNode->parent;
    if (myStackPtr->empty()) {
        // No Parent Node, is 1 level B+ Tree
        parentNode = NULL;
    }
    else {
        // Parent Exists, so must be Internal Node
        parentNode = (InternalNode*) myStackPtr->top();
        myStackPtr->pop();
    }

    // Boolean to determine if we are at Level 1.
    bool reachRoot = false;
    do {
        // Case 2.1 - If Parent Does not exist, i.e. currently height is 1
        if (parentNode == NULL) {
            // Create a New Parent Node
            InternalNode *newParentNode = (InternalNode*) malloc(sizeof(InternalNode));
            newParentNode->numKeysInserted = 1;
            // newParentNode->parent = NULL;
            // Update the TWO Children Nodes. Old is Smaller
            newParentNode->children[0] = targetLeafNode;
            newParentNode->children[1] = newLeafNode;

            // Insert the ONE key in parent, smallest value in RST
            newParentNode->keys[0] = newLeafNode->keys[0];
        
            // Update the Height of the Tree
            this->height += 1;

            // Update the Root of the Tree
            this->root = newParentNode;

            // End Method, Return 0 for no issues
            reachRoot = true;
        }
        // Case 2.2 - Parent Node is not Full
        else if (parentNode->numKeysInserted < NUM_KEYS) {
            // Case 2.2.1 - Update the Parent Node Directly
            // InternalNode *existingParentNode = targetLeafNode->parent;
            
            // TBH Not very sure of this
            // New Key to be inserted is the largest value since parent keys are already sorted
            unsigned short int newKey = newLeafNode->keys[0];

            // Cannot update the numKeys until we insert the new key
            parentNode->keys[parentNode->numKeysInserted] = newKey;
            // Num Children Nodes is always numKeys + 1
            parentNode->children[parentNode->numKeysInserted + 1] = newLeafNode;

            // Update the Number of Keys Inserted
            parentNode->numKeysInserted += 1;

            // Don't need update parent, end process
            reachRoot = true;
        }
        // Case 2.3 - Parent Node is Full
        else if (parentNode->numKeysInserted >= NUM_KEYS) {
            /**
             * @brief The keys in the parent nodes are already sorted.
             * So the new key-child pair will be the largest and to the
             * right.
             */

            // Min Internal Nodes: Floor(n/2) = 19 when n is 39
            int numKeysLeftParent;
            int numKeysRightParent;

            // Determine the Number of Keys each node should have
            if ((parentNode->numKeysInserted + 1) % 2 == 0) {
                // Even no. of nodes, so split equally
                numKeysLeftParent = (parentNode->numKeysInserted + 1) / 2;
                numKeysRightParent = (parentNode->numKeysInserted + 1) / 2;
            }
            else {
                // Odd no. of nodes, left child takes 1 more
                numKeysLeftParent = std::floor((parentNode->numKeysInserted + 1) / 2) + 1;
                numKeysRightParent = std::floor((parentNode->numKeysInserted + 1) / 2);
            }

            // Create a Sibling Node to Parent, i.e. Uncle Node
            InternalNode *newUncleNode = (InternalNode*) (new InternalNode());
            newUncleNode->numKeysInserted = 0;

            // Copy over the Keys and Children of original node to the uncle node
            int counter = 0;
            for (int i = numKeysLeftParent; i < (parentNode->numKeysInserted); i++) {
                // Sibling Node starts from 0, while Parent Node starts from Middle cos larger
                newUncleNode->keys[counter] = parentNode->keys[i];
                newUncleNode->children[counter+1] = parentNode->children[i+1];

                counter += 1;
            }
            // Add the Last Key to the Rightmost Sibling Node {Smallest Key of New Leaf Node}
            newUncleNode->keys[counter] = newLeafNode->keys[0];
            newUncleNode->children[counter+1] = newLeafNode;

            // Have the Last Pointer point to NULL
            newUncleNode->children[NUM_KEYS - 1] = NULL;

            // Update Attributes for Uncle Node (On the Right)
            newUncleNode->numKeysInserted = numKeysRightParent;

            // Update Attributes for Original Parent Node (On the Left)
            parentNode->numKeysInserted = numKeysLeftParent;

            // Parent Node to Point to sibling Node
            parentNode->children[NUM_KEYS - 1] = newUncleNode;

            targetLeafNode = parentNode;
            newLeafNode = newUncleNode;
            // Update the Parent Node to be the Grandparent Node now
            if (myStackPtr->empty()) {
                // Sets up for Case 2.1
                parentNode = NULL;
            }
            else {
                // Grandparent must also be internal node
                parentNode = (InternalNode*) myStackPtr->top();
                myStackPtr->pop();
            }
            // parentNode = parentNode->parent;
            // Remember Parent is Smaller than its sibling
            // reachRoot still false
            reachRoot = false;
        }
    } while (!reachRoot);
    // No issues    
    return 0;
}


std::pair<bool, int> BPlusTree :: findRecordInTree(unsigned short int key, std::stack<Node*> *stackPtr, Record **recordPtr) {
    // Track the Number of Index & Data Blocks Accessed
    int numIndexBlocks = 1;

    stackPtr->push(root);
    // INCREMENT LOAD COUNTER
    Node* next = NULL;
    Record* r = NULL;
    bool found = false;

    for (int i = 0; i < height-1; i++) {
        for (int j = 0; j < stackPtr->top()->numKeysInserted; j++) {
            if (stackPtr->top()->keys[j] > key) {
                // INCREMENT LOAD COUNTER
                next = ((InternalNode*) stackPtr->top())->children[j];
                break;
            }
        }
        numIndexBlocks += 1;
        // If next hasn't been assigned, means it refers to the last node pointer
        (next == NULL) && (next = ((InternalNode*) stackPtr->top())->children[stackPtr->top()->numKeysInserted]);
        stackPtr->push(next);
        next = NULL;
    }

    // Look for Record in Leaf Index Block
    int recordFirstAppearanceIndex = 0;
    for (int j = 0; j < stackPtr->top()->numKeysInserted; j++) {
        if (stackPtr->top()->keys[j] == key) {
            r = ((LeafNode*) stackPtr->top())->records[j];
            recordFirstAppearanceIndex = j;
            found = true;
            break;
        }
    }

    if (found == true) {
        if (recordPtr != NULL) {
            (*recordPtr) = r;
        }
    }

    // std::cout << "Number of Index Nodes Accessed: " << numIndexBlocks << std::endl;

    return std::make_pair(found, numIndexBlocks);
}


std::vector<std::pair<Node*, int> > BPlusTree :: _ancestry(unsigned short int key) {

    std::vector<std::pair<Node*, int> > res;
    Node* curr = root;
    int _, i;
    unsigned short int *keys;

    // internal nodes
    for (_=1; _<height; _++) {
        keys = curr->keys;
        for (i=0; i<curr->numKeysInserted; i++) {
            if (keys[i] > key) break;
        }
        res.push_back(std::make_pair(curr, i));
        curr = ((InternalNode*) curr)->children[i];
    }

    // leaf node
    keys = curr->keys;
    for (i=0; i<curr->numKeysInserted; i++) {
        if (keys[i] == key) {
            res.push_back(std::make_pair(curr, i));
            return res;
        }
    }
    return std::vector<std::pair<Node*, int> >();
}


void BPlusTree :: _updateUpstream(Node*, std::vector<std::pair<Node*, int> > st) {
    Node* temp;
    InternalNode* node;
    InternalNode* parent;
    InternalNode* sibling;
    int offset, i_sibling, offset_parent;
    
    std::tie(temp, offset) = st.back();
    st.pop_back();
    node = (InternalNode*) temp;

    _shift(node, offset+1, -1); // delete

    if (node == root) {
        if (node->numKeysInserted == 0) {
            root = node->children[0];
            height--;
        } else if (node->numKeysInserted == -1) {
            root = nullptr;
            height--;
        }
        return;
    }
    
    if (node->numKeysInserted >= MIN_INTERNAL_KEYS) return _updateFirstLeft(st, node->children[0]->keys[0]);

    // borrow
    std::tie(temp, offset_parent) = st.back();
    parent = (InternalNode*) temp;
    i_sibling = _sibling(parent, offset_parent);

    if (i_sibling != -1) {
        sibling = (InternalNode*) parent->children[i_sibling];

        if (offset_parent > i_sibling) {
            // borrow from left sibling
            _shift(node, 0, 1); // make space for borrowed key
            node->children[0] = sibling->children[sibling->numKeysInserted--];
            node->keys[0] = _leftmost(node->children[1]);
            parent->keys[offset_parent-1] = _leftmost(node->children[0]);
        } else {
            // borrow from right sibling
            node->keys[node->numKeysInserted++] = sibling->children[0]->keys[0];
            node->children[node->numKeysInserted] = sibling->children[0];
            _shift(sibling, 1, -1); // delete sibling's first key & ptr
            parent->keys[i_sibling-1] = _leftmost(sibling->children[0]);
        }
        return;
    }
    
    // merge onto right sibling
    if (offset_parent == 0) node->mergeRight((InternalNode*) parent->children[offset_parent+1]);
    // merge onto left sibling
    else node->mergeLeft((InternalNode*) parent->children[offset_parent-1]);

    _updateUpstream(parent, st);
}


Record* BPlusTree :: updateIndex(unsigned short int deletedKey) {
    std::vector<std::pair<Node*, int> > st = _ancestry(deletedKey);

    if (st.empty()) return nullptr;

    Node* node;
    Node* temp;
    LeafNode* sibling;
    LeafNode* leaf;
    InternalNode* parent;
    Record* res;
    int offset, offset_parent, i_sibling, i;
    
    std::tie(node, offset) = st.back();
    leaf = (LeafNode*) node;
    st.pop_back();
    res = leaf->records[offset];
    _shift(leaf, offset+1, -1); // delete

    // ------------ CASE 1 ------------
    // Sufficient keys remaining
    
    if (leaf->numKeysInserted >= MIN_LEAF_KEYS) {
        _updateFirstLeft(st, leaf->keys[0]);
        return res;
    }

    // ------------ CASE 2 ------------
    // Borrow from sibling

    std::tie(temp, offset_parent) = st.back();

    parent = (InternalNode*) temp;
    i_sibling = _leafSibling(parent, offset_parent);

    if (i_sibling != -1) {
        sibling = (LeafNode*) (parent->children[i_sibling]);

        if (node->keys[0] > sibling->keys[0]) {
            // borrow from left sibling
            _shift(leaf, 0, 1); // make space for borrowed key
            leaf->records[0] = sibling->records[--sibling->numKeysInserted];
            leaf->keys[0] = sibling->keys[sibling->numKeysInserted];
            parent->keys[offset_parent-1] = leaf->keys[0];
        } else {
            // borrow from right sibling
            leaf->keys[leaf->numKeysInserted] = sibling->keys[0];
            leaf->records[leaf->numKeysInserted++] = sibling->records[0];
            _shift(sibling, 1, -1); // delete sibling's first key & ptr
            parent->keys[i_sibling-1] = sibling->keys[0];
            _updateFirstLeft(st, leaf->keys[0]);
        }
        return res;
    } 

    // ------------ CASE 3 ------------
    // Unable to borrow, hence need to merge with sibling & delete parent's ptr
    // since we delete a leaf node, we need to update it's left leaf's pointer

    for (i=st.size()-1; i>=0; i--) {
        offset = st[i].second;
        if (offset > 0) break; 
    }

    if (i > 0) {
        temp = ((InternalNode*) st[i].first)->children[offset-1];
        while (dynamic_cast<InternalNode*>(temp)) {
            temp = ((InternalNode*) temp)->children[temp->numKeysInserted+1];
        }
        ((LeafNode*) temp)->next = leaf->next;
    }

    // merge onto right sibling
    if (offset_parent == 0) leaf->mergeRight((LeafNode*) parent->children[offset_parent+1]);
    // merge onto left sibling
    else leaf->mergeLeft((LeafNode*) parent->children[offset_parent-1]);
    
    _updateUpstream(parent, st);
    return res;
}


void BPlusTree :: print() {
    std::queue<std::queue<Node*> > curr;
    std::queue<std::queue<Node*> > next;
    std::queue<Node*> temp;
    std::queue<Node*> temp2;
    unsigned short int* keys;
    Node** children;
    Node* child;
    int i;

    temp.push(root);
    curr.push(temp);
    for (int _=1; _<height; _++) {
        while (!curr.empty()) {
            temp = curr.front();
            curr.pop();

            std::cout << "[ ";
            
            while (!temp.empty()) {
                temp2 = std::queue<Node*>();
                child = temp.front();
                temp.pop();
                keys = child->keys;
                children = ((InternalNode*) child)->children;
                std::cout << "(";

                for (i = 0; i<child->numKeysInserted; i++) {
                    std::cout << bytesToFloat(keys[i]) << ",";
                    temp2.push(children[i]);
                }
                temp2.push(children[i]);
                next.push(temp2);
                std::cout << ") ";
            }
            std::cout << "] ";
            
        }
        std::cout << std::endl;
        std::swap(curr, next);
        next = std::queue<std::queue<Node*> >();
    }
    while (!curr.empty()) {
        temp = curr.front();
        curr.pop();

        std::cout << "[ ";
        while (!temp.empty()) {
            child = temp.front();
            keys = child->keys;
            temp.pop();

            std::cout << "(";
            for (i=0; i<child->numKeysInserted; i++) {
                std::cout << bytesToFloat(keys[i]) << ",";
            }
            std::cout << ") ";
        }
        std::cout << "] ";
    }
    std::cout<<std::endl;
}

std::pair<Record*, Record*> BPlusTree :: findRecordsInRange(unsigned short int key1, unsigned short int key2) {
    int numIndexBlocksAccessed = 1;

    Node* curr = root;
    Node* prev;
    // INCREMENT LOAD COUNTER
    Record* start;
    Record* end = NULL;

    // Search the B+ Tree
    for (int i = 0; i < height-1; i++) {
        for (int j = 0; j < curr->numKeysInserted; j++) {
            if (key1 < curr->keys[j]) {
                // INCREMENT LOAD COUNTER
                curr = ((InternalNode*) curr)->children[j];
                break;
            }
        }
        // If next hasn't been assigned, means it refers to the last node pointer
        (curr == NULL) && (curr = ((InternalNode*) curr)->children[curr->numKeysInserted]); // INCREMENT LOAD COUNTER
        numIndexBlocksAccessed += 1;
    }

    // Search the Leaf Node to find the Start Record
    for (int j = 0; j < curr->numKeysInserted; j++) {
        if (key1 <= curr->keys[j]) {
            start = ((LeafNode*) curr)->records[j];
            break;
        }
    }

    // Search the curr Datablock and subsequent datablock to find End Record
    while (curr != NULL) {
        for (int j = 0; j < curr->numKeysInserted; j++) {
            if (key2 == curr->keys[j]) {
                end = ((LeafNode*) curr)->records[j];
                break;
            } else if (key2 < curr->keys[j] && j == 0) {
                end = ((LeafNode*) prev)->records[prev->numKeysInserted-1];
                break;
            } else if (key2 < curr->keys[j]) {
                end = ((LeafNode*) curr)->records[j-1];
                break;
            }
        }
        if (end != NULL) {
            break;
        }

        prev = curr;
        // INCREMENT LOAD COUNTER IF NEXT NOT NULL
        curr = ((LeafNode*) curr)->next;
        if (curr != NULL) {
            numIndexBlocksAccessed += 1;
        }
    }

    // Probably means that the upper bound is larger than all keys in the index
    if (end == NULL) {
        end = ((LeafNode*) prev)->records[prev->numKeysInserted-1];
    }

    cout << "Number of Index Nodes Accessed: " << numIndexBlocksAccessed << endl;

    return std::make_pair(start, end);
}