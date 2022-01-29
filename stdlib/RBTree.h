#ifndef OS_RBTREE_H
#define OS_RBTREE_H

#ifdef GSTDLIB
#include <iostream>
#include <cassert>
#include <queue>
#else
#include "Details.h"
#define assert(s)
#endif

template <typename T>
class RBTree {

public:
    enum class Color : bool {
        Red,
        Black
    };

    struct Node {
        struct Node *parent { nullptr };
        struct Node *left { nullptr };
        struct Node *right { nullptr };

        T value;
        Color color { Color::Red };
    };

    Node* find_smallest_not_below(T v){
        Node* node = _find_smallest_not_below(v);
        return node;
    }

    void insert(T value, void* addr = nullptr){
        //Node* x=new Node{nullptr, nullptr, nullptr};
        Node* x;

        if (addr) {
            x = (Node*)addr;
        } else {
            UNDEFINED();
            //x = new Node{nullptr, nullptr, nullptr};
        }
        x->value = value;
        x->left = nullptr;
        x->right = nullptr;
        x->parent = nullptr;
        insert(x);
    }

    bool remove_by_value(T v) {
        Node* node = find_node(v);
        if(!node) return false;
        remove_by_node(node);
        return true;
    }

    bool exists(T needle){
        return find_node(needle);
    }

    bool remove_by_node(Node* remove_node){
        Node* replacement_node;
        Node* replacement_node_child;
        if (remove_node == root && remove_node->left == nullptr && remove_node->right == nullptr){ //the case where we have a single node tree
            root = nullptr;
            return true;
        }

        if(remove_node->left == nullptr || remove_node->right == nullptr){
            replacement_node = remove_node;
        } else {
            replacement_node = tree_successor(remove_node);
        }

        //relocated remove_node is expected to have at most one child based on its assignment above
        //in the if branch above which by definition has only one child
        //the else branch where the tree_successor will end up in the min_node(remove_node->right) which may only have a right child
        if (replacement_node->left != nullptr){
            replacement_node_child = replacement_node->left;
        } else {
            replacement_node_child = replacement_node->right;
        }

        //since the removed remove_node has at least one child, we have to fix its parent
        if (replacement_node_child != nullptr){
            replacement_node_child->parent = replacement_node->parent;
        }

        if (replacement_node->parent == nullptr){
            root = replacement_node_child;
        } else if (replacement_node == replacement_node->parent->left){
            replacement_node->parent->left = replacement_node_child;
        } else {
            replacement_node->parent->right = replacement_node_child;
        }
        Node* replacement_node_child_parent = replacement_node->parent;

        if (replacement_node != remove_node) { //we should need to update the parent, since we want the node to be stored in its value
            remove_node->value = replacement_node->value;
        }

        if (replacement_node->color == Color::Black){
            remove_fix(replacement_node_child, replacement_node->parent);
        }

        //this branch is useful when the node's address is derived by its value. So, when we are replacing a node's
        //value with another, we also have to update its address
        if (replacement_node != remove_node) {
            //replacement_node->value is already set
            replacement_node->parent = remove_node->parent;
            replacement_node->left = remove_node->left;
            replacement_node->right = remove_node->right;
            replacement_node->color = remove_node->color;
            if (!remove_node->parent)
                root = replacement_node;
            else if (remove_node->parent->left==remove_node)
                remove_node->parent->left = replacement_node;
            else
                remove_node->parent->right = replacement_node;
        }
        //node r
        return true;
    }

    //temporary, move test functionality into test folder
    void test_left_rotate(){
        Node a{};
        Node b = a;
        Node c = a;
        Node d = a;
        Node e = a;
        a.left = &b;
        a.right = &c;
        b.right = &d;
        c.left = &e;

        rotate_left(&a);
        assert(root==&c);
        assert(c.left==&a);
        assert(c.right==nullptr);
        assert(a.right==&e);
        assert(a.parent==&c);
        assert(e.parent==&a);
    }

