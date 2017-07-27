class Scene
{
	constructor(gl)
	{
		this.models = [];
		this.lights = [];
	}

	setCamera(camIn)
	{
		this.cam = camIn;
	}

	get Cam()
	{
		return this.cam;
	}

	get Lights()
	{
		return this.lights;
	}

	get Models()
	{
		return this.models;
	}

	addLight(newLight)
	{
		this.lights.push(newLight);
	}

	addModel(newModel)
	{
		this.models.push(newModel);
	}

	update()
	{
		this.cam.update();
	}
}