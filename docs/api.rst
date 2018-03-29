
API
===

Network Interface
-----------------

The C++ interface for handling networks is as follows:

.. code-block:: cpp

  /// Network
  ///   This class holds network data.
  ///     * Loads specification files
  ///     * Outputs Graphviz visualizations
  ///     * Provides an interface to networks
  class Network {
  public:
    /// load
    ///   load from network specification file
    void 
    load ( const char* filename );

    /// size
    ///   Return the number of nodes in the network
    int
    size ( void ) const;

    /// index
    ///   Return index of node given name string
    int 
    index ( std::string const& name ) const;

    /// name
    ///   Return name of node (given by index)
    std::string const&
    name ( int index ) const;

    /// inputs
    ///   Return a list of inputs to a node
    std::vector<int> const&
    inputs ( int index ) const;

    /// outputs
    ///   Return a list of outputs to a node
    std::vector<int> const&
    outputs ( int index ) const;

    /// logic
    ///   Return the logic of a node (given by index)
    std::vector<std::vector<int>> const&
    logic ( int index ) const;

    /// interaction
    ///   Return the interaction type of an edge
    ///   False for repression, true for activation
    bool
    interaction ( int source, int target ) const;

    /// graphviz
    ///   Return a graphviz string (dot language)
    std::string
    graphviz ( std::vector<std::string> const& theme = 
    { "aliceblue", // background color
      "beige",     // node color
      "black", "darkgoldenrod", "blue", "orange", "red", "yellow" // edge group colors
    } ) const;


This is more-or-less self-documenting. The methods ``name`` and ``index`` provide inverse look-up tables that give either the name of a node or its index given the other piece of information. Edges may be queried via ``interaction`` to see if they are activating or repressing; a query on a non-existent edge results in undefined behavior (likely a segmentation fault). The logic of a network node can be obtained through the method ``logic`` and the return type is 

.. code-block:: cpp

  std::vector<std::vector<int>> const&


The inner ``std::vector<int>`` represents a logic factor as an array of node indices, and the outer ``std::vector`` stores a logic as an array of logic factors.

The ``graphviz`` method returns a dot language representation of the network (see <<NetworkSpecFile>> for an example). The color theme is an array of color names starting with the background color, followed by node color, and then input factor colors (i.e. input edges are colored according to which factor they belong to in the target node's logic). If there are more factors for some node than colors specified, then it rotates back to the first (and some groups become indistinguishable). This behavior can be used to disable the coloring of edges according to group by only giving three colors in the theme, which would then be background color, node color, and edge color.
