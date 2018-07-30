#ifndef WET_1_AVLTREE_H
#define WET_1_AVLTREE_H
#include <iostream>
#include <ostream>
#include <algorithm>
#define ZERO 0

template <class T>
class Node {
private:
	T data;
	int height;
	Node* left;
	Node* right;

public:
	Node(const T& value) : data(value) {
        this->left = NULL;
        this->right = NULL;
        this->height = 0;
    };
	~Node() {};

    //--------------------------------------------------------------------------

	T& getValue() {
        return this->data;
    };

    //--------------------------------------------------------------------------

	void setValue(Node* root) {
        this->data = root->data;
    };

    //--------------------------------------------------------------------------

	void setLeft(Node* left) {
        this->left = left;
    };

    //--------------------------------------------------------------------------

	void setRight(Node* right){
        this->right = right;
    };

    //--------------------------------------------------------------------------

	void setHeight(int height) {
        this->height = height;
    };

    //--------------------------------------------------------------------------

	Node* getLeft() const {
        return this->left;
    };

    //--------------------------------------------------------------------------

	Node* getRight() const {
        return this->right;
    };

    //--------------------------------------------------------------------------

	int getHeight() const {
		if (!this)
		{
			return -1;
		}
		return this->height;};

    //--------------------------------------------------------------------------

	void print() const {
        std::cout << data << std::endl;
    };
};

//-----------------------------------------------------------------------------

template <class T, class func>
class AvlTree   {
private:
    Node<T>* root;
	func compare;

    //--------------------------------------------------------------------------

	/*
	* Remove nodes recursively from tree
	* (operates as long as nodes exist in the route)
	*/
	void removeNode(Node <T> *node) {
		if (node) {
			removeNode(node->getLeft());
			removeNode(node->getRight());
			delete node;
		}
	};

    //--------------------------------------------------------------------------

    Node<T>* insertToMiddle(Node<T>* root, T value) {
        //Task #1 : find new node's location + insert
        //Case #1 : location founded - insert here!
        if (!root)  {
            Node<T>* new_node = new Node<T>(value);
            root = new_node;
            if (!new_node)  {
                //Case : memory allocation error
                return NULL;
            }
        }

        //Case #2 : value is too small - keep searching in left subtree
        else if (compare(value,root->getValue())==-1)   {
            root->setLeft(insertToMiddle(root->getLeft(), value));
            //Task #2 (back from Recursion - following route)
            //Check AVL balancing
            if (balanceFactor(root) == 2)   {
                int bf_left = balanceFactor(root->getLeft());
                if (bf_left < 0)    {
                    //LR
                    root = rightLeftRotate(root);
                }
                else    {
                    //LL
                    root = rotateRight(root);
                }
            }
        }

        //Case #3 : value is too big - keep searching in right subtree
        else if (compare(value , root->getValue())==1)   {
            root->setRight(insertToMiddle(root->getRight(), value));
            //Task #2 (back from Recursion - following route)
            //Check AVL balancing
            if (balanceFactor(root) == -2)  {
                int bf_right = balanceFactor(root->getRight());
                if (bf_right > 0)   {
                    //RL
                    root = leftRightRotate(root);
                }
                else    {
                    //RR
                    root = rotateLeft(root);
                }
            }
        }

		//Task #3 : both trees balanced
		//Update height and finish algorithm
        int new_height = 0;
        if (root->getLeft() && root->getRight())
            new_height = 1 + std::max(root->getLeft()->getHeight(), root->getRight()->getHeight());
        else if (root->getLeft())
            new_height = 1 + root->getLeft()->getHeight();
        else if (root->getRight())
            new_height = 1 + root->getRight()->getHeight();
		root->setHeight(new_height);
        return root;
	};

	//--------------------------------------------------------------------------

