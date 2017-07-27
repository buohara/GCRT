class Renderer
{
	constructor(gl)
	{
		gl.clearColor(0.0, 0.0, 0.0, 1.0);
		gl.enable(gl.DEPTH_TEST);
		gl.depthFunc(gl.LEQUAL);
		gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

		this.scn = new Scene();

		var cam = new Camera(
			vec3.fromValues(8.0, 8.0, 8.0),
			vec3.fromValues(0.0, 0.0, 0.0),
			vec3.fromValues(0.0, 0.0, 1.0),
			4.0 / 3.0,
			75.0,
			1.0,
			100.0
		);

		this.scn.setCamera(cam);	
		
		this.scn.addModel(new Box(gl));
		
		this.scn.addLight(
			new PointLight(
				vec3.fromValues(0.0, 10.0, 5.0),
				vec3.fromValues(10.0, 50.0, 60.0)
			)
		);

		this.passes = [];
		this.passes.push(new RenderPass(1200, 800, gl));
	}

	render()
	{
		for (var i = 0; i < this.passes.length; i++)
		{
			this.passes[i].render(this.scn);
		}

		window.requestAnimationFrame(this.render.bind(this));
	}
}