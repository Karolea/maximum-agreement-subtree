#include <stdio.h>
#include <string>
#include <iostream>

#define RESET 1
#define RETAIN 0
#define ROWS 1
#define COLUMNS 2


using namespace std;


/* single tree vertex */
struct vertex_T
{
	vertex_T* parent = nullptr;
	vertex_T* son = nullptr;
	vertex_T* brother = nullptr;
	string name = "";
};

struct tree_T
{
	vertex_T* root = nullptr;
	int leaves_n;								//number of leaves
	int internal_vertices_n;					//number of internal vertices
	vertex_T** leaves_p = nullptr;				//array of pointers to leaves
	vertex_T** internal_vertices_p = nullptr;	//array of pointers to internal vertices
	vertex_T** all_vertices_p = nullptr;		//array of pointers to all vertices
};


void new_vertex(string definition, vertex_T* v, int* int_vert_n, int* leaves_n, int reset = RETAIN)
{
	static int i = 0;					//index of a character in string 'definition'
	static int depth = 0;				//current tree depth
	static int leaves = 0;				//number of leaves
	static int internal_vertices = 0;	//number of internal vertices

	if (reset == RESET) i = depth = leaves = internal_vertices = 0;

	char c = definition[i];
	vertex_T* current_vertex = v;

	while (c != ')' && c != ';')
	{
		if (c == '(')
		{
			current_vertex->name = "I";
			current_vertex->name.push_back(internal_vertices + '0');
			current_vertex->son = new vertex_T;
			current_vertex->son->parent = current_vertex;
			++internal_vertices;
			++i;
			++depth;
			new_vertex(definition, current_vertex->son, int_vert_n, leaves_n, RETAIN);
			++i;
		}
		else if (c == ',')
		{
			current_vertex->brother = new vertex_T;
			current_vertex->brother->parent = current_vertex->parent;
			current_vertex = current_vertex->brother;
			++i;
			++leaves;
		}
		else
		{
			current_vertex->name.push_back(c);
			++i;
		}

		c = definition[i];
	}

	--depth;

	*int_vert_n = internal_vertices;
	*leaves_n = leaves + 1;
}

/* fills the arrays of pointers to vertices of the given tree */
void fill_arrays(vertex_T* v, vertex_T** leaves_p, vertex_T** internal_vertices_p, int reset = RETAIN)
{
	vertex_T* current_vertex = v;
	static int l = 0;						//index in the array of pointers to leaves
	static int i = 0;						//index in the array of pointers to internal vertices

	if (reset == RESET) l = i = 0;

	while (current_vertex != nullptr)
	{
		if (current_vertex->name[0] > '9')	//name starts with character other than a digit, so it's an internal vertex
		{
			internal_vertices_p[i] = current_vertex;
			++i;
		}
		else
		{
			leaves_p[l] = current_vertex;
			++l;
		}

		if (current_vertex->son != nullptr)
		{
			fill_arrays(current_vertex->son, leaves_p, internal_vertices_p, RETAIN);
		}

		current_vertex = current_vertex->brother;
	}
}

void build_tree(string definition, tree_T* tree)
{
	tree->root = new vertex_T;
	new_vertex(definition, tree->root, &(tree->internal_vertices_n), &(tree->leaves_n), RESET);
	tree->root->name = "ROOT";

	int l = tree->leaves_n;
	tree->leaves_p = new vertex_T* [l];
	int i = tree->internal_vertices_n;
	tree->internal_vertices_p = new vertex_T* [i];
	tree->all_vertices_p = new vertex_T* [l + i];

	fill_arrays(tree->root, tree->leaves_p, tree->internal_vertices_p, RESET);

	for (int j = 0; j < l + i; ++j)
	{
		if (j < l) tree->all_vertices_p[j] = tree->leaves_p[j];
		else tree->all_vertices_p[j] = tree->internal_vertices_p[j - l];
	}
}

/* for testing only */
void print_tree(vertex_T* v, int depth)
{	
	vertex_T* current_vertex = v;

	while (current_vertex != nullptr)
	{
		cout << current_vertex->name << " depth:" << depth << "\n";
		
		if (current_vertex->son != nullptr)
		{
			print_tree(current_vertex->son, depth + 1);
		}
		
		current_vertex = current_vertex->brother;
	}
}

/* checks if a subtree contains a specific vertex (defined by its name) */
bool find_vertex(vertex_T* start, string goal)
{
	vertex_T* current_vertex = start;

	while (current_vertex != nullptr)
	{
		if (current_vertex->name.compare(goal) == 0) return true;
		
		if (current_vertex->son != nullptr)
		{
			if (find_vertex(current_vertex->son, goal)) return true;
		}

		current_vertex = current_vertex->brother;
	}

	return false;
}

