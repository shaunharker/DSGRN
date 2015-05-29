
//////////////////////
// GLOBAL VARIABLES //
//////////////////////

var gc_width = document.getElementById("leftpanel").clientWidth;
var gc_height = document.getElementById("leftpanel").clientHeight;
var gc_colors = d3.scale.category10();


var gc_svg = d3.select('#grapheditor')
  .append('svg')
  .attr("viewBox", "0 0 " + gc_width + " " + gc_height)
  .attr('width', "100%")
  .attr('height', "100%");

var ni_width = document.getElementById("logiceditor").clientWidth - 22;
var ni_height = document.getElementById("logiceditor").clientHeight - 25;

var ni_svg = d3.select("#logiceditor")
  .append("svg")
  .attr("viewBox", "0 0 " + ni_width + " " + ni_height)
  .attr("width", "100%")
  .attr("height", "100%")
  .append("g")
  .attr("transform", "translate(" + 40 + "," + 0 + ")");


// set up initial nodes and links
//  - nodes are known by 'id', not by index in array.
//    type of link: true for arrow head, false for blunt heads.
var gc_nodes = [
    {id: 0, name:"X0"},
    {id: 1, name:"X1"},
    {id: 2, name:"X2"}
  ];

var lastNodeId = 2;

var gc_links = [
    {source: gc_nodes[0], target: gc_nodes[1], type: true },
    {source: gc_nodes[1], target: gc_nodes[2], type: true }
  ];

var inputlogic = {
  "0" : [],
  "1" : [[0]],
  "2" : [[1]] 
};

function removeLogicLink ( i, j ) {
  inputlogic[j] = inputlogic[j].map(function(d){return d.filter(function(e){return e !== i;});});
  inputlogic[j] = inputlogic[j].filter(function(d){return d.length > 0;});
}

function insertLogicLink ( i, j, k ) {
  if ( i === j ) { 
    inputlogic[k].push([i]);
  } else {
  inputlogic[k] = inputlogic[k].map(function(d){ 
    if(d.some(function(e){return e === j;})) d.push(i); return d;});
  }
}

function addLink ( x, y ) {
  if (gc_links.some(function(l){return (l.source.id === x.id && l.target.id === y.id);})) return;
  gc_links.push({source: x, target: y, type:true });
  inputlogic[y.id].push([x.id]);
}

function removeLink ( x, y ) {
  var i = x.id, j = y.id;
  gc_links = gc_links.filter(function(d){return (d.source.id !== i || d.target.id !== j); });
  removeLogicLink(i,j);
}

function addNode ( point ) {
  var node = {id: ++lastNodeId, name: ('X' + lastNodeId)};
  node.x = point[0];
  node.y = point[1];
  gc_nodes.push(node);
  inputlogic[lastNodeId] = []; 
}

function removeNode ( x ) {
  // Remove all links touching x
  var i = x.id;
  var remove = gc_links.filter(function(d){return (d.source.id === i || d.target.id === i); });
  remove.forEach(function(d){ removeLink(d.source, d.target);});
  delete inputlogic[i];
  gc_links = gc_links.filter(function(d){return (d.source.id !== i && d.target.id !== i); });
  gc_nodes = gc_nodes.filter(function(d){return (d.id !== i);});
}

var gc_selected_node = null,
    gc_selected_link = null, 
    ni_selected_node = null;

function updatePanels () {
  updateNodeInspector ();
  updateGraphBuilder ();
  updateNetworkSpecification ();
}

function coordinateTransform(screenPoint, my_svg)
{
  var CTM = my_svg.getScreenCTM();
  var p = my_svg.createSVGPoint();
  p.x = screenPoint[0];
  p.y = screenPoint[1];
  //return p.matrixTransform( CTM );
  //return p.matrixTransform( CTM.inverse() );
  return p;
}

//////////////////////////
// NODE INSPECTOR PANEL //
//////////////////////////

var treecounter = 0;


