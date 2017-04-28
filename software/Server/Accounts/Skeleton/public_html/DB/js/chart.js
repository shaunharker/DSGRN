// chart.js
// adapted from http://bl.ocks.org/ZJONSSON/3918369
function CreateTimeseriesChart ( div, content ) {
  var margin = {top: 20, right: 80, bottom: 30, left: 50},
  width = 600 - margin.left - margin.right,
  height = 300 - margin.top - margin.bottom;

  var x = d3.time.scale().range([0, width]);
  var y = d3.scale.linear().range([height, 0]);
  var color = d3.scale.category20();
  var xAxis = d3.svg.axis().scale(x).orient("bottom");
  var yAxis = d3.svg.axis().scale(y).orient("left");
  var line = d3.svg.line()
    .interpolate("basis")
    .x(function(d) { return x(d.time); })
    .y(function(d) { return y(d.value); });

  var svg = d3.select(div).append("svg")
    .attr("width", width + margin.left + margin.right)
    .attr("height", height + margin.top + margin.bottom)
    .append("g")
    .attr("transform", "translate(" + margin.left + "," + margin.top + ")");

  data = JSON.parse(content);

  // Example: data = {"variables" : ["var1", "var2"], 
  //                  "times" : [ 0.0, 0.01, 0.02, ... ], 
  //                  "timeseries" : [ [1.0, 1.0], [1.1, 1.4], ...] }

  // Create the ordinal scale corresponding to various plots
  color.domain(d3.keys(data["variables"]));

  // Juggle the data into a form we can plot
  // zip function, as in python
  function zip(arrays) {
    return arrays[0].map(function(_,i){
      return arrays.map(function(array){return array[i]})
    });
  }
  zipped_data = zip([ data["times"], data["timeseries"] ]);
  //  Example: zipped_data = [ [time1, [val1, val2, val2, ... ]] , [time2, [ ... ]], ... ]
  var components = data["variables"].map( function(name, index) {
    return {
      name: name,
      values: zipped_data.map(function(d) {
        return {time: d[0], value: +d[1][index]};
      })
    };
  });

  x.domain(d3.extent(data["times"], function(d) { return d; }));

  y.domain([
    d3.min(components, function(c) { return d3.min(c.values, function(v) { return v.value; }); }),
    d3.max(components, function(c) { return d3.max(c.values, function(v) { return v.value; }); })
    ]);

  svg.append("g")
  .attr("class", "x axis")
  .attr("transform", "translate(0," + height + ")")
  .call(xAxis);

  svg.append("g")
  .attr("class", "y axis")
  .call(yAxis)
  .append("text")
  .attr("transform", "rotate(-90)")
  .attr("y", 6)
  .attr("dy", ".71em")
  .style("text-anchor", "end")
  .text("Expression Level");

  var component = svg.selectAll(".component")
  .data(components)
  .enter().append("g")
  .attr("class", "component");

  component.append("path")
  .attr("class", "line")
  .attr("d", function(d) { return line(d.values); })
  .attr("data-legend",function(d) { return d.name})
  .style("stroke", function(d) { return color(d.name); });

  component.append("text")
  .datum(function(d) { return {name: d.name, value: d.values[d.values.length - 1]}; })
  .attr("transform", function(d) { return "translate(" + x(d.value.time) + "," + y(d.value.value) + ")"; })
  .attr("x", 3)
  .attr("dy", ".35em")
  .text(function(d) { return d.name; });

  legend = svg.append("g")
  .attr("class","legend")
  .attr("transform","translate(50,30)")
  .style("font-size","12px")
  .call(d3.legend)

  setTimeout(function() { 
    legend
    .style("font-size","20px")
    .attr("data-style-padding",10)
    .call(d3.legend)
  });
}