    void test_right_rotate(){
        Node a{};
        Node b = a;
        Node c = a;
        Node d = a;
        Node e = a;
        a.left = &b;
        a.right = &c;
        b.right = &d;
        c.left = &e;

        rotate_right(&a);
        assert(root==&b);
        assert(b.right==&a);
        assert(b.left==nullptr);
        assert(a.left==&d);
        assert(b.parent == nullptr);
        assert(a.parent == &b);
        assert(d.parent == &a);
    }


#ifdef GSTDLIB
void inorder_print(){
        _inorder_print(root);
    }

    void _inorder_print(Node *node){
        if(!node)
            return;

        _inorder_print(node->left);
        printf("%d\n", node->value);
        _inorder_print(node->right);
    }

    int get_depth(){
        int depth = 0;
        std::queue<std::pair<int, Node*>> nodes;

        nodes.push({1, root});
        while(!nodes.empty()){
            auto p_info = nodes.front();
            nodes.pop();

            if(!p_info.second){
                if (p_info.first>depth)
                    depth = p_info.first;
            } else {
                nodes.push({p_info.first + 1, p_info.second->left});
                nodes.push({p_info.first + 1, p_info.second->right});
            }
        }
        return depth;
    }

    long strlen(const char *str)
    {
        const char *s;

        for (s = str; *s; ++s);
        return (s - str);
    }

    void centered_print(char* s, int str_len, int frame_len, Color color=Color::Black){
        int left_padding = (frame_len-str_len)/2+(frame_len-str_len)%2;
        int right_padding = (frame_len-str_len)/2;

        char* color_seq;
        char reset_attrs[] = "\033[0m";
        char red_color[] = "\033[0;31m";

        switch (color){
            default:
                color_seq = reset_attrs;
                break;
                case Color::Red:
                    color_seq = red_color;
                    break;
        }

        if(left_padding)
            printf("%*c", left_padding, ' ');

        printf("%s", color_seq);
        int node_left_padding = (str_len-strlen(s))/2;
        int node_right_padding = (str_len-strlen(s))/2 + (str_len-strlen(s))%2;

        if(node_left_padding)
            printf("%*c", node_left_padding, ' ');

        printf("%s", s);

        if(node_right_padding)
            printf("%*c", node_right_padding, ' ');

        printf("%s", reset_attrs);

        if(right_padding)
            printf("%-*c", right_padding, ' ');

        //printf("(f=%d, sl=%d) %d %d %d %d strlen()=%d\n", frame_len, str_len, left_padding, right_padding, node_left_padding, node_right_padding, strlen(s));
    }

    void visual_print(){
        typedef struct {
            int level;
            Node* node;
        } t_pinfo;

        static Node* empty_node = (Node*)1;

        int node_info_len = 6;
        int tree_depth = get_depth();
        int remaining_nodes = 1;
        std::queue<t_pinfo> nodes;
        int last_level = -1;

        if (root)
            nodes.push({0, root});

        while(remaining_nodes>0){
            Color color = Color::Black;
            t_pinfo p_info = nodes.front();
            nodes.pop();
            if(p_info.level > last_level){
                last_level = p_info.level;
                printf("\n");
            }

            char node_info[100];
            if(!p_info.node){
                remaining_nodes--;
                color = Color::Black;
                sprintf(node_info, "$");
                nodes.push({p_info.level + 1, empty_node});
                nodes.push({p_info.level + 1, empty_node});
            } else if (p_info.node == empty_node) {
                color = Color::Black;
                sprintf(node_info, "-");
                nodes.push({p_info.level + 1, empty_node});
                nodes.push({p_info.level + 1, empty_node});
            } else {
                remaining_nodes--;
                color = p_info.node->color;
                sprintf(node_info,"%d", p_info.node->value);
                nodes.push({p_info.level + 1, p_info.node->left});
                nodes.push({p_info.level + 1, p_info.node->right});
                remaining_nodes+=2;
            }

            centered_print(node_info, node_info_len, (1<<(tree_depth-p_info.level-1))*node_info_len, color);

        }
        printf("\n");
    }
#endif

protected:

