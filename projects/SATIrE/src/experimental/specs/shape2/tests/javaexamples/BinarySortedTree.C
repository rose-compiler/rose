#define NULL ((BinarySortedTree*)0)

using namespace std;

// implementation of operation for a binary sorted tree
class BinarySortedTree {
  public: 
  int key;
  BinarySortedTree *left;
  BinarySortedTree *right;
};

// in an existing tree a new element is inserted
// iterative method
BinarySortedTree *InsertInTree(BinarySortedTree *tree, BinarySortedTree *newelement) {
	BinarySortedTree *dummy = tree;
	BinarySortedTree *prev = tree;

	if (dummy == NULL) {
	  dummy = new BinarySortedTree();
	  dummy = newelement;
	  tree = dummy;
	} else {
    while (dummy !=  NULL) {
      if (dummy->key > newelement->key) {
        prev = dummy;
        dummy = dummy->left;
      } else if (dummy->key < newelement->key) {
        prev = dummy;
        dummy = dummy->right;
      }
    } // while (dummy != NULL)

    if (prev->key > newelement->key) {
      prev->left = new BinarySortedTree();
      prev->left->key = newelement->key;
      prev->left->left = NULL;
      prev->left->right = NULL;
    } else if (prev->key < newelement->key) {
      prev->right = new BinarySortedTree();
      prev->right->key = newelement->key;
      prev->right->left = NULL;
      prev->right->right = NULL;
    }
  }
	return tree;
}

// a binary sorted tree is created
BinarySortedTree *CreateTree() {
	BinarySortedTree *newelement;
	BinarySortedTree *resulttree;

	newelement = new BinarySortedTree();
	newelement->key = 10;
	newelement->right = NULL;
	newelement->left = NULL;
	resulttree = InsertInTree(NULL,newelement);
	
 	newelement = new BinarySortedTree();
	newelement->key = 5;
	newelement->right = NULL;
	newelement->left = NULL;
	resulttree = InsertInTree(resulttree,newelement);

 	newelement = new BinarySortedTree();
	newelement->key = 1;
	newelement->right = NULL;
	newelement->left = NULL;
	resulttree = InsertInTree(resulttree,newelement);

	newelement = new BinarySortedTree();
	newelement->key = 8;
	newelement->right = NULL;
	newelement->left = NULL;
	resulttree = InsertInTree(resulttree,newelement);

	newelement = new BinarySortedTree();
	newelement->key = 14;
	newelement->right = NULL;
	newelement->left = NULL;
	resulttree = InsertInTree(resulttree,newelement);

	newelement = new BinarySortedTree();
	newelement->key = 15;
	newelement->right = NULL;
	newelement->left = NULL;
	resulttree = InsertInTree(resulttree,newelement);

	newelement = new BinarySortedTree();
	newelement->key = 7;
	newelement->right = NULL;
	newelement->left = NULL;
	resulttree = InsertInTree(resulttree,newelement);

	newelement = new BinarySortedTree();
	newelement->key = 11;
	newelement->right = NULL;
	newelement->left = NULL;
	resulttree = InsertInTree(resulttree,newelement);

	return resulttree;
}

// shows the binary sorted tree 
// recursive output
void ShowTree(BinarySortedTree *tree) {
	if (tree != NULL) {
    //cout << tree->key << endl;
    //cout << endl;
	  ShowTree(tree->left);
	  ShowTree(tree->right);
	}
}