function updateNodeInspector () {
  var tree = d3.layout.tree()
      .size([ni_height, ni_width - 160]);

  var diagonal = d3.svg.diagonal()
      .projection(function(d) { return [d.y, d.x]; });

  var json = {};
  if ( !gc_selected_node ) {
    if ( gc_selected_link ) {
      gc_selected_node = gc_selected_link.target;
    } else {
      gc_selected_node = gc_nodes[0];
    }
  }

  json["name"] = gc_selected_node.name;//id.toString();
  var logic = inputlogic[ gc_selected_node.id ];
  json["children"] = logic . map ( function(d, i) {  
    var group = {"name":"+"};
    group["children"] = d . map ( function ( e, j ) { 
      return {"name":e.toString()};
    });
    return group;
  });
  
  // Create tree layout
  var ni_nodes = tree.nodes(json).reverse(),
      ni_links = tree.links(ni_nodes);

  // Update nodes
  var nodedata = ni_svg
      .selectAll("g.treenode")
      .data(ni_nodes, function(d) { return d.id || (d.id = ++ treecounter); });
  
  nodedata.exit().remove();

  var nodedata_entering = nodedata
      .enter()
      .append("g")
      .attr("class", "treenode")
      .attr("transform", function(d) { return "translate(" + d.y + "," + d.x + ")"; });

  nodedata_entering
      .append("circle")
      .attr("r", 5.0)
      .on("mousedown", function(d) {
        // Ignore this click
        if (d.name.lastIndexOf("+", 0) === 0) return;

        // If already selected, unselect.
        if ( ni_selected_node === d ) {
          ni_selected_node = null;
          d3.select(this).style('fill', '#fff');
          return;
        } 
        // Select
        if ( ni_selected_node === null ) {
          if ( d["name"] === gc_selected_node.name ) return;
          ni_selected_node = d;
          d3.select(this).style('fill', 'steelblue');
          return;
        }
        // Combine groups
        var i = +ni_selected_node["name"];
        var k = gc_selected_node.id;
        if ( d["name"] === gc_selected_node.name ) {
          removeLogicLink(i,k);
          insertLogicLink(i,i,k);
        } else {
          var j = +d["name"];
          removeLogicLink(i,k);
          insertLogicLink(i,j,k);
        }
        ni_selected_node = null;
        updatePanels ();
      })
      .on('mouseover', function(d) {
        d3.select(this).attr('transform', 'scale(2.0)');
      })
      .on('mouseout', function(d) {
        d3.select(this).attr('transform', '');
      });

  nodedata_entering
      .append("text")
      .attr("dx", function(d) { 
        if (d.name.lastIndexOf("+", 0) === 0) return 0;
        return d.children ? -8 : 8; })
      .attr("dy", 3)
      .attr("text-anchor", function(d) { 
        if (d.name.lastIndexOf("+", 0) === 0) return "middle";
        return d.children ? "end" : "start"; })
      .text(function(d) { 
        if ( d.name.lastIndexOf("+", 0) === 0 ) return "+";
        if ( d.name === gc_selected_node.name ) return d.name;
        return gc_nodes.filter(function(e){return e.id.toString() === d.name; })[0].name;
      });

  // Update links
  var linkdata = ni_svg
      .selectAll("path.treelink")
      .data(ni_links, function(d) { return d.source.id + ',' + d.target.id; });

  linkdata.enter()
      .insert("path","g")
      .attr("class", "treelink")
      .attr("d", diagonal);

  linkdata.exit()
      .remove();

  ni_svg.
    on("mouseup", function(d){

    });
};


/////////////////////////////
// GRAPH CONSTRUCTOR PANEL //
/////////////////////////////

// init D3 force layout
var gc_force = d3.layout.force()
    .nodes(gc_nodes)
    .links(gc_links)
    .size([gc_width, gc_height])
    .linkDistance(150)
    .charge(-500)
    .on('tick', gc_tick)

// define arrow markers for graph links
gc_svg
    .append('gc_svg:defs').append('gc_svg:marker')
    .attr('id', 'arrow')
    .attr('viewBox', '0 -5 10 10')
    .attr('refX', 6)
    .attr('markerWidth', 3)
    .attr('markerHeight', 3)
    .attr('orient', 'auto')
    .append('gc_svg:path')
    .attr('d', 'M0,-5L10,0L0,5')
    .attr('fill', '#000');

gc_svg
    .append('gc_svg:defs').append('gc_svg:marker')
    .attr('id', 'blunt')
    .attr('viewBox', '0 -5 10 10')
    .attr('refX', 4)
    .attr('markerWidth', 3)
    .attr('markerHeight', 3)
    .attr('orient', 'auto')
    .append('gc_svg:path')
    .attr('stroke', 'black')
    .attr('stroke-width', '4')
    .attr('d', 'M5,-5L5,5')
    .attr('fill', '#000');

