var t = 1297110663,
	v = 70,
	data = d3.range(33).map(next);
	
function next() {
	return {
		time: ++t,
		value: v = ~~Math.max(10, Math.min(90, v + 10 * (Math.random() - .5)))
	};
}

setInterval(function() {
	data.shift();
	data.push(next());
	redraw();
}, 1500);

var w = 30,
	h = 800;
	
var x = d3.scale.linear()
	.domain([0, 1])
	.range([0, w]);

var y = d3.scale.linear()
	.domain([0, 100])
	.rangeRound([0, h]);

var chart = d3.select("body")
	.append("svg:svg")
		.attr("class", "chart")
		.attr("width", w * data.length - 1)
		.attr("height", h);

chart.selectAll("rect")
		.data(data)
	.enter().append("svg:rect")
		.attr("x", function(d, i) { return x(i) - .5; })
		.attr("y", function(d) { return h - y(d.value) - .5; })
		.attr("width", w)
		.attr("height", function(d) { return y(d.value); });

chart.append("svg:line")
	.attr("x1", 0)
	.attr("x2", w * data.length)
	.attr("y1", h - .5)
	.attr("y2", h - .5)
	.attr("stroke", "#000");

/* USED FOR WHEN YOU HAVE CHANGING DATA AND EACH BAR 
REPRESENTS A DIFFERENT THING */
function redraw() { 
	chart.selectAll("rect")
			.data(data)
		.transition()
			.duration(1000)
			.attr("y", function(d) { return h - y(d.value) - .5; })
			.attr("height", function(d) { return y(d.value); });
}

/* Creates a QUEUE of bars */
// function redraw() { 
// 	var rect = chart.selectAll("rect")
// 		.data(data, function(d) { return d.time; });
// 	
// 	//enter
// 	rect.enter().insert("svg:rect", "line")
// 			.attr("x", function(d, i) { return x(i + 1) - .5; })
// 			.attr("y", function(d) { return h - y(d.value) - .5; })
// 			.attr("width", w)
// 			.attr("height", function(d) { return y(d.value); })
// 		.transition()
// 			.duration(1000)
// 			.attr("x", function(d, i) { return x(i) - .5; });
// 	//update
// 	rect.transition()
// 			.duration(1000)
// 			.attr("x", function(d, i) { return x(i) - .5; });
// 	
// 	//exit
// 	rect.exit().transition()
// 		.duration(1000)
// 		.attr("x", function(d, i) { return x(i - 1) - .5; })
// 		.remove();
// 		
// }



