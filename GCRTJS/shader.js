class Shader
{
	var name;
	var program;

	constructor(gl, name, vsSrc, psSrc)
	{
		var vsID = gl.createShader(gl.GL_VERTEX_sHADER);
		gl.shaderSource(vsID, 1, vsSrc, null);

		var psID = gl.createShader(gl.GL_FRAGMENT_sHADER);
		gl.shaderSource(psID, 1, psSrc, null);

		program = gl.createProgram();
		gl.attachShader(program, vsID);
		gl.attachShader(program, psID);
		gl.linkProgram(program);
	}
}