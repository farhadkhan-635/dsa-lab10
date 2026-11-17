// Name    : Farhad Khan
// Reg No  : 549034
// Section : BSCS-15E
// Lab 10, Task 1: BST deletion

#include <iostream>
using namespace std;

struct Node
{
    int data;
    Node* left;
    Node* right;
    int height;    // not used in this task, it is needed for the AVL tasks
};

// new node: no children, height 1
Node* createNode(int value)
{
    Node* node = new Node;
    node->data = value;
    node->left = nullptr;
    node->right = nullptr;
    node->height = 1;
    return node;
}

// normal BST insertion, a duplicate key is ignored
Node* insertBST(Node* root, int value)
{
    if (root == nullptr)
    {
        return createNode(value);
    }
    if (value < root->data)
    {
        root->left = insertBST(root->left, value);
    }
    else if (value > root->data)
    {
        root->right = insertBST(root->right, value);
    }
    return root;
}

// the inorder successor of a node with two children is the smallest key in
// its right subtree, which is the leftmost node of that subtree
Node* findMin(Node* root)
{
    Node* curr = root;
    while (curr != nullptr && curr->left != nullptr)
    {
        curr = curr->left;
    }
    return curr;
}

// deletes value from the tree and returns the root of the updated subtree.
// The caller must store the result back (root = deleteBST(root, value)),
// because the root itself can change, for example when it is deleted.
Node* deleteBST(Node* root, int value)
{
    // empty subtree: the key is not in the tree, so nothing changes.
    // This also covers deletion from a completely empty tree.
    if (root == nullptr)
    {
        cout << "Key " << value << " not found, tree not changed" << endl;
        return nullptr;
    }

    if (value < root->data)
    {
        root->left = deleteBST(root->left, value);     // reconnect what comes back
    }
    else if (value > root->data)
    {
        root->right = deleteBST(root->right, value);
    }
    else
    {
        // this node holds the key, there are three cases

        // case 1: leaf, just remove it
        if (root->left == nullptr && root->right == nullptr)
        {
            cout << "Deleted " << value << " (case 1: leaf node)" << endl;
            delete root;
            return nullptr;               // the parent's pointer becomes nullptr
        }

        // case 2: one child, that child takes the place of the node
        if (root->left == nullptr)
        {
            Node* child = root->right;
            cout << "Deleted " << value << " (case 2: one child, " << child->data
                 << " takes its place)" << endl;
            delete root;
            return child;
        }
        if (root->right == nullptr)
        {
            Node* child = root->left;
            cout << "Deleted " << value << " (case 2: one child, " << child->data
                 << " takes its place)" << endl;
            delete root;
            return child;
        }

        // case 3: two children. Copy the inorder successor into this node
        // (the order of the tree stays correct), then delete the successor
        // from the right subtree. The successor has no left child, so that
        // second deletion is always case 1 or case 2.
        Node* successor = findMin(root->right);
        cout << "Node " << value << " has two children (case 3), inorder successor is "
             << successor->data << ", copying it up" << endl;
        root->data = successor->data;
        root->right = deleteBST(root->right, successor->data);
    }

    return root;   // this subtree keeps the same root
}

// ---------------------------------------------------------------- helpers

void inorder(Node* root)
{
    if (root == nullptr)
    {
        return;
    }
    inorder(root->left);
    cout << root->data << " ";
    inorder(root->right);
}

void preorder(Node* root)
{
    if (root == nullptr)
    {
        return;
    }
    cout << root->data << " ";
    preorder(root->left);
    preorder(root->right);
}

// the tree turned on its side: root at the left, right subtree on top,
// left subtree at the bottom
void printSideways(Node* root, int depth)
{
    if (root == nullptr)
    {
        return;
    }
    printSideways(root->right, depth + 1);
    for (int i = 0; i < depth; i++)
    {
        cout << "      ";
    }
    cout << root->data << endl;
    printSideways(root->left, depth + 1);
}

// fills out[] with the keys in inorder or preorder
const int MAX_KEYS = 64;
void collect(Node* root, bool inOrder, int* out, int& n)
{
    if (root == nullptr)
    {
        return;
    }
    if (!inOrder)
    {
        out[n++] = root->data;
    }
    collect(root->left, inOrder, out, n);
    if (inOrder)
    {
        out[n++] = root->data;
    }
    collect(root->right, inOrder, out, n);
}

// prints one traversal next to its prediction and says if they match
bool compareTraversal(const char* label, Node* root, bool inOrder, const int* expected, int expectedN)
{
    int actual[MAX_KEYS];
    int n = 0;
    collect(root, inOrder, actual, n);

    bool same = (n == expectedN);
    for (int i = 0; same && i < n; i++)
    {
        if (actual[i] != expected[i])
        {
            same = false;
        }
    }

    cout << label << " actual    : ";
    for (int i = 0; i < n; i++)
    {
        cout << actual[i] << " ";
    }
    cout << endl << label << " predicted : ";
    for (int i = 0; i < expectedN; i++)
    {
        cout << expected[i] << " ";
    }
    cout << endl;
    return same;
}

