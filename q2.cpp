// Name    : Farhad Khan
// Reg No  : 549034
// Section : BSCS-15E
// Lab 10, Task 2: AVL insertion and rotations

#include <iostream>
using namespace std;

struct Node
{
    int data;
    Node* left;
    Node* right;
    int height;    // height of the subtree that starts at this node
};

// new node: no children, height 1 (a leaf)
Node* createNode(int value)
{
    Node* node = new Node;
    node->data = value;
    node->left = nullptr;
    node->right = nullptr;
    node->height = 1;
    return node;
}

// ------------------------------------------------- height and balance

// an empty subtree has height 0, so a leaf (1 + max(0, 0)) has height 1
int height(Node* node)
{
    return (node == nullptr) ? 0 : node->height;
}

int maxOf(int a, int b)
{
    return (a > b) ? a : b;
}

// recalculates the stored height from the children's heights
void updateHeight(Node* node)
{
    node->height = 1 + maxOf(height(node->left), height(node->right));
}

// balance factor = left height minus right height.
// A balanced node has -1, 0 or 1. Above 1 it is left heavy, below -1 right heavy.
int balanceFactor(Node* node)
{
    return (node == nullptr) ? 0 : height(node->left) - height(node->right);
}

// ------------------------------------------------------------ rotations

// the left child of root moves up and root becomes its right child.
// The old root is lower now, so its height is updated before the new root's.
Node* rightRotate(Node* root)
{
    Node* newRoot = root->left;
    Node* middle = newRoot->right;   // subtree that changes parent

    newRoot->right = root;
    root->left = middle;

    updateHeight(root);              // old root first, it is now below newRoot
    updateHeight(newRoot);
    return newRoot;                  // the caller links this into the tree
}

// mirror image of rightRotate
Node* leftRotate(Node* root)
{
    Node* newRoot = root->right;
    Node* middle = newRoot->left;

    newRoot->left = root;
    root->right = middle;

    updateHeight(root);
    updateHeight(newRoot);
    return newRoot;
}

// counts rotations so main can tell whether any was needed
int rotationsDone = 0;

// updates the height of root, checks its balance and fixes it if needed.
// The rotation is chosen from the child's balance factor, not from the
// inserted key.
Node* rebalance(Node* root)
{
    updateHeight(root);                    // heights first, then the balance check
    int bf = balanceFactor(root);

    if (bf > 1)                            // left heavy
    {
        int childBf = balanceFactor(root->left);
        if (childBf >= 0)
        {
            cout << "  Node " << root->data << " is unbalanced (balance factor " << bf
                 << "), left child balance " << childBf
                 << ": left-left case, right rotation" << endl;
            if (childBf == 0)
            {
                cout << "    (child balance is 0, so one rotation is enough)" << endl;
            }
            rotationsDone++;
            return rightRotate(root);
        }
        cout << "  Node " << root->data << " is unbalanced (balance factor " << bf
             << "), left child balance " << childBf
             << ": left-right case, left rotation on " << root->left->data
             << " then right rotation on " << root->data << endl;
        rotationsDone += 2;
        root->left = leftRotate(root->left);
        return rightRotate(root);
    }

    if (bf < -1)                           // right heavy
    {
        int childBf = balanceFactor(root->right);
        if (childBf <= 0)
        {
            cout << "  Node " << root->data << " is unbalanced (balance factor " << bf
                 << "), right child balance " << childBf
                 << ": right-right case, left rotation" << endl;
            if (childBf == 0)
            {
                cout << "    (child balance is 0, so one rotation is enough)" << endl;
            }
            rotationsDone++;
            return leftRotate(root);
        }
        cout << "  Node " << root->data << " is unbalanced (balance factor " << bf
             << "), right child balance " << childBf
             << ": right-left case, right rotation on " << root->right->data
             << " then left rotation on " << root->data << endl;
        rotationsDone += 2;
        root->right = rightRotate(root->right);
        return leftRotate(root);
    }

    return root;                           // already balanced
}

// inserts like a BST, then updates heights and rebalances on the way back up.
// The returned root is always linked back by the caller, because a rotation
// can change which node is the root of a subtree.
Node* insertAVL(Node* root, int value)
{
    if (root == nullptr)
    {
        return createNode(value);
    }

    if (value < root->data)
    {
        root->left = insertAVL(root->left, value);
    }
    else if (value > root->data)
    {
        root->right = insertAVL(root->right, value);
    }
    else
    {
        cout << "  Duplicate " << value << " ignored" << endl;
        return root;                       // nothing changed, so nothing to fix
    }

    return rebalance(root);
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

// the tree turned on its side, each key shown with its height
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
    cout << root->data << " (h" << root->height << ")" << endl;
    printSideways(root->left, depth + 1);
}

