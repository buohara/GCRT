var gl;
var renderer;

function handleKeyDown(event)
{
	renderer.handleKeyDown(event);
}

function handleKeyUp(event)
{
	renderer.handleKeyUp(event);
}

function handleMouseDown(event)
{
	renderer.handleMouseDown(event);
}

function handleMouseUp(event)
{
	renderer.handleMouseUp(event);
}

function handleMouseMove(event)
{
	renderer.handleMouseMove(event);
}

function startGCRT()
{
	var canvas = document.getElementById('glCanvas');
	gl = canvas.getContext('webgl2') || canvas.getContext('experimental-webgl');

	if (!gl)
	{
		alert('Unable to initialize WebGL. Please check your browser for support.');
		return;
	}

	document.onkeydown 		= handleKeyDown;
	document.onkeyup 		= handleKeyUp;

	canvas.onmousedown 		= handleMouseDown;
    document.onmouseup 		= handleMouseUp;
    document.onmousemove 	= handleMouseMove;

	renderer = new Renderer(gl);
	renderer.render();
}