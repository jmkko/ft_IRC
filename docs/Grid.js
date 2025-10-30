// Created by Ilia:Ilia@hazardous.net

// parent: is id of html element
// Hold a buffer
class Grid {
	constructor (p5, width, height, cellsize, parent) {
		this.p5	= p5;
      	this.buffer;
		this.ctx;
		this.cells;
		this.noise = {x:1, y:5000, z:0};
		this.cellSize;
		this.blur;
		this.saturation;
		this.tones;
		this.init(width, height, cellsize, parent);
	}

	init(width, height, size, parent) {
		let p = this.p5;
		this.noise = {x:1, y:5000, z:0};
		if(width > max_width){
			this.ctx = p.resizeCanvas(max_width , height);
		} else if (width <= max_width) {
			this.ctx = p.resizeCanvas(width, height);
		}
		this.cellSize = size;
		this.buffer = p.createGraphics(width, height);
		this.cells = [];
		this.res = {x:this.p5.floor(width / size) + 1,
					y:this.p5.floor(height / size) + 1};
		for (let i = 0; i < this.res.x; i++) {
			for (let j = 0; j < this.res.y; j++) {
				let index = i + j * this.res.y;
				this.cells.push(new Cell(p, i, j, size, 0));
			}
		}
	}

	// Print text on buffer
	text_to_buffer(str, align, x, y, textSize) {
		let buffer = this.buffer;
		let p = this.p5;
		let xOff, yOff;
		buffer.fill(0);
		buffer.textSize(textSize);
		buffer.textFont("Ubuntu");
		buffer.textLeading(2);
		buffer.textStyle(p.BOLDITALIC);
		let textWidth = buffer.textWidth(str);
		let textHeight = (p.textAscent() + p.textDescent()); // * (lines.lenght + 1);
		if (align == "justify") {
			xOff = (p.width - textWidth) / 2;
			yOff = 0;
		} else if (align == "center") {
			xOff = (p.width - textWidth) / 2
			yOff = (p.height - textHeight) / 2;
		}
		buffer.text(str, xOff + x, yOff + y);
	}
	
	img_to_buffer(img, x, y) { 
		this.buffer.background(255);
		this.buffer.image(img, x, y, 3 * img.width, 3 * img.height);
	}
	// calc average color of the buffer on each cell
	average_color(buffer, cell) {
		let total = 0;
		let cellSize = cell.size;
		for (let x = cell.realPos.x; x < cell.realPos.x + cellSize; x++) {
			for (let y = cell.realPos.y; y < cell.realPos.y + cellSize; y++) {
				let pixelIndex = (x + y * buffer.width) * 4;
				let greyShade = (	buffer.pixels[pixelIndex] +
									buffer.pixels[pixelIndex + 1] +
									buffer.pixels[pixelIndex + 2]) / 3;
				total += greyShade;
			}
		}
		return (total / (cellSize * cellSize));
	}

	// Print buffer on cells of grid
	buffer_to_grid(threshold, group) {
		let buffer = this.buffer;

		buffer.loadPixels();
		let cellSize = this.cellSize;
		for (let cell of this.cells) {
			let average = this.average_color(buffer, cell);
			if (average <= threshold) {
				cell.printed = true;
				cell.group = group;
			}
		}
		buffer.clear();
    buffer.background(255); // optional: make opaque
	}

	run_noise(amount) {
		let p = this.p5;
		let index, n;

		this.noise.x = 1;
		for (let i = 0; i < this.res.x; i++) {
			this.noise.y = 99;
			for (let j = 0 ; j < this.res.y; j++) {
				n = p.noise(this.noise.x, this.noise.y, this.noise.z)
				index = j + i * this.res.y;
				this.cells[index].n = n;
				this.noise.y += amount;
			}
			this.noise.x += amount;
		}
	}

	display(option, alpha) {
		let p = this.p5;
		let blr = this.blur;
		let sat = this.saturation;
		let ton = this.tones;
		for (let cell of this.cells) {
			let color = p.floor(p.map(cell.n, 0, 1, -sat, ton + sat)) * (255 / ton);
			cell.n += p.random(-blr, blr);
			if (option == "noise") {
				cell.draw(color, alpha);
			} else if (option == "char") {
				cell.putChar();
			}
		};
	}
	
	shadeFrom(x,y){
		let p = this.p5;
		let center = p.createVector(this.res / 2, this.res/2)
		let radius = 50
		for (let i = 0; i < this.res.x; i++) {
			for (let j = 0; j < this.res.y; j++) {
				let pos = p.createVector(i,j)
				let dist = pos.dist(center)
				if(dist>radius) this.cells[i+j*this.res.y].alpha = 255;
			}
		}
	}
}
