/*
 * This file is part of Canviz. See http://www.canviz.org/
 * $Id: canviz.js 265 2009-05-19 13:35:13Z ryandesign.com $
 */

var CanvizTokenizer = Class.create({
	initialize: function(str) {
		this.str = str;
	},
	takeChars: function(num) {
		if (!num) {
			num = 1;
		}
		var tokens = new Array();
		while (num--) {
			var matches = this.str.match(/^(\S+)\s*/);
			if (matches) {
				this.str = this.str.substr(matches[0].length);
				tokens.push(matches[1]);
			} else {
				tokens.push(false);
			}
		}
		if (1 == tokens.length) {
			return tokens[0];
		} else {
			return tokens;
		}
	},
	takeNumber: function(num) {
		if (!num) {
			num = 1;
		}
		if (1 == num) {
			return Number(this.takeChars());
		} else {
			var tokens = this.takeChars(num);
			while (num--) {
				tokens[num] = Number(tokens[num]);
			}
			return tokens;
		}
	},
	takeString: function() {
		var byteCount = Number(this.takeChars()), charCount = 0, charCode;
		if ('-' != this.str.charAt(0)) {
			return false;
		}
		while (0 < byteCount) {
			++charCount;
			charCode = this.str.charCodeAt(charCount);
			if (0x80 > charCode) {
				--byteCount;
			} else if (0x800 > charCode) {
				byteCount -= 2;
			} else {
				byteCount -= 3;
			}
		}
		var str = this.str.substr(1, charCount);
		this.str = this.str.substr(1 + charCount).replace(/^\s+/, '');
		return str;
	}
});

