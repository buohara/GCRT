class Camera
{
	constructor(
		posIn,
		lookDirIn,
		upIn,
		aspectIn,
		fovIn,
		nclipIn,
		fclipIn
		)
	{
		this.pos 		= posIn;
		this.lookDir 	= lookDirIn;
		this.up 		= upIn;
		this.aspect 	= aspectIn;
		this.fov 		= fovIn;
		this.nclip 		= nclipIn;
		this.fclip 		= fclipIn;
		this.projection = mat4.create();
		this.view 		= mat4.create();

		mat4.perspective(
			this.projection, 
			this.fov, 
			this.aspect,
			this.nclip,
			this.fclip)
		;
		
		mat4.lookAt(
			this.view, 
			this.pos, 
			this.lookDir, 
			this.up
		);
	}

	get Projection()
	{
		return this.projection;
	}

	get View()
	{
		return this.view;
	}
}