    void remove_fix(Node* node, Node* parent){

        while(node != root && (!node || node->color == Color::Black)){
            bool current_node_communist = node == parent->left;
            Node* sibling = current_node_communist ? parent->right : parent->left;

            //since the current node can only be black (or nul) and non-root, a non-null sibling should exist
            assert(sibling!=nullptr);
            if(sibling->color==Color::Red){ //case 1, sibling is Red
                sibling->color = Color::Black;
                parent->color = Color::Red;
                if(current_node_communist){
                    rotate_left(parent);
                    sibling = parent->right;
                }else{
                    rotate_right(parent);
                    sibling = parent->left;
                }
            }

            //sibling must not be null here based on case 1
            assert(sibling!=nullptr);
            if((!sibling->left || sibling->left->color==Color::Black) && (!sibling->right || sibling->right->color == Color::Black)){
                //case 2: no red children, so we can change the color of the sibling to red and make its k double blacked
                sibling->color = Color::Red;
                node = parent;
            } else {
                //case 3,4: sibling is not null and we have at least one red child based on case 2

                if(current_node_communist && (!sibling->right || sibling->right->color == Color::Black)){
                    sibling->left->color = Color::Black;
                    sibling->color = Color::Red;
                    rotate_right(sibling);
                    sibling = parent->right;
                } else if (!current_node_communist && (!sibling->left || sibling->left->color == Color::Black)){
                    sibling->right->color = Color::Black;
                    sibling->color = Color::Red;
                    rotate_left(sibling);
                    sibling = parent->left;
                }

                //case 4:
                // >right child of the sibling is red when current_node_communist
                // >left child of the sibling is red when !current_node_communist
                sibling->color = parent->color;
                parent->color = Color::Black;

                if(current_node_communist){
                    sibling->right->color = Color::Black;
                    rotate_left(parent);
                } else {
                    sibling->left->color = Color::Black;
                    rotate_right(parent);
                }
                node = root;
            }
            parent = node->parent;
        }
        node->color = Color::Black;
    }

    Node* find_min(Node* n){
        assert(n!=nullptr);

        Node* min_node=n;
        while(n->left){
            min_node = n->left;
            n=n->left;
        }
        return min_node;
    }

    Node* find_max(Node* n){
        assert(n!=nullptr);

        Node* max_node=n;
        while(n->right){
            max_node = n->right;
            n=n->right;
        }
        return max_node;
    }

    Node* tree_successor(Node* node){
        assert(node!=nullptr);

        if(node->right!=nullptr){
            return find_min(node->right);
        }

        Node* next_node = node->parent;
        while (next_node && next_node->right == node){
            node = next_node;
            next_node = next_node->parent;
        }
        return next_node;
    }

    Node* find_node(T needle){
        if (!root) return nullptr;

        Node *current_node = root;

        while (current_node!=nullptr) {
            if (current_node->value == needle){
                return current_node;
            } else if (current_node->value > needle) {
                current_node = current_node->left;
            } else
                current_node = current_node->right;
        }
        return nullptr;
    }

    Node* _find_smallest_not_below(T needle){
        if (!root) return nullptr;

        Node *current_node = root;
        Node* candidate = nullptr;

        while (current_node!=nullptr) {
            if (current_node->value == needle){
                return current_node;
            } else if (current_node->value > needle) {
                candidate = current_node;
                current_node = current_node->left;
            } else
                current_node = current_node->right;
        }
        return candidate;
    }

