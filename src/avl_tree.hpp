#include <functional>
#include <iostream>

/**
 * AVL Tree Class
 * Can't insert the same key more than once
 */

template<class T,typename Comp = std::less<T>>
class AVLTree {
    
    struct node {    
        T key;
        int height = -1;
        int size = 0;
        node* left;
        node* right;
        node* parent;

        node(const T& key,node* left,node* right,node* parent) : key(key), 
        left(left), right(right), parent(parent), height(0), size(1) {}

        node() : left(nullptr),right(nullptr),parent(nullptr) {}   
    };

    Comp comp;
        
    static node NULL_NODE;
    static node* NILL;
    
    static node* successor(node* x) {
        if(x->right!=NILL) {
            x=x->right;
            while(x->left!=NILL) x=x->left;
            return x;
        }

        node* y = x->parent;
        while(y!=NILL && y->right==x) {
            x = y;
            y = x->parent;
        }

        return y;
    }

    static node* predecessor(node* x) {
        if(x->left!=NILL) {
            x=x->left;
            while(x->right!=NILL) {
                x=x->right;
            }
            return x;
        }

        node* y = y->parent;
        while(y!=NILL && y->left==x) {
            x = y;
            y = x->parent;
        }

        return y;
    }


    /**
     * private helper function
     * to help destructor to deallocate all memory
     * recursively in linear time
     */ 
    void erase_sub_tree(node* x) {
        if(x==NILL) return;
        erase_sub_tree(x->left);
        erase_sub_tree(x->right);
        delete x;
    }

    class iterator {
        friend class AVLTree<T,Comp>;
        node* it;
        iterator(node* iter) : it(iter) {}
    public:
        iterator() {};

        iterator& operator++() {
            it = successor(it);
            return *this;
        }

        iterator& operator--() {
            it = predecessor(it);
            return *this;
        }

        T operator*() const {return it->key;}
        bool operator==(const iterator& rhs) const {return it==rhs.it;}
        bool operator!=(const iterator& rhs) const {return it!=rhs.it;}
        iterator& operator=(const iterator& rhs) {
            it = rhs.it; 
            return *this;
        }

    };

    node* root = NILL;


    /**
     * A private helper function for the erase method
     * replaces subtree rooted at u with subtree rooted at v
     * assigns root when necessary
     * DOES NOT FIX AUGMENTATION OR PRESERVE AVL PROPERTIES
     */ 
    inline void transplant(node* u,node* v) {
        if(u->parent==NILL) {
            root = v;
        } else if(u->parent->left == u) {
            u->parent->left = v;
        } else {
            u->parent->right = v;
        }

        if(v!=NILL) {
            v->parent = u->parent;
        }
    }




    /**
     * A max function
     */ 
    inline int max(int a,int b) {
        if(a>b) return a;
        return b;
    }

    
    inline void relax_augmentation(node* x) {
        x->height = max(x->left->height,x->right->height)+1;
        x->size = x->left->size + x->right->size + 1;
    }

    /**
     * x is not NILL
     * x must have a right child
     * must adjust root when necessary
     * must fix augmentation code locally
     */ 
    void single_rotate_left(node* x) {
        node* y = x->right;
        
        x->right = y->left;
        if(x->right!=NILL) x->right->parent = x;
        
        y->parent = x->parent;
        if(y->parent==NILL) {
            root = y;
        } else if(x == x->parent->left) {
            y->parent->left = y;
        } else {
            y->parent->right = y;
        }

        y->left = x;
        x->parent = y;

        relax_augmentation(x);
        relax_augmentation(y);
    }

    /**
     * x is not NILL
     * x must have a left child
     * must adjust root when necessary
     * must fix augmentation code locally
     */ 
    void single_rotate_right(node* x) {
        node* y = x->left;
        
        x->left = y->right;
        if(x->left!=NILL) x->left->parent = x;
        
        y->parent = x->parent;
        if(y->parent==NILL) {
            root = y;
        } else if(x == x->parent->left) {
            y->parent->left = y;
        } else {
            y->parent->right = y;
        }

        y->right = x;
        x->parent = y;

        x->height = max(x->left->height,x->right->height) + 1;
        x->size = x->left->size + x->right->size + 1;
        
        relax_augmentation(x);
        relax_augmentation(y);
    }

    /**
     * x must have right child and right-left grandchild
     */
    void double_rotate_left(node* x) {
        single_rotate_right(x->right);
        single_rotate_left(x);
    }