/* generates all k-variations without repetition of set S of n elements, then for each generated variation checks 
if such variation of indices gives better result for the maximal assignment problem than previously found one */
void variations_wo_rep(int* S, int n, int k, int depth, int** matrix, int* result, int dim, int reset = RETAIN)
{
	static int* variation = new int[k];
	if (reset == RESET)
	{
		delete[] variation;
		variation = new int [k];
	}

	bool occurred;

	if (depth < k)
	{
		for (int i = 0; i < n; ++i)
		{
			occurred = false;

			for (int j = 0; j < depth; ++j)
			{
				if (variation[j] == S[i])
				{
					occurred = true;
					break;
				}
			}

			if (!occurred)
			{
				variation[depth] = S[i];
				variations_wo_rep(S, n, k, depth + 1, matrix, result, dim, RETAIN);
			}
		}
	}
	else
	{
		int sum = 0;
		
		/* dim indicates which dimension is "smaller" (i.e. if there are more rows or columns),
		so that the generated variation of indices relates the other */
		if (dim == ROWS)
		{
			for (int i = 0; i < k; ++i) sum += matrix[i][variation[i]];
		}
		else
		{
			for (int i = 0; i < k; ++i) sum += matrix[variation[i]][i];
		}

		if (sum > *result) *result = sum;
	}
}

/* returns number of sons of the given vertex */
int count_sons(vertex_T* v)
{
	int sons = 0;

	vertex_T* current_vertex = v->son;

	while (current_vertex != nullptr)
	{
		++sons;
		current_vertex = current_vertex->brother;
	}

	return sons;
}

int compare_internal_vertices(vertex_T* v1, vertex_T* v2, int** matrix, tree_T* t1, tree_T* t2)
{
	int sons1_n = count_sons(v1);			//number of sons of vertex v1
	int sons2_n = count_sons(v2);			//number of sons of vertex v2

	int** sons_matrix = new int* [sons1_n];	//matrix used to compare sons of both vertices

	for (int i = 0; i < sons1_n; ++i)
	{
		sons_matrix[i] = new int[sons2_n];
	}

	vertex_T* son1;							//currently compared son of vertex v1
	vertex_T* son2;							//currently compared son of vertex v2

	/* numbers of all vertices in trees t1 and t2 */
	int vertices_n1 = t1->leaves_n + t1->internal_vertices_n;
	int vertices_n2 = t2->leaves_n + t2->internal_vertices_n;

	int result = 0;							//best result obtained after performing all comparisons

	int index_v1, index_v2;					//indices of vertices v1 and v2 in the main matrix
	for (int i = 0; i < vertices_n1; ++i)
	{
		if (t1->all_vertices_p[i]->name.compare(v1->name) == 0)
		{
			index_v1 = i;
			break;
		}
	}
	for (int i = 0; i < vertices_n2; ++i)
	{
		if (t2->all_vertices_p[i]->name.compare(v2->name) == 0)
		{
			index_v2 = i;
			break;
		}
	}

	/* filling sons_matrix + comparing v1 with all v2's sons and vice versa */
	for (int i = 0; i < sons1_n; ++i)
	{
		son1 = v1->son;
		for (int k = 0; k < i; ++k) son1 = son1->brother;

		int index1;							//index of son1 in the main matrix
		for (int j = 0; j < vertices_n1; ++j)
		{
			if (t1->all_vertices_p[j]->name.compare(son1->name) == 0)
			{
				index1 = j;
				break;
			}
		}

		/* checking if vertex v2 can be identified with current son of vertex v1 */
		if (matrix[index1][index_v2] > result) result = matrix[index1][index_v2];
		
		for (int j = 0; j < sons2_n; ++j)
		{
			son2 = v2->son;
			for (int k = 0; k < j; ++k) son2 = son2->brother;
			
			int index2;						//index of son2 in the main matrix			
			for (int k = 0; k < vertices_n2; ++k)
			{
				if (t2->all_vertices_p[k]->name.compare(son2->name) == 0)
				{
					index2 = k;
					break;
				}
			}

			/* checking if vertex v1 can be identified with current son of vertex v2 */
			if (matrix[index_v1][index2] > result) result = matrix[index_v1][index2];

			sons_matrix[i][j] = matrix[index1][index2];
		}
	}

	/* solving the assignment problem for sons_matrix by checking all possible assignments
	(generating all proper variations without repetition) */
	int* possible_indices;	//set of all indices possible to choose (indices of the "bigger" dimension of sons_array)

	if (sons1_n >= sons2_n)	//there are more rows than columns in the sons_matrix
	{
		possible_indices = new int[sons1_n];
		for (int i = 0; i < sons1_n; ++i) possible_indices[i] = i;

		variations_wo_rep(possible_indices, sons1_n, sons2_n, 0, sons_matrix, &result, COLUMNS, RESET);

		delete[] possible_indices;
	}
	else
	{
		possible_indices = new int[sons2_n];
		for (int i = 0; i < sons2_n; ++i) possible_indices[i] = i;

		variations_wo_rep(possible_indices, sons2_n, sons1_n, 0, sons_matrix, &result, ROWS, RESET);

		delete[] possible_indices;
	}

	for (int i = 0; i < sons1_n; ++i) delete[] sons_matrix[i];
	delete[] sons_matrix;

	
	return result;
}

