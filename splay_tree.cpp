#include <functional>
#include <bits/stdc++.h>
using namespace std;

/**
 * splay Tree Class
 * Can't insert the same key more than once
 */

template<class T,typename Comp = std::less<T>>
class splay_tree {    
    struct node {    
        T key;
        int height = -1;
        int size = 0;
        node* left;
        node* right;
        node* parent;

        node(const T& key,node* left,node* right,node* parent) : key(key), 
        left(left), right(right), parent(parent), height(0), size(1) {}

        node() : left(this),right(this),parent(this) {}   
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
        friend class splay_tree<T,Comp>;
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
     * Does a Single splay Rotation
     */ 
    void single_splay(node* x) {
        if(x->parent->parent==NILL) {
            if(x==x->parent->left) single_rotate_right(x->parent);
            else single_rotate_left(x->parent);
            
        } else {
            node* grandfather = x->parent->parent;
            relax_augmentation(x->parent);
            if(x->parent == grandfather->left) {
                if(x == x->parent->left) {
                    single_rotate_right(grandfather);
                    single_rotate_right(x->parent);
                } else {
                    single_rotate_left(x->parent);
                    single_rotate_right(x->parent);
                }
            } else {
                if(x == x->parent->right) {
                    single_rotate_left(grandfather);
                    single_rotate_left(x->parent);
                } else {
                    single_rotate_right(x->parent);
                    single_rotate_left(x->parent);
                }
            }
        }
    }

    void splay(node* x) {
        while(x->parent!=NILL) {
            single_splay(x);
        }
    }

    /**
     * A helper function for the erase(iterator) method
     * z can't be NILL
     */ 
    void erase(node* z) {
        splay(z);

        if(z->left==NILL) transplant(z,z->right);
        else if(z->right==NILL) transplant(z,z->left);
        else {
            node* y = predecessor(z); //y is not NILL and y has no left child cause z->left is not NILL
            root = z->left;
            root->parent = NILL;
            
            splay(y);

            y->right = z->right;
            if(y->right!=NILL) y->right->parent = y;
            
            relax_augmentation(y);
        }

        delete z;
    }

    public:
    iterator search(const T& val) {
        node* x = root;
        node* prev = NILL;
        while(x!=NILL) {
            prev = x;
            if(comp(x->key,val)) x = x->right;
            else if(comp(val,x->key)) x = x->left;
            else {
                splay(x);
                return iterator(x);
            }

        }

        if(prev!=NILL) splay(prev);
        return iterator(NILL);
    }
    

    /**
     * Inserts a new node into the tree 
     * preserves augmentation properties
     * If the value already exists in the tree it does not insert it again 
     * but does a splay from the leaf
     */ 
    void insert(const T& val) {
        node* y = NILL;
        node* x = root;

        while(x!=NILL) {
            y = x;
            
            if(comp(x->key,val)) x = x->right;
            else if(comp(val,x->key)) x = x->left;
            else {
                splay(x);
                return; //value already in tree
            }
        }
        
        node* z = new node(val,NILL,NILL,NILL);
        z->parent = y;

        if(y==NILL) root = z;
        else if(comp(val,y->key)) y->left = z;
        else y->right = z;


        splay(z);
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
        if(x!=NILL) splay(x);
        return iterator(x);
    }

    iterator end() {
        return iterator(NILL);
    }

    void print(iterator it) {
        cout << it.it->key << " " << it.it->height << " " << it.it->size << endl;
    }


    
    /**
     * Finds the kth smallest node
     * k is 0 indexed
     */ 
    iterator find_by_order(int k) {
        k++;
        node* x = root;
        node* y = NILL;
        while(x!=NILL) {
            y = x;
            if(x->left->size>=k) x = x->left;
            else if(x->left->size+1==k) {
                splay(x);
                return iterator(x);
            }
            else {
                k-=(x->left->size+1);
                x = x->right;
            }
        }

        if(y!=NILL) splay(y);
        return iterator(x);
    }

    /**
     * returns number of nodes smaller than val
     * 
     */
    int order_of_key(const T& val) {
        node* x = root;
        node* prev = NILL;
        int p = 0;
        while(x!=NILL) {
            prev = x;
            if(x->key==val) {
                p+=x->left->size;
                splay(x);
                return p;
            } else if(x->key>val) {
                x=x->left;
            } else {
                p += x->left->size+1;
                x = x->right;
            }
        }

        if(prev!=NILL) splay(prev);
        return p;
    }

    ~splay_tree() {
        erase_sub_tree(root);
    }

