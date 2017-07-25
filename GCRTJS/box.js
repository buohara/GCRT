class Box
{
	var vaoID;
	var posVboID;
	var normVboID;
	var uvVboID;
	var tanVboID;
	var numVerts;

	constructor(gl)
	{
		vaoID = gl.createVertexArray();
		gl.bindVertexArray(vaoID);

		nVerts = 36;

		genPositions(gl);
		genNormals(gl);
		genUVs(gl);
		genTans(gl);
	}

	genPositions(gl)
	{
		var pos = [];

		var hx = 1.0;
	    var hy = 1.0;
	    var hz = 1.0;

	    // Front

	    pos.push(new Vec3(hx, hy, hz));
	    pos.push(new Vec3(hx, -hy, hz));
	    pos.push(new Vec3(hx, -hy, -hz));
	    pos.push(new Vec3(hx, hy, hz));
	    pos.push(new Vec3(hx, -hy, -hz));
	    pos.push(new Vec3(hx, hy, -hz));
	    pos.push(new Vec3(-hx, -hy, hz));
	    pos.push(new Vec3(-hx, hy, hz));
	    pos.push(new Vec3(-hx, hy, -hz));
	    pos.push(new Vec3(-hx, -hy, hz));
	    pos.push(new Vec3(-hx, hy, -hz));
	    pos.push(new Vec3(-hx, -hy, -hz));
	    pos.push(new Vec3(-hx, hy, hz));
	    pos.push(new Vec3(hx, hy, hz));
	    pos.push(new Vec3(hx, hy, -hz));
	    pos.push(new Vec3(-hx, hy, hz));
	    pos.push(new Vec3(hx, hy, -hz));
	    pos.push(new Vec3(-hx, hy, -hz));
	    pos.push(new Vec3(hx, -hy, hz));
	    pos.push(new Vec3(-hx, -hy, hz));
	    pos.push(new Vec3(-hx, -hy, -hz));
	    pos.push(new Vec3(hx, -hy, hz));
	    pos.push(new Vec3(-hx, -hy, -hz));
	    pos.push(new Vec3(hx, -hy, -hz));
	    pos.push(new Vec3(hx, hy, hz));
	    pos.push(new Vec3(-hx, hy, hz));
	    pos.push(new Vec3(-hx, -hy, hz));
	    pos.push(new Vec3(hx, hy, hz));
	    pos.push(new Vec3(-hx, -hy, hz));
	    pos.push(new Vec3(hx, -hy, hz));
	    pos.push(new Vec3(-hx, hy, -hz));
	    pos.push(new Vec3(hx, hy, -hz));
	    pos.push(new Vec3(hx, -hy, -hz));
	    pos.push(new Vec3(-hx, hy, -hz));
	    pos.push(new Vec3(hx, -hy, -hz));
	    pos.push(new Vec3(-hx, -hy, -hz));

	    var pos32 = new Float32Array(pos.length * 3);

	    for(var i = 0; i < pos.length; i++)
	    {
	    	pos32[3 * i] = pos[i].x;
	    	pos32[3 * i + 1] = pos[i].y;
	    	pos32[3 * i + 2] = pos[i].z;
	    }

	    posVboID = gl.CreateBuffer();
	    gl.bindBuffer(gl.ARRAY_BUFFER, posVboID);
	    gl.bufferData(gl.ARRAY_BUFFER, pos32, gl.STATIC_DRAW, 0, pos32.length);

	    gl.vertexAttribPointer(0, 3, gl.GL_FLOAT, false, 0, 0);
    	gl.enableVertexAttribArray(0);
	}

	genNormals(gl)
	{
		var norms = [];

	    norms.push(new Vec3(1.0, 0.0, 0.0));
	    norms.push(new Vec3(1.0, 0.0, 0.0));
	    norms.push(new Vec3(1.0, 0.0, 0.0));
	    norms.push(new Vec3(1.0, 0.0, 0.0));
	    norms.push(new Vec3(1.0, 0.0, 0.0));
	    norms.push(new Vec3(1.0, 0.0, 0.0));
	    norms.push(new Vec3(-1.0, 0.0, 0.0));
	    norms.push(new Vec3(-1.0, 0.0, 0.0));
	    norms.push(new Vec3(-1.0, 0.0, 0.0));
	    norms.push(new Vec3(-1.0, 0.0, 0.0));
	    norms.push(new Vec3(-1.0, 0.0, 0.0));
	    norms.push(new Vec3(-1.0, 0.0, 0.0));
	    norms.push(new Vec3(0.0, 1.0, 0.0));
	    norms.push(new Vec3(0.0, 1.0, 0.0));
	    norms.push(new Vec3(0.0, 1.0, 0.0));
	    norms.push(new Vec3(0.0, 1.0, 0.0));
	    norms.push(new Vec3(0.0, 1.0, 0.0));
	    norms.push(new Vec3(0.0, 1.0, 0.0));
	    norms.push(new Vec3(0.0, -1.0, 0.0));
	    norms.push(new Vec3(0.0, -1.0, 0.0));
	    norms.push(new Vec3(0.0, -1.0, 0.0));
	    norms.push(new Vec3(0.0, -1.0, 0.0));
	    norms.push(new Vec3(0.0, -1.0, 0.0));
	    norms.push(new Vec3(0.0, -1.0, 0.0));
	    norms.push(new Vec3(0.0, 0.0, 1.0));
	    norms.push(new Vec3(0.0, 0.0, 1.0));
	    norms.push(new Vec3(0.0, 0.0, 1.0));
	    norms.push(new Vec3(0.0, 0.0, 1.0));
	    norms.push(new Vec3(0.0, 0.0, 1.0));
	    norms.push(new Vec3(0.0, 0.0, 1.0));
	    norms.push(new Vec3(0.0, 0.0, -1.0));
	    norms.push(new Vec3(0.0, 0.0, -1.0));
	    norms.push(new Vec3(0.0, 0.0, -1.0));
	    norms.push(new Vec3(0.0, 0.0, -1.0));
	    norms.push(new Vec3(0.0, 0.0, -1.0));
	    norms.push(new Vec3(0.0, 0.0, -1.0));

	    var norms32 = new Float32Array(norms.length * 3);

	    for(var i = 0; i < norms.length; i++)
	    {
	    	norms32[3 * i]     = norms[i].x;
	    	norms32[3 * i + 1] = norms[i].y;
	    	norms32[3 * i + 2] = norms[i].z;
	    }

	    normVboID = gl.CreateBuffer();
	    gl.bindBuffer(gl.ARRAY_BUFFER, normVboID);
	    gl.bufferData(gl.ARRAY_BUFFER, norms32, gl.STATIC_DRAW, 0, norms32.length);

	    gl.vertexAttribPointer(1, 3, gl.GL_FLOAT, false, 0, 0);
    	gl.enableVertexAttribArray(1);
	}

	genUVs(gl)
	{
		var uvs = [];

	    uvs.push(new Vec2(1.0, 0.0));
	    uvs.push(new Vec2(0.0, 0.0));
	    uvs.push(new Vec2(0.0, 1.0));
	    uvs.push(new Vec2(1.0, 0.0));
	    uvs.push(new Vec2(0.0, 1.0));
	    uvs.push(new Vec2(1.0, 1.0));
	    uvs.push(new Vec2(1.0, 0.0));
	    uvs.push(new Vec2(0.0, 0.0));
	    uvs.push(new Vec2(0.0, 1.0));
	    uvs.push(new Vec2(1.0, 0.0));
	    uvs.push(new Vec2(0.0, 1.0));
	    uvs.push(new Vec2(1.0, 1.0));
	    uvs.push(new Vec2(1.0, 0.0));
	    uvs.push(new Vec2(0.0, 0.0));
	    uvs.push(new Vec2(0.0, 1.0));
	    uvs.push(new Vec2(1.0, 0.0));
	    uvs.push(new Vec2(0.0, 1.0));
	    uvs.push(new Vec2(1.0, 1.0));
	    uvs.push(new Vec2(1.0, 0.0));
	    uvs.push(new Vec2(0.0, 0.0));
	    uvs.push(new Vec2(0.0, 1.0));
	    uvs.push(new Vec2(1.0, 0.0));
	    uvs.push(new Vec2(0.0, 1.0));
	    uvs.push(new Vec2(1.0, 1.0));
	    uvs.push(new Vec2(1.0, 0.0));
	    uvs.push(new Vec2(0.0, 0.0));
	    uvs.push(new Vec2(0.0, 1.0));
	    uvs.push(new Vec2(1.0, 0.0));
	    uvs.push(new Vec2(0.0, 1.0));
	    uvs.push(new Vec2(1.0, 1.0));
	    uvs.push(new Vec2(1.0, 0.0));
	    uvs.push(new Vec2(0.0, 0.0));
	    uvs.push(new Vec2(0.0, 1.0));
	    uvs.push(new Vec2(1.0, 0.0));
	    uvs.push(new Vec2(0.0, 1.0));
	    uvs.push(new Vec2(1.0, 1.0));

	    var uvs32 = new Float32Array(uvs.length * 2);

	    for(var i = 0; i < uvs.length; i++)
	    {
	    	uvs32[2 * i]     = uvs[i].x;
	    	uvs32[2 * i + 1] = uvs[i].y;
	    }

	    normVboID = gl.CreateBuffer();
	    gl.bindBuffer(gl.ARRAY_BUFFER, normVboID);
	    gl.bufferData(gl.ARRAY_BUFFER, uvs32, gl.STATIC_DRAW, 0, uvs32.length);

	    gl.vertexAttribPointer(2, 2, gl.GL_FLOAT, false, 0, 0);
    	gl.enableVertexAttribArray(2);
	}

	genTans(gl)
	{
		var tans = [];

	    tans.push_back(vec3(0.0, 1.0, 0.0));
	    tans.push_back(vec3(0.0, 1.0, 0.0));
	    tans.push_back(vec3(0.0, 1.0, 0.0));
	    tans.push_back(vec3(0.0, 1.0, 0.0));
	    tans.push_back(vec3(0.0, 1.0, 0.0));
	    tans.push_back(vec3(0.0, 1.0, 0.0));
	    tans.push_back(vec3(0.0, -1.0, 0.0));
	    tans.push_back(vec3(0.0, -1.0, 0.0));
	    tans.push_back(vec3(0.0, -1.0, 0.0));
	    tans.push_back(vec3(0.0, -1.0, 0.0));
	    tans.push_back(vec3(0.0, -1.0, 0.0));
	    tans.push_back(vec3(0.0, -1.0, 0.0));
	    tans.push_back(vec3(-1.0, 0.0, 0.0));
	    tans.push_back(vec3(-1.0, 0.0, 0.0));
	    tans.push_back(vec3(-1.0, 0.0, 0.0));
	    tans.push_back(vec3(-1.0, 0.0, 0.0));
	    tans.push_back(vec3(-1.0, 0.0, 0.0));
	    tans.push_back(vec3(-1.0, 0.0, 0.0));
	    tans.push_back(vec3(1.0, 0.0, 0.0));
	    tans.push_back(vec3(1.0, 0.0, 0.0));
	    tans.push_back(vec3(1.0, 0.0, 0.0));
	    tans.push_back(vec3(1.0, 0.0, 0.0));
	    tans.push_back(vec3(1.0, 0.0, 0.0));
	    tans.push_back(vec3(1.0, 0.0, 0.0));
	    tans.push_back(vec3(1.0, 0.0, 0.0));
	    tans.push_back(vec3(1.0, 0.0, 0.0));
	    tans.push_back(vec3(1.0, 0.0, 0.0));
	    tans.push_back(vec3(1.0, 0.0, 0.0));
	    tans.push_back(vec3(1.0, 0.0, 0.0));
	    tans.push_back(vec3(1.0, 0.0, 0.0));
	    tans.push_back(vec3(-1.0, 0.0, 0.0));
	    tans.push_back(vec3(-1.0, 0.0, 0.0));
	    tans.push_back(vec3(-1.0, 0.0, 0.0));
	    tans.push_back(vec3(-1.0, 0.0, 0.0));
	    tans.push_back(vec3(-1.0, 0.0, 0.0));
	    tans.push_back(vec3(-1.0, 0.0, 0.0));

	    var tans32 = new Float32Array(tans.length * 2);

	    for(var i = 0; i < tans.length; i++)
	    {
	    	tans32[3 * i]     = tans[i].x;
	    	tans32[3 * i + 1] = tans[i].y;
	    	tans32[3 * i + 2] = tans[i].y;
	    }

	    normVboID = gl.CreateBuffer();
	    gl.bindBuffer(gl.ARRAY_BUFFER, normVboID);
	    gl.bufferData(gl.ARRAY_BUFFER, tans32, gl.STATIC_DRAW, 0, tans32.length);

	    gl.vertexAttribPointer(3, 3, gl.GL_FLOAT, false, 0, 0);
    	gl.enableVertexAttribArray(3);
	}

	draw(gl)
	{
		gl.bindVertexArray(vaoID);
		gl.drawArrays(GL_TRIANGLES, 0, nVerts);
		gl.bindVertexArray(0);
	}
}