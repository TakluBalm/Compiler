#ifndef GRAPH_H
#define GRAPH_H

typedef struct graph* graph_p;
typedef struct graph_node* graph_node_p;

struct graph{
	int vertices;
	graph_node_p* edges;
	enum{
		DIRECTED,
		UNDIRECTED
	} type;
	void (*addEdge)(int i, int j, graph_p g);
};

graph_p GraphInit(int v, char type);
void PrintGraph(graph_p g);
void ResetGraph(graph_p g);
void DelGraph(graph_p g);

#endif