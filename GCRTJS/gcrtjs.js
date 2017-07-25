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

	var renderer = new Renderer(gl);
}