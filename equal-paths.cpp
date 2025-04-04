#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)
#include<iostream>
#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here
bool IfEqualPaths(Node * node, int depth, int& LeafDepth){
	if (node == NULL){
		return true; 
	}

	if (node->left == NULL && node->right == NULL){
		if(LeafDepth == -1 ){
			LeafDepth = depth;
		}
		return depth == LeafDepth; 
	}

	bool RightSideCheck = IfEqualPaths(node->right, depth + 1, LeafDepth);
	bool LeftSideCheck = IfEqualPaths(node->left, depth + 1, LeafDepth);

	return RightSideCheck && LeftSideCheck;
}


bool equalPaths(Node * root)
{
    // Add your code below
	int LeafDepth = -1; 
	return IfEqualPaths(root, 0, LeafDepth); 
}