    static void split(splay_tree<T,Comp>& t,splay_tree<T,Comp>& s1,splay_tree<T,Comp>& s2,const T& val) {
        bool flag = (t.search(val)!=t.end());
        
        s1.erase_sub_tree(s1.root);
        s2.erase_sub_tree(s2.root);
        
        s1.root = NILL;
        s2.root = NILL;
        
        if(t.root!=NILL) { 
            if(!t.comp(val,t.root->key)) {
                s1.root = t.root;
                s2.root = t.root->right;
                
                s1.root->right=NILL;
                s2.root->parent=NILL;
            } else {
                s2.root = t.root;
                s1.root = t.root->left;

                s2.root->left = NILL;
                s1.root->parent = NILL;
            }
        }

        relax_augmentation(s1.root);
        relax_augmentation(s2.root);
        t.root = NILL;
    } 

    static void join(splay_tree<T,Comp>& t,splay_tree<T,Comp>& s1,splay_tree<T,Comp>& s2) {
        t.erase_sub_tree(t.root);
        
        if(s1.root == NILL && s2.root == NILL) t.root = NILL;
        else if(s1.root == NILL) {
            t.root = s2.root;
            s2.root = NILL;
        } else if(s2.root ==NILL){
            t.root = s1.root;
            s1.root = NILL;
        } else {
            s1.find_by_order(s1.size()-1);       
            s1.root->right = s2.root;
            s2.root->parent = s1.root;
            s2.root = NILL;
            t.root = s1.root;
            relax_augmentation(t.root);
            s1.root = NILL;
        }
    }


    struct cell_display {
        string   valstr;
        bool     present;
        cell_display() : present(false) {}
        cell_display(std::string valstr) : valstr(valstr), present(true) {}
    };

    using display_rows = vector< vector< cell_display > >;

    inline int depth(node* x) {
        return x->height+1;
    }

    void print_root() {
        print(iterator(root));
    }

    display_rows get_row_display() const {
        // start off by traversing the tree to
        // build a vector of vectors of Node pointers
        //vector<node*> traversal_stack;
        vector< std::vector<node*> > rows;
        if(!root) return display_rows();

        node *p = root;
        //cout << p->key << "***********" << endl;
        const int max_depth = root->height+1;
        rows.resize(max_depth);
        // int depth = 0;
        // for(;;) {
        //     // Max-depth Nodes are always a leaf or null
        //     // This special case blocks deeper traversal
        //     if(depth == max_depth-1) {
        //         rows[depth].push_back(p);
        //         if(depth == 0) break;
        //         --depth;
        //         continue;
        //     }

        //     // First visit to node?  Go to left child.
        //     if(traversal_stack.size() == depth) {
        //         rows[depth].push_back(p);
        //         traversal_stack.push_back(p);
        //         if(p!=NILL) p = p->left;
        //         ++depth;
        //         continue;
        //     }

        //     // Odd child count? Go to right child.
        //     if(rows[depth+1].size() % 2) {
        //         p = traversal_stack.back();
        //         if(p!=NILL) p = p->right;
        //         ++depth;
        //         continue;
        //     }

        //     // Time to leave if we get here

        //     // Exit loop if this is the root
        //     if(depth == 0) break;

        //     traversal_stack.pop_back();
        //     p = traversal_stack.back();
        //     --depth;
        // }

        // Use rows of Node pointers to populate rows of cell_display structs.
        // All possible slots in the tree get a cell_display struct,
        // so if there is no actual Node at a struct's location,
        // its boolean "present" field is set to false.
        // The struct also contains a string representation of
        // its Node's value, created using a std::stringstream object.

        rows[0].push_back(root);
        for(int i=1;i<root->height+1;i++) {
            for(auto x:rows[i-1]) {
                rows[i].push_back(x->left);
                rows[i].push_back(x->right);
            }
        }

        display_rows rows_disp;
        std::stringstream ss;
        for(const auto& row : rows) {
            rows_disp.emplace_back();
            for(node* pn : row) {
                if(pn!=NILL) {
                    ss << pn->key;
                    rows_disp.back().push_back(cell_display(ss.str()));
                    ss = std::stringstream();
                } else {
                    rows_disp.back().push_back(cell_display());
                }   
            }   
        }

        return rows_disp;
    }


