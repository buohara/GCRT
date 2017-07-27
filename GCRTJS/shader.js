class Shader
{
	constructor(glIn, nameIn, vsSrc, psSrc)
	{
		this.gl = glIn;
		this.name = nameIn;

		var vsID = this.gl.createShader(this.gl.VERTEX_SHADER);
		this.gl.shaderSource(vsID, vsSrc);
		this.gl.compileShader(vsID);

		var compilationLog = this.gl.getShaderInfoLog(vsID);
		console.log('VS: ' + compilationLog);

		var psID = this.gl.createShader(this.gl.FRAGMENT_SHADER);
		this.gl.shaderSource(psID, psSrc);
		this.gl.compileShader(psID);

		compilationLog = this.gl.getShaderInfoLog(psID);
		console.log('PS: ' + compilationLog);

		this.program = this.gl.createProgram();
		this.gl.attachShader(this.program, vsID);
		this.gl.attachShader(this.program, psID);
		this.gl.linkProgram(this.program);
	}

	get Program()
	{
		return this.program;
	}
}