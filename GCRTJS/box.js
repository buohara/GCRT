class Box
{
	constructor(glIn)
	{
		this.gl = glIn;
		this.vaoID = this.gl.createVertexArray();
		this.gl.bindVertexArray(this.vaoID);
		this.kd = vec3.fromValues(0.1, 0.1, 0.2);

		this.nVerts = 36;

		this.genPositions();
		this.genNormals();
		this.genUVs();
		this.genTans();

		this.model = mat4.create();
	}

	genPositions()
	{
		var pos = [];

		var hx = 0.5;
	    var hy = 0.5;
	    var hz = 0.5;

	    // Front

	    pos.push(vec3.fromValues(hx, hy, hz));
	    pos.push(vec3.fromValues(hx, -hy, hz));
	    pos.push(vec3.fromValues(hx, -hy, -hz));
	    pos.push(vec3.fromValues(hx, hy, hz));
	    pos.push(vec3.fromValues(hx, -hy, -hz));
	    pos.push(vec3.fromValues(hx, hy, -hz));
	    pos.push(vec3.fromValues(-hx, -hy, hz));
	    pos.push(vec3.fromValues(-hx, hy, hz));
	    pos.push(vec3.fromValues(-hx, hy, -hz));
	    pos.push(vec3.fromValues(-hx, -hy, hz));
	    pos.push(vec3.fromValues(-hx, hy, -hz));
	    pos.push(vec3.fromValues(-hx, -hy, -hz));
	    pos.push(vec3.fromValues(-hx, hy, hz));
	    pos.push(vec3.fromValues(hx, hy, hz));
	    pos.push(vec3.fromValues(hx, hy, -hz));
	    pos.push(vec3.fromValues(-hx, hy, hz));
	    pos.push(vec3.fromValues(hx, hy, -hz));
	    pos.push(vec3.fromValues(-hx, hy, -hz));
	    pos.push(vec3.fromValues(hx, -hy, hz));
	    pos.push(vec3.fromValues(-hx, -hy, hz));
	    pos.push(vec3.fromValues(-hx, -hy, -hz));
	    pos.push(vec3.fromValues(hx, -hy, hz));
	    pos.push(vec3.fromValues(-hx, -hy, -hz));
	    pos.push(vec3.fromValues(hx, -hy, -hz));
	    pos.push(vec3.fromValues(hx, hy, hz));
	    pos.push(vec3.fromValues(-hx, hy, hz));
	    pos.push(vec3.fromValues(-hx, -hy, hz));
	    pos.push(vec3.fromValues(hx, hy, hz));
	    pos.push(vec3.fromValues(-hx, -hy, hz));
	    pos.push(vec3.fromValues(hx, -hy, hz));
	    pos.push(vec3.fromValues(-hx, hy, -hz));
	    pos.push(vec3.fromValues(hx, hy, -hz));
	    pos.push(vec3.fromValues(hx, -hy, -hz));
	    pos.push(vec3.fromValues(-hx, hy, -hz));
	    pos.push(vec3.fromValues(hx, -hy, -hz));
	    pos.push(vec3.fromValues(-hx, -hy, -hz));

	    var pos32 = new Float32Array(pos.length * 3);

	    for(var i = 0; i < pos.length; i++)
	    {
	    	pos32[3 * i] = pos[i][0];
	    	pos32[3 * i + 1] = pos[i][1];
	    	pos32[3 * i + 2] = pos[i][2];
	    }

	    this.posVboID = this.gl.createBuffer();
	    this.gl.bindBuffer(this.gl.ARRAY_BUFFER, this.posVboID);
	    this.gl.bufferData(this.gl.ARRAY_BUFFER, pos32, gl.STATIC_DRAW, 0, pos32.length);

	    this.gl.vertexAttribPointer(0, 3, this.gl.FLOAT, gl.FALSE, 0, 0);
    	this.gl.enableVertexAttribArray(0);
	}

	genNormals()
	{
		var norms = [];

	    norms.push(vec3.fromValues(1.0, 0.0, 0.0));
	    norms.push(vec3.fromValues(1.0, 0.0, 0.0));
	    norms.push(vec3.fromValues(1.0, 0.0, 0.0));
	    norms.push(vec3.fromValues(1.0, 0.0, 0.0));
	    norms.push(vec3.fromValues(1.0, 0.0, 0.0));
	    norms.push(vec3.fromValues(1.0, 0.0, 0.0));
	    norms.push(vec3.fromValues(-1.0, 0.0, 0.0));
	    norms.push(vec3.fromValues(-1.0, 0.0, 0.0));
	    norms.push(vec3.fromValues(-1.0, 0.0, 0.0));
	    norms.push(vec3.fromValues(-1.0, 0.0, 0.0));
	    norms.push(vec3.fromValues(-1.0, 0.0, 0.0));
	    norms.push(vec3.fromValues(-1.0, 0.0, 0.0));
	    norms.push(vec3.fromValues(0.0, 1.0, 0.0));
	    norms.push(vec3.fromValues(0.0, 1.0, 0.0));
	    norms.push(vec3.fromValues(0.0, 1.0, 0.0));
	    norms.push(vec3.fromValues(0.0, 1.0, 0.0));
	    norms.push(vec3.fromValues(0.0, 1.0, 0.0));
	    norms.push(vec3.fromValues(0.0, 1.0, 0.0));
	    norms.push(vec3.fromValues(0.0, -1.0, 0.0));
	    norms.push(vec3.fromValues(0.0, -1.0, 0.0));
	    norms.push(vec3.fromValues(0.0, -1.0, 0.0));
	    norms.push(vec3.fromValues(0.0, -1.0, 0.0));
	    norms.push(vec3.fromValues(0.0, -1.0, 0.0));
	    norms.push(vec3.fromValues(0.0, -1.0, 0.0));
	    norms.push(vec3.fromValues(0.0, 0.0, 1.0));
	    norms.push(vec3.fromValues(0.0, 0.0, 1.0));
	    norms.push(vec3.fromValues(0.0, 0.0, 1.0));
	    norms.push(vec3.fromValues(0.0, 0.0, 1.0));
	    norms.push(vec3.fromValues(0.0, 0.0, 1.0));
	    norms.push(vec3.fromValues(0.0, 0.0, 1.0));
	    norms.push(vec3.fromValues(0.0, 0.0, -1.0));
	    norms.push(vec3.fromValues(0.0, 0.0, -1.0));
	    norms.push(vec3.fromValues(0.0, 0.0, -1.0));
	    norms.push(vec3.fromValues(0.0, 0.0, -1.0));
	    norms.push(vec3.fromValues(0.0, 0.0, -1.0));
	    norms.push(vec3.fromValues(0.0, 0.0, -1.0));

	    var norms32 = new Float32Array(norms.length * 3);

	    for(var i = 0; i < norms.length; i++)
	    {
	    	norms32[3 * i]     = norms[i][0];
	    	norms32[3 * i + 1] = norms[i][1];
	    	norms32[3 * i + 2] = norms[i][2];
	    }

	    this.normVboID = this.gl.createBuffer();
	    this.gl.bindBuffer(this.gl.ARRAY_BUFFER, this.normVboID);
	    this.gl.bufferData(this.gl.ARRAY_BUFFER, norms32, this.gl.STATIC_DRAW, 0, norms32.length);

	    this.gl.vertexAttribPointer(1, 3, this.gl.FLOAT, gl.FALSE, 0, 0);
    	this.gl.enableVertexAttribArray(1);
	}

	genUVs()
	{
		var uvs = [];

	    uvs.push(vec2.fromValues(1.0, 0.0));
	    uvs.push(vec2.fromValues(0.0, 0.0));
	    uvs.push(vec2.fromValues(0.0, 1.0));
	    uvs.push(vec2.fromValues(1.0, 0.0));
	    uvs.push(vec2.fromValues(0.0, 1.0));
	    uvs.push(vec2.fromValues(1.0, 1.0));
	    uvs.push(vec2.fromValues(1.0, 0.0));
	    uvs.push(vec2.fromValues(0.0, 0.0));
	    uvs.push(vec2.fromValues(0.0, 1.0));
	    uvs.push(vec2.fromValues(1.0, 0.0));
	    uvs.push(vec2.fromValues(0.0, 1.0));
	    uvs.push(vec2.fromValues(1.0, 1.0));
	    uvs.push(vec2.fromValues(1.0, 0.0));
	    uvs.push(vec2.fromValues(0.0, 0.0));
	    uvs.push(vec2.fromValues(0.0, 1.0));
	    uvs.push(vec2.fromValues(1.0, 0.0));
	    uvs.push(vec2.fromValues(0.0, 1.0));
	    uvs.push(vec2.fromValues(1.0, 1.0));
	    uvs.push(vec2.fromValues(1.0, 0.0));
	    uvs.push(vec2.fromValues(0.0, 0.0));
	    uvs.push(vec2.fromValues(0.0, 1.0));
	    uvs.push(vec2.fromValues(1.0, 0.0));
	    uvs.push(vec2.fromValues(0.0, 1.0));
	    uvs.push(vec2.fromValues(1.0, 1.0));
	    uvs.push(vec2.fromValues(1.0, 0.0));
	    uvs.push(vec2.fromValues(0.0, 0.0));
	    uvs.push(vec2.fromValues(0.0, 1.0));
	    uvs.push(vec2.fromValues(1.0, 0.0));
	    uvs.push(vec2.fromValues(0.0, 1.0));
	    uvs.push(vec2.fromValues(1.0, 1.0));
	    uvs.push(vec2.fromValues(1.0, 0.0));
	    uvs.push(vec2.fromValues(0.0, 0.0));
	    uvs.push(vec2.fromValues(0.0, 1.0));
	    uvs.push(vec2.fromValues(1.0, 0.0));
	    uvs.push(vec2.fromValues(0.0, 1.0));
	    uvs.push(vec2.fromValues(1.0, 1.0));

	    var uvs32 = new Float32Array(uvs.length * 2);

	    for(var i = 0; i < uvs.length; i++)
	    {
	    	uvs32[2 * i]     = uvs[i][0];
	    	uvs32[2 * i + 1] = uvs[i][1];
	    }

	    this.normVboID = this.gl.createBuffer();
	    this.gl.bindBuffer(this.gl.ARRAY_BUFFER, this.normVboID);
	    this.gl.bufferData(this.gl.ARRAY_BUFFER, uvs32, this.gl.STATIC_DRAW, 0, uvs32.length);

	    this.gl.vertexAttribPointer(2, 2, this.gl.FLOAT, gl.FALSE, 0, 0);
    	this.gl.enableVertexAttribArray(2);
	}

	genTans()
	{
		var tans = [];

	    tans.push(vec3.fromValues(0.0, 1.0, 0.0));
	    tans.push(vec3.fromValues(0.0, 1.0, 0.0));
	    tans.push(vec3.fromValues(0.0, 1.0, 0.0));
	    tans.push(vec3.fromValues(0.0, 1.0, 0.0));
	    tans.push(vec3.fromValues(0.0, 1.0, 0.0));
	    tans.push(vec3.fromValues(0.0, 1.0, 0.0));
	    tans.push(vec3.fromValues(0.0, -1.0, 0.0));
	    tans.push(vec3.fromValues(0.0, -1.0, 0.0));
	    tans.push(vec3.fromValues(0.0, -1.0, 0.0));
	    tans.push(vec3.fromValues(0.0, -1.0, 0.0));
	    tans.push(vec3.fromValues(0.0, -1.0, 0.0));
	    tans.push(vec3.fromValues(0.0, -1.0, 0.0));
	    tans.push(vec3.fromValues(-1.0, 0.0, 0.0));
	    tans.push(vec3.fromValues(-1.0, 0.0, 0.0));
	    tans.push(vec3.fromValues(-1.0, 0.0, 0.0));
	    tans.push(vec3.fromValues(-1.0, 0.0, 0.0));
	    tans.push(vec3.fromValues(-1.0, 0.0, 0.0));
	    tans.push(vec3.fromValues(-1.0, 0.0, 0.0));
	    tans.push(vec3.fromValues(1.0, 0.0, 0.0));
	    tans.push(vec3.fromValues(1.0, 0.0, 0.0));
	    tans.push(vec3.fromValues(1.0, 0.0, 0.0));
	    tans.push(vec3.fromValues(1.0, 0.0, 0.0));
	    tans.push(vec3.fromValues(1.0, 0.0, 0.0));
	    tans.push(vec3.fromValues(1.0, 0.0, 0.0));
	    tans.push(vec3.fromValues(1.0, 0.0, 0.0));
	    tans.push(vec3.fromValues(1.0, 0.0, 0.0));
	    tans.push(vec3.fromValues(1.0, 0.0, 0.0));
	    tans.push(vec3.fromValues(1.0, 0.0, 0.0));
	    tans.push(vec3.fromValues(1.0, 0.0, 0.0));
	    tans.push(vec3.fromValues(1.0, 0.0, 0.0));
	    tans.push(vec3.fromValues(-1.0, 0.0, 0.0));
	    tans.push(vec3.fromValues(-1.0, 0.0, 0.0));
	    tans.push(vec3.fromValues(-1.0, 0.0, 0.0));
	    tans.push(vec3.fromValues(-1.0, 0.0, 0.0));
	    tans.push(vec3.fromValues(-1.0, 0.0, 0.0));
	    tans.push(vec3.fromValues(-1.0, 0.0, 0.0));

	    var tans32 = new Float32Array(tans.length * 3);

	    for(var i = 0; i < tans.length; i++)
	    {
	    	tans32[3 * i]     = tans[i][0];
	    	tans32[3 * i + 1] = tans[i][1];
	    	tans32[3 * i + 2] = tans[i][2];
	    }

	    this.tanVboID = this.gl.createBuffer();
	    this.gl.bindBuffer(this.gl.ARRAY_BUFFER, this.tanVboID);
	    this.gl.bufferData(this.gl.ARRAY_BUFFER, tans32, this.gl.STATIC_DRAW, 0, tans32.length);

	    this.gl.vertexAttribPointer(3, 3, this.gl.FLOAT, gl.FALSE, 0, 0);
    	this.gl.enableVertexAttribArray(3);
	}

	setModel(modelIn)
	{
		this.model = modelIn;
	}

	get Model()
	{
		return this.model;
	}

	draw()
	{
		this.gl.bindVertexArray(this.vaoID);
		this.gl.drawArrays(this.gl.TRIANGLES, 0, 30);
	}
}