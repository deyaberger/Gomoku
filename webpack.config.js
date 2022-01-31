const path = require('path');
const HtmlWebpackPlugin = require('html-webpack-plugin');
const CopyWebpackPlugin = require('copy-webpack-plugin');
const {CleanWebpackPlugin} = require('clean-webpack-plugin');

module.exports = {
  entry: {
	  main : path.join(__dirname, "src/index.js"),
	  form : path.join(__dirname, "src/form/form.js"),
	  game : path.join(__dirname, "src/game/game.js"),
  },
  output: {
    path: path.resolve(__dirname, 'dist'),
    filename: '[name].bundle.js'
  },
  module: {
    rules: [
		{
		  test: /\.js$/,
		  exclude: /node_modules/,
		  use: {
			loader: "babel-loader"
		  }
		},
		{
			test: /\.(css|scss)$/,
			use: ["style-loader", "css-loader", "sass-loader"]
		  },
		  {
			test: /\.svg$/,
			use: [
			  {
				loader: 'svg-url-loader',
				options: {
				  limit: 10000,
				},
			  },
			],
		  },
		  {
			test: /\.(png|svg|jpg|jpeg|gif)$/i,
			type: 'asset/resource',
		  },
		  {
			test: /\.(woff(2)?|ttf|eot)(\?v=\d+\.\d+\.\d+)?$/,
			use: [
			  {
				loader: 'file-loader',
				options: {
				  name: '[name].[ext]',
				  outputPath: 'img/'
				}
			  }
			]
		  }
	  ]
  },
  plugins: [
	  new CleanWebpackPlugin(),
	  new CopyWebpackPlugin({
		patterns: [
			{
				from: path.join(__dirname, "./src/img/"),
				to: "img",
			}
		]
	  }),
    new HtmlWebpackPlugin({
		filename : "index.html",
		template: path.join(__dirname, "./src/index.html"),
		chunks: ["main"]
	}),
	new HtmlWebpackPlugin({
		filename : "computer.html",
		template: path.join(__dirname, "./src/form/computer.html"),
		chunks: ["form"]
	}),
	new HtmlWebpackPlugin({
		filename : "humans.html",
		template: path.join(__dirname, "./src/form/humans.html"),
		chunks: ["form"]
	}),
	new HtmlWebpackPlugin({
		filename : "game.html",
		template: path.join(__dirname, "./src/game/game.html"),
		chunks: ["game", "form"]
	}),
  ],
   devtool: "source-map",
  mode: "development",
  devServer: {
    static: path.resolve(__dirname, './dist'),
    open: true,
    port: 4000
  }
};