    Node<T>* removeAux(Node<T>* root, const T& value) {
        //Task #1 : Binary search algorithm on Binary tree.
        //Case #1 : unable to find element
        if (!root)  {
            return NULL;
        }

		Node<T>* temp;
		//Case #2 : value is in the left subtree.
		if (compare(value , root->getValue())==-1) {
			root->setLeft(removeAux(root->getLeft(), value));
		}

		//Case #3 : value is in the right subtree.
		else if (compare(value , root->getValue())==1) {
                root->setRight(removeAux(root->getRight(), value));
            }

        //Case #4 : value found - back from recursion on the same route.
        //Task #2 : check kids and handle deletion accordingly
		else if (root->getLeft() && root->getRight()) {
            //Case #1 : element has 2 kids. In this case, switch the minimal
            //data that is bigger than the deleted data with it.
            temp = findMin(root->getRight());
            root->setValue(temp);
            root->setRight(removeAux(root->getRight(), root->getValue()));
        }

        //Case #2 : Otherwise-deleted node already has 1 or 0 kids.
        //NOTE : we will get to it sooner or later anyway.
        else {
            temp = root;
            if (!root->getLeft())
                root = root->getRight();
            else if (!root->getRight())
                root = root->getLeft();
          //  delete temp;
        }

        if (!root) return root;     //0 kids case.

        //Update height after deletion
        int kids_height = 1 + std::max(root->getLeft()->getHeight(), root->getRight()->getHeight());
        root->setHeight(kids_height);

        //Task #3 : Handle AVL balancing
        if (balanceFactor(root) == 2) {
            if (balanceFactor(root->getLeft()) >=0) {
                //LL
                root = rotateRight(root);
            }
            else {
                //LR
                root = rightLeftRotate(root);

            }
        }
        if (balanceFactor(root) == -2) {
            if (balanceFactor(root->getRight()) <= 0) {
                //RR
                root = rotateLeft(root);
            }
            else {
                //RL
                root = leftRightRotate(root);
            }
        }
        return root;
	};

	//--------------------------------------------------------------------------

	/*
	input: root of Avl tree
	output: the height of current tree
	(max between left and right son +1)
	*/
    int height(Node<T>* root) const {
		if (root != NULL) {
			int left = height(root->getLeft());
			int right = height(root->getRight());
			return (((left > right) ? left : right) + 1);
		}
		return 0;
	};

	//--------------------------------------------------------------------------

    Node<T>* rotateLeft(Node<T> *root) {
		//Rotate tree to the left
		Node<T>* future_root = root->getRight();
		root->setRight(future_root->getLeft());
		future_root->setLeft(root);

        //Update height accordingly
        root->setHeight(std::max(root->getLeft()->getHeight(), root->getRight()->getHeight()) + 1);
        future_root->setHeight(std::max(root->getRight()->getHeight(), root->getHeight()) + 1);

        //Return new root
        return future_root;
	};

	//--------------------------------------------------------------------------

    Node<T>* rotateRight(Node<T>* root) {
		//Rotate tree to the right
		Node<T>* temp = root->getLeft();
		root->setLeft(temp->getRight());
		temp->setRight(root);

        //Update height accordingly
        root->setHeight(std::max(root->getLeft()->getHeight(), root->getRight()->getHeight()) + 1);
        temp->setHeight(std::max(temp->getLeft()->getHeight(), root->getHeight()) + 1);

        //Return new root
        return temp;
	};

	//--------------------------------------------------------------------------

    Node<T>* leftRightRotate(Node<T>* root)
	{
		root->setRight(rotateRight(root->getRight()));
		return rotateLeft(root);
	};

	//--------------------------------------------------------------------------

    Node<T>* rightLeftRotate(Node<T> *root) {
		root->setLeft(rotateLeft(root->getLeft()));
		return rotateRight(root);
	};

	//--------------------------------------------------------------------------

    Node<T>* findMin(Node<T> *root) {
		//Empty tree - return NULL
		if (!root) {
			return NULL;
		}
		else {
			//No left subtree - root is the minimum
			if (!root->getLeft()) {
				return root;
			}
			//There is a subtree - keep searching for minimum
			else {
				return findMin(root->getLeft());
			}
		}
	};

