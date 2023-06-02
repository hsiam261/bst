#include <functional>
#include <iostream>
#include <string>

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
	iterator find(const T& val) {
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
		std::cout << it.it->key << " " << it.it->height << " " << it.it->size << std::endl;
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
		bool flag = (t.find(val)!=t.end());

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





};

template<class T,class Comp>
typename splay_tree<T,Comp>::node splay_tree<T,Comp>::NULL_NODE = {};


template<class T,class Comp>
typename splay_tree<T,Comp>::node* splay_tree<T,Comp>::NILL = &splay_tree<T,Comp>::NULL_NODE;