// shows the tree and checks both traversals against the predictions
void showTree(Node* root, const int* inExp, int inN, const int* preExp, int preN)
{
    if (root == nullptr)
    {
        cout << "(empty tree)" << endl;
    }
    else
    {
        printSideways(root, 0);
    }
    bool a = compareTraversal("Inorder  ", root, true, inExp, inN);
    bool b = compareTraversal("Preorder ", root, false, preExp, preN);
    cout << "Matches prediction: " << ((a && b) ? "yes" : "NO") << endl;
}

void freeTree(Node* root)
{
    if (root == nullptr)
    {
        return;
    }
    freeTree(root->left);
    freeTree(root->right);
    delete root;
}

int main()
{
    // hand drawn tree after inserting 50 30 70 20 40 60 80 65:
    // 50 is the root. Its left child is 30, which has children 20 and 40.
    // Its right child is 70, which has children 60 and 80,
    // and 60 has one right child, 65.
    int values[] = {50, 30, 70, 20, 40, 60, 80, 65};
    Node* root = nullptr;
    for (int i = 0; i < 8; i++)
    {
        root = insertBST(root, values[i]);
    }

    cout << "TREE AFTER INSERTING 50 30 70 20 40 60 80 65" << endl;
    int in0[] = {20, 30, 40, 50, 60, 65, 70, 80};
    int pre0[] = {50, 30, 20, 40, 70, 60, 65, 80};
    showTree(root, in0, 8, pre0, 8);

    // prediction: 20 is a leaf (case 1), it is simply removed
    cout << endl << "DELETE 20 (prediction: leaf, case 1)" << endl;
    root = deleteBST(root, 20);
    int in1[] = {30, 40, 50, 60, 65, 70, 80};
    int pre1[] = {50, 30, 40, 70, 60, 65, 80};
    showTree(root, in1, 7, pre1, 7);

    // prediction: 60 has one child (65), case 2, 65 takes the place of 60
    cout << endl << "DELETE 60 (prediction: one child, case 2)" << endl;
    root = deleteBST(root, 60);
    int in2[] = {30, 40, 50, 65, 70, 80};
    int pre2[] = {50, 30, 40, 70, 65, 80};
    showTree(root, in2, 6, pre2, 6);

    // prediction: 50 is the root with two children, case 3. The smallest key
    // in its right subtree is 65, so 65 becomes the new root.
    cout << endl << "DELETE 50 (prediction: root with two children, case 3)" << endl;
    root = deleteBST(root, 50);
    int in3[] = {30, 40, 65, 70, 80};
    int pre3[] = {65, 30, 40, 70, 80};
    showTree(root, in3, 5, pre3, 5);

    // a key that is not in the tree must change nothing
    cout << endl << "DELETE 99 (missing key, tree must stay the same)" << endl;
    root = deleteBST(root, 99);
    showTree(root, in3, 5, pre3, 5);

    // root with only one child: the child becomes the new root
    cout << endl << "TEST: root with one child (tree 10, 20), delete 10" << endl;
    Node* t1 = nullptr;
    t1 = insertBST(t1, 10);
    t1 = insertBST(t1, 20);
    t1 = deleteBST(t1, 10);
    int in4[] = {20};
    showTree(t1, in4, 1, in4, 1);

    // two children where the successor itself has a child:
    // successor of 50 is 60, and 60 has the right child 65
    cout << endl << "TEST: successor with a child (tree 50 30 70 60 80 65), delete 50" << endl;
    Node* t2 = nullptr;
    int v2[] = {50, 30, 70, 60, 80, 65};
    for (int i = 0; i < 6; i++)
    {
        t2 = insertBST(t2, v2[i]);
    }
    t2 = deleteBST(t2, 50);
    int in5[] = {30, 60, 65, 70, 80};
    int pre5[] = {60, 30, 70, 65, 80};
    showTree(t2, in5, 5, pre5, 5);

    // deletion from an empty tree
    cout << endl << "TEST: deletion from an empty tree" << endl;
    Node* empty = nullptr;
    empty = deleteBST(empty, 10);
    cout << "Root is nullptr: " << (empty == nullptr ? "yes" : "no") << endl;

    // deleting the only node must give back nullptr
    cout << endl << "TEST: deleting the only node of a tree" << endl;
    Node* single = createNode(10);
    single = deleteBST(single, 10);
    cout << "Root is nullptr: " << (single == nullptr ? "yes" : "no") << endl;

    // release everything that is left
    freeTree(root);
    freeTree(t1);
    freeTree(t2);
    return 0;
}
