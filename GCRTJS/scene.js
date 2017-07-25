class Scene
{
	var models;
	var lights;
	var cam;

	constructor(gl)
	{
		models = [];
		models.push(new Box(gl))

		lights = [];
		
		lights.push(
			new PointLight(
				new Vec3(0.0, 10.0, 5.0),
				new Vec3(10.0, 50.0, 60.0)
			)
		);
		
		cam = new Camera(
			new Vec3(8.0, 8.0, 8.0),
			new Vec3(0.0, 0.0, 0.0) 
			new Vec3(0.0, 0.0, 1.0),
			4.0 / 3.0,
			75.0,
			1.0,
			100.0
		);
	}

	setCamera(camIn)
	{
		cam = camIn;
	}

	get cam()
	{
		return cam;
	}

	get lights()
	{
		return lights;
	}

	get models()
	{
		return models;
	}

	addLight(newLight)
	{
		lights.push(newLight);
	}

	addModel(newModel)
	{
		models.push(newModel);
	}
}