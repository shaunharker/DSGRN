# Graphics.py
# Shaun Harker
# 2018-03-27
# MIT LICENSE

import IPython.display

def makehtml(item):
    if type(item) == type([]) or type(item) == type(()):
        return '<div>[' + ','.join([makehtml(i) for i in item]) + ']</div>'
    try:
        itemhtml = item._repr_html_()
    except AttributeError:
        try:
            itemhtml = item._repr_svg_()
        except AttributeError:
            itemhtml = str(item)
    return itemhtml  

class View:
    def __init__(self, obj):
        self.html = makehtml(obj)
    def _repr_html_(self):
        return self.html

class Table:
    def __init__(self, headers, rows, etc = None):
        self.headers = headers
        self.rows = rows
        self.etc = etc
    def tablestring(self, rows):
        html = '<div style="border:1px black">'
        html += '<table>'
        html += "<tr>"
        for item in self.headers:
            html += '<th>' 
            html += '<div style="text-align:center;">'
            html += makehtml(item) + '</div></th>'
        html += "</tr>"
        for row in rows:
            html += "<tr>"
            for item in row:
                html += '<td>'
                html += '<div style="text-align:center;">'
                html += makehtml(item) + '</div></td>'
            html += "</tr>"
        html += '</div></table></div>'
        return html
    def _repr_html_(self):
        return self.tablestring(self.rows)
    def view(self, rowindices = None):
        if not rowindices:
            rowindices = range(0,len(self.rows))
        if type(rowindices) == type(0): # single row table
            rowindices = [rowindices]
        return View(self.tablestring((self.rows[i] for i in rowindices)))