var CanvizEntity = Class.create({
	initialize: function(defaultAttrHashName, name, canviz, rootGraph, parentGraph, immediateGraph) {
		this.defaultAttrHashName = defaultAttrHashName;
		this.name = name;
		this.canviz = canviz;
		this.rootGraph = rootGraph;
		this.parentGraph = parentGraph;
		this.immediateGraph = immediateGraph;
		this.attrs = $H();
		this.drawAttrs = $H();
	},
	initBB: function() {
		var matches = this.getAttr('pos').match(/([0-9.]+),([0-9.]+)/);
		var x = Math.round(matches[1]);
		var y = Math.round(this.canviz.height - matches[2]);
		this.bbRect = new Rect(x, y, x, y);
	},
	getAttr: function(attrName, escString) {
		if (Object.isUndefined(escString)) escString = false;
		var attrValue = this.attrs.get(attrName);
		if (Object.isUndefined(attrValue)) {
			var graph = this.parentGraph;
			while (!Object.isUndefined(graph)) {
				attrValue = graph[this.defaultAttrHashName].get(attrName);
				if (Object.isUndefined(attrValue)) {
					graph = graph.parentGraph;
				} else {
					break;
				}
			}
		}
		if (attrValue && escString) {
			attrValue = attrValue.replace(this.escStringMatchRe, function(match, p1) {
				switch (p1) {
					case 'N': // fall through
					case 'E': return this.name;
					case 'T': return this.tailNode;
					case 'H': return this.headNode;
					case 'G': return this.immediateGraph.name;
					case 'L': return this.getAttr('label', true);
				}
				return match;
			}.bind(this));
		}
		return attrValue;
	},
	draw: function(ctx, ctxScale, redrawCanvasOnly) {
		var i, tokens, fillColor, strokeColor;
		if (!redrawCanvasOnly) {
			this.initBB();
			var bbDiv = new Element('div');
			this.canviz.elements.appendChild(bbDiv);
		}
		
		this.drawAttrs.each(function(drawAttr) {
			var command = drawAttr.value;
//			debug(command);
			var tokenizer = new CanvizTokenizer(command);
			var token = tokenizer.takeChars();
			if (token) {
				var dashStyle = 'solid';
				ctx.save();
				while (token) {
//					debug('processing token ' + token);
					switch (token) {
						case 'E': // filled ellipse
						case 'e': // unfilled ellipse
							var filled = ('E' == token);
							var cx = tokenizer.takeNumber();
							var cy = this.canviz.height - tokenizer.takeNumber();
							var rx = tokenizer.takeNumber();
							var ry = tokenizer.takeNumber();
							var path = new Ellipse(cx, cy, rx, ry);
							break;
						case 'P': // filled polygon
						case 'p': // unfilled polygon
						case 'L': // polyline
							var filled = ('P' == token);
							var closed = ('L' != token);
							var numPoints = tokenizer.takeNumber();
							tokens = tokenizer.takeNumber(2 * numPoints); // points
							var path = new Path();
							for (i = 2; i < 2 * numPoints; i += 2) {
								path.addBezier([
									new Point(tokens[i - 2], this.canviz.height - tokens[i - 1]),
									new Point(tokens[i],     this.canviz.height - tokens[i + 1])
								]);
							}
							if (closed) {
								path.addBezier([
									new Point(tokens[2 * numPoints - 2], this.canviz.height - tokens[2 * numPoints - 1]),
									new Point(tokens[0],                  this.canviz.height - tokens[1])
								]);
							}
							break;
						case 'B': // unfilled b-spline
						case 'b': // filled b-spline
							var filled = ('b' == token);
							var numPoints = tokenizer.takeNumber();
							tokens = tokenizer.takeNumber(2 * numPoints); // points
							var path = new Path();
							for (i = 2; i < 2 * numPoints; i += 6) {
								path.addBezier([
									new Point(tokens[i - 2], this.canviz.height - tokens[i - 1]),
									new Point(tokens[i],     this.canviz.height - tokens[i + 1]),
									new Point(tokens[i + 2], this.canviz.height - tokens[i + 3]),
									new Point(tokens[i + 4], this.canviz.height - tokens[i + 5])
								]);
							}
							break;
						case 'I': // image
							var l = tokenizer.takeNumber();
							var b = this.canviz.height - tokenizer.takeNumber();
							var w = tokenizer.takeNumber();
							var h = tokenizer.takeNumber();
							var src = tokenizer.takeString();
							if (!this.canviz.images[src]) {
								this.canviz.images[src] = new CanvizImage(this.canviz, src);
							}
							this.canviz.images[src].draw(ctx, l, b - h, w, h);
							break;
						case 'T': // text
							var l = Math.round(ctxScale * tokenizer.takeNumber() + this.canviz.padding);
							var t = Math.round(ctxScale * this.canviz.height + 2 * this.canviz.padding - (ctxScale * (tokenizer.takeNumber() + this.canviz.bbScale * fontSize) + this.canviz.padding));
							var textAlign = tokenizer.takeNumber();
							var textWidth = Math.round(ctxScale * tokenizer.takeNumber());
							var str = tokenizer.takeString();
							if (!redrawCanvasOnly && !/^\s*$/.test(str)) {
//								debug('draw text ' + str + ' ' + l + ' ' + t + ' ' + textAlign + ' ' + textWidth);
								str = str.escapeHTML();
								do {
									matches = str.match(/ ( +)/);
									if (matches) {
										var spaces = ' ';
										matches[1].length.times(function() {
											spaces += '&nbsp;';
										});
										str = str.replace(/  +/, spaces);
									}
								} while (matches);
								var text;
								var href = this.getAttr('URL', true) || this.getAttr('href', true);
								if (href) {
									var target = this.getAttr('target', true) || '_self';
									var tooltip = this.getAttr('tooltip', true) || this.getAttr('label', true);
//									debug(this.name + ', href ' + href + ', target ' + target + ', tooltip ' + tooltip);
									text = new Element('a', {href: href, target: target, title: tooltip});
									['onclick', 'onmousedown', 'onmouseup', 'onmouseover', 'onmousemove', 'onmouseout'].each(function(attrName) {
										var attrValue = this.getAttr(attrName, true);
										if (attrValue) {
											text.writeAttribute(attrName, attrValue);
										}
									}.bind(this));
									text.setStyle({
										textDecoration: 'none'
									});
								} else {
									text = new Element('span');
								}
								text.update(str);
								text.setStyle({
									fontSize: Math.round(fontSize * ctxScale * this.canviz.bbScale) + 'px',
									fontFamily: fontFamily,
									color: strokeColor.textColor,
									position: 'absolute',
									textAlign: (-1 == textAlign) ? 'left' : (1 == textAlign) ? 'right' : 'center',
									left: (l - (1 + textAlign) * textWidth) + 'px',
									top: t + 'px',
									width: (2 * textWidth) + 'px',
									zIndex: 100
								});
								if (1 != strokeColor.opacity) text.setOpacity(strokeColor.opacity);
								this.canviz.elements.appendChild(text);
							}
							break;
						case 'C': // set fill color
						case 'c': // set pen color
							var fill = ('C' == token);
							var color = this.parseColor(tokenizer.takeString());
							if (fill) {
								fillColor = color;
								ctx.fillStyle = color.canvasColor;
							} else {
								strokeColor = color;
								ctx.strokeStyle = color.canvasColor;
							}
							break;
						case 'F': // set font
							fontSize = tokenizer.takeNumber();
							fontFamily = tokenizer.takeString();
							switch (fontFamily) {
								case 'Times-Roman':
									fontFamily = 'Times New Roman';
									break;
								case 'Courier':
									fontFamily = 'Courier New';
									break;
								case 'Helvetica':
									fontFamily = 'Arial';
									break;
								default:
									// nothing
							}
//							debug('set font ' + fontSize + 'pt ' + fontFamily);
							break;
						case 'S': // set style
							var style = tokenizer.takeString();
							switch (style) {
								case 'solid':
								case 'filled':
									// nothing
									break;
								case 'dashed':
								case 'dotted':
									dashStyle = style;
									break;
								case 'bold':
									ctx.lineWidth = 2;
									break;
								default:
									matches = style.match(/^setlinewidth\((.*)\)$/);
									if (matches) {
										ctx.lineWidth = Number(matches[1]);
									} else {
										debug('unknown style ' + style);
									}
							}
							break;
						default:
							debug('unknown token ' + token);
							return;
					}
					if (path) {
						this.canviz.drawPath(ctx, path, filled, dashStyle);
						if (!redrawCanvasOnly) this.bbRect.expandToInclude(path.getBB());
						path = undefined;
					}
					token = tokenizer.takeChars();
				}
				if (!redrawCanvasOnly) {
					bbDiv.setStyle({
						position: 'absolute',
						left:   Math.round(ctxScale * this.bbRect.l + this.canviz.padding) + 'px',
						top:    Math.round(ctxScale * this.bbRect.t + this.canviz.padding) + 'px',
						width:  Math.round(ctxScale * this.bbRect.getWidth()) + 'px',
						height: Math.round(ctxScale * this.bbRect.getHeight()) + 'px'
					});
				}
				ctx.restore();
			}
		}.bind(this));
	},
	parseColor: function(color) {
		var parsedColor = {opacity: 1};
		// rgb/rgba
		if (/^#(?:[0-9a-f]{2}\s*){3,4}$/i.test(color)) {
			return this.canviz.parseHexColor(color);
		}
		// hsv
		var matches = color.match(/^(\d+(?:\.\d+)?)[\s,]+(\d+(?:\.\d+)?)[\s,]+(\d+(?:\.\d+)?)$/);
		if (matches) {
			parsedColor.canvasColor = parsedColor.textColor = this.canviz.hsvToRgbColor(matches[1], matches[2], matches[3]);
			return parsedColor;
		}
		// named color
		var colorScheme = this.getAttr('colorscheme') || 'X11';
		var colorName = color;
		matches = color.match(/^\/(.*)\/(.*)$/);
		if (matches) {
			if (matches[1]) {
				colorScheme = matches[1];
			}
			colorName = matches[2];
		} else {
			matches = color.match(/^\/(.*)$/);
			if (matches) {
				colorScheme = 'X11';
				colorName = matches[1];
			}
		}
		colorName = colorName.toLowerCase();
		var colorSchemeName = colorScheme.toLowerCase();
		var colorSchemeData = Canviz.prototype.colors.get(colorSchemeName);
		if (colorSchemeData) {
			var colorData = colorSchemeData[colorName];
			if (colorData) {
				return this.canviz.parseHexColor('#' + colorData);
			}
		}
		colorData = Canviz.prototype.colors.get('fallback')[colorName];
		if (colorData) {
			return this.canviz.parseHexColor('#' + colorData);
		}
		if (!colorSchemeData) {
			debug('unknown color scheme ' + colorScheme);
		}
		// unknown
		debug('unknown color ' + color + '; color scheme is ' + colorScheme);
		parsedColor.canvasColor = parsedColor.textColor = '#000000';
		return parsedColor;
	}
});

