#ifndef DEBUG
#define DEBUG 0
#endif

#include "include.h"

struct graph_node{
	int key;
	graph_node_p next;
};

graph_node_p GetNode(int key){
	graph_node_p n = malloc(sizeof(*n));
	n->key = key;
	n->next = NULL;
	return n;
}

void ADD_EDGE(int i, int j, graph_p g){
	if(i > g->vertices || j > g->vertices){
		printf("Cannot add edge. Not valid edge.\n");
	}
	graph_node_p n = GetNode(j);
	n->next = g->edges[i];
	g->edges[i] = n;
}

graph_p GraphInit(int v, char type){
	graph_p g = malloc(sizeof(*g));
	g->vertices = v;
	g->edges = malloc(v*sizeof(**(g->edges)));
	g->type = type;
	g->addEdge = ADD_EDGE;
	for(int i = 0; i < v; i++){
		g->edges[i] = NULL;
	}
	return g;
}

void PrintList(graph_node_p start){
	if(start == NULL){
		printf("\n");
		return;
	}
	graph_node_p current = start->next;
	printf("%d", start->key);
	while(current != NULL){
		printf(" -> %d", current->key);
		current = current->next;
	}
	printf("\n");
}

void PrintGraph(graph_p g){
	int v = g->vertices;
	for(int i = 0; i < v; i++){
		printf("Vertex %d: ", i);
		PrintList(g->edges[i]);
	}
}

void ResetGraph(graph_p g){
	for(int i = 0; i < g->vertices; i++){
		graph_node_p current = g->edges[i];
		while(current){
			graph_node_p next = current->next;
			free(current);
			current = next;
		}
	}
	free(g->edges);
	g->edges = NULL;
	g->vertices = 0;
}

void DelGraph(graph_p g){
	ResetGraph(g);
	free(g);
}