// prints key, height and balance factor of every node (preorder)
void printNodeInfo(Node* root)
{
    if (root == nullptr)
    {
        return;
    }
    cout << "  key " << root->data << "  height " << root->height
         << "  balance " << balanceFactor(root) << endl;
    printNodeInfo(root->left);
    printNodeInfo(root->right);
}

// works out the real height of every subtree again (without trusting the
// stored values) and clears ok if a stored height is wrong or a node is
// out of balance
int verify(Node* root, bool& ok)
{
    if (root == nullptr)
    {
        return 0;
    }
    int lh = verify(root->left, ok);
    int rh = verify(root->right, ok);
    int real = 1 + maxOf(lh, rh);
    if (root->height != real)
    {
        ok = false;
    }
    if (lh - rh > 1 || rh - lh > 1)
    {
        ok = false;
    }
    return real;
}

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

// prints one traversal and says if it equals the expected one
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

    cout << label << " : ";
    for (int i = 0; i < n; i++)
    {
        cout << actual[i] << " ";
    }
    cout << endl;
    return same;
}

void showResult(Node* root, const int* preExp, int preN, const int* inExp, int inN)
{
    printSideways(root, 0);
    bool a = compareTraversal("Preorder", root, false, preExp, preN);
    bool b = compareTraversal("Inorder ", root, true, inExp, inN);
    bool balanced = true;
    verify(root, balanced);
    cout << "Traversals match the expected ones: " << ((a && b) ? "yes" : "NO") << endl;
    cout << "Every node balanced and every height correct: " << (balanced ? "yes" : "NO") << endl;
    printNodeInfo(root);
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

// inserts the values one by one into a new empty AVL tree and shows the result
void runSequence(const char* title, const char* explanation, const int* values, int n,
                 const int* preExp, int preN, const int* inExp, int inN)
{
    cout << endl << title << endl;
    cout << explanation << endl;

    Node* root = nullptr;                  // every sequence starts from an empty tree
    rotationsDone = 0;
    for (int i = 0; i < n; i++)
    {
        root = insertAVL(root, values[i]);
    }
    cout << "Rotations performed: " << rotationsDone << endl;
    showResult(root, preExp, preN, inExp, inN);

    freeTree(root);                        // release the whole tree before the next test
}

int main()
{
    // the four cases all end with the same tree, 20 as the root with the
    // children 10 and 30, so the expected traversals are the same
    int pre3[] = {20, 10, 30};
    int in3[] = {10, 20, 30};

    int a[] = {30, 20, 10};
    runSequence("SEQUENCE (a): 30, 20, 10",
                "Inserting 10 makes 30 left heavy and its left child 20 is left heavy too: LL case, one right rotation.",
                a, 3, pre3, 3, in3, 3);

    int b[] = {10, 20, 30};
    runSequence("SEQUENCE (b): 10, 20, 30",
                "Inserting 30 makes 10 right heavy and its right child 20 is right heavy too: RR case, one left rotation.",
                b, 3, pre3, 3, in3, 3);

    int c[] = {30, 10, 20};
    runSequence("SEQUENCE (c): 30, 10, 20",
                "Inserting 20 makes 30 left heavy but its left child 10 is right heavy: LR case, left rotation on 10 then right rotation on 30.",
                c, 3, pre3, 3, in3, 3);

    int d[] = {10, 30, 20};
    runSequence("SEQUENCE (d): 10, 30, 20",
                "Inserting 20 makes 10 right heavy but its right child 30 is left heavy: RL case, right rotation on 30 then left rotation on 10.",
                d, 3, pre3, 3, in3, 3);

    // extra tests beyond the four required sequences
    int e[] = {1, 2, 3, 4, 5, 6, 7};
    int preE[] = {4, 2, 1, 3, 6, 5, 7};
    int inE[] = {1, 2, 3, 4, 5, 6, 7};
    runSequence("EXTRA (e): 1 to 7 in increasing order",
                "Sorted input would make a chain in a plain BST. The AVL tree rotates and stays perfectly balanced.",
                e, 7, preE, 7, inE, 7);

    int f[] = {50, 20, 70, 10, 30, 25};
    int preF[] = {30, 20, 10, 25, 50, 70};
    int inF[] = {10, 20, 25, 30, 50, 70};
    runSequence("EXTRA (f): 50, 20, 70, 10, 30, 25",
                "Inserting 25 makes the root 50 left heavy and its left child 20 right heavy: LR case with subtrees that move.",
                f, 6, preF, 6, inF, 6);

    int g[] = {20, 10, 30, 10, 20};
    runSequence("EXTRA (g): 20, 10, 30, then 10 and 20 again",
                "Duplicates are ignored, the tree must stay 20 with children 10 and 30.",
                g, 5, pre3, 3, in3, 3);

    return 0;
}