    void rotate_right(Node *x) {
        assert(x!=nullptr);
        Node *pivot = x->left;
        assert(pivot!=nullptr);
        Node *pivot_detached_branch = pivot->right;
        pivot->right = x;
        x->left = pivot_detached_branch;

        if (pivot_detached_branch!=nullptr){
            pivot_detached_branch->parent = x;
        }

        if (x->parent == nullptr){
            root = pivot;
        } else {
            Node *parent = x->parent;
            if (parent->left==x){
                parent->left = pivot;
            } else {
                parent->right = pivot;
            }
        }

        pivot->parent = x->parent;
        x->parent = pivot;

    }

    void rotate_left(Node *x){
        assert(x!=nullptr);
        Node *pivot = x->right;
        assert(pivot!=nullptr);

        Node *pivot_detached_branch = pivot->left;
        pivot->left = x;
        x->right = pivot_detached_branch;

        if (pivot_detached_branch!=nullptr){
            pivot_detached_branch->parent = x;
        }

        if (x->parent == nullptr){
            root = pivot;
        } else {
            Node *parent = x->parent;
            if (parent->left==x){
                parent->left = pivot;
            } else {
                parent->right = pivot;
            }
        }

        pivot->parent = x->parent;
        x->parent = pivot;
    }

    Node* get_parent(Node* node){
        return node==nullptr?nullptr:node->parent;
    }

    Node* get_grandparent(Node* node){
        return node==nullptr || node->parent==nullptr?nullptr:node->parent->parent;
    }

    Color get_other_color(Color color){
        if (color==Color::Red) return Color::Black;
        return Color::Red;
    }

    Node* get_sibling(Node* node){
        if (!node) return nullptr;
        auto p = get_parent(node);
        if (!p) return nullptr;
        return p->left==node?p->right:p->left;
    }

    Node *get_uncle(Node *node){
        return get_sibling(get_parent(node));
    }

    Node *get_close_nephew(Node *node){
        auto p = get_parent(node);
        auto s = get_sibling(node);
        if(p->left == s) {
            return s->right;
        } else {
            return s->left;
        }
    }

    //implement a binary search class?
    void insert(Node* x){
        Node *current_node = root;
        Node *prev_node;

        if (root == nullptr) {
            x->color = Color::Black;
            root = x;
            return;
        }

        while (current_node!=nullptr) {
            prev_node = current_node;
            if (current_node->value > x->value) {
                current_node = current_node->left;
            } else
                current_node = current_node->right;
        }

        Node *new_node_parent = prev_node;

        if (new_node_parent->value > x->value) {
            new_node_parent->left = x;
        } else {
            new_node_parent->right = x;
        }

        x->parent = new_node_parent;
        x->left = nullptr;
        x->right = nullptr;
        x->color = Color::Red;

        Node *parent_node=nullptr, *uncle=nullptr;
        bool communist_parent;
        current_node = x;
        while (current_node && current_node->parent && current_node->parent->color == Color::Red){
            parent_node = current_node->parent;
            communist_parent = parent_node->parent && parent_node==parent_node->parent->left;
            uncle = get_uncle(current_node);

            if (uncle && uncle->color==Color::Red){
                parent_node->color = Color::Black;
                if (uncle)
                    uncle->color = Color::Black;
                parent_node->parent->color = Color::Red;
                current_node = parent_node->parent;
            } else  {
                if (parent_node && communist_parent && parent_node->right == current_node) {
                    current_node = parent_node;
                    rotate_left(current_node);
                } else if (parent_node && !communist_parent && parent_node->left == current_node) {
                    current_node = parent_node;
                    rotate_right(current_node);
                }
                current_node->parent->color = Color::Black;
                current_node->parent->parent->color = Color::Red;
                if (communist_parent)
                    rotate_right(current_node->parent->parent);
                else
                    rotate_left(current_node->parent->parent);
            }
        }
        root->color = Color::Black;
    }


private:
    Node *root = nullptr;
};

#endif //OS_RBTREE_H