void compare_trees(tree_T* t1, tree_T* t2)
{
	int l1 = t1->leaves_n;
	int l2 = t2->leaves_n;
	int i1 = t1->internal_vertices_n;
	int i2 = t2->internal_vertices_n;
	
	int dim1 = l1 + i1;					//number of rows in the comparison matrix
	int dim2 = l2 + i2;					//number of columns in the comparison matrix

	int** matrix = new int*[dim1];
	for (int i = 0; i < dim1; ++i)
	{
		matrix[i] = new int[dim2];
	}

	/* filling first part of the matrix (l1-l2 comparison) */
	for (int i = 0; i < l1; ++i)
	{
		for (int j = 0; j < l2; ++j)
		{
			if (t1->leaves_p[i]->name.compare(t2->leaves_p[j]->name) == 0) matrix[i][j] = 1;
			else matrix[i][j] = 0;
		}
	}

	/* filling second part of the matrix (i1-l2 comparison) */
	for (int i = i1 - 1; i >= 0; --i)
	{
		for (int j = 0; j < l2; ++j)
		{
			if (find_vertex(t1->internal_vertices_p[i]->son, t2->leaves_p[j]->name)) matrix[l1 + i][j] = 1;
			else matrix[l1 + i][j] = 0;
		}
	}

	/* filling third part of the matrix (l1-i2 comparison) */
	for (int i = 0; i < l1; ++i)
	{
		for (int j = i2 - 1; j >= 0; --j)
		{
			if (find_vertex(t2->internal_vertices_p[j]->son, t1->leaves_p[i]->name)) matrix[i][l2 + j] = 1;
			else matrix[i][l2 + j] = 0;
		}
	}

	/* filling fourth part of the matrix (i1-i2 comparison) */
	for (int i = i1 - 1; i >= 0; --i)
	{
		for (int j = i2 - 1; j >= 0; --j)
		{
			/* vertices to compare */
			vertex_T* v1 = t1->internal_vertices_p[i];
			vertex_T* v2 = t2->internal_vertices_p[j];
			
			matrix[l1 + i][l2 + j] = compare_internal_vertices(v1, v2, matrix, t1, t2);
		}
	}

	/*
	for (int i = 0; i < dim1; ++i)
	{
		for (int j = 0; j < dim2; ++j) cout << matrix[i][j] << " ";
		cout << '\n';
	}
	*/

	/* final result of comparison of both trees – the minimum number of leaves that need to be removed 
	from both trees to make them isomorphic */
	cout << l1 - matrix[l1][l2] << '\n';

	for (int i = 0; i < dim1; ++i) delete[] matrix[i];
	delete[] matrix;
}


int main()
{
	int n;
	scanf_s("%d", &n);
	tree_T* trees = new tree_T[n];

	string definition;				//definition of a tree in Newick format

	for (int i = 0; i < n; ++i)
	{
		cin >> definition;
		build_tree(definition, &trees[i]);
	}

	for (int i = 0; i < n; ++i)
	{
		for (int j = i + 1; j < n; ++j)
		{
			compare_trees(&trees[i], &trees[j]);
		}
	}


	return 0;
}

/* test inputs */
/*
(4,(8,1,(5,3)),(9,2,(10,(7,6))));
(10,(8,(9,(5,4)),(6,2,3)),(7,1));

(1,5,(10,6,3),(2,(8,7)),(9,4));
((7,(3,(4,9,(1,2)))),8,(5,(10,6)));
(7,((6,(9,5),(8,3)),(1,(2,10,4))));
(7,(8,3,4,6,1),(9,5,10),2);
*/