class Camera
{
	constructor(
		posIn,
		lookAtIn,
		upIn,
		aspectIn,
		fovIn,
		nclipIn,
		fclipIn
		)
	{
		this.pos 		= posIn;
		this.lookAt 	= lookAtIn;
		this.up 		= upIn;
		this.aspect 	= aspectIn;
		this.fov 		= fovIn;
		this.nclip 		= nclipIn;
		this.fclip 		= fclipIn;
		this.projection = mat4.create();
		this.view 		= mat4.create();

		this.keysDown = [];
		this.txSpeed = 0.1;
		this.rotSpeed = 0.001;

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
			this.lookAt, 
			this.up
		);
	}

	handleKeyDown(event)
	{
		this.keysDown[event.keyCode] = true;
	}

	handleKeyUp(event)
	{
		this.keysDown[event.keyCode] = false;
	}

	handleMouseDown(event)
	{
		this.mouseDown = true;
		this.mx1 = event.clientX;
		this.my1 = event.clientY;
	}

	handleMouseUp(event)
	{
		this.mouseDown = false;
	}

	handleMouseMove(event)
	{
		if (this.mouseDown == true)
		{
			var mx2 = event.clientX;
			var my2 = event.clientY;

			var dx = mx2 - this.mx1;
			var dy = my2 - this.my1;

			var lookDir = vec3.create();
			var right 	= vec3.create();

			vec3.subtract(lookDir, this.lookAt, this.pos);
			vec3.normalize(lookDir, lookDir);
			vec3.cross(right, lookDir, this.up);

			var level = vec3.fromValues(lookDir[0], lookDir[1], 0.0);
			vec3.normalize(level, level);
			var theta = vec3.dot(lookDir, level);

			var yaw 	= mat4.create();
			var pitch 	= mat4.create();

			mat4.fromRotation(yaw, this.rotSpeed * Math.sin(theta) * dx, this.up);
			mat4.fromRotation(pitch, this.rotSpeed * dy, right);

			vec3.transformMat4(lookDir, lookDir, yaw);
			vec3.transformMat4(lookDir, lookDir, pitch);

			vec3.add(this.lookAt, this.pos, lookDir);

			mat4.lookAt(
				this.view, 
				this.pos, 
				this.lookAt, 
				this.up
			);

			this.mx1 = mx2;
			this.my1 = my2;
		}
	}

	get Projection()
	{
		return this.projection;
	}

	get View()
	{
		return this.view;
	}

	update()
	{
		var lookDir = vec3.create();
		var right 	= vec3.create();

		vec3.subtract(lookDir, this.lookAt, this.pos);
		vec3.normalize(lookDir, lookDir);
		vec3.cross(right, lookDir, this.up);

		if (this.keysDown[87] == true)
		{
			vec3.scaleAndAdd(this.pos, this.pos, lookDir, 2 * this.txSpeed);
			vec3.scaleAndAdd(this.lookAt, this.lookAt, lookDir, 2 * this.txSpeed);
		}
		else if (this.keysDown[83] == true)
		{
			vec3.scaleAndAdd(this.pos, this.pos, lookDir, -2 * this.txSpeed);
			vec3.scaleAndAdd(this.lookAt, this.lookAt, lookDir, -2 * this.txSpeed);		
		}

		if (this.keysDown[68] == true)
		{
			vec3.scaleAndAdd(this.pos, this.pos, right, -this.txSpeed);
			vec3.scaleAndAdd(this.lookAt, this.lookAt, right, -this.txSpeed);
			
		}
		else if (this.keysDown[65] == true)
		{
			vec3.scaleAndAdd(this.pos, this.pos, right, this.txSpeed);
			vec3.scaleAndAdd(this.lookAt, this.lookAt, right, this.txSpeed);
		}

		mat4.lookAt(
			this.view, 
			this.pos, 
			this.lookAt, 
			this.up
		);
	}
}