
function insertAfter(newNode, referenceNode) {
    referenceNode.parentNode.insertBefore(newNode, referenceNode.nextSibling);
}

function phpQuery(url, args, callback) {
  var xmlhttp=new XMLHttpRequest();
  xmlhttp.onreadystatechange=function() {
      if (xmlhttp.readyState==4 && xmlhttp.status==200) {
          callback(xmlhttp.responseText);
      }
  }
  xmlhttp.open("GET",(args.length === 0) ? url : (url+"?"+args),true);
  xmlhttp.send();
}

function clearContent () {
  var filterbox = document.getElementById("filterbox");
  var parent = document.getElementById("paneldiv");
  var network;
  while (parent.firstChild) {
    if ( parent.firstChild.chosen ) {
      network = parent.firstChild;
    }
    parent.removeChild(parent.firstChild);
  }  
  parent.appendChild(network);
  parent.appendChild(filterbox);
  window.numItems = 0;
  window.loadingComplete = false;
}

function loadContent () {
  document.getElementById("loadingbar").innerHTML = "<p>Loading...</p>";

  var FP = document.querySelector('input[name="FP"]:checked').value;
  var FPON = document.querySelector('input[name="FPON"]:checked').value;
  var FPOFF = document.querySelector('input[name="FPOFF"]:checked').value;
  var FC = document.querySelector('input[name="FC"]:checked').value;
  var XC = document.querySelector('input[name="XC"]:checked').value;
  window . loadingContent = true;
  var url = "http://chomp.rutgers.edu/Projects/DSGRN/DB/php/ZooServer.php";
  var begin = window.numItems;
  var end = begin + 25;
  var args = "db=" + window.database + "&cmd=annotationcohort&start=" + begin + "&end=" + end 
    + "&fp=" + FP + "&fpon=" + FPON + "&fpoff=" + FPOFF + "&fc=" + FC + "&xc=" + XC ;
  window.numItems = end;
  phpQuery ( url, args, displayContent );
}

function displayContent ( content ) {
  var filterbox = document.getElementById("filterbox");
  filterbox.style.display = "";
  var data = JSON.parse(content);
  var parent = document.getElementById("paneldiv");
  data["data"].forEach ( function (s, i) { 
    var div = document.createElement('div');
    parent . appendChild ( div );
    var graphvizstring = 'digraph { bgcolor=lightsteelblue; node [style=filled fillcolor=white];' + s["Graphviz"] . slice (9); // "digraph {" has 9 characters
    div.innerHTML = '<p class="morsegraphinfo"> Morse Graph ' + s["MorseGraphIndex"] + '<br/>'
                    + 'Parameters: ' + s["Frequency"] + "</p>" 
                    + Viz(graphvizstring, "svg");
    div.children[1].setAttribute("width", "200");
    div.children[1].setAttribute("height", "200");
    div.className = 'digraph morsegraph';
    div.ZooData = s;
    div.onclick = function () {
      clicked = this; 
      phpQuery("http://chomp.rutgers.edu/Projects/DSGRN/DB/php/ZooServer.php", 
               "db=" + window.database + "&cmd=parameters&mgi=" + clicked.ZooData["MorseGraphIndex"], 
               function (content) {
        var data = JSON.parse(content);

        var result = document.createElement('div');
        var text = document.createElement('p');
        text.innerHTML = "Parameters for Morse Graph " + clicked.ZooData["MorseGraphIndex"];
        result.appendChild(text);
        data["data"].forEach( function (s, i) {
          var p = document.createElement('p');
          result.appendChild ( p );
          p . innerHTML =  s["ParameterIndex"];
          p . className = "parameter";
          p . onclick = function () {
            phpQuery("http://chomp.rutgers.edu/Projects/DSGRN/DB/php/ZooServer.php", 
                     "db=" + window.database + "&cmd=inequalities&pi=" + p.innerHTML, 
                     function (content) {
                      var div = document.createElement('div');
                      insertAfter( div, p.parentNode);
                      div.className = 'inequalities';
                      div.innerHTML = '<p> Parameter Inequalities (Parameter ' + s["ParameterIndex"] + ')</p> <pre>' + content + "</pre>";
                     });
            phpQuery("http://chomp.rutgers.edu/Projects/DSGRN/DB/php/ZooServer.php", 
                     "db=" + window.database + "&cmd=domaingraph&pi=" + p.innerHTML, 
                     function (content) {
                      var div = document.createElement('div');
                      insertAfter( div, p.parentNode);
                      div.className = 'domaingraph';
                      var graphvizstring = 'digraph { bgcolor=lightsteelblue; node [style=filled fillcolor=white];' + content . slice (9); // "digraph {" has 9 characters
                      if ( graphvizstring.length < 10000 ) {
                        div.innerHTML = '<p> Domain Graph (Parameter ' + s["ParameterIndex"] + ')</p>' + Viz(graphvizstring, "svg");
                        div.children[1].setAttribute("width", "500");
                        div.children[1].setAttribute("height", "500");
                      } else {
                        div.innerHTML = '<p> Domain Graph (' + s["ParameterIndex"] + ')</p> <p> (Too large to display) </p>';
                      }
                     });
            phpQuery("http://chomp.rutgers.edu/Projects/DSGRN/DB/php/ZooServer.php", 
                     "db=" + window.database + "&cmd=timeseries&pi=" + p.innerHTML,
                     function ( content ) {
                      var div = document.createElement('div');
                      insertAfter( div, p.parentNode);
                      div.className = 'timeseries';
                      CreateTimeseriesChart(div, content);
                     });   
          };
        });
        result.className = 'parameterlist';
        insertAfter ( result, clicked );
      });
    }
  });
  window.loadingContent = false;
  if ( data["data"] . length === 0 ) { 
    window.loadingComplete = true;
    document.getElementById("loadingbar").innerHTML = "<p>Loading complete.</p>";
  }
}

/// Code run at startup. It queries for networks and displays them in panels.
/// Clicking on a panel results in a Morse Graph Zoo being displayed.
phpQuery("http://chomp.rutgers.edu/Projects/DSGRN/DB/php/ZooServer.php", 
  "db=registry.db&cmd=networks", 
  function (content) {
    var data = JSON.parse(content);
    data["data"].forEach ( function ( s, i ) { 
      var div = document.createElement('div');
      var parent = document.getElementById("paneldiv");
      parent . appendChild ( div );
      var graphvizstring = s["Graphviz"];
      div.innerHTML = '<p>' + s["Name"] + '</p>' 
                    + '<p> Dimension ' + s["Dimension"] + '</p>' 
                    + Viz(graphvizstring, "svg");
      div.children[2].setAttribute("width", "200");
      div.children[2].setAttribute("height", "200");
      div.className = 'digraph network';
      div.filename = s["Filename"];
      div.chosen = false;
      div.onclick = function () {
        div.onclick = function () {};
        div.chosen = true;
        window.database = div.filename;
        window.numItems = 0;
        window.loadingComplete = false;
        clearContent ();
        loadContent ();
        window.onscroll = function(ev) {
            if ( window.loadingContent || window.loadingComplete ) return;
            if ((window.innerHeight + window.scrollY) >= .9*document.body.offsetHeight) {
                loadContent ();
            }
        };
      };
    });
  });

