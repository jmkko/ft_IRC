// made by Hazardous.editorial -- Ilia

function PerlinFog(p, option = {}) {
    let		grid 
	let		mouse;
    let		cell_size = option.cellSize || 5;			// size of cells in px
	const	noise_precision = 16;	// Nois Precisions [1 - 32]
	const	cursor_size = 80;		// size of Halo effect around the cursor
	let		parent;
	let w, h; 
	p.setup = function () {
		parent = p._userNode;
		w = window.innerWidth;
		h = window.innerHeight;
		p.createCanvas(w, h).parent(parent);
		p.noCursor();
		p.frameRate(10);
		p.noiseDetail(noise_precision);
		p.noStroke();
		p.pixelDensity(1);
		grid = new Grid(p, w, h, cell_size, parent)
		grid.init(w, h, cell_size, parent);
		grid.saturation = 1;	// [0 - 100]
		grid.blur = 0.08;		//noisy blur effect (randomness)
		grid.tones = 10;		// shades of grey
		p.printText(grid, 60);
		grid.run_noise(0.01);
		grid.display("noise", 255);
	};

	p.draw = function() {
		//p.background(0).clear();
		mouse = p.createVector(p.mouseX, p.mouseY)
		grid.run_noise(0.01);
		for (let cell of grid.cells) {
			let dist = mouse.dist(cell.realPos);
			if (dist < cursor_size) {
				cell.n += p.map(dist, 0, cursor_size, 0.25, 0);
			}
		}
		grid.display("noise", 50);
		grid.noise.z += 0.008; 
	};

	p.windowResized = function () {
    if (!grid) return; // prevent crash
		w = window.innerWidth;
		h = window.innerHeight;
		p.resizeCanvas(w, h); // 🔧 resize canvas!
		// cell_size = Math.floor(w * 0.01);
		grid.init(w, h, cell_size, parent);
		grid.run_noise(0.01);
		grid.display("noise", 255);
		p.printText(grid, 30);
	};

	p.printInfos = function(x, y) {
		p.fill(255)
		p.textSize(14);
		p.text("size: ", 30 + x, 30 + y);
		p.text(cell_size, 70 + x, 30 + y);
		p.text("frame Rate: " + Math.round(p.frameRate()), 30 + x, 44 + y);
	}

	p.printText = function(grid, size) {
		grid.buffer.background(255);
		let fontSize = p.floor(0.0833 * window.innerWidth - 1.65);
		if (window.innerWidth > 600)
			grid.text_to_buffer("HAZARDOUS  ÉDiTORiAL","justify", 0, 160, fontSize);
		else
			grid.text_to_buffer("HAZARDOUS  ÉDiTORiAL","justify", 0, 100, fontSize);
		grid.buffer_to_grid(180, "title");
	}

	p.handleClick = function(grid, mouse) {
		console.log("mouse has been clicked");
	}

	p.mouseClicked = function() {
		p.handleClick(grid, mouse);
	}
};