var CanvizNode = Class.create(CanvizEntity, {
	initialize: function($super, name, canviz, rootGraph, parentGraph) {
		$super('nodeAttrs', name, canviz, rootGraph, parentGraph, parentGraph);
	}
});
Object.extend(CanvizNode.prototype, {
	escStringMatchRe: /\\([NGL])/g
});

var CanvizEdge = Class.create(CanvizEntity, {
	initialize: function($super, name, canviz, rootGraph, parentGraph, tailNode, headNode) {
		$super('edgeAttrs', name, canviz, rootGraph, parentGraph, parentGraph);
		this.tailNode = tailNode;
		this.headNode = headNode;
	}
});
Object.extend(CanvizEdge.prototype, {
	escStringMatchRe: /\\([EGTHL])/g
});

var CanvizGraph = Class.create(CanvizEntity, {
	initialize: function($super, name, canviz, rootGraph, parentGraph) {
		$super('attrs', name, canviz, rootGraph, parentGraph, this);
		this.nodeAttrs = $H();
		this.edgeAttrs = $H();
		this.nodes = $A();
		this.edges = $A();
		this.subgraphs = $A();
	},
	initBB: function() {
		var coords = this.getAttr('bb').split(',');
		this.bbRect = new Rect(coords[0], this.canviz.height - coords[1], coords[2], this.canviz.height - coords[3]);
	},
	draw: function($super, ctx, ctxScale, redrawCanvasOnly) {
		$super(ctx, ctxScale, redrawCanvasOnly);
		[this.subgraphs, this.nodes, this.edges].each(function(type) {
			type.each(function(entity) {
				entity.draw(ctx, ctxScale, redrawCanvasOnly);
			});
		});
	}
});
Object.extend(CanvizGraph.prototype, {
	escStringMatchRe: /\\([GL])/g
});