    // row_formatter takes the vector of rows of cell_display structs 
    // generated by get_row_display and formats it into a test representation
    // as a vector of strings
    vector<string> row_formatter(const display_rows& rows_disp) const {
        using s_t = string::size_type;

        // First find the maximum value string length and put it in cell_width
        s_t cell_width = 0;
        for(const auto& row_disp : rows_disp) {
            for(const auto& cd : row_disp) {
                if(cd.present && cd.valstr.length() > cell_width) {
                    cell_width = cd.valstr.length();
        }   }   }

        // make sure the cell_width is an odd number
        if(cell_width % 2 == 0) ++cell_width;

        // formatted_rows will hold the results
        vector<string> formatted_rows;

        // some of these counting variables are related,
        // so its should be possible to eliminate some of them.
        s_t row_count = rows_disp.size();

        // this row's element count, a power of two
        s_t row_elem_count = 1 << (row_count-1);

        // left_pad holds the number of space charactes at the beginning of the bottom row
        s_t left_pad = 0;

        // Work from the level of maximum depth, up to the root
        // ("formatted_rows" will need to be reversed when done) 
        for(s_t r=0; r<row_count; ++r) {
            const auto& cd_row = rows_disp[row_count-r-1]; // r reverse-indexes the row
            // "space" will be the number of rows of slashes needed to get
            // from this row to the next.  It is also used to determine other
            // text offsets.
            s_t space = (s_t(1) << r) * (cell_width + 1) / 2 - 1;
            // "row" holds the line of text currently being assembled
            string row;
            // iterate over each element in this row
            for(s_t c=0; c<row_elem_count; ++c) {
                // add padding, more when this is not the leftmost element
                row += string(c ? left_pad*2+1 : left_pad, ' ');
                if(cd_row[c].present) {
                    // This position corresponds to an existing Node
                    const string& valstr = cd_row[c].valstr;
                    // Try to pad the left and right sides of the value string
                    // with the same number of spaces.  If padding requires an
                    // odd number of spaces, right-sided children get the longer
                    // padding on the right side, while left-sided children
                    // get it on the left side.
                    s_t long_padding = cell_width - valstr.length();
                    s_t short_padding = long_padding / 2;
                    long_padding -= short_padding;
                    row += string(c%2 ? short_padding : long_padding, ' ');
                    row += valstr;
                    row += string(c%2 ? long_padding : short_padding, ' ');
                } else {
                    // This position is empty, Nodeless...
                    row += string(cell_width, ' ');
                }
            }
            // A row of spaced-apart value strings is ready, add it to the result vector
            formatted_rows.push_back(row);

            // The root has been added, so this loop is finsished
            if(row_elem_count == 1) break;

            // Add rows of forward- and back- slash characters, spaced apart
            // to "connect" two rows' Node value strings.
            // The "space" variable counts the number of rows needed here.
            s_t left_space  = space + 1;
            s_t right_space = space - 1;
            for(s_t sr=0; sr<space; ++sr) {
                string row;
                for(s_t c=0; c<row_elem_count; ++c) {
                    if(c % 2 == 0) {
                        row += string(c ? left_space*2 + 1 : left_space, ' ');
                        row += cd_row[c].present ? '/' : ' ';
                        row += string(right_space + 1, ' ');
                    } else {
                        row += string(right_space, ' ');
                        row += cd_row[c].present ? '\\' : ' ';
                    }
                }
                formatted_rows.push_back(row);
                ++left_space;
                --right_space;
            }
            left_pad += space + 1;
            row_elem_count /= 2;
        }

        // Reverse the result, placing the root node at the beginning (top)
        std::reverse(formatted_rows.begin(), formatted_rows.end());

        return formatted_rows;
    }

    // Trims an equal number of space characters from
    // the beginning of each string in the vector.
    // At least one string in the vector will end up beginning
    // with no space characters.
    static void trim_rows_left(vector<string>& rows) {
        if(!rows.size()) return;
        auto min_space = rows.front().length();
        for(const auto& row : rows) {
            auto i = row.find_first_not_of(' ');
            if(i==string::npos) i = row.length();
            if(i == 0) return;
            if(i < min_space) min_space = i;
        }
        for(auto& row : rows) {
            row.erase(0, min_space);
    }   }

    // Dumps a representation of the tree to cout
    void Dump() {
        int d = root->height+1;

        // If this tree is empty, tell someone
        if(d == 0) {
            cout << " <empty tree>\n";
            return;
        }

        
        // This tree is not empty, so get a list of node values...
        const auto rows_disp = get_row_display();
        // then format these into a text representation...
        auto formatted_rows = row_formatter(rows_disp);
        // then trim excess space characters from the left sides of the text...
        trim_rows_left(formatted_rows);
        // then dump the text to cout.
        for(const auto& row : formatted_rows) {
            std::cout << ' ' << row << '\n';
        }
    }
};

template<class T,class Comp>
typename splay_tree<T,Comp>::node splay_tree<T,Comp>::NULL_NODE = {};


template<class T,class Comp>
typename splay_tree<T,Comp>::node* splay_tree<T,Comp>::NILL = &splay_tree<T,Comp>::NULL_NODE;

int main() {
    splay_tree<int> s;
    while(true) {
        char c;
        int t;
        cin >> c >> t;
        if(c=='i') {
            s.insert(t);
        } else if(c=='s') {
            s.search(t);
        } else {
            auto it = s.search(t);
            s.erase(it);
        }
        cout << "root : ";
        s.print_root();
        // for(auto it=s.begin();it!=s.end();++it) {
        //     s.print(it);
        // }

        s.Dump();
    }
}