class Camera
{
	var pos;
	var lookDir;
	var up;

	var aspect;
	var fov;
	var nclip;
	var fclip;

	var projection;
	var view;

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
		pos 	= posIn;
		lookDir = lookDirIn;
		up 		= upIn;
		aspect 	= aspectIn;
		fov 	= fovIn;
		nclip 	= nclipIn;
		fclip 	= fclipIn;

		mat4.perspective(projection, fov, aspect, nclip, fclip);
		mat4.lookAt(view, pos, lookDir, up);
	}

	get projection()
	{
		return this.projection;
	}

	get view()
	{
		return this.view;
	}
}