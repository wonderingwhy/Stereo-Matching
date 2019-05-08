#ifndef ENERGY_H
#define ENERGY_H

#include <assert.h>
#include "Graph.h"

class Energy : Graph<short, short, int> {
public:
    typedef node_id Var;
    typedef short Value; ///< Type of a value in a single term
    typedef int TotalValue; ///< Type of a value of the total energy

    Energy(int hintNbNodes = 0, int hintNbArcs = 0);
    ~Energy();

    Var add_variable(Value E0 = 0, Value E1 = 0);
    void add_constant(Value E);
    void add_term1(Var x, Value E0, Value E1);
    void add_term2(Var x, Var y, Value E00, Value E01, Value E10, Value E11);
    void forbid01(Var x, Var y);

    TotalValue minimize();
    int get_var(Var x) const;

private:
    TotalValue Econst; ///< Constant added to the energy
};

/// Constructor.
/// For efficiency, it is advised to give appropriate hint sizes.
inline Energy::Energy(int hintNbNodes, int hintNbArcs)
    : Graph<short, short, int>(hintNbNodes, hintNbArcs), Econst(0)
{}

/// Destructor
inline Energy::~Energy() {}

/// Add a new binary variable
inline Energy::Var Energy::add_variable(Value E0, Value E1) {
    Energy::Var var = add_node();
    add_term1(var, E0, E1);
    return var;
}

/// Add a constant to the energy function
inline void Energy::add_constant(Value A) {
    Econst += A;
}

/// Add a term E(x) of one binary variable to the energy function, where
/// E(0)=E0, E(1)=E1. E0 and E1 can be arbitrary.
inline void Energy::add_term1(Var x, Value E0, Value E1) {
    add_tweights(x, E1, E0);
}

/// Add a term E(x,y) of two binary variables to the energy function, where
/// E(0,0)=A, E(0,1)=B, E(1,0)=C, E(1,1)=D.
/// The term must be regular, i.e. E00+E11 <= E01+E10
inline void Energy::add_term2(Var x, Var y, Value A, Value B, Value C, Value D) {
    // E = A B = B B + A-B 0 +    0    0
    //     C D   D D   A-B 0   B+C-A-D 0
    add_tweights(x, D, B);
    add_tweights(y, 0, A - B);
    add_edge(x, y, 0, B + C - A - D);
}

/// Forbid (x,y)=(0,1) by putting infinite value to the arc from x to y.
inline void Energy::forbid01(Var x, Var y) {
    add_edge_infty(x, y);
}

/// After construction of the energy function, call this to minimize it.
/// Return the minimum of the function
inline Energy::TotalValue Energy::minimize() {
    return Econst + maxflow();
}

/// After 'minimize' has been called, determine the value of variable 'x'
/// in the optimal solution. Can be 0 or 1.
inline int Energy::get_var(Var x) const {
    return (int)what_segment(x, SINK);
}

#endif