// an element will be removed from a given tree
// iterative method
BinarySortedTree *RemoveFromTree(BinarySortedTree *tree, BinarySortedTree *element) {
	BinarySortedTree *dummy  = tree;
	BinarySortedTree *search = tree;
	BinarySortedTree *act    = tree;
  BinarySortedTree *prev   = tree;
	int side = 0;

	// Search the element to be removed
	while (search != NULL && search->key != element->key) {
	  if (element->key < search->key) {
	  	dummy = search;
	  	search = search->left;
	  	side = 1;
	  } else if (element->key > search->key) {
      dummy = search;
      search = search->right;
      side = 2;
	  }
	} // (while (search != NULL && search->key != element->key)

	// no element has been found
	if (search == NULL) {
	  //cout << "no element found" << endl;
  }

	// if the element is the root
	else if (search->key == element->key && side == 0) {
    //search for the new element
	  if (search->left == NULL && search->right == NULL) {
		  dummy = NULL;
    } else if (search->left == NULL && search->right != NULL) {
		  dummy = search->right;
    } else if (search->left != NULL && search->right == NULL) {
		  dummy = search->left;
    } else if (search->left != NULL && search->right != NULL) {
      prev = search; // first one step left
      search = search->left;
      if (search != NULL) {
        while (search->left != NULL || search->right != NULL) {
          if (search->right != NULL) {
            prev = search;
            search = search->right;
          } else {
            prev = search;
            search = search->left;
          }
        }// while (search->left != NULL->->
      }// if (search != NULL)
      dummy->key = search->key;
      if (search->key == prev->left->key) {
        prev->left = NULL;
      } else {
        prev->right = NULL;
      }
    } // else if (search->left != NULL && search->right != NULL)
	}
  // if the element is on the left side of dummy-father
  else if (search->key == element->key && side == 1) {
    if (search->left == NULL && search->right == NULL) {
      dummy->left = NULL;
    } else if (search->left == NULL && search->right != NULL) {
      dummy->left = search->right;
    } else if (search->left != NULL && search->right == NULL) {
      dummy->left = search->left;
    } else if (search->left != NULL && search->right != NULL) {
      prev = search; // first one step left
      search = search->left;
      if (search != NULL) {
        while (search->left != NULL || search->right != NULL) {
          if (search->right != NULL) {
            prev = search;
            search = search->right;
          } else {
            prev = search;
            search = search->left;
          }
        }// while (search->left != NULL->->
      }// if (search != NULL)
      dummy->left->key = search->key;
      if (search->key == prev->left->key) {
        prev->left = NULL;
      } else {
        prev->right = NULL;
      }
    } // else if (search->left != NULL && search->right != NULL)
  }
	// if the element is on the right side of the dummy-father
	else if (search->key == element->key && side == 2) {
	  if (search->left == NULL && search->right == NULL) {
		  dummy->right = NULL;
    } else if (search->left == NULL && search->right != NULL) {
		  dummy->right = search->right;
    } else if (search->left != NULL && search->right == NULL) {
		  dummy->right = search->left;
    } else if (search->left != NULL && search->right != NULL) {
		  prev = search; 
		  search = search->left; // first one step left
		  if (search != NULL) {
		    while (search->left != NULL && search->right != NULL) {
		      if (search->right != NULL) {
		        prev = search;
		        search = search->right;
		      } else {
            prev = search;
            search = search->left;
          } // if (search->right != NULL)
        }// while (search->left != NULL->->
      }// if (search != NULL)
      dummy->right->key = search->key;
      if (search->key == prev->left->key) {
        prev->left = NULL;
      } else {
        prev->right = NULL;
      }
    } // else if (search->left != NULL && search->right != NULL)
  }
  return tree;
}


    
int main(int argc, char **argv) {
	BinarySortedTree *newelement = new BinarySortedTree();
	newelement->key = 20;
	newelement->right = NULL;
	newelement->left = NULL;
	
  BinarySortedTree *removeelement = new BinarySortedTree();
	removeelement->key =8;
	removeelement->right = NULL;
	removeelement->left = NULL;

	BinarySortedTree *resulttree = CreateTree();
  //cout << "CreateTree" << endl;
	ShowTree(resulttree);

	resulttree = InsertInTree(resulttree,newelement);
  //cout << "InsertInTree: " << newelement->key << endl;
	ShowTree(resulttree);

	resulttree = RemoveFromTree(resulttree,removeelement);
	//cout << "RemoveFromTree: " << removeelement->key << endl;
	ShowTree(resulttree);

  return 0;
}