    void double_rotate_right(node* x) {
        single_rotate_left(x->left);
        single_rotate_right(x);
    }

    void rotate_left(node* x) {
        node* r = x->right;
        if(r->right->height>r->left->height) single_rotate_left(x);
        else double_rotate_left(x);
    }

    void rotate_right(node* x) {
        node* l = x->left;
        if(l->left->height>l->right->height) single_rotate_right(x);
        else double_rotate_right(x);
    }

    /**
     * locally modifies tree
     * updates augmentation
     * rebalances tree if there if there is an avl property vialation at node
     * i.e abs(balance factor(x)) >1 
     */ 
    void rebalance(node* x) {
        int hl = x->left->height;
        int hr = x->right->height;
        if(hr>hl+1) rotate_left(x);
        else if(hl>hr+1) rotate_right(x);
        else relax_augmentation(x);
    }

    /**
     * given a node x in a subtree rooted at r 
     * where there is an avl or augmentation violation
     * fix_tree(x,r) modifies the subtree and brings it 
     * back to a valid AVL state
     */ 
    void fix_tree(node* x,node* r) {
        while(x!=r->parent) {
            rebalance(x);
            x=x->parent;
        }
    }
    


    /**
     * A helper function for the erase(iterator) method
     * z can't be NILL
     */ 
    void erase(node* z) {
        if(z->left==NILL) {
            transplant(z,z->right);
            fix_tree(z->parent,root);
        } else if(z->right==NILL) {
            transplant(z,z->left);
            fix_tree(z->parent,root);
        } else {
            node* y = successor(z); //y is not NILL and y has no left child cause z->right is not NILL
            if(y->parent!=z) {
                transplant(y,y->right);
                fix_tree(y->parent,z);

                y->right = z->right;
                y->right->parent = y;
            }

            transplant(z,y);
            y->left = z->left;
            y->left->parent = y;
            
            fix_tree(y,root);                
        }

        delete z;
    }

    public:
    iterator find(const T& val) {
        node* x = root;
        while(x!=NILL) {
            if(comp(x->key,val)) x = x->right;
            else if(comp(val,x->key)) x = x->left;
            else return iterator(x);

        }
        return iterator(NILL);
    }

    /**
     * Inserts a new node into the tree and rebalances it accordingly
     * to preserve avl properties if the node is not present
     * If the value already exists in the tree it does nothing.
     */ 
    void insert(const T& val) {
        node* y = NILL;
        node* x = root;

        while(x!=NILL) {
            y = x;
            
            if(comp(x->key,val)) x = x->right;
            else if(comp(val,x->key)) x = x->left;
            else return; //value already in tree
        }
        
        node* z = new node(val,NILL,NILL,NILL);
        z->parent = y;

        if(y==NILL) root = z;
        else if(comp(val,y->key)) y->left = z;
        else y->right = z;

        fix_tree(z,root);
    }

    void erase(iterator it) {
        erase(it.it);
    }



    bool empty() {
        return !(root->size);
    }

    unsigned size() {
        return root->size;
    }
    
    iterator begin() {
        node* x = root;
        if(x!=NILL) {
            while(x->left!=NILL) {
                x=x->left;
            }
        }
        return iterator(x);
    }

    iterator end() {
        return iterator(NILL);
    }

    void print(iterator it) {
	    std::cout << it.it->key << " " << it.it->height << " " << it.it->size << std::endl;
    }


    
    /**
     * Finds the kth smallest node
     * k is 0 indexed
     */ 
    iterator find_by_order(int k) {
        k++;
        node* x = root;
        while(x!=NILL) {
            if(x->left->size>=k) x = x->left;
            else if(x->left->size+1==k) return iterator(x);
            else {
                k-=(x->left->size+1);
                x = x->right;
            }
        }

        return iterator(x);
    }

    /**
     * returns number of nodes smaller than val
     * 
     */
    int order_of_key(const T& val) {
        node* x = root;
        int p = 0;
        while(x!=NILL) {
            if(x->key==val) {
                p+=x->left->size;
                return p;
            } else if(x->key>val) {
                x=x->left;
            } else {
                p += x->left->size+1;
                x = x->right;
            }
        }

        return p;
    }

    ~AVLTree() {
        erase_sub_tree(root);
    }
};

template<class T,class Comp>
typename AVLTree<T,Comp>::node AVLTree<T,Comp>::NULL_NODE = {};


template<class T,class Comp>
typename AVLTree<T,Comp>::node* AVLTree<T,Comp>::NILL = &AVLTree<T,Comp>::NULL_NODE;
