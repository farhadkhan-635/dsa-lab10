# DSA Lab 10: BST Deletion and AVL Trees

Data Structures and Algorithms, Department of Computing

| | |
|---|---|
| Name | Farhad Khan |
| CMS ID | 549034 |
| Class | BSCS-15E |
| Instructor | Sara Tariq Sheikh |
| Lab Engineer | Hassan Mohtashim |
| Lab Date | 16 November 2026 |

## About

This lab adds deletion to binary search trees (BST) and introduces AVL trees, which restore balance with rotations after insertion and deletion. Every tree is built from scratch with a `Node` structure (`data`, `left`, `right`, `height`) and no library containers. Each task is one standalone source file with tests in `main()`, and every tree is released before the program ends.

## Files

| Name | Task | What it does |
|---|---|---|
| q1 | BST deletion | `deleteBST()` handles a leaf, one child and two children (inorder successor), and returns the updated subtree root. Predictions for deleting 20, 60 and 50 are compared with the actual traversals. Also tests a missing key, an empty tree, an only node (returns `nullptr`), a root with one child, and a successor that has a child. |
| q2 | AVL insertion and rotations | `height`, `balanceFactor`, `rightRotate`, `leftRotate`, `rebalance` and `insertAVL`. Runs sequences (a) 30 20 10, (b) 10 20 30, (c) 30 10 20 and (d) 10 30 20 on separate empty trees, each ending as 20 with children 10 and 30. Extra tests: 1 to 7 in order, an LR case with moving subtrees, and duplicates. |
| q3 | AVL deletion | `deleteAVL()` does BST deletion, then updates height and rebalances at every ancestor on the way back. Inserts 30 20 40 10 25 50 5, deletes 50 (rotation at 30) and 40 (no rotation), then tests a heavy child with balance 0 on both sides, LR, RL and RR on deletion, a two-children deletion, a missing key, an empty tree and a one-node tree. |

Each one is a single source file (q1 is `q1.cpp`, q2 is `q2.cpp`, and so on).

## How to run

Each file is a complete program on its own, so run them one at a time.

**Using an IDE (easiest)**

1. Open the file, for example `q1.cpp`, in your IDE.
2. Press the compile and run button.
   - Dev-C++: F11
   - Code::Blocks: F9
   - Visual Studio: Ctrl + F5
   - VS Code with a C++ extension: the play button at the top right
3. A console window opens. All three programs run their tests on their own and need no input.

**Using the terminal (optional)**

```
g++ q1.cpp -o q1
./q1
```

On Windows use `.\q1` for the second line. For the other tasks replace q1 with q2 and q3.

## Sample output

q3, deleting 50 from the AVL tree makes node 30 unbalanced:

```
DELETE 50
  Deleted 50 (leaf node)
  Node 30 is unbalanced (balance factor 2), left child balance 1: left-left case, right rotation
Preorder : 20 10 5 30 25 40
Inorder  : 5 10 20 25 30 40
Every node balanced and every height correct: yes
```

q3, heavy child with balance factor 0:

```
DELETE 40
  Deleted 40 (leaf node)
  Node 30 is unbalanced (balance factor 2), left child balance 0: left-left case, right rotation
    (child balance is 0, so one rotation is enough)
```

## Memory safety

- A deleted node is released with `delete`, and the returned child pointer is linked back into the tree.
- `freeTree()` releases whatever is left of every tree before the program ends.
- Pointers are checked against `nullptr` before `->` is used.
- All three programs were built with AddressSanitizer and UndefinedBehaviorSanitizer and ran with no errors or leaks. The insertion and deletion functions were also compared against a reference set over thousands of random operations, with the AVL rules checked after every step.

## Concepts covered

- **BST deletion has three cases.** A leaf is removed directly. A node with one child is replaced by that child. A node with two children takes the key of its inorder successor (the smallest key in its right subtree), and then that successor is deleted from the right subtree. A missing key changes nothing. Every call returns the updated subtree root, which is why the caller writes `root = deleteBST(root, value)`.
- **Height and balance factor.** An empty subtree has height 0 and a leaf has height 1. Height = 1 + max(left height, right height). Balance factor = left height minus right height, and an AVL node must have -1, 0 or 1.
- **Four imbalance cases.** Left-left needs one right rotation. Right-right needs one left rotation. Left-right needs a left rotation on the child and then a right rotation. Right-left needs a right rotation on the child and then a left rotation.
- **Rotations return the new subtree root.** In a rotation the old root is lowered, so its height is updated before the new root's height.
- **Rotation is chosen from the child's balance factor, not from the key.** A deleted key says nothing about which side is heavy. For a left-heavy node a right rotation is used when the left child's balance is at least 0, otherwise left then right. The right-heavy node is the mirror image.
- **Why one rotation is enough when the heavy child has balance 0.** Both subtrees of that child have the same height h. After the rotation the child is the new root with h on one side and h + 1 on the other, and the old root has h and h - 1, so every balance stays within -1 and 1. The subtree is as tall as before the deletion, so no ancestor changes. A double rotation is needed only when the child leans the opposite way, because then the tall grandchild is on the inner side and a single rotation would only move the extra height to the other side.
- **Deletion checks every ancestor.** After one deletion, a rotation may be needed at a node far above the deleted one, so each node on the path back to the root updates its height and rebalances.

## Reference

Goodrich, M. T., Tamassia, R., & Mount, D. M. (2011). *Data Structures and Algorithms in C++* (2nd ed.). Wiley.
