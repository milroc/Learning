var less = require('less');

var parser = new(less.Parser)({
                    paths: ['.', './lib'], // Specify search paths for @import directives
                        filename: 'style.less' // Specify a filename, for better error messages
                        });

parser.parse('.class { width: 1 + 1 }', function (e, tree) {
                    tree.toCSS({ compress: true }); // Minify CSS output
                    });
