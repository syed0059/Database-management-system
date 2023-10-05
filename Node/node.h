#include "../Block Manager/blockManager.h"


#define NUM_KEYS 39


typedef struct Node {
    // Given the size of the key and the size of a pointer, the number of keys/pointers should be 39
    float keys[NUM_KEYS];
    int numKeysInserted;
};


typedef struct InternalNode: Node {
    Node* children[NUM_KEYS+1];
    int numChildrenNodes;

    // Added Parent Pointer so that I can reference the parent for insertion to update
    struct InternalNode* parent = NULL;

    /**
     * @brief Merge current node with a sibling internal node on the right
     *        Merging is done right after deletion & shifting of key + ptr
     * 
     * @param right Sibling internal node on the right of this node
    */
    void merge(InternalNode* right);    


    /**
     * @brief Borrow last pointer & key from sibling internal node on the left
     * 
     * @param left Sibling internal node on the left of this node
    */
    void borrowLeft(InternalNode* left);


    /**
     * @brief Borrow first pointer & key from sibling internal node on the right
     * 
     * @param left Sibling internal node on the right of this node; 
    */
    void borrowRight(InternalNode* right);
};


typedef struct LeafNode: Node {
    Record* records[NUM_KEYS];
    int numRecordsInserted;
    LeafNode* next;

    // Added Parent Pointer so that I can reference the parent for insertion to update
    InternalNode* parent = NULL;

    /**
     * @brief Merge current node with a sibling leaf node on the right
     *        Merging is done right after deletion & shifting of key + ptr
     * 
     * @param right Sibling leaf node on the right of this node
    */
    void merge(LeafNode* right);


    /**
     * @brief Borrow last pointer & key from sibling leaf node on the left
     * 
     * @param left Sibling leaf node on the left of this node; 
    */
    void borrowLeft(LeafNode* left);


    /**
     * @brief Borrow first pointer & key from sibling leaf node on the right
     * 
     * @param left Sibling leaf node on the right of this node; 
    */
    void borrowRight(LeafNode* right);
};