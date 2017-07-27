class RenderPass
{
	constructor(w, h, glIn)
	{
		this.gl = glIn;
		this.fboW = w;
		this.fboH = h;
		
		this.shader = new Shader(
			this.gl, 
			"RenderPass", 
			renderShaderVS, 
			renderShaderPS
		);

		this.shaderProg = this.shader.Program;
	}

	render(scn)
	{
		this.gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
		this.gl.useProgram(this.shaderProg);
		this.gl.viewport(0, 0, this.fboW, this.fboH);

		var viewID = this.gl.getUniformLocation(this.shaderProg, "view");
		var view = scn.Cam.View;
		this.gl.uniformMatrix4fv(viewID, false, view);

		var projID = this.gl.getUniformLocation(this.shaderProg, "proj");
		var proj = scn.cam.projection;
		this.gl.uniformMatrix4fv(projID, false, proj);

		var lightPosID = this.gl.getUniformLocation(this.shaderProg, "lightPos");
		var lightPos = scn.lights[0].pos;
		this.gl.uniform3fv(lightPosID, lightPos);

		var lightColID = this.gl.getUniformLocation(this.shaderProg, "lightColor");
		var lightCol = scn.lights[0].color;
		this.gl.uniform3fv(lightColID, lightCol);

		var models = scn.Models;

		for (var i = 0; i < models.length; i++)
		{
			var model = models[i].model;
			var modelInv = mat4.create();
			var modelInvT = mat4.create();
			mat4.invert(modelInv, model);
			mat4.transpose(modelInvT, modelInv);

			var kd = models[i].kd;
			var kdID = this.gl.getUniformLocation(this.shaderProg, "kd");
			this.gl.uniform3fv(kdID, kd);

			var modelID = this.gl.getUniformLocation(this.shaderProg, "model");
			this.gl.uniformMatrix4fv(modelID, false, model);

			var modelInvID = this.gl.getUniformLocation(this.shaderProg, "modelInv");
			this.gl.uniformMatrix4fv(modelInvID, false, modelInvT);

			models[i].draw();
		}
	}
}