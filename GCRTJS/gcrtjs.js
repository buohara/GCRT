var gl;

function initWebGL(canvas)
{
	gl = null;
	gl = canvas.getContext('webgl') || canvas.getContext('experimental-webgl');

	if (!gl)
	{
		alert('Unable to initialize WebGL. Please check your browser for support.');
	}

	return gl;
}

function startGCRT()
{
	var canvas = document.getElementById('glCanvas');
	gl = initWebGL(canvas);

	if (!gl)
	{
		return;
	}

	gl.clearColor(0.0, 0.0, 0.0, 0.0);
	gl.enable(gl.DEPTH_TEST);
	gl.depthFunc(gl.LEQUAL);
	gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
}