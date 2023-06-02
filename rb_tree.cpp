#include <functional>
#include <bits/stdc++.h>
using namespace std;

/**
 * RBTree Class
 * Can't insert the same key more than once
 */
template<class T,typename Comp = std::less<T>>
class RBTree {
    enum _color {red,black};
    struct node {    
        T key;
        int height = -1;
        int size = 0;
        node* left;
        node* right;
        node* parent;
        _color color = black; 

        node(const T& key,node* left,node* right,node* parent) : key(key), 
        left(left), right(right), parent(parent), height(0), size(1), color(red) {}

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
        friend class RBTree<T,Comp>;
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
     * A max function
     */ 
    inline int max(int a,int b) {
        if(a>b) return a;
        return b;
    }

    inline void swap(_color& c1,_color& c2) {
        _color temp = c1;
        c1 = c2;
        c2 = temp;
    }
    
    /**
     * if x is NILL it keeps the status quo i.e. in the valid state(size = 0 and height = -1)
     * Otherwise it updates the height and size assuming that it's children have valid
     * size and height values
     */ 
    inline void relax_augmentation(node* x) {
        if(x==NILL) return;
        x->height = max(x->left->height,x->right->height)+1;
        x->size = x->left->size + x->right->size + 1;
    }

    /**
     * x is not NILL
     * x must have a right child
     * must adjust root when necessary
     * must fix augmentation code locally
     * Assuming decendents of x has valid augmentation
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
     * Assuming decendents of x has valid augmentation
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
     * traverses from node y to root and relaxes augmentation values on all nodes on the path
     * Used in insert, insert_fix, erase, deletion_fix and rotations
     * in insert and insert_fixup fix_augmentation() is not used on NILL
     * in deletion_fix it is but the NILL was assigned a parent so, no problem is faced
     */ 
    void fix_augmentation(node* y) {
        while(y!=root) {
            relax_augmentation(y);
            y=y->parent;
        }

        relax_augmentation(root);
    }



    /**
     * private helper function to rebalance, recolor and fix augmentation 
     * after a successful insertion operation
     */  
    void rb_insert_fixup(node* z) {
        while(z->parent->color==red) {
            if(z->parent==z->parent->parent->left) {
                node* uncle = z->parent->parent->right;
                if(uncle->color==red) {
                    z->parent->parent->color = red;
                    uncle->color = black;
                    z->parent->color = black;

                    relax_augmentation(z->parent);
                    relax_augmentation(z->parent->parent);      
                    
                    z = z->parent->parent;
                } else {
                    if(z==z->left->right) {
                        z = z->parent;
                        single_rotate_left(z);
                    }
                    z->parent->color = black;
                    z->parent->parent->color = red;
                    relax_augmentation(z->parent->parent);
                    single_rotate_right(z->parent->parent);
                }

            } else {
                node* uncle = z->parent->parent->left;
                if(uncle->color==red) {
                    z->parent->parent->color = red;
                    uncle->color = black;
                    z->parent->color = black;

                    relax_augmentation(z->parent);
                    relax_augmentation(z->parent->parent);      
                    
                    z = z->parent->parent;
                } else {
                    if(z==z->right->left) {
                        z = z->parent;
                        single_rotate_right(z);
                    }
                    z->parent->color = black;
                    z->parent->parent->color = red;
                    relax_augmentation(z->parent->parent);
                    single_rotate_left(z->parent->parent);
                }
            }
        }

        fix_augmentation(z);
        root->color = black;
    }
    
    /**
     * A private helper function for the erase method
     * replaces subtree rooted at u with subtree rooted at v
     * assigns root when necessary
     * DOES NOT FIX AUGMENTATION OR PRESERVE RB PROPERTIES
     */ 
    inline void transplant(node* u,node* v) {
        if(u->parent==NILL) {
            root = v;
        } else if(u->parent->left == u) {
            u->parent->left = v;
        } else {
            u->parent->right = v;
        }
        v->parent = u->parent;
    }

