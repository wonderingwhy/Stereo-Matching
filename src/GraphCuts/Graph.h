#ifndef GRAPH_H
#define GRAPH_H

#include <string.h>
#include <assert.h>
#include <vector>
#include <queue>
#include <limits>


template <typename captype, typename tcaptype, typename flowtype> class Graph {
public:
    typedef enum { SOURCE = 0, SINK = 1} termtype; ///< terminals
    typedef int node_id;
    typedef int arc_id;

    Graph(int hintNbNodes = 0, int hintNbArcs = 0);
    virtual ~Graph();

    node_id add_node();
    void add_edge(node_id i, node_id j, captype capij, captype capji);
    void add_edge_infty(node_id i, node_id j);
    void add_tweights(node_id i, tcaptype capS, tcaptype capT);

    flowtype maxflow();
    termtype what_segment(node_id i, termtype defaultSegm = SOURCE) const;

private:
    struct node;
    struct arc;

    /// A node of the graph
    struct node {
        arc_id first;  ///< first outgoing arc
        arc *parent;   ///< initial path to root (a terminal node) if in tree
        node *next;    ///< pointer to next active node (itself if last node)
        int ts;        ///< timestamp showing when DIST was computed
        int dist;      ///< distance to the terminal
        termtype term; ///< source or sink tree? (only if parent!=NULL)
        tcaptype cap;  ///< capacity of arc SOURCE->node(>0) or node->SINK(<0)
    };
    /// An arc of the graph
    struct arc {
        node_id head;  ///< node the arc points to
        arc_id next;   ///< next arc with the same originating node
        arc_id sister; ///< reverse arc
        captype cap;   ///< residual capacity
    };

    std::vector<node> nodes; ///< All nodes of graph
    std::vector<arc> arcs;   ///< All arcs of graph

    flowtype flow; ///< total flow
    node *activeBegin, *activeEnd; ///< list of active nodes
    std::queue<node *> orphans; ///< list of pointers to orphans
    int time; ///< monotonically increasing global counter

    // special constants for node.parent
    arc *TERMINAL; ///< arc to terminal
    arc *ORPHAN;   ///< arc to orphan

    // functions for processing active list
    void set_active(node *i);
    node *next_active();

    // functions for processing orphans
    void set_orphan(node *i);
    void process_orphan(node *i);
    void adopt_orphans();

    void maxflow_init();
    int dist_to_root(node *j);
    arc *grow_tree(node *i);
    captype find_bottleneck(arc *midarc);
    void push_flow(arc *midarc, captype f);
    void augment(arc *middle_arc);
};

// Necessary for templates: provide full implementation
#include "Graph.hpp"
#include "GraphMaxFlow.hpp"

#endif