// line displayed when dragging new nodes
var gc_drag_line = gc_svg.append('gc_svg:path')
  .attr('class', 'link dragline hidden')
  .attr('d', 'M0,0L0,0');

// handles to link and node element groups
var gc_path = gc_svg.append('gc_svg:g').selectAll('g');
var gc_circle = gc_svg.append('gc_svg:g').selectAll('g');

// mouse event vars
var gc_mousedown_link = null,
    gc_mousedown_node = null,
    gc_mouseup_node = null;

function resetMouseVars() {
  gc_mousedown_node = null;
  gc_mouseup_node = null;
  gc_mousedown_link = null;
}

// update force layout (called automatically each iteration)
function gc_tick() {
  // draw directed edges with proper padding from node centers
  function drawpath ( d ) {
    if ( d.target === d.source ) {
      var X = d.source.x,
          Y = d.source.y;
      return 'M' + (X+20) + ',' + (Y) 
           + 'C' + (X+50) + ',' + (Y) + ',' 
                 + (X+50) + ',' + (Y-50) + ','
                 + (X) + ',' + (Y-50)
           + 'C' + (X-50) + ',' + (Y-50) + ','
                 + (X-50) + ',' + (Y) + ','
                 + (X-20) + ',' + (Y);
    }
    var deltaX = d.target.x - d.source.x,
        deltaY = d.target.y - d.source.y,
        dist = Math.sqrt(deltaX * deltaX + deltaY * deltaY),
        normX = deltaX / dist,
        normY = deltaY / dist,
        sourcePadding = 12;//12;
        targetPadding = 17;//17;
        sourceX = d.source.x + (sourcePadding * normX) - normY*4 ,
        sourceY = d.source.y + (sourcePadding * normY) + normX*4,
        targetX = d.target.x - (targetPadding * normX) - normY*4,
        targetY = d.target.y - (targetPadding * normY) + normX*4,
        midX = (sourceX + targetX) / 2.0 - normY*24,
        midY = (sourceY + targetY) / 2.0 + normX*24;
    return 'M' + sourceX + ',' + sourceY + 'Q' + midX + ',' + midY + ',' + targetX + ',' + targetY;
  }

  gc_path.selectAll('path').attr('d', drawpath);
  gc_circle.attr('transform', function(d) {
    return 'translate(' + d.x + ',' + d.y + ')';
  });
}

