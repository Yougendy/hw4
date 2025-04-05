#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    void rotateLeft(AVLNode<Key, Value>* node);
    void rotateRight(AVLNode<Key, Value>* node);
    void rebalanceAfterInsert(AVLNode<Key, Value>* node);
    void rebalanceAfterRemove(AVLNode<Key, Value>* node);
    AVLNode<Key, Value>* internalInsert(const std::pair<const Key, Value>& new_item);


};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    AVLNode<Key, Value>* inserted = internalInsert(new_item);
    if (inserted == nullptr) return; // value already existed and was updated

    rebalanceAfterInsert(inserted->getParent());
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    Node<Key, Value>* node = this->internalFind(key);
    if (node == nullptr) return;

    AVLNode<Key, Value>* avlNode = static_cast<AVLNode<Key, Value>*>(node);

    // Swap with predecessor if necessary
    if (avlNode->getLeft() && avlNode->getRight()) {
        Node<Key, Value>* pred = this->predecessor(avlNode);
        this->nodeSwap(avlNode, static_cast<AVLNode<Key, Value>*>(pred));
    }

    AVLNode<Key, Value>* parent = avlNode->getParent();
    AVLNode<Key, Value>* child = avlNode->getLeft() ? avlNode->getLeft() : avlNode->getRight();

    if (child) child->setParent(parent);

    if (parent == nullptr) {
        this->root_ = child;
    } else if (parent->getLeft() == avlNode) {
        parent->setLeft(child);
    } else {
        parent->setRight(child);
    }

    delete avlNode;
    rebalanceAfterRemove(parent);
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}

template<class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::internalInsert(const std::pair<const Key, Value>& new_item)
{
    if (this->root_ == nullptr) {
        this->root_ = new AVLNode<Key, Value>(new_item.first, new_item.second, nullptr);
        return static_cast<AVLNode<Key, Value>*>(this->root_);
    }

    AVLNode<Key, Value>* curr = static_cast<AVLNode<Key, Value>*>(this->root_);
    AVLNode<Key, Value>* parent = nullptr;

    while (curr != nullptr) {
        parent = curr;
        if (new_item.first < curr->getKey()) {
            curr = curr->getLeft();
        } else if (new_item.first > curr->getKey()) {
            curr = curr->getRight();
        } else {
            curr->setValue(new_item.second);
            return nullptr;
        }
    }

    AVLNode<Key, Value>* newNode = new AVLNode<Key, Value>(new_item.first, new_item.second, parent);
    if (new_item.first < parent->getKey()) {
        parent->setLeft(newNode);
    } else {
        parent->setRight(newNode);
    }

    return newNode;
}

template<class Key, class Value>
void AVLTree<Key, Value>::rebalanceAfterInsert(AVLNode<Key, Value>* node)
{
    while (node != nullptr) {
        int8_t balance = node->getBalance();
        if (node->getLeft() == nullptr && node->getRight() != nullptr)
            node->updateBalance(-1);
        else if (node->getRight() == nullptr && node->getLeft() != nullptr)
            node->updateBalance(1);
        else if (node->getLeft() != nullptr && node->getRight() == nullptr &&
                 node->getLeft()->getKey() < node->getKey())
            node->updateBalance(1);
        else if (node->getRight() != nullptr && node->getLeft() == nullptr &&
                 node->getRight()->getKey() > node->getKey())
            node->updateBalance(-1);

        balance = node->getBalance();
        if (balance == 0) {
            break;
        } else if (balance == -2) {
            if (node->getRight()->getBalance() == 1)
                rotateRight(node->getRight());
            rotateLeft(node);
            break;
        } else if (balance == 2) {
            if (node->getLeft()->getBalance() == -1)
                rotateLeft(node->getLeft());
            rotateRight(node);
            break;
        }

        node = node->getParent();
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::rebalanceAfterRemove(AVLNode<Key, Value>* node)
{
    while (node != nullptr) {
        int8_t balance = node->getBalance();

        int8_t leftHeight = node->getLeft() ? node->getLeft()->getBalance() : -1;
        int8_t rightHeight = node->getRight() ? node->getRight()->getBalance() : -1;
        node->setBalance(leftHeight - rightHeight);

        balance = node->getBalance();

        if (balance == -2) {
            AVLNode<Key, Value>* rightChild = node->getRight();
            if (rightChild->getBalance() == 1)
                rotateRight(rightChild);
            rotateLeft(node);
        } else if (balance == 2) {
            AVLNode<Key, Value>* leftChild = node->getLeft();
            if (leftChild->getBalance() == -1)
                rotateLeft(leftChild);
            rotateRight(node);
        }

        node = node->getParent();
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* x)
{
    AVLNode<Key, Value>* y = x->getRight();
    x->setRight(y->getLeft());
    if (y->getLeft()) y->getLeft()->setParent(x);
    y->setParent(x->getParent());

    if (!x->getParent()) {
        this->root_ = y;
    } else if (x == x->getParent()->getLeft()) {
        x->getParent()->setLeft(y);
    } else {
        x->getParent()->setRight(y);
    }

    y->setLeft(x);
    x->setParent(y);

    x->setBalance(0);
    y->setBalance(0);
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* y)
{
    AVLNode<Key, Value>* x = y->getLeft();
    y->setLeft(x->getRight());
    if (x->getRight()) x->getRight()->setParent(y);
    x->setParent(y->getParent());

    if (!y->getParent()) {
        this->root_ = x;
    } else if (y == y->getParent()->getLeft()) {
        y->getParent()->setLeft(x);
    } else {
        y->getParent()->setRight(x);
    }

    x->setRight(y);
    y->setParent(x);

    y->setBalance(y->getBalance() - 1 - std::max<int8_t>(x->getBalance(), static_cast<int8_t>(0)));
    x->setBalance(x->getBalance() - 1 + std::min<int8_t>(y->getBalance(), static_cast<int8_t>(0)));
}



#endif