	//--------------------------------------------------------------------------

    Node<T>* findMax(Node<T> *root) {
		//Empty tree - return NULL
		if (!root) {
			return NULL;
		}
		else {
			//No right subtree - root is the maximum
			if (!root->getRight()) {
				return root;
			}
			//There is a subtree - keep searching for maximum
			else {
				return findMax(root->getRight());
			}
		}
	};
	//--------------------------------------------------------------------------

public:
    AvlTree(bool flag=0) : root(NULL), compare(flag) {};      //DONE
	~AvlTree()
	{
		if (root) {
			removeNode(root);
		}
	};

    //--------------------------------------------------------------------------

    Node<T>* insert(const T &value) {
        if (!find(this->root, value))    {
            //Case : insertion to tree (requires search and balancing)
            root = insertToMiddle(this->root, value);
			return root;
        }
		return find(this->root, value);
	};

	//--------------------------------------------------------------------------

    Node<T>* remove(const T& value) {
		//Case #1 : empty tree
		if (!this->root) {
			return NULL;
		}

		//Case #2 : handle deletion recursively.
		root = removeAux(this->root, value);
		return root;
	};

	//--------------------------------------------------------------------------

	Node<T>* getRoot() const {
        return root;
    };

	//--------------------------------------------------------------------------

    Node<T>* find(Node<T>* root, const T& value) const {
		//Binary search algorithm on Binary tree.
		if (root != NULL)
		{
			if (compare(root->getValue() , value)==0) {
				return root;
			}
			else if (compare(root->getValue() , value)==1) {
				return find(root->getLeft(), value);
			}
			else {
				return find(root->getRight(), value);
			}
		}
		//Case : value not found in tree.
		return NULL;
	};

	//--------------------------------------------------------------------------

	/*
	input: root of Avl tree
	output: the balance factor of the current tree (left son height - right son height)
	*/
    int balanceFactor(Node<T>* root) const {
		if (root != NULL) {
			return (root->getLeft()->getHeight() - root->getRight()->getHeight());
		}
		return 0;
	};

	//--------------------------------------------------------------------------

	/*
	input: root of Avl tree
	process: the function will go recursivley and print
	the Avl tree inorder(left son than current root then right son)
	for binaric tree inorder will print the tree sorted
	*/
    void printInOrder(Node<T>* root) const {
		if (root != NULL) {
			printInOrder(root->getLeft());
			root->print();
			printInOrder(root->getRight());
		}
	};

    //--------------------------------------------------------------------------

    int treeSize()const {
        return treeSizeHelper(this->getRoot());
    }

    //--------------------------------------------------------------------------

    int treeSizeHelper(Node<T>* root)const {
        if (root == NULL) { return ZERO; }
        return (treeSizeHelper(root->getLeft()) + treeSizeHelper(root->getRight()) + 1);
    }

    //--------------------------------------------------------------------------

	void printTreeToArray(T* arr, Node<T>* root, int* index) {
		if (root != NULL)
		{
			printTreeToArray(arr, root->getLeft(), index);
			T val = root->getValue();
			(arr)[(*index)] = val;
			(*index)++;
			printTreeToArray(arr,root->getRight(),index);
		}

	}

	//--------------------------------------------------------------------------

	void makeAvlFromArray(T arr[], int left, int right)	{
        this->root = sortArrayToAvl(arr, left, right);
	}

	Node<T>* sortArrayToAvl(T arr[], int left, int right)	{
		if (left > right)	{
			return NULL;
		}

		int middle = (left + right) / 2;
		Node<T>* root = new Node<T>(arr[middle]);

		root->setLeft(sortArrayToAvl(arr, left, middle-1));

		root->setRight(sortArrayToAvl(arr, middle+1, right));

		return root;
	}


};

#endif //WET_1_AVLTREE_H