var Canviz = Class.create({
	maxXdotVersion: '1.2',
	colors: $H({
		fallback:{
			black:'000000',
			lightgrey:'d3d3d3',
			white:'ffffff'
		}
	}),
	initialize: function(container, url, urlParams) {
		// excanvas can't init the element if we use new Element()
		this.canvas = document.createElement('canvas');
		Element.setStyle(this.canvas, {
			position: 'absolute'
		});
		if (!Canviz.canvasCounter) Canviz.canvasCounter = 0;
		this.canvas.id = 'canviz_canvas_' + ++Canviz.canvasCounter;
		this.elements = new Element('div');
		this.elements.setStyle({
			position: 'absolute'
		});
		this.container = $(container);
		this.container.setStyle({
			position: 'relative'
		});
		this.container.appendChild(this.canvas);
		if (Prototype.Browser.IE) {
			G_vmlCanvasManager.initElement(this.canvas);
			this.canvas = $(this.canvas.id);
		}
		this.container.appendChild(this.elements);
		this.ctx = this.canvas.getContext('2d');
		this.scale = 1;
		this.padding = 8;
		this.dashLength = 6;
		this.dotSpacing = 4;
		this.graphs = $A();
		this.images = new Hash();
		this.numImages = 0;
		this.numImagesFinished = 0;
		if (url) {
			this.load(url, urlParams);
		}
	},
	setScale: function(scale) {
		this.scale = scale;
	},
	setImagePath: function(imagePath) {
		this.imagePath = imagePath;
	},
	load: function(url, urlParams) {
		//$('debug_output').innerHTML = '';
		new Ajax.Request(url, {
			method: 'get',
			parameters: urlParams,
			onComplete: function(response) {
				this.parse(response.responseText);
			}.bind(this)
		});
	},
	parse: function(xdot) {
		this.graphs = $A();
		this.width = 0;
		this.height = 0;
		this.maxWidth = false;
		this.maxHeight = false;
		this.bbEnlarge = false;
		this.bbScale = 1;
		this.dpi = 96;
		this.bgcolor = {opacity: 1};
		this.bgcolor.canvasColor = this.bgcolor.textColor = '#ffffff';
		var lines = xdot.split(/\r?\n/);
		var i = 0;
		var line, lastChar, matches, rootGraph, isGraph, entity, entityName, attrs, attrName, attrValue, attrHash, drawAttrHash;
		var containers = $A();
		while (i < lines.length) {
			line = lines[i++].replace(/^\s+/, '');
			if ('' != line && '#' != line.substr(0, 1)) {
				while (i < lines.length && ';' != (lastChar = line.substr(line.length - 1, line.length)) && '{' != lastChar && '}' != lastChar) {
					if ('\\' == lastChar) {
						line = line.substr(0, line.length - 1);
					}
					line += lines[i++];
				}
//				debug(line);
				if (0 == containers.length) {
					matches = line.match(this.graphMatchRe);
					if (matches) {
						rootGraph = new CanvizGraph(matches[3], this);
						containers.unshift(rootGraph);
						containers[0].strict = !Object.isUndefined(matches[1]);
						containers[0].type = ('graph' == matches[2]) ? 'undirected' : 'directed';
						containers[0].attrs.set('xdotversion', '1.0');
						this.graphs.push(containers[0]);
//						debug('graph: ' + containers[0].name);
					}
				} else {
					matches = line.match(this.subgraphMatchRe);
					if (matches) {
						containers.unshift(new CanvizGraph(matches[1], this, rootGraph, containers[0]));
						containers[1].subgraphs.push(containers[0]);
//						debug('subgraph: ' + containers[0].name);
					}
				}
				if (matches) {
//					debug('begin container ' + containers[0].name);
				} else if ('}' == line) {
//					debug('end container ' + containers[0].name);
					containers.shift();
					if (0 == containers.length) {
						break;
					}
				} else {
					matches = line.match(this.nodeMatchRe);
					if (matches) {
						entityName = matches[2];
						attrs = matches[5];
						drawAttrHash = containers[0].drawAttrs;
						isGraph = false;
						switch (entityName) {
							case 'graph':
								attrHash = containers[0].attrs;
								isGraph = true;
								break;
							case 'node':
								attrHash = containers[0].nodeAttrs;
								break;
							case 'edge':
								attrHash = containers[0].edgeAttrs;
								break;
							default:
								entity = new CanvizNode(entityName, this, rootGraph, containers[0]);
								attrHash = entity.attrs;
								drawAttrHash = entity.drawAttrs;
								containers[0].nodes.push(entity);
						}
//						debug('node: ' + entityName);
					} else {
						matches = line.match(this.edgeMatchRe);
						if (matches) {
							entityName = matches[1];
							attrs = matches[8];
							entity = new CanvizEdge(entityName, this, rootGraph, containers[0], matches[2], matches[5]);
							attrHash = entity.attrs;
							drawAttrHash = entity.drawAttrs;
							containers[0].edges.push(entity);
//							debug('edge: ' + entityName);
						}
					}
					if (matches) {
						do {
							if (0 == attrs.length) {
								break;
							}
							matches = attrs.match(this.attrMatchRe);
							if (matches) {
								attrs = attrs.substr(matches[0].length);
								attrName = matches[1];
								attrValue = this.unescape(matches[2]);
								if (/^_.*draw_$/.test(attrName)) {
									drawAttrHash.set(attrName, attrValue);
								} else {
									attrHash.set(attrName, attrValue);
								}
//								debug(attrName + ' ' + attrValue);
								if (isGraph && 1 == containers.length) {
									switch (attrName) {
										case 'bb':
											var bb = attrValue.split(/,/);
											this.width  = Number(bb[2]);
											this.height = Number(bb[3]);
											break;
										case 'bgcolor':
											this.bgcolor = rootGraph.parseColor(attrValue);
											break;
										case 'dpi':
											this.dpi = attrValue;
											break;
										case 'size':
											var size = attrValue.match(/^(\d+|\d*(?:\.\d+)),\s*(\d+|\d*(?:\.\d+))(!?)$/);
											if (size) {
												this.maxWidth  = 72 * Number(size[1]);
												this.maxHeight = 72 * Number(size[2]);
												this.bbEnlarge = ('!' == size[3]);
											} else {
												debug('can\'t parse size');
											}
											break;
										case 'xdotversion':
											if (0 > this.versionCompare(this.maxXdotVersion, attrHash.get('xdotversion'))) {
												debug('unsupported xdotversion ' + attrHash.get('xdotversion') + '; this script currently supports up to xdotversion ' + this.maxXdotVersion);
											}
											break;
									}
								}
							} else {
								debug('can\'t read attributes for entity ' + entityName + ' from ' + attrs);
							}
						} while (matches);
					}
				}
			}
		}
/*
		if (this.maxWidth && this.maxHeight) {
			if (this.width > this.maxWidth || this.height > this.maxHeight || this.bbEnlarge) {
				this.bbScale = Math.min(this.maxWidth / this.width, this.maxHeight / this.height);
				this.width  = Math.round(this.width  * this.bbScale);
				this.height = Math.round(this.height * this.bbScale);
			}
		}
*/
//		debug('done');
		this.draw();
	},
	draw: function(redrawCanvasOnly) {
		if (Object.isUndefined(redrawCanvasOnly)) redrawCanvasOnly = false;
		var ctxScale = this.scale * this.dpi / 72;
		var width  = Math.round(ctxScale * this.width  + 2 * this.padding);
		var height = Math.round(ctxScale * this.height + 2 * this.padding);
		if (!redrawCanvasOnly) {
			this.canvas.width  = width;
			this.canvas.height = height;
			this.canvas.setStyle({
				width:  width  + 'px',
				height: height + 'px'
			});
			this.container.setStyle({
				width:  width  + 'px',
				height: height + 'px'
			});
			while (this.elements.firstChild) {
				this.elements.removeChild(this.elements.firstChild);
			}
		}
		this.ctx.save();
		this.ctx.lineCap = 'round';
		this.ctx.fillStyle = this.bgcolor.canvasColor;
		this.ctx.fillRect(0, 0, width, height);
		this.ctx.translate(this.padding, this.padding);
		this.ctx.scale(ctxScale, ctxScale);
		this.graphs[0].draw(this.ctx, ctxScale, redrawCanvasOnly);
		this.ctx.restore();
	},
	drawPath: function(ctx, path, filled, dashStyle) {
		if (filled) {
			ctx.beginPath();
			path.makePath(ctx);
			ctx.fill();
		}
		if (ctx.fillStyle != ctx.strokeStyle || !filled) {
			switch (dashStyle) {
				case 'dashed':
					ctx.beginPath();
					path.makeDashedPath(ctx, this.dashLength);
					break;
				case 'dotted':
					var oldLineWidth = ctx.lineWidth;
					ctx.lineWidth *= 2;
					ctx.beginPath();
					path.makeDottedPath(ctx, this.dotSpacing);
					break;
				case 'solid':
				default:
					if (!filled) {
						ctx.beginPath();
						path.makePath(ctx);
					}
			}
			ctx.stroke();
			if (oldLineWidth) ctx.lineWidth = oldLineWidth;
		}
	},
	unescape: function(str) {
		var matches = str.match(/^"(.*)"$/);
		if (matches) {
			return matches[1].replace(/\\"/g, '"');
		} else {
			return str;
		}
	},
	parseHexColor: function(color) {
		var matches = color.match(/^#([0-9a-f]{2})\s*([0-9a-f]{2})\s*([0-9a-f]{2})\s*([0-9a-f]{2})?$/i);
		if (matches) {
			var canvasColor, textColor = '#' + matches[1] + matches[2] + matches[3], opacity = 1;
			if (matches[4]) { // rgba
				opacity = parseInt(matches[4], 16) / 255;
				canvasColor = 'rgba(' + parseInt(matches[1], 16) + ',' + parseInt(matches[2], 16) + ',' + parseInt(matches[3], 16) + ',' + opacity + ')';
			} else { // rgb
				canvasColor = textColor;
			}
		}
		return {canvasColor: canvasColor, textColor: textColor, opacity: opacity};
	},
	hsvToRgbColor: function(h, s, v) {
		var i, f, p, q, t, r, g, b;
		h *= 360;
		i = Math.floor(h / 60) % 6;
		f = h / 60 - i;
		p = v * (1 - s);
		q = v * (1 - f * s);
		t = v * (1 - (1 - f) * s);
		switch (i) {
			case 0: r = v; g = t; b = p; break;
			case 1: r = q; g = v; b = p; break;
			case 2: r = p; g = v; b = t; break;
			case 3: r = p; g = q; b = v; break;
			case 4: r = t; g = p; b = v; break;
			case 5: r = v; g = p; b = q; break;
		}
		return 'rgb(' + Math.round(255 * r) + ',' + Math.round(255 * g) + ',' + Math.round(255 * b) + ')';
	},
	versionCompare: function(a, b) {
		a = a.split('.');
		b = b.split('.');
		var a1, b1;
		while (a.length || b.length) {
			a1 = a.length ? a.shift() : 0;
			b1 = b.length ? b.shift() : 0;
			if (a1 < b1) return -1;
			if (a1 > b1) return 1;
		}
		return 0;
	},
	// an alphanumeric string or a number or a double-quoted string or an HTML string
	idMatch: '([a-zA-Z\u0080-\uFFFF_][0-9a-zA-Z\u0080-\uFFFF_]*|-?(?:\\.\\d+|\\d+(?:\\.\\d*)?)|"(?:\\\\"|[^"])*"|<(?:<[^>]*>|[^<>]+?)+>)'
});
Object.extend(Canviz.prototype, {
	// ID or ID:port or ID:compassPoint or ID:port:compassPoint
	nodeIdMatch: Canviz.prototype.idMatch + '(?::' + Canviz.prototype.idMatch + ')?(?::' + Canviz.prototype.idMatch + ')?'
});
Object.extend(Canviz.prototype, {
	graphMatchRe: new RegExp('^(strict\\s+)?(graph|digraph)(?:\\s+' + Canviz.prototype.idMatch + ')?\\s*{$', 'i'),
	subgraphMatchRe: new RegExp('^(?:subgraph\\s+)?' + Canviz.prototype.idMatch + '?\\s*{$', 'i'),
	nodeMatchRe: new RegExp('^(' + Canviz.prototype.nodeIdMatch + ')\\s+\\[(.+)\\];$'),
	edgeMatchRe: new RegExp('^(' + Canviz.prototype.nodeIdMatch + '\\s*-[->]\\s*' + Canviz.prototype.nodeIdMatch + ')\\s+\\[(.+)\\];$'),
	attrMatchRe: new RegExp('^' + Canviz.prototype.idMatch + '=' + Canviz.prototype.idMatch + '(?:[,\\s]+|$)')
});

var CanvizImage = Class.create({
	initialize: function(canviz, src) {
		this.canviz = canviz;
		++this.canviz.numImages;
		this.finished = this.loaded = false;
		this.img = new Image();
		this.img.onload = this.onLoad.bind(this);
		this.img.onerror = this.onFinish.bind(this);
		this.img.onabort = this.onFinish.bind(this);
		this.img.src = this.canviz.imagePath + src;
	},
	onLoad: function() {
		this.loaded = true;
		this.onFinish();
	},
	onFinish: function() {
		this.finished = true;
		++this.canviz.numImagesFinished;
		if (this.canviz.numImages == this.canviz.numImagesFinished) {
			this.canviz.draw(true);
		}
	},
	draw: function(ctx, l, t, w, h) {
		if (this.finished) {
			if (this.loaded) {
				ctx.drawImage(this.img, l, t, w, h);
			} else {
				debug('can\'t load image ' + this.img.src);
				this.drawBrokenImage(ctx, l, t, w, h);
			}
		}
	},
	drawBrokenImage: function(ctx, l, t, w, h) {
		ctx.save();
		ctx.beginPath();
		new Rect(l, t, l + w, t + w).draw(ctx);
		ctx.moveTo(l, t);
		ctx.lineTo(l + w, t + w);
		ctx.moveTo(l + w, t);
		ctx.lineTo(l, t + h);
		ctx.strokeStyle = '#f00';
		ctx.lineWidth = 1;
		ctx.stroke();
		ctx.restore();
	}
});

function debug(str, escape) {
	str = String(str);
	if (Object.isUndefined(escape)) {
		escape = true;
	}
	if (escape) {
		str = str.escapeHTML();
	}
	//$('debug_output').innerHTML += '&raquo;' + str + '&laquo;<br />';
}