// update graph (called when needed)
function updateGraphBuilder() {
  // Handle link data
  gc_path = gc_path.data(gc_links, function(d){return d.source.id + ',' + d.target.id;});

  // Exiting links
  gc_path.exit().remove();

  // Entering links
  var entering = gc_path.enter().append('gc_svg:g');
  entering.classed('selected', function(d) { return d === gc_selected_link; });
  entering.append('gc_svg:path')
    .attr('class', 'link')
    .style('marker-end', function(d) { return d.type ? 'url(#arrow)' : 'url(#blunt)' ; });

  entering.append('gc_svg:path')
    .attr('class', 'linkselectfield');

  entering.on('mousedown', function(d) {
      if(d3.event.ctrlKey) return;
      // select link
      gc_mousedown_link = d;
      if(gc_mousedown_link === gc_selected_link) { 
        gc_selected_link = null;
        d.type = ! d.type;
      }
      else gc_selected_link = gc_mousedown_link;
      gc_selected_node = null;
      updatePanels();
    });


  // Updating links  
  gc_path.classed('selected', function(d) { return d === gc_selected_link; })
      .selectAll('path.link')
      .style('stroke-dasharray', function(d) { return (d === gc_selected_link) ? '10,2' : null; })
      .style('marker-end', function(d) { return d.type ? 'url(#arrow)' : 'url(#blunt)' ; });

  // Node data
  gc_circle = gc_circle.data(gc_nodes, function(d) { return d.id; });

  // Entering nodes
  var g = gc_circle.enter().append('gc_svg:g');

  g.append('gc_svg:circle')
    .attr('class', 'node')
    .attr('r', 12)
    .style('fill', function(d) { return (d === gc_selected_node) ? d3.rgb(gc_colors(d.id)).brighter().toString() : gc_colors(d.id); })
    .style('stroke', function(d) { return d3.rgb(gc_colors(d.id)).darker().toString(); })
    .on('mouseover', function(d) {
      if(!gc_mousedown_node || d === gc_mousedown_node) return;
      // enlarge target node
      d3.select(this).attr('transform', 'scale(1.1)');
    })
    .on('mouseout', function(d) {
      if(!gc_mousedown_node || d === gc_mousedown_node) return;
      // unenlarge target node
      d3.select(this).attr('transform', '');
    })
    .on('mousedown', function(d) {
      if(d3.event.ctrlKey) return;

      // select node
      gc_mousedown_node = d;
      if(gc_mousedown_node === gc_selected_node) gc_selected_node = null;
      else gc_selected_node = gc_mousedown_node;

      ni_selected_node = null;
      gc_selected_link = null;
      gc_drag_line
        .style('marker-end', 'url(#arrow)')
        .classed('hidden', false)
        .attr('d', 'M' + gc_mousedown_node.x + ',' + gc_mousedown_node.y + 'L' + gc_mousedown_node.x + ',' + gc_mousedown_node.y);

      updatePanels();
    })
    .on('mouseup', function(d) {
      if(!gc_mousedown_node) return;

      // needed by FF
      gc_drag_line
        .classed('hidden', true)
        .style('marker-end', '');

      // check for drag-to-self
      gc_mouseup_node = d;
      if(gc_mouseup_node === gc_mousedown_node) { resetMouseVars(); return; }

      // unenlarge target node
      d3.select(this).attr('transform', '');

      // Add the new link
      addLink ( gc_mousedown_node, gc_mouseup_node );

      // Unselect any selections
      gc_selected_link = null;
      gc_selected_node = null;
      ni_selected_node = null;
      updatePanels();
    });

  // show node IDs
  g.append('gc_svg:text')
      .attr('x', 0)
      .attr('y', 4)
      .attr('class', 'id')
      .text(function(d) { return d.name; });

  // Updating nodes
  gc_circle.selectAll('circle')
    .style('fill', function(d) { return (d === gc_selected_node) ? d3.rgb(gc_colors(d.id)).brighter().toString() : gc_colors(d.id); });

  // Exiting nodes
  gc_circle.exit().remove();

  // Force layout
  gc_force.start();
}

function mousedown() {
  gc_svg.classed('active', true);
  if(d3.event.ctrlKey || gc_mousedown_node || gc_mousedown_link) return;
  var point = d3.mouse(this);
  addNode(point);
  updatePanels();
}

function mousemove() {
  if(! gc_mousedown_node) return;
  point = d3.mouse(this);
  correctedpoint = coordinateTransform ( point, this );
  gc_drag_line.attr('d', 'M' + gc_mousedown_node.x + ',' + gc_mousedown_node.y + 'L' + correctedpoint.x + ',' + correctedpoint.y);
  updatePanels();
}

function mouseup() {
  if( gc_mousedown_node) {
    gc_drag_line
      .classed('hidden', true)
      .style('marker-end', '');
  }
  gc_svg.classed('active', false);
  resetMouseVars();
}


// only respond once per keydown
var lastKeyDown = -1;

function keydown() {
  d3.event.preventDefault();

  if(lastKeyDown !== -1) return;
  lastKeyDown = d3.event.keyCode;

  // ctrl
  if(d3.event.keyCode === 17) {
    gc_circle.call(gc_force.drag);
    gc_svg.classed('ctrl', true);
  }

  if(!gc_selected_node && !gc_selected_link) return;
  switch(d3.event.keyCode) {
    case 8: // backspace
    case 46: // delete
      if(gc_selected_link) {
        removeLink ( gc_selected_link.source, gc_selected_link.target );
      } else if(gc_selected_node) {
        removeNode ( gc_selected_node );
      }
      gc_selected_link = null;
      gc_selected_node = null;
      ni_selected_node = null;
      updatePanels();
      break;
    case 83: // s
      if(gc_selected_node) {
        addLink ( gc_selected_node, gc_selected_node );
        updatePanels();
      }
      break;
  }
}

function keyup() {
  lastKeyDown = -1;

  // ctrl
  if(d3.event.keyCode === 17) {
    gc_circle
      .on('mousedown.drag', null)
      .on('touchstart.drag', null);
    gc_svg.classed('ctrl', false);
  }
}



