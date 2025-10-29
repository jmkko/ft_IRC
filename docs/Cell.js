class Cell {
	constructor(p5, i, j, size) {
		this.p5 = p5;
		this.printed = false;
		this.gridPos = this.p5.createVector(i, j);
		this.realPos = this.p5.createVector(i * size, j * size);
		this.n = 0.0;
		this.size = size;
		this.group = 0;; 
	}

	putChar(color) {
		let p = this.p5;
		let chars = ['.', ':', '-', '=', '+', '*', '#', '%', '@'];
		let index;
		if (this.printed)
			index = p.floor((1 - this.n) * chars.length);
		else
			index = p.floor(this.n * chars.length);
		p.textSize(this.size);
		if (this.printed)
			color = 255 - color;
		p.fill(color);
		p.text(chars[index], this.realPos.x, this.realPos.y);
	}

	draw(color, alpha) {
		let p = this.p5;
		if (this.printed)
			color = 255 - color;
		p.fill(color, 20, 89, alpha);
		p.rect(this.realPos.x, this.realPos.y, this.size);
	}

	contains(px, py) {
		if (px >= this.realPos.x && px <= this.realPos.x + this.size &&
			py >= this.realPos.y && py <= this.realPos.y + this.size ) {
			return true;
		}
		return false;
	}

	changeColor() {
		this.draw(34, 89, 244);
	}
}
