#!/bin/bash 
set -xe

SEED=0
NUM_TESTS=1000000

python3 preprocess.py gnu_ost_randomized_stress_test.cpp > gnu_ost.cpp
g++ -std=c++14 -o gnu_ost.out -O3 gnu_test.cpp
time ./gnu_test.out $SEED $NUM_TESTS > gnu_test.txt 


# AVLTree: test diff with gnu-test
python3 preprocess.py avl_tree_randomized_stress_test.cpp > avl_test.cpp
g++ -std=c++14 -o avl_test.out -O3 avl_test.cpp
time ./avl_test.out $SEED $NUM_TESTS > avl_test.txt 
diff gnu_test.txt avl_test.txt


# Red-Black Tree: test diff with gnu-test
python3 preprocess.py rb_tree_randomized_stress_test.cpp > rb_test.cpp
g++ -std=c++14 -o rb_test.out -O3 rb_test.cpp 
time ./rb_test.out $SEED $NUM_TESTS > rb_test.txt 
diff gnu_test.txt rb_test.txt



# splay_tree: test diff with gnu-test
python3 preprocess.py splay_tree_randomized_stress_test.cpp > splay_test.cpp
g++ -std=c++14 -o splay_test.out -O3 splay_test.cpp
time ./splay_test.out $SEED $NUM_TESTS > splay_test.txt
diff gnu_test.txt splay_test.txt




