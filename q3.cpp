// Name    : Farhad Khan
// Reg No  : 549034
// Section : BSCS-15E
// Lab 10, Task 3: AVL insertion and deletion

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

// the smallest key of a subtree is its leftmost node. For a node with two
// children this is the inorder successor.
Node* findMin(Node* root)
{
    Node* curr = root;
    while (curr != nullptr && curr->left != nullptr)
    {
        curr = curr->left;
    }
    return curr;
}

// deletes value like a BST first, then updates the height and rebalances this
// node on the way back up, so every ancestor of the deleted node is checked.
// The rotation is chosen from the child's balance factor, never from the
// deleted value (a deleted key says nothing about which side is heavy).
Node* deleteAVL(Node* root, int value)
{
    // key not found (this also covers an empty tree), nothing changes
    if (root == nullptr)
    {
        cout << "  Key " << value << " not found, tree not changed" << endl;
        return nullptr;
    }

    if (value < root->data)
    {
        root->left = deleteAVL(root->left, value);
    }
    else if (value > root->data)
    {
        root->right = deleteAVL(root->right, value);
    }
    else
    {
        if (root->left == nullptr || root->right == nullptr)
        {
            // leaf or one child: the child (or nullptr for a leaf) takes the place
            Node* child = (root->left != nullptr) ? root->left : root->right;
            if (child == nullptr)
            {
                cout << "  Deleted " << value << " (leaf node)" << endl;
            }
            else
            {
                cout << "  Deleted " << value << " (one child, " << child->data
                     << " takes its place)" << endl;
            }
            delete root;
            root = child;
        }
        else
        {
            // two children: copy the inorder successor up, then delete it
            // from the right subtree (that call also rebalances on its way back)
            Node* successor = findMin(root->right);
            cout << "  Node " << value << " has two children, inorder successor is "
                 << successor->data << ", copying it up" << endl;
            root->data = successor->data;
            root->right = deleteAVL(root->right, successor->data);
        }
    }

    // the subtree became empty (a deleted leaf), there is nothing to rebalance
    if (root == nullptr)
    {
        return nullptr;
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

// shows the tree, compares both traversals with the expected ones and checks
// the AVL rules. With details on, every node's key, height and balance factor
// is printed as well.
void showResult(Node* root, const int* preExp, int preN, const int* inExp, int inN, bool details)
{
    if (root == nullptr)
    {
        cout << "(empty tree)" << endl;
    }
    else
    {
        printSideways(root, 0);
    }
    bool a = compareTraversal("Preorder", root, false, preExp, preN);
    bool b = compareTraversal("Inorder ", root, true, inExp, inN);
    bool balanced = true;
    verify(root, balanced);
    cout << "Traversals match the expected ones: " << ((a && b) ? "yes" : "NO") << endl;
    cout << "Every node balanced and every height correct: " << (balanced ? "yes" : "NO") << endl;
    if (details)
    {
        printNodeInfo(root);
    }
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

// builds an AVL tree from the values, in the given order
Node* buildTree(const int* values, int n)
{
    Node* root = nullptr;
    for (int i = 0; i < n; i++)
    {
        root = insertAVL(root, values[i]);
    }
    return root;
}

// deletes one key and shows what happened. It says when no rotation was needed.
Node* deleteAndShow(Node* root, int key, const int* preExp, int preN,
                    const int* inExp, int inN, bool details)
{
    cout << endl << "DELETE " << key << endl;
    rotationsDone = 0;
    root = deleteAVL(root, key);
    if (rotationsDone == 0)
    {
        cout << "  No rotation was needed." << endl;
    }
    showResult(root, preExp, preN, inExp, inN, details);
    return root;
}

int main()
{
    // ---------- main scenario from the manual
    // hand drawn tree after inserting 30 20 40 10 25 50 5:
    // 30 is the root. Its left child 20 has children 10 and 25, and 10 has
    // the left child 5. Its right child 40 has only the right child 50.
    cout << "PART 1: insert 30 20 40 10 25 50 5, then delete 50 and 40" << endl;
    int values[] = {30, 20, 40, 10, 25, 50, 5};
    Node* root = buildTree(values, 7);     // no rotation is needed while inserting

    int pre0[] = {30, 20, 10, 5, 25, 40, 50};
    int in0[] = {5, 10, 20, 25, 30, 40, 50};
    cout << "Tree after the insertions:" << endl;
    showResult(root, pre0, 7, in0, 7, true);

    // deleting 50 (a leaf) leaves 40 as a leaf, still balanced. At 30 the left
    // subtree has height 3 and the right subtree height 1, so the balance
    // factor is 2. The left child 20 has balance 1, so this is the left-left
    // case: one right rotation at 30.
    int pre1[] = {20, 10, 5, 30, 25, 40};
    int in1[] = {5, 10, 20, 25, 30, 40};
    root = deleteAndShow(root, 50, pre1, 6, in1, 6, true);

    // deleting 40 leaves 30 with only the left child 25 (balance 1). The root 20
    // has two subtrees of height 2, so every node stays balanced and no
    // rotation is needed.
    int pre2[] = {20, 10, 5, 30, 25};
    int in2[] = {5, 10, 20, 25, 30};
    root = deleteAndShow(root, 40, pre2, 5, in2, 5, true);
    freeTree(root);

    // ---------- heavy child with balance factor 0
    // Why one rotation is enough when the heavy child has balance factor 0:
    // both subtrees of that child have the same height h. After the rotation
    // the child is the new root with h on one side and h + 1 on the other, and
    // the old root has h and h - 1, so every balance stays between -1 and 1.
    // The subtree is as tall as before the deletion, so no ancestor changes.
    // A double rotation is needed only when the child leans the opposite way,
    // because then the tall grandchild is on the inner side and one rotation
    // would just move the extra height to the other side.
    cout << endl << "PART 2: heavy child with balance factor 0" << endl;

    // left side: 30 has children 20 and 40, and 20 has children 10 and 25.
    // Deleting 40 makes 30 left heavy (balance 2) and 20 has balance 0.
    int v20[] = {30, 20, 40, 10, 25};
    root = buildTree(v20, 5);
    int pre3[] = {20, 10, 30, 25};
    int in3[] = {10, 20, 25, 30};
    root = deleteAndShow(root, 40, pre3, 4, in3, 4, true);
    freeTree(root);

    // right side, the mirror image: deleting 10 makes 20 right heavy and its
    // right child 30 has balance 0, so one left rotation is enough
    int v21[] = {20, 10, 30, 25, 40};
    root = buildTree(v21, 5);
    int pre4[] = {30, 20, 25, 40};
    int in4[] = {20, 25, 30, 40};
    root = deleteAndShow(root, 10, pre4, 4, in4, 4, true);
    freeTree(root);

    // ---------- the other rebalancing cases on deletion
    cout << endl << "PART 3: double rotations and RR on deletion" << endl;

    // left-right: 30 has the children 10 and 40, and 10 has the right child 20.
    // Deleting 40 makes 30 left heavy but its left child 10 is right heavy.
    int v30[] = {30, 10, 40, 20};
    root = buildTree(v30, 4);
    int pre5[] = {20, 10, 30};
    int in5[] = {10, 20, 30};
    root = deleteAndShow(root, 40, pre5, 3, in5, 3, false);
    freeTree(root);

    // right-left: 30 has the children 20 and 50, and 50 has the left child 40.
    // Deleting 20 makes 30 right heavy but its right child 50 is left heavy.
    int v31[] = {30, 50, 20, 40};
    root = buildTree(v31, 4);
    int pre6[] = {40, 30, 50};
    int in6[] = {30, 40, 50};
    root = deleteAndShow(root, 20, pre6, 3, in6, 3, false);
    freeTree(root);

    // right-right: 30 has the children 20 and 50, and 50 has the right child 60.
    int v32[] = {30, 20, 50, 60};
    root = buildTree(v32, 4);
    int pre7[] = {50, 30, 60};
    int in7[] = {30, 50, 60};
    root = deleteAndShow(root, 20, pre7, 3, in7, 3, false);
    freeTree(root);

    // ---------- node with two children
    cout << endl << "PART 4: delete a node with two children" << endl;
    int v40[] = {20, 10, 30, 5, 15, 25, 35};
    root = buildTree(v40, 7);
    // 20 has two children, its successor is 25 (the smallest key on its right)
    int pre8[] = {25, 10, 5, 15, 30, 35};
    int in8[] = {5, 10, 15, 25, 30, 35};
    root = deleteAndShow(root, 20, pre8, 6, in8, 6, false);
    freeTree(root);

    // ---------- boundary cases
    cout << endl << "PART 5: missing key, empty tree, one node" << endl;

    int v50[] = {20, 10, 30};
    root = buildTree(v50, 3);
    int pre9[] = {20, 10, 30};
    int in9[] = {10, 20, 30};
    root = deleteAndShow(root, 99, pre9, 3, in9, 3, false);    // must change nothing
    freeTree(root);

    cout << endl << "Empty tree, delete 10:" << endl;
    Node* empty = nullptr;
    empty = deleteAVL(empty, 10);
    cout << "Root is nullptr: " << (empty == nullptr ? "yes" : "no") << endl;

    cout << endl << "One node tree (7), delete 7:" << endl;
    Node* single = insertAVL(nullptr, 7);
    single = deleteAVL(single, 7);
    cout << "Root is nullptr: " << (single == nullptr ? "yes" : "no") << endl;

    return 0;
}