/////////////////////////////////
// NETWORK SPECIFICATION PANEL //
/////////////////////////////////

var ComponentSize = {
"1 1 1" : 3,
"1 2 1" : 6,
"1 3 1" : 10,
"1 4 1" : 15,
"1 5 1" : 21,
"2 1 2" : 6,
"2 1 1 1" : 6,
"2 2 2" : 20,
"2 2 1 1" : 20,
"2 3 2" : 50,
"2 3 1 1" : 50,
"2 4 2" : 105,
"2 4 1 1" : 105,
"2 5 2" : 196,
"2 5 1 1" : 196,
"3 1 3" : 20,
"3 1 1 1 1" : 20,
"3 1 1 2" : 20,
"3 2 3" : 150,
"3 2 1 1 1" : 150,
"3 2 1 2" : 155,
"3 3 3" : 707,
"3 3 1 1 1" : 707,
"3 3 1 2" : 756,
"3 4 3" : 2518,
"3 4 1 1 1" : 2518,
"3 4 1 2" : 2778,
"3 5 3" : 7416,
"3 5 1 1 1" : 7416,
"3 5 1 2" : 8412,
"4 1 4" : 150,
"4 1 1 1 1 1" : 150,
"4 2 4" : 3287,
"4 2 1 1 1 1" : 3287,
"4 3 4" : 35368,
"4 3 1 1 1 1" : 35368,
"4 4 4" : 253146,
"4 4 1 1 1 1" : 253146,
"5 1 5" : 3287,
"5 1 1 1 1 1 1" : 3287
};

function numberEdgesIn ( i ) {
 return gc_links . filter ( function(d){return d.target.id === i;}).length;
}

function numberEdgesOut ( i ) {
 return gc_links . filter ( function(d){return d.source.id === i;}).length;
}

function numberWithCommas(x) {
    return x.toString().replace(/\B(?=(\d{3})+(?!\d))/g, ",");
}

var factorialmem = [];
function factorial (n) {
  if (n == 0 || n == 1)
    return 1;
  if (factorialmem[n] > 0)
    return factorialmem[n];
  return factorialmem[n] = factorial(n-1) * n;
}

function updateNetworkSpecification () {
  var spec = '';
  var parametergraphsize = 1;
  var reorderings = 1;
  gc_nodes.forEach ( function(d){
    var target = d.id;
    var logic = inputlogic[d.id];
    var outer_parse = logic.reduce(function(prev, curr, i) {
      var inner_parse = curr.reduce(function(prev, curr, i ) {
        var sourcelink = gc_links . filter ( function(d){ 
          return d.source.id === curr && d.target.id === target;
        })[0];
        var sourcename = sourcelink.source.name;
        var source = sourcelink.type ? sourcename : '~' + sourcename;
        return i ? prev + '+' + source : source;
      }, '');
      return i ? prev + '(' + inner_parse + ')' : '(' + inner_parse + ')';
    }, '');
    spec += d.name + ' : ' + outer_parse + '<br>';
    var n = numberEdgesIn ( d.id );
    var m = numberEdgesOut (d.id );
    reorderings = reorderings * factorial ( m );
    var logicclass = logic . map ( function(d){return d.length;} ).sort().join(" ");
    var component = n + " " + m + " " + logicclass;
    if ( n > 0 && m > 0 )
    if ( component in ComponentSize ) {
      var componentsize = ComponentSize[component];
      parametergraphsize = parametergraphsize * componentsize;
    } else {
      parametergraphsize = 0;
    }
    if ( n === 0 ) parametergraphsize = parametergraphsize * (m+1);
  });
  var graphinfo = "Parameter Graph Size for fixed threshold ordering = " 
                  + numberWithCommas(parametergraphsize) + '<br>' 
                  + "Number of threshold reorderings = " + numberWithCommas(reorderings) 
                  + '<br>' + "Parameter Graph Size including all threshold orderings = " 
                  + numberWithCommas(parametergraphsize*reorderings) + '<br>';
  d3.select("#netspec").html( spec );
  d3.select("#graphinfo").html( graphinfo );
}


// app starts here
gc_svg.on('mousedown', mousedown)
  .on('mousemove', mousemove)
  .on('mouseup', mouseup);
d3.select(window)
  .on('keydown', keydown)
  .on('keyup', keyup);
updatePanels();