    /**
     * A helper function for the erase(iterator) method
     * z can't be NILL
     */ 
    void erase(node* z) {
        node* y = z;
        _color y_original_color = y->color;
        node* x;
        if(z->left==NILL) {
            x = z->right;
            transplant(z,z->right);
        } else if(z->right==NILL) {
            x = z->left;
            transplant(z,z->left);
        } else {
            y = successor(z); //y is not NILL and y has no left child cause z->right is not NILL
            y_original_color = y->color;
            x = y->right;
            if(y->parent==z) {
                x->parent = y;    
            } else {
                transplant(y,y->right);
                y->right = z->right;
                y->right->parent = y;
            }

            transplant(z,y);
            y->left = z->left;
            y->left->parent = y;
            y->color = z->color;              
        }

        delete z;
        if(y_original_color==black) {
            rb_delete_fix_up(x);
        } else {
            fix_augmentation(x);
        }
    }


    /**
     * private helper function to recolor, rebalance and fix augmentation 
     * after a deletion operation
     */ 
    void rb_delete_fix_up(node* x) {
        node* fixer = x;
        /**
         * loop invariants :
         * all children of x has the correct augmentation
         * x is doubly black
         * x is not root
         */ 
        while(x!=root && x->color!=black) {
            if(x==x->parent->left) { 
                node* brother = x->parent->right;
                if(brother->color==red) {
                    brother->color = black;
                    x->parent->color = red;
                    single_rotate_left(x->parent);
                    brother = x->parent->right;
                }

                //bro is now black 
                if(brother->left->color==black && brother->right->color==black) {
                    brother->color = red;
                    relax_augmentation(x);
                    x = x->parent;
                } else {
                    if(brother->right->color==black) {
                        brother->left->color = black;
                        brother->color = red;
                        single_rotate_right(brother);
                        brother = x->parent->right;
                    }

                    brother->color = x->parent->color;
                    x->parent->color = black;
                    brother->right->color = black;
                    single_rotate_left(x->parent);
                    fixer = x;
                    x = root;
                }

            } else {
                node* brother = x->parent->left;
                if(brother->color==red) {
                    brother->color = black;
                    x->parent->color = red;
                    single_rotate_right(x->parent);
                    brother = x->parent->left;
                } //done

                //bro is now black 
                if(brother->left->color==black && brother->right->color==black) {
                    brother->color = red;
                    relax_augmentation(x);
                    x = x->parent; //done
                } else {
                    if(brother->left->color==black) {
                        brother->right->color = black;
                        brother->color = red;
                        single_rotate_left(brother);
                        brother = x->parent->left;
                    }

                    brother->color = x->parent->color;
                    x->parent->color = black;
                    brother->left->color = black;
                    single_rotate_right(x->parent);
                    fixer = x;
                    x = root;
                }

            }
        }

        fix_augmentation(fixer);
    }


    public:
    iterator search(const T& val) {
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

        rb_insert_fixup(z);
    }

    void erase(iterator it) {
        erase(it.it);
    }



    bool empty() {
        return !(root->size);
    }
    unsigned size() {
        root->size;
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
        cout << it.it->key << " " << it.it->height << " " << it.it->size << " " << ((it.it->color==red)?"red":"black") << endl;
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

    ~RBTree() {
        erase_sub_tree(root);
    }
};

template<class T,class Comp>
typename RBTree<T,Comp>::node RBTree<T,Comp>::NULL_NODE = {};


template<class T,class Comp>
typename RBTree<T,Comp>::node* RBTree<T,Comp>::NILL = &RBTree<T,Comp>::NULL_NODE;

int main() {
    RBTree<int> t;
    t.insert(1);
    t.insert(2);
    t.insert(3);
    t.insert(4);
    t.insert(5);
    t.insert(6);
    t.insert(7);

    for(auto it = t.begin();it!=t.end();++it) {
        t.print(it);
    }
}
