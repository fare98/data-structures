#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <vector>
#include <algorithm>


struct TreeNode {
    int key;
    TreeNode* left;
    TreeNode* right;
};

int tree_height(TreeNode* node) {
    if (!node) return 0;
    return 1 + std::max(tree_height(node->left), tree_height(node->right));
}

int balance_factor(TreeNode* node) {
    return tree_height(node->right) - tree_height(node->left);
}

bool is_avl(TreeNode* node, bool& avl_violation) {
    if (!node) {
        return true;
    }

    // Traverse the right subtree
    is_avl(node->right, avl_violation);

    // Traverse the left subtree
    is_avl(node->left, avl_violation);

    // Visit the node
    int bal = balance_factor(node);
    std::cout << "bal(" << node->key << ") = " << bal;

    if (bal > 1 || bal < -1) {
        avl_violation = true;
        std::cout << " (AVL violation!)";
    }

    std::cout << std::endl;
}


void insert_node(TreeNode*& node, int key) {
    if (!node) {
        node = new TreeNode{key, nullptr, nullptr};
        return;
    }

    if (key < node->key) {
        insert_node(node->left, key);
    } else if (key > node->key) {
        insert_node(node->right, key);
    }
}

void statistics(TreeNode* node, int& min, int& max, int& sum, int& count) {
    if (!node) return;

    statistics(node->left, min, max, sum, count);
    min = std::min(min, node->key);
    max = std::max(max, node->key);
    sum += node->key;
    count++;
    statistics(node->right, min, max, sum, count);
}

bool simple_search(TreeNode* node, int key, std::vector<int>& path) {
    if (!node) return false;

    path.push_back(node->key);
    if (node->key == key) return true;

    if (key < node->key && simple_search(node->left, key, path)) return true;
    if (key > node->key && simple_search(node->right, key, path)) return true;

    path.pop_back();
    return false;
}

bool is_same_structure(TreeNode* tree, TreeNode* subtree) {
    if (!tree && !subtree) return true;
    if (!tree || !subtree) return false;
    if (tree->key != subtree->key) return false;
    return is_same_structure(tree->left, subtree->left) &&
           is_same_structure(tree->right, subtree->right);
}

bool contains_subtree(TreeNode* tree, TreeNode* subtree) {
    if (!subtree) return true;
    if (!tree) return false;
    if (tree->key == subtree->key) {
        return contains_subtree(tree->left, subtree->left) && contains_subtree(tree->right, subtree->right);
    }
    return contains_subtree(tree->left, subtree) || contains_subtree(tree->right, subtree);
}

bool subtree_search(TreeNode* tree, TreeNode* subtree) {
    if (!tree) return false;
    if (contains_subtree(tree, subtree)) return true;
    return subtree_search(tree->left, subtree) || subtree_search(tree->right, subtree);
}


int main(int argc, char* argv[]) {
    if (argc < 2 || argc > 3) {
        std::cerr << "Input either 1 or 2 .txt files" << std::endl;
        return 1;
    }

    TreeNode* search_tree = nullptr;
    TreeNode* subtree = nullptr;

    // Read search tree
    std::ifstream search_tree_file(argv[1]);
    if (!search_tree_file.is_open()) {
        std::cerr << "Error opening file: " << argv[1] << std::endl;
        return 1;
    }

    int key;
    while (search_tree_file >> key) {
        insert_node(search_tree, key);
    }
    search_tree_file.close();

    if (argc == 2) {
        bool avl_violation = false;
        is_avl(search_tree, avl_violation);
        if (!avl_violation) {
            std::cout << std::endl;
            std::cout << "AVL: yes" << std::endl;
        } else {
            std::cout << std::endl;
            std::cout << "AVL: no" << std::endl;
        }

        int min = INT_MIN, max = INT_MAX, sum = 0, count = 0;
        statistics(search_tree, min, max, sum, count);
        double avg = static_cast<double>(sum) / count;
        std::cout << "min: " << min << ", max: " << max << ", avg: " << avg << std::endl;
    } else {
        // Read subtree
        std::ifstream subtree_file(argv[2]);
        if (!subtree_file.is_open()) {
            std::cerr << "Error opening file: " << argv[2] << std::endl;
            return 1;
        }

        int subtree_key_count = 0;
        while (subtree_file >> key) {
            insert_node(subtree, key);
            subtree_key_count++;
        }
        subtree_file.close();

        if (subtree_key_count == 1) {
            std::vector<int> path;
            if (simple_search(search_tree, subtree->key, path)) {
                std::cout << subtree->key << " found";
                for (int p : path) {
                    std::cout << " " << p;
                }
                std::cout << std::endl;
            } else {
                std::cout << subtree->key << " not found!" << std::endl;
            }
        } else {
            if (subtree_search(search_tree, subtree)) {
                std::cout << "Subtree found" << std::endl;
            } else {
                std::cout << "Subtree not found!" << std::endl;
            }
        }
    }

    return 0;
}

