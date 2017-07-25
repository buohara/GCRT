class RenderPass
{
	var shaderProg;
	var fboW;
	var fboH;
	var gl;

	constructor(w, h, glIn)
	{
		gl = glIn;
		fboW = w;
		fboH = h;
	}

	render(scn)
	{
		gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
		gl.useProgram(shaderProg);

		var viewID = gl.getUniformLocation(shaderProg, "view");
		var view = scn.cam.view;
		gl.uniformMatrix4fv(viewID, 1, false, view);

		var projID = gl.getUniformLocation(shaderProg, "proj");
		var proj = scn.cam.projection;
		gl.uniformMatrix4fv(projID, 1, false, proj);

		var lightPosID = gl.getUniformLocation(shaderProg, "lightPos");
		var lightPos = scn.lights[0].pos;
		gl.uniform3fv(lightPosID, 1, lightPos);

		var lightColID = gl.getUniformLocation(shaderProg, "lightColor");
		var lightCol = scn.lights[0].color;
		gl.uniform3fv(lightColID, 1, lightCol);

		var models = scn.models;
	}
}