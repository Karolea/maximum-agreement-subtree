# maximum-agreement-subtree

Program reads a set of _n_ rooted trees given in the [Newick format](https://evolution.gs.washington.edu/phylip/newicktree.html) and for each pair of trees resolves so-called [maximum agreement subtree problem](https://en.wikipedia.org/wiki/Maximum_agreement_subtree_problem) – it calculates the minimum number of leaves that should be removed from each tree to make them isomorphic.  
Here are two illustrative trees, both having 10 identical leaves labelled from 1 to 10:

(4,(8,1,(5,3)),(9,2,(10,(7,6))));  
(10,(8,(9,(5,4)),(6,2,3)),(7,1));

Given trees are stored in the memory in the form of general trees having one pointer to the child node, one pointer to the brother node and one pointer to the parent node. Below is shown such representation of two previously mentioned trees:  

![general trees representation](http://stos.eti.pg.gda.pl/~goluch/lab/img/GeneralTreeParents.png)  

Pointers (arrows) pointing downwards represent parent-child relation; pointers ponting leftwards represent brother-brother relation. Pointers pointing upwards represent son-parent relation – every node except the root has such pointer.

All nodes in every tree are given labels – a number for leaves (e.g. 2) and a combination of ‘I’ letter and a number for internal nodes (e.g. I4).


### algorithm
For each pair of trees: 
1.	a square array is created where dimensions contain all vertices of first and second tree respectively
2.	the array is filled according to the following rules:
    1.	**_leaf-leaf_ comparison**: if leaves have same label – put 1, else – put 0
    2.	**_internal node-leaf_ comparison (and vice versa)**: if inside an internal node (i.e. in a subtree where the current internal node is the root) of one tree there is a leaf corresponding to the leaf of the second tree – put 1, else – put 0
    3.	**_internal node-internal node_ comparison**: three possibilities need to be checked and the algorithm chooses the one giving best result (i.e. the largest number of corresponding leaves, which is then inserted into the array):
        1.	check if one of the sons of AX node can be identified with the BX node
        2.	check if one of the sons of BX node can be identified with the AX node
        3.	identify the sons of AX node with the sons of BX node (not necessarily all) so that the number of identical nodes is as large as possible  

The last comparison – comparison of the roots – gives the number of leaves of the maximum agreement subtree. Subtracting it from the initial number of leaves in one tree gives the result.
