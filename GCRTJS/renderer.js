class Renderer
{
	var scn;
	var passes;

	constructor(gl)
	{
		gl.clearColor(0.0, 0.0, 0.0, 0.0);
		gl.enable(gl.DEPTH_TEST);
		gl.depthFunc(gl.LEQUAL);
		gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
		gl.polygonMode(gl.FRONT_AND_BACK, gl.FILL);

		scn = new Scene();

		var cam = new Camera(
			new Vec3(8.0, 8.0, 8.0),
			new Vec3(0.0, 0.0, 0.0) 
			new Vec3(0.0, 0.0, 1.0),
			4.0 / 3.0,
			75.0,
			1.0,
			100.0
		);

		scn.setCamera(cam);	
		scn.AddModel(new Box(gl));
		scn.AddLight(
			new PointLight(
				new Vec3(0.0, 10.0, 5.0),
				new Vec3(10.0, 50.0, 60.0)
			)
		);

		passes = [];
		passes.push(new RenderPass(1200, 800, gl));
	}

	render()
	{
		for (var i = 0; i < passes.length; i++)
		{
			passes[i].render(scn);
		}
	}
}