var data = [4, 8, 15, 16, 23, 42];

/*******HTML *******/
/*
var chart = d3.select("body")
	.append("div")
		.attr("class", "chart");

var x = d3.scale.linear()
	.domain([0, d3.max(data)])
	.range(["0px", chart.style("width")]);

chart.selectAll("div")
		.data(data)
	.enter().append("div")
		.attr("class", "bar")
		.style("width", x)
		.text(String);
*/

/******SVG ********/
var chart = d3.select(".content")
	.append("svg:svg")
		.attr("class", "chart")
		.attr("width", 440)
		.attr("height", 140)
	.append("svg:g")
		.attr("transform", "translate(10,15)");

var x = d3.scale.linear()
	.domain([0, d3.max(data)])
	.range([0, 420]);
	
var y = d3.scale.ordinal()
	.domain(data)
	.rangeBands([0, 120]);

chart.selectAll("rect")
		.data(data)
	.enter().append("svg:rect")
		.attr("y", y)
		.attr("width", x)
		.attr("height", y.rangeBand());

chart.selectAll("text")
		.data(data)
	.enter().append("svg:text")
		.attr("x", x)
		.attr("y", function(d) { return y(d) + y.rangeBand() / 2; })
		.attr("dx", -3) 
		.attr("dy", ".35em") 
		.attr("text-anchor", "end") 
		.text(String);

//creating labels
chart.selectAll("line")
		.data(x.ticks(10))
	.enter().append("svg:line")
		.attr("x1", x)
		.attr("x2", x)
		.attr("y1", 0)
		.attr("y2", 120)
		.attr("stroke", "#ccc");

chart.append("svg:line")
	.attr("y1", 0)
	.attr("y2", 120)
	.attr("stroke", "#000");

chart.selectAll("text.rule")
		.data(x.ticks(10))
	.enter().append("svg:text")
		.attr("class", "rule")
		.attr("x", x)
		.attr("y", 0)
		.attr("dy", -3)
		.attr("text-anchor", "middle")
		.text